//============================================================================
// Name        : NodeTree.cpp
// Author      : Avi
// Revision    : $Revision: #57 $ 
//
// Copyright 2009-2019 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0 
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
// In applying this licence, ECMWF does not waive the privileges and immunities 
// granted to it by virtue of its status as an intergovernmental organisation 
// nor does it submit to any jurisdiction. 
//
// Description :
//============================================================================

#include <sstream>

#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>  // requires boost date and time lib

#include "RepeatAttr.hpp"
#include "Indentor.hpp"
#include "Ecf.hpp"
#include "Log.hpp"
#include "Str.hpp"
#include "PrintStyle.hpp"
#include "Cal.hpp"
#include "Serialization.hpp"

using namespace std;
using namespace ecf;
using namespace boost::gregorian;
using namespace boost::posix_time;

const Repeat& Repeat::EMPTY() { static const Repeat REPEAT = Repeat(); return REPEAT; }

//=========================================================================

Repeat::Repeat() = default;
Repeat::Repeat( const RepeatDate& r) : type_(std::make_unique<RepeatDate>(r)) {}
Repeat::Repeat( const RepeatDateList& r) : type_(std::make_unique<RepeatDateList>(r)) {}
Repeat::Repeat( const RepeatInteger& r) : type_(std::make_unique<RepeatInteger>(r)) {}
Repeat::Repeat( const RepeatEnumerated& r) : type_(std::make_unique<RepeatEnumerated>(r)) {}
Repeat::Repeat( const RepeatString& r) : type_(std::make_unique<RepeatString>(r)) {}
Repeat::Repeat( const RepeatDay& r) : type_(std::make_unique<RepeatDay>(r)) {}

Repeat::~Repeat() = default;

Repeat::Repeat( const Repeat& rhs)
{
	if ( rhs.type_) {
	   type_.reset( rhs.type_->clone());
	}
}

Repeat& Repeat::operator=(const Repeat& rhs)
{
   type_.reset(nullptr);
	if ( rhs.type_) {
      type_.reset( rhs.type_->clone());
	}
	return *this;
}

Repeat& Repeat::operator=(Repeat&& rhs)
{
   type_ = std::move(rhs.type_); // since rhs.type_ is a unique_ptr no need to set rhs.type_ to NULL
   return *this;
}

bool Repeat::operator==(const Repeat& rhs) const
{
	if (!type_ && rhs.type_) return false;
	if (type_ && !rhs.type_) return false;
	if (!type_ && !rhs.type_) return true	;
	return type_->compare(rhs.type_.get());
}

const std::string& Repeat::name() const {
   return (type_.get()) ? type_->name() : Str::EMPTY();
}

void Repeat::print( std::string& os ) const {
	if (type_) {
		Indentor in;
		Indentor::indent(os); write(os); os += "\n";
	}
}

// =========================================================================
RepeatBase::~RepeatBase() = default;

void RepeatBase::incr_state_change_no()
{
	state_change_no_ = Ecf::incr_state_change_no();

#ifdef DEBUG_STATE_CHANGE_NO
	std::cout << "RepeatBase::incr_state_change_no()\n";
#endif
}

void  RepeatBase::update_repeat_genvar() const
{
   // **** reset name since generated variables are not persisted
   var_.set_name( name_ );

   // valueAsString() use the last_valid_value() which should always be in range.
   // Note repeat::value() can be on e past the last valid value, at expiration of Repeat loop
   //      However Repeat::last_valid_value() will just return the last valid value.
   var_.set_value( valueAsString() );
}

std::string RepeatBase::toString() const
{
   std::string ret;
   write(ret);
   return ret;
}

// =============================================================

RepeatDate::RepeatDate( const std::string& variable,
                        int start,
                        int end,
                        int delta /* always in days*/
) : RepeatBase(variable), start_(start), end_(end), delta_(delta), value_(start)
{
   if ( !Str::valid_name( variable ) ) {
      throw std::runtime_error("RepeatDate::RepeatDate: Invalid name: " + variable);
   }

   if (delta == 0) {
      std::stringstream ss; ss << "repeat " << variable << " " << start << " " << end << " " << delta;
      throw std::runtime_error("Invalid Repeat date: the delta can not be zero" + ss.str());
   }

   std::string theStart = boost::lexical_cast< std::string >(start);
   if (theStart.size() != 8) {
      std::stringstream ss; ss << "repeat " << variable << " " << start << " " << end << " " << delta;
      throw std::runtime_error("Invalid Repeat date: The start is not a valid date. Please use yyyymmdd format." + ss.str());
   }
   std::string theEnd = boost::lexical_cast< std::string >(end);
   if (theEnd.size() != 8) {
      std::stringstream ss; ss << "repeat " << variable << " " << start << " " << end << " " << delta;
      throw std::runtime_error("Invalid Repeat date: The end is not a valid date. Please use yyyymmdd format." + ss.str());
   }


   if (delta_ > 0) {
      // assert end => start
      if (!(end >= start)) {
         std::stringstream ss; ss << "repeat " << variable << " " << start << " " << end << " " << delta;
         throw std::runtime_error("Invalid Repeat date: The end must be greater than the start date, when delta is positive " + ss.str());
      }
   }
   else {
      // assert start >= end
      if (!(start >= end)) {
         std::stringstream ss; ss << "repeat " << variable << " " << start << " " << end << " " << delta;
         throw std::runtime_error("Invalid Repeat date: The start must be greater than the end date, when delta is negative " + ss.str());
      }
   }

	// Use date lib to check YMD
	try {
		boost::gregorian::date(from_undelimited_string(theStart));
		boost::gregorian::date(from_undelimited_string(theEnd));
	}
	catch (std::exception& e) {
      std::stringstream ss; ss << "repeat " << variable << " " << start << " " << end << " " << delta;
		throw std::runtime_error("Invalid Repeat date: The start/end is not a valid date." + ss.str());
	}
}

void RepeatDate::gen_variables(std::vector<Variable>& vec) const
{
   vec.push_back(yyyy_);
   vec.push_back(mm_);
   vec.push_back(dom_);
   vec.push_back(dow_);
   vec.push_back(julian_);
   RepeatBase::gen_variables(vec);
}

const Variable& RepeatDate::find_gen_variable(const std::string& name) const
{
   if (name == name_) return var_;
   if (name == julian_.name()) return julian_;
   if (name == dow_.name()) return dow_;
   if (name == dom_.name()) return dom_;
   if (name == mm_.name()) return mm_;
   if (name == yyyy_.name()) return yyyy_;
   return Variable::EMPTY();
}

void RepeatDate::update_repeat_genvar() const
{
   RepeatBase::update_repeat_genvar();

   yyyy_.set_name( name_ + "_YYYY");    yyyy_.set_value("<invalid>");
   mm_.set_name( name_ + "_MM");        mm_.set_value("<invalid>" );
   dom_.set_name( name_ + "_DD");       dom_.set_value("<invalid>");
   dow_.set_name( name_ + "_DOW");      dom_.set_value("<invalid>");
   julian_.set_name( name_ + "_JULIAN");julian_.set_value("<invalid>");

   yyyy_.set_name( name_ + "_YYYY");
   mm_.set_name( name_ + "_MM");
   dom_.set_name( name_ + "_DD");
   dow_.set_name( name_ + "_DOW");
   julian_.set_name( name_ + "_JULIAN");

   update_repeat_genvar_value();
}

void RepeatDate::update_repeat_genvar_value() const
{
   std::string date_as_string = valueAsString();
   if (valid()) {
      try {
         boost::gregorian::date the_date(from_undelimited_string(date_as_string));
         if (the_date.is_special()) {
            std::stringstream ss;
            ss << "RepeatDate::update_repeat_genvar(): invalid current date: " << date_as_string << " is_special";
            log(Log::ERR,ss.str());
            return;
         }
         //int day_of_year  = the_date.day_of_year();
         int day_of_week  = the_date.day_of_week().as_number();
         int day_of_month = the_date.day();
         int month        = the_date.month();
         int year         = the_date.year();

         yyyy_.set_value(boost::lexical_cast<std::string>(year));
         mm_.set_value(boost::lexical_cast<std::string>(month));
         dom_.set_value(boost::lexical_cast<std::string>(day_of_month));
         dow_.set_value(boost::lexical_cast<std::string>(day_of_week));

         long last_value = last_valid_value();
         long julian = Cal::date_to_julian( last_value );
         julian_.set_value(boost::lexical_cast<std::string>(julian));
      }
      catch (std::exception& e) {
         std::stringstream ss;
         ss << "RepeatDate::update_repeat_genvar_value : " << toString() << "\n The current date(" << date_as_string  << ") is not valid";
         log(Log::ERR,ss.str());
         return;
      }
   }
}


bool RepeatDate::compare(RepeatBase* rb) const
{
	auto* rhs = dynamic_cast<RepeatDate*>(rb);
	if(!rhs) return false;
	return operator==(*rhs);
}

void RepeatDate::setToLastValue()
{
   set_value(end_);
}

long RepeatDate::last_valid_value() const
{
   return valid_value(value_);
}

long RepeatDate::valid_value(long value) const
{
   if (delta_ > 0) {
      if (value < start_) return start_;
      if (value > end_)   return end_;
      return value;
   }
   if (value > start_) return start_;
   if (value < end_)   return end_;
   return value;
}

long RepeatDate::last_valid_value_minus(int val) const
{
   long last_value = last_valid_value();
   long julian = Cal::date_to_julian(last_value);
   julian -= val;
   return Cal::julian_to_date(julian);
}

long RepeatDate::last_valid_value_plus(int val) const
{
   long last_value = last_valid_value();
   long julian = Cal::date_to_julian(last_value);
   julian += val;
   return Cal::julian_to_date(julian);
}

void RepeatDate::reset()
{
   set_value(start_);
}

void RepeatDate::write(std::string& ret) const
{
   ret += "repeat date ";  ret += name_; ret += " ";
   ret += boost::lexical_cast<std::string>(start_); ret += " ";
   ret += boost::lexical_cast<std::string>(end_);   ret += " ";
   ret += boost::lexical_cast<std::string>(delta_);

   if (!PrintStyle::defsStyle() && (value_ != start_)) {
      ret += " # ";
      ret += boost::lexical_cast<std::string>(value_);
   }
}

std::string RepeatDate::dump() const
{
	std::stringstream ss;
	ss << toString() << " value(" << value_ << ")";
	return ss.str();
}

bool RepeatDate::operator==(const RepeatDate& rhs) const
{
	if (name_ != rhs.name_) {
		return false;
	}
	if (start_ != rhs.start_) {
		return false;
	}
	if (end_ != rhs.end_) {
		return false;
	}
	if (delta_ != rhs.delta_) {
		return false;
	}
   if (value_ != rhs.value_) {
      return false;
   }
	return true;
}

std::string RepeatDate::valueAsString() const
{
	/// will throw a boost::bad_lexical_cast& if value is not convertible to a string
 	try {
		return boost::lexical_cast< std::string >( last_valid_value() );
	}
	catch ( boost::bad_lexical_cast& ) {
		LOG_ASSERT(false,"RepeatDate::valueAsString(): could not convert value " << value_ << " to a string");
	}
	return string();
}

std::string RepeatDate::value_as_string(int index) const
{
   /// will throw a boost::bad_lexical_cast& if value is not convertible to a string
   try {
      return boost::lexical_cast< std::string >( index );
   }
   catch ( boost::bad_lexical_cast& ) {}
   return string();
}

std::string RepeatDate::next_value_as_string() const
{
   long val = last_valid_value();

   long julian = Cal::date_to_julian(val);
   julian += delta_;
   val = Cal::julian_to_date(julian);

   try { return boost::lexical_cast< std::string >( valid_value(val) );}
   catch ( boost::bad_lexical_cast& ) {}
   return string();
}

std::string RepeatDate::prev_value_as_string() const
{
   long val = last_valid_value();

   long julian = Cal::date_to_julian(val);
   julian -= delta_;
   val = Cal::julian_to_date(julian);

   try { return boost::lexical_cast< std::string >( valid_value(val) );}
   catch ( boost::bad_lexical_cast& ) {}
   return string();
}

void RepeatDate::increment()
{
   long julian = Cal::date_to_julian(value_);
   julian += delta_;
   set_value(Cal::julian_to_date(julian));
}

void RepeatDate::change( const std::string& newdate)
{
 	if (newdate.size() != 8) {
      std::stringstream ss;
      ss << "RepeatDate::change: " << toString() << " The new date is not valid, expected 8 characters in yyyymmdd format but found " << newdate;
		throw std::runtime_error(ss.str());
 	}

	long the_new_date = 0;
 	try {
 		the_new_date = boost::lexical_cast< long >( newdate );
	}
	catch ( boost::bad_lexical_cast& ) {
      std::stringstream ss;
      ss << "RepeatDate::change: " << toString() << " The new date(" << newdate << ") is not convertible to an long";
 		throw std::runtime_error(ss.str());
	}

	// Use date lib to check YMD
	try { boost::gregorian::date(from_undelimited_string(newdate));}
	catch (std::exception& e) {
      std::stringstream ss;
      ss << "RepeatDate::change: " << toString() << " The new date(" << newdate << ") is not valid";
 		throw std::runtime_error(ss.str());
	}

	changeValue(the_new_date);
}

void RepeatDate::changeValue(long the_new_date)
{
   if (delta_ > 0) {
      if (the_new_date < start_ || the_new_date > end_) {
         std::stringstream ss;
         ss << "RepeatDate::changeValue: " << toString() << "\nThe new date should be in the range[" << start_ << " : " << end_ << "] but found " << the_new_date;
         throw std::runtime_error(ss.str());
      }
   }
   else {
      if (the_new_date > start_ || the_new_date < end_) {
         std::stringstream ss;
         ss << "RepeatDate::changeValue: " << toString() << "\nThe new date should be in the range[" << start_ << " : " << end_ << "] but found " << the_new_date;
         throw std::runtime_error(ss.str());
      }
   }

   // Check new value is in step. ECFLOW-325 repeat date 7
   long julian_new_date = Cal::date_to_julian(the_new_date);
   long julian_start = Cal::date_to_julian(start_);
   long diff = julian_new_date - julian_start;
   if ( diff % delta_ != 0 ) {
       std::stringstream ss;
       ss << "RepeatDate::changeValue: " << toString() << "\nThe new date " << the_new_date << " is not in line with the delta/step";
       throw std::runtime_error(ss.str());
   }

	set_value(the_new_date);
}

void RepeatDate::set_value(long the_new_date)
{
   // Note: the node is incremented one past, the last value
   // In Node we increment() then check for validity
   // hence the_new_value may be outside of the valid range.
   // This can be seen when do a incremental sync,
   // *hence* allow memento to copy the value as is.
   value_ = the_new_date;
   update_repeat_genvar_value();
   incr_state_change_no();
}

//======================================================================================

RepeatDateList::RepeatDateList( const std::string& variable, const std::vector<int>& l)
: RepeatBase(variable), list_(l)
{
   if ( !Str::valid_name( variable ) ) {
      throw std::runtime_error("RepeatDateList: Invalid name: " + variable);
   }
   if (list_.empty()) throw std::runtime_error("RepeatDateList: " + variable + " is empty");

   for(int i : list_) {
      std::string date_i = boost::lexical_cast< std::string >(i);
      if (date_i.size() != 8) {
         std::stringstream ss; ss << "Invalid Repeat datelist : " << variable << " the date " << i  << " is not valid. Please use yyyymmdd format.";
         throw std::runtime_error("Invalid Repeat datelist " + ss.str());
      }

      try { boost::gregorian::date(from_undelimited_string(date_i));}
      catch (std::exception& e) {
         std::stringstream ss; ss << "Invalid Repeat datelist : " << variable << " the date " << i  << " is not valid. Please use yyyymmdd format.";
         throw std::runtime_error("Invalid Repeat datelist " + ss.str());
      }
   }
}

void RepeatDateList::gen_variables(std::vector<Variable>& vec) const
{
   vec.push_back(yyyy_);
   vec.push_back(mm_);
   vec.push_back(dom_);
   vec.push_back(dow_);
   vec.push_back(julian_);
   RepeatBase::gen_variables(vec);
}

const Variable& RepeatDateList::find_gen_variable(const std::string& name) const
{
   if (name == name_) return var_;
   if (name == yyyy_.name()) return yyyy_;
   if (name == mm_.name()) return mm_;
   if (name == dom_.name()) return dom_;
   if (name == dow_.name()) return dow_;
   if (name == julian_.name()) return julian_;
   return Variable::EMPTY();
}

void RepeatDateList::update_repeat_genvar() const
{
   RepeatBase::update_repeat_genvar();

   yyyy_.set_name( name_ + "_YYYY");    yyyy_.set_value("<invalid>");
   mm_.set_name( name_ + "_MM");        mm_.set_value("<invalid>" );
   dom_.set_name( name_ + "_DD");       dom_.set_value("<invalid>");
   dow_.set_name( name_ + "_DOW");      dom_.set_value("<invalid>");
   julian_.set_name( name_ + "_JULIAN");julian_.set_value("<invalid>");

   update_repeat_genvar_value();
}

void RepeatDateList::update_repeat_genvar_value() const
{
   if (valid()) {
      std::string date_as_string = valueAsString();
      try {
         boost::gregorian::date the_date(from_undelimited_string(date_as_string));
         if (the_date.is_special()) {
            std::stringstream ss;
            ss << "RepeatDateList::update_repeat_genvar_value(): " << toString() << "\n invalid current date: " << date_as_string << " is special ";
            log(Log::ERR,ss.str());
            return;
         }

         //int day_of_year  = the_date.day_of_year();
         int day_of_week  = the_date.day_of_week().as_number();
         int day_of_month = the_date.day();
         int month        = the_date.month();
         int year         = the_date.year();

         yyyy_.set_value(boost::lexical_cast<std::string>(year));
         mm_.set_value(boost::lexical_cast<std::string>(month));
         dom_.set_value(boost::lexical_cast<std::string>(day_of_month));
         dow_.set_value(boost::lexical_cast<std::string>(day_of_week));

         long last_value = last_valid_value();
         long julian = Cal::date_to_julian( last_value );
         julian_.set_value(boost::lexical_cast<std::string>(julian));
      }
      catch (std::exception& e) {
         std::stringstream ss;
         ss << "RepeatDateList::update_repeat_genvar_value(): " << toString() << "\n invalid current date: " << date_as_string;
         log(Log::ERR,ss.str());
      }
   }
}

int RepeatDateList::start() const
{
   if (list_.empty()) return 0;
   return list_[0];
}
int RepeatDateList::end() const
{
   if (list_.empty()) return 0;
   return list_[list_.size() - 1];
}

bool RepeatDateList::compare(RepeatBase* rb) const
{
   auto* rhs = dynamic_cast<RepeatDateList*>(rb);
   if(!rhs) return false;
   return operator==(*rhs);
}

void RepeatDateList::write(std::string& ret) const
{
   ret += "repeat datelist ";  ret += name_;
   for(int date: list_) { ret += " \"";  ret += boost::lexical_cast<std::string>(date); ret += "\""; }
   if (!PrintStyle::defsStyle() && (currentIndex_ != 0)) {
      ret += " # ";
      ret += boost::lexical_cast<std::string>(currentIndex_);
   }
}

std::string RepeatDateList::dump() const
{
   std::stringstream ss;
   ss << toString() << " ordinal-value(" << value() << ") value-as-string(" << valueAsString() << ")";
   return ss.str();
}

void RepeatDateList::reset()
{
   set_value( 0 );
}

void RepeatDateList::increment()
{
   set_value( currentIndex_ + 1 );
}

long RepeatDateList::value() const
{
   if (list_.empty()) return 0;

   if (currentIndex_ >= 0 && currentIndex_ < static_cast<int>(list_.size()) ) {
       return list_[currentIndex_] ;
   }
   return 0;
}

long RepeatDateList::last_valid_value() const
{
   if (list_.empty()) return 0;

   if (currentIndex_ >= 0 && currentIndex_ < static_cast<int>(list_.size()) ) {
       return list_[currentIndex_] ;
   }
   if (currentIndex_ < 0) return list_[0] ;
   if ( currentIndex_ >= static_cast<int>(list_.size())) return list_[list_.size()-1] ;
   return 0;
}

long  RepeatDateList::last_valid_value_minus(int val) const
{
   long last_value = last_valid_value();
   if (last_value == 0) return 0;

   long julian = Cal::date_to_julian(last_value);
   julian -= val;
   return Cal::julian_to_date(julian);
}

long RepeatDateList::last_valid_value_plus(int val) const
{
   long last_value = last_valid_value();
   if (last_value == 0) return 0;

   long julian = Cal::date_to_julian(last_value);
   julian += val;
   return Cal::julian_to_date(julian);
}

void RepeatDateList::setToLastValue()
{
   if (list_.empty()) return;
   set_value(static_cast<int> ( list_.size()  - 1));
}

std::string RepeatDateList::valueAsString() const
{
   return boost::lexical_cast<std::string>( last_valid_value() );
}

std::string RepeatDateList::value_as_string(int index) const
{
   if (list_.empty()) return string("0");
   if (index >= 0 && index < static_cast<int>(list_.size())) {
      return boost::lexical_cast<std::string>( list_[index] ) ;
   }
   if (index < 0) return boost::lexical_cast<std::string>( list_[0] ) ;
   if (index >= static_cast<int>(list_.size())) return boost::lexical_cast<std::string>( list_[ list_.size()-1] ) ;
   return std::string();
}

std::string RepeatDateList::next_value_as_string() const
{
   if (list_.empty()) return string("0");

   int index = currentIndex_;
   index++;
   return value_as_string(index);
}

std::string RepeatDateList::prev_value_as_string() const
{
   if (list_.empty()) return string("0");

   int index = currentIndex_;
   index--;
   return value_as_string(index);
}

void RepeatDateList::change( const std::string& newValue)
{
   // See if if matches one of the dates
   int new_val = 0;
   try {  new_val =  boost::lexical_cast<int>( newValue  );}
   catch ( ... ) {
      std::stringstream ss;
      ss << "RepeatDateList::change: " << toString() << "\nThe new value " << newValue << " is must be convertible to integer, and correspond to an existing value\n";
      throw std::runtime_error(ss.str());
   }

   for(size_t i = 0; i < list_.size(); i++) {
      if ( list_[i] == new_val) {
         set_value(i);
         return;
      }
   }

   std::stringstream ss;
   ss << "RepeatDateList::change: " << toString() << "\nThe new value " << newValue << " is not a valid member of the date list\n";
   throw std::runtime_error(ss.str());
}

void RepeatDateList::changeValue( long the_new_index)
{
   if (list_.empty()) return;

   if ( the_new_index < 0 || the_new_index >= static_cast<int>(list_.size())) {
      std::stringstream ss;
      ss << "RepeatDateList::changeValue:" << toString() << "\nThe new value '" << the_new_index << "' is not a valid index ";
      ss << "expected range[0-" << list_.size()-1 << "] but found '" << the_new_index << "'";
      throw std::runtime_error( ss.str() );
   }
   set_value(the_new_index);
}

void RepeatDateList::set_value(long the_new_index)
{
   if (list_.empty()) return;

   // Note: the node is incremented one past, the last value
   // In Node we increment() then check for validity
   // hence the_new_value may be outside of the valid range.
   // This can be seen when do a incremental sync,
   // *hence* allow memento to copy the value as is.
   currentIndex_ = the_new_index;
   update_repeat_genvar_value();
   incr_state_change_no();
}

bool RepeatDateList::operator==(const RepeatDateList& rhs) const
{
   if (name_ != rhs.name_) {
#ifdef DEBUG
      if (Ecf::debug_equality()) {
         std::cout << "RepeatDateList::operator==( name_(" << name_ << ") != rhs.name_(" << rhs.name_ << "))\n";
      }
#endif
      return false;
   }
   if (list_ != rhs.list_) {
#ifdef DEBUG
      if (Ecf::debug_equality()) {
         std::cout << "RepeatDateList::operator==( list_ != rhs.list_ )\n";
      }
#endif
      return false;
   }
   if (currentIndex_ != rhs.currentIndex_) {
#ifdef DEBUG
      if (Ecf::debug_equality()) {
         std::cout << "RepeatDateList::operator==( currentIndex_(" << currentIndex_ << ") != rhs.currentIndex_(" << rhs.currentIndex_ << "))\n";
      }
#endif
      return false;
   }
   return true;
}

//======================================================================================

RepeatInteger::RepeatInteger( const std::string& variable, int start, int end, int delta ) :
	RepeatBase( variable ), start_( start ), end_( end ), delta_( delta ), value_( start )
{
//	cout << toString() << "\n";
   if ( !Str::valid_name( variable ) ) {
      throw std::runtime_error("RepeatInteger: Invalid name: " + variable);
   }
}
RepeatInteger::RepeatInteger() = default;

bool RepeatInteger::compare(RepeatBase* rb) const
{
	auto* rhs = dynamic_cast<RepeatInteger*>(rb);
	if(!rhs) return false;
	return operator==(*rhs);
}

void RepeatInteger::reset() {
	value_ = start_;
	incr_state_change_no();
}

long RepeatInteger::last_valid_value() const
{
   return valid_value(value_);
}

long RepeatInteger::valid_value(long value) const
{
   if (delta_ > 0) {
      if (value < start_) return start_;
      if (value > end_)   return end_;
      return value;
   }
   if (value > start_) return start_;
   if (value < end_)   return end_;
   return value;
}

void RepeatInteger::increment() {
	value_ += delta_;
	incr_state_change_no();
}

void RepeatInteger::change( const std::string& newValue)
{
 	long the_new_value = 0;
 	try {
 		the_new_value = boost::lexical_cast< long >( newValue );
	}
	catch ( boost::bad_lexical_cast& ) {
      std::stringstream ss;
      ss << "RepeatInteger::change:" << toString() << " The new value(" << newValue << ") is not convertible to an long";
 		throw std::runtime_error( ss.str() );
	}
	changeValue(the_new_value);
}

void RepeatInteger::changeValue(long the_new_value)
{
	if (delta_ > 0) {
		if (the_new_value < start_ || the_new_value > end_ ) {
			std::stringstream ss;
			ss << "RepeatInteger::changeValue:" << toString() << ". The new value should be in the range[" << start_ << "-" << end_ << "] but found " << the_new_value;
			throw std::runtime_error(ss.str());
		}
	}
	else {
      if (the_new_value > start_ || the_new_value < end_ ) {
         std::stringstream ss;
         ss << "RepeatInteger::changeValue:" << toString() << ". The new value should be in the range[" << start_ << "-" << end_ << "] but found " << the_new_value;
         throw std::runtime_error(ss.str());
      }
	}
	set_value(the_new_value);
}

void RepeatInteger::set_value(long the_new_value)
{
   // To be used by Memento only. as it does no checking
   // Note: the node is incremented one past, the last value
   // In Node we increment() then check for validity
   // hence the_new_value may be outside of the valid range.
   // This can be seen when do a incremental sync,
   // *hence* allow memento to copy the value as is.
   value_ = the_new_value;
   incr_state_change_no();
}

void RepeatInteger::setToLastValue()
{
   value_ = end_;
	incr_state_change_no();
}

void RepeatInteger::write(std::string& ret) const
{
   ret += "repeat integer ";  ret += name_; ret += " ";
   ret += boost::lexical_cast<std::string>(start_);  ret += " ";
   ret += boost::lexical_cast<std::string>(end_);
   if (delta_ != 1) { ret += " ";  ret += boost::lexical_cast<std::string>(delta_); }

   if (!PrintStyle::defsStyle() && (value_ != start_)) {
      ret += " # ";
      ret += boost::lexical_cast<std::string>(value_);
   }
}

std::string RepeatInteger::dump() const
{
	std::stringstream ss;
	ss << toString() << " value(" << value_ << ")";
	return ss.str();
}

bool RepeatInteger::operator==(const RepeatInteger& rhs) const
{
	if (name_ != rhs.name_) {
#ifdef DEBUG
      if (Ecf::debug_equality()) {
         std::cout << "RepeatInteger::operator==( name_(" << name_ << ") != rhs.name_(" << rhs.name_ << "))" << "\n";
      }
#endif
		return false;
	}
	if (start_ != rhs.start_) {
#ifdef DEBUG
      if (Ecf::debug_equality()) {
         std::cout << "RepeatInteger::operator==( start_(" << start_ << ") != rhs.start_(" << rhs.start_ << "))" << "\n";
      }
#endif
		return false;
	}
	if (end_ != rhs.end_) {
#ifdef DEBUG
      if (Ecf::debug_equality()) {
         std::cout << "RepeatInteger::operator==( end_(" << end_ << ") != rhs.end_(" << rhs.end_ << "))" << "\n";
      }
#endif
		return false;
	}
	if (delta_ != rhs.delta_) {
#ifdef DEBUG
      if (Ecf::debug_equality()) {
         std::cout << "RepeatInteger::operator==( delta_(" << delta_ << ") != rhs.delta_(" << rhs.delta_ << "))" << "\n";
      }
#endif
		return false;
	}
   if (value_ != rhs.value_) {
#ifdef DEBUG
      if (Ecf::debug_equality()) {
         std::cout << "RepeatInteger::operator==( value_(" << value_ << ") != rhs.value_(" << rhs.value_ << "))" << "\n";
      }
#endif
      return false;
   }
	return true;
}

std::string RepeatInteger::valueAsString() const
{
	/// will throw a boost::bad_lexical_cast& if value is not convertible to a string
 	try {
		return boost::lexical_cast< std::string >( last_valid_value() );
	}
	catch ( boost::bad_lexical_cast& ) { LOG_ASSERT(false,"");}
	return string();
}

std::string RepeatInteger::value_as_string(int index) const
{
   /// will throw a boost::bad_lexical_cast& if value is not convertible to a string
   try {
      return boost::lexical_cast< std::string >( index );
   }
   catch ( boost::bad_lexical_cast& ) {}
   return string();
}

std::string RepeatInteger::next_value_as_string() const
{
   long val = last_valid_value();
   val += delta_;
   try { return boost::lexical_cast< std::string >( valid_value(val) );}
   catch ( boost::bad_lexical_cast& ) {}
   return string();
}

std::string RepeatInteger::prev_value_as_string() const
{
   long val = last_valid_value();
   val -= delta_;
   try { return boost::lexical_cast< std::string >( valid_value(val) );}
   catch ( boost::bad_lexical_cast& ) {}
   return string();
}

//======================================================================================

RepeatEnumerated::RepeatEnumerated( const std::string& variable, const std::vector<std::string>& theEnums)
: RepeatBase(variable), theEnums_(theEnums) 
{
   if ( !Str::valid_name( variable ) ) {
      throw std::runtime_error("RepeatEnumerated: Invalid name: " + variable);
   }
   if (theEnums.empty()) {
      throw std::runtime_error("RepeatEnumerated: " + variable + " is empty");
   }
}

int RepeatEnumerated::end() const   {
	if ( theEnums_.empty() ) return 0;
	return static_cast<int>(theEnums_.size()-1);
}

bool RepeatEnumerated::compare(RepeatBase* rb) const
{
	auto* rhs = dynamic_cast<RepeatEnumerated*>(rb);
	if(!rhs) return false;
	return operator==(*rhs);
}

void RepeatEnumerated::write(std::string& ret) const
{
   ret += "repeat enumerated ";  ret += name_;
   for(const string& s: theEnums_) { ret += " \""; ret += s; ret += "\""; }
   if (!PrintStyle::defsStyle() && (currentIndex_ != 0)) {
      ret += " # ";
      ret += boost::lexical_cast<std::string>(currentIndex_);
   }
}

std::string RepeatEnumerated::dump() const
{
	std::stringstream ss;
	ss << toString() << " ordinal-value(" << value() << ")   value-as-string(" << valueAsString() << ")";
 	return ss.str();
}

void RepeatEnumerated::reset() {
	currentIndex_ = 0;
	incr_state_change_no();
}

void RepeatEnumerated::increment() {
	currentIndex_++;
	incr_state_change_no();
}

long RepeatEnumerated::value() const
{
   if (currentIndex_ >= 0 && currentIndex_ < static_cast<int>(theEnums_.size()) ) {
      try {
         return boost::lexical_cast<int>( theEnums_[currentIndex_] );
      }
      catch ( boost::bad_lexical_cast& ) {
         // Ignore and return currentIndex_
      }
   }
   return currentIndex_;
}

long RepeatEnumerated::last_valid_value() const
{
   if (!theEnums_.empty()) {
      if (currentIndex_ < 0) {
         try { return boost::lexical_cast<int>( theEnums_[0] ); }
         catch ( boost::bad_lexical_cast& ) { /* Ignore and return first index */ }
         return 0;
      }
      if (currentIndex_ >= static_cast<int>(theEnums_.size())) {

         try { return boost::lexical_cast<int>( theEnums_[theEnums_.size()-1] ); }
         catch ( boost::bad_lexical_cast& ) { /* Ignore and return last index */ }
         return static_cast<long>(theEnums_.size()-1);
      }

      // return current value as integer or as index
      return value();
   }
   return 0;
}

void RepeatEnumerated::setToLastValue()
{
	currentIndex_ = static_cast< int > ( theEnums_.size()  - 1);
	if (currentIndex_ < 0) currentIndex_ = 0;
	incr_state_change_no();
}

std::string RepeatEnumerated::valueAsString() const
{
   // This must always return a valid value
   if (!theEnums_.empty()) {

      // Returns the last valid value
      if (currentIndex_ < 0)
         return theEnums_[0]; // return first

      if (currentIndex_ >= static_cast<int>(theEnums_.size())) {
         return theEnums_[theEnums_.size()-1]; // return last
      }

      return theEnums_[currentIndex_];
   }
   return std::string();
}

std::string RepeatEnumerated::value_as_string(int index) const
{
   if (index >= 0 && index < static_cast<int>(theEnums_.size())) {
      return theEnums_[index];
   }
   return std::string();
}

std::string RepeatEnumerated::next_value_as_string() const
{
   if (theEnums_.empty()) return string();

   int index = currentIndex_;
   index++;
   if (index < 0) return theEnums_[0]; // return first
   if (index >= static_cast<int>(theEnums_.size())) return theEnums_[theEnums_.size()-1]; // return last
   return theEnums_[index];
}

std::string RepeatEnumerated::prev_value_as_string() const
{
   if (theEnums_.empty()) return string();

   int index = currentIndex_;
   index--;
   if (index < 0) return theEnums_[0]; // return first
   if (index >= static_cast<int>(theEnums_.size())) return theEnums_[theEnums_.size()-1]; // return last
   return theEnums_[index];
}

void RepeatEnumerated::change( const std::string& newValue)
{
	// See if if matches one of the enums
	for(size_t i = 0; i < theEnums_.size(); i++) {
		if ( theEnums_[i] == newValue) {
			currentIndex_ = i;
			incr_state_change_no();
			return;
		}
	}

	// If the value is convertible to an integer, treat as an index
  	try {
 		auto the_new_value = boost::lexical_cast< long >( newValue );
 		changeValue(the_new_value); // can throw if out of range
 		return;
	}
	catch ( boost::bad_lexical_cast& ) {}


	std::stringstream ss;
	ss << "RepeatEnumerated::change:" << toString() << "\nThe new value " << newValue << " is not a valid index or a member of the enumerated list\n";
	throw std::runtime_error(ss.str());
}

void RepeatEnumerated::changeValue( long the_new_value)
{
 	if ( the_new_value < 0 || the_new_value >= static_cast<int>(theEnums_.size())) {
		std::stringstream ss;
		ss << "RepeatEnumerated::changeValue:" << toString() << "\nThe new value '" << the_new_value << "' is not a valid index ";
		ss << "expected range[0-" << theEnums_.size()-1 << "] but found '" << the_new_value << "'";
		throw std::runtime_error( ss.str() );
	}
 	set_value(the_new_value);
}

void RepeatEnumerated::set_value(long the_new_value)
{
   // Note: the node is incremented one past, the last value
   // In Node we increment() then check for validity
   // hence the_new_value may be outside of the valid range.
   // This can be seen when do a incremental sync,
   // *hence* allow memento to copy the value as is.
   currentIndex_ = the_new_value;
   incr_state_change_no();
}

bool RepeatEnumerated::operator==(const RepeatEnumerated& rhs) const
{
	if (name_ != rhs.name_) {
#ifdef DEBUG
      if (Ecf::debug_equality()) {
         std::cout << "RepeatEnumerated::operator==( name_(" << name_ << ") != rhs.name_(" << rhs.name_ << "))\n";
      }
#endif
		return false;
	}
	if (theEnums_ != rhs.theEnums_) {
#ifdef DEBUG
      if (Ecf::debug_equality()) {
         std::cout << "RepeatEnumerated::operator==( theEnums_ != rhs.theEnums_ )\n";
      }
#endif
		return false;
	}
   if (currentIndex_ != rhs.currentIndex_) {
#ifdef DEBUG
      if (Ecf::debug_equality()) {
         std::cout << "RepeatEnumerated::operator==( currentIndex_(" << currentIndex_ << ") != rhs.currentIndex_(" << rhs.currentIndex_ << "))\n";
      }
#endif
      return false;
   }
	return true;
}


//======================================================================================

RepeatString::RepeatString( const std::string& variable, const std::vector<std::string>& theEnums)
: RepeatBase(variable), theStrings_(theEnums) 
{
   if ( !Str::valid_name( variable ) ) {
      throw std::runtime_error("RepeatString:: Invalid name: " + variable);
   }
   if (theEnums.empty()) {
      throw std::runtime_error("RepeatString : " + variable + " is empty");
   }
}

int RepeatString::end() const   {
	if ( theStrings_.empty() ) return 0;
	return static_cast<int>(theStrings_.size()-1);
}

bool RepeatString::compare(RepeatBase* rb) const
{
	auto* rhs = dynamic_cast<RepeatString*>(rb);
	if(!rhs) return false;
	return operator==(*rhs);
}

void RepeatString::write(std::string& ret) const
{
   ret += "repeat string ";  ret += name_;
   for(const string& s: theStrings_) { ret += " \""; ret += s; ret += "\""; }
   if (!PrintStyle::defsStyle() && (currentIndex_ != 0)) {
      ret += " # ";
      ret += boost::lexical_cast<std::string>(value());
   }
}

std::string RepeatString::dump() const
{
	std::stringstream ss;
	ss << toString() << " ordinal-value(" << value() << ")   value-as-string(" << valueAsString() << ")";
 	return ss.str();
}

void RepeatString::reset() {
	currentIndex_ = 0;
	incr_state_change_no();
}

long RepeatString::last_valid_value() const
{
   if (!theStrings_.empty()) {
      if (currentIndex_ < 0)  return 0;
      if (currentIndex_ >= static_cast<int>(theStrings_.size())) return static_cast<long>(theStrings_.size()-1);
      return currentIndex_;
   }
   return 0;
}

std::string RepeatString::next_value_as_string() const
{
   if (!theStrings_.empty()) {
      int index = currentIndex_;
      index++;
      if (index < 0) return theStrings_[0]; // return first
      if (index >= static_cast<int>(theStrings_.size())) return theStrings_[theStrings_.size()-1]; // return last
      return theStrings_[index];
   }
   return string();
}

std::string RepeatString::prev_value_as_string() const
{
   if (!theStrings_.empty()) {
      int index = currentIndex_;
      index--;
      if (index < 0) return theStrings_[0]; // return first
      if (index >= static_cast<int>(theStrings_.size())) return theStrings_[theStrings_.size()-1]; // return last
      return theStrings_[index];
   }
   return string();
}

void RepeatString::increment() {
	currentIndex_++;
	incr_state_change_no();
}

void RepeatString::setToLastValue() {
	currentIndex_ = static_cast<int>(theStrings_.size()-1);
   if (currentIndex_ < 0) currentIndex_ = 0;
	incr_state_change_no();
}

std::string RepeatString::valueAsString() const
{
   if (!theStrings_.empty()) return theStrings_[last_valid_value()];
   return std::string();
}

std::string RepeatString::value_as_string(int index) const
{
   if (index >= 0 && index < static_cast<int>(theStrings_.size())) {
      return theStrings_[index];
   }
   return std::string();
}

void RepeatString::change( const std::string& newValue)
{
	// See if if matches one of the strings
	for(size_t i = 0; i < theStrings_.size(); i++) {
		if ( theStrings_[i] == newValue) {
			currentIndex_ = i;
			incr_state_change_no();
			return;
		}
	}

	// If the value is convertible to an integer, treat as an index
  	try {
 		long the_new_value = boost::lexical_cast< int >( newValue );
 		changeValue(the_new_value);
 		return;
	}
	catch ( boost::bad_lexical_cast& ) {}

	std::stringstream ss;
	ss << "RepeatString::change: " << toString() << "\nThe new value " << newValue << " is not a valid index or member of the string list";
	throw std::runtime_error(ss.str());
}

void RepeatString::changeValue( long the_new_value)
{
	if ( the_new_value < 0 || the_new_value >= static_cast<int>(theStrings_.size())) {
		std::stringstream ss;
		ss << "RepeatString::change: " << toString() << " The new the integer " << the_new_value << " is not a valid index ";
		ss << "expected range[0-" << theStrings_.size()-1 << "]'";
		throw std::runtime_error( ss.str() );
	}
	set_value(the_new_value);
}

void RepeatString::set_value(long the_new_value)
{
   // Note: the node is incremented one past, the last value
   // In Node we increment() then check for validity
   // hence the_new_value may be outside of the valid range.
   // This can be seen when do a incremental sync,
   // *hence* allow memento to copy the value as is.
   currentIndex_ = the_new_value;
   incr_state_change_no();
}

bool RepeatString::operator==(const RepeatString& rhs) const
{
	if (name_ != rhs.name_) {
		return false;
	}
	if (theStrings_ != rhs.theStrings_) {
		return false;
	}
   if (currentIndex_ != rhs.currentIndex_) {
      return false;
   }
	return true;
}

//=======================================================================================

bool RepeatDay::compare(RepeatBase* rb) const
{
	auto* rhs = dynamic_cast<RepeatDay*>(rb);
	if(!rhs) return false;
	return operator==(*rhs);
}

void RepeatDay::write(std::string& ret) const
{
   ret += "repeat day ";
   ret += boost::lexical_cast<std::string>(step_);
}

std::string RepeatDay::dump() const
{
	return toString();
}

bool RepeatDay::operator==(const RepeatDay& rhs) const
{
	if (step_ != rhs.step_) {
		return false;
	}
	return true;
}
// =========================================================================================


template<class Archive>
void RepeatBase::serialize(Archive & ar)
{
   ar( CEREAL_NVP(name_) );
}

template<class Archive>
void RepeatDate::serialize(Archive & ar, std::uint32_t const version )
{
   ar(cereal::base_class<RepeatBase>(this),
      CEREAL_NVP(start_),
      CEREAL_NVP(end_),
      CEREAL_NVP(delta_),
      CEREAL_NVP(value_)
   );
}

template<class Archive>
void RepeatInteger::serialize(Archive & ar, std::uint32_t const version )
{
   ar(cereal::base_class<RepeatBase>(this),
      CEREAL_NVP(start_),
      CEREAL_NVP(end_),
      CEREAL_NVP(delta_),
      CEREAL_NVP(value_)
   );
}

template<class Archive>
void RepeatEnumerated::serialize(Archive & ar, std::uint32_t const version )
{
   ar(cereal::base_class<RepeatBase>(this),
      CEREAL_NVP(theEnums_),
      CEREAL_NVP(currentIndex_)
   );
}

template<class Archive>
void RepeatDateList::serialize(Archive & ar, std::uint32_t const version )
{
   ar(cereal::base_class<RepeatBase>(this),
      CEREAL_NVP(list_),
      CEREAL_NVP(currentIndex_)
   );
}

template<class Archive>
void RepeatString::serialize(Archive & ar, std::uint32_t const version )
{
   ar(cereal::base_class<RepeatBase>(this),
      CEREAL_NVP(theStrings_),
      CEREAL_NVP(currentIndex_)
   );
}

template<class Archive>
void RepeatDay::serialize(Archive & ar, std::uint32_t const version )
{
   ar(cereal::base_class<RepeatBase>(this),
      CEREAL_NVP(step_));
}

template<class Archive>
void Repeat::serialize(Archive & ar, std::uint32_t const version )
{
   ar(CEREAL_NVP(type_));
}

CEREAL_TEMPLATE_SPECIALIZE(RepeatBase);
CEREAL_TEMPLATE_SPECIALIZE_V(RepeatDate);
CEREAL_TEMPLATE_SPECIALIZE_V(RepeatDateList);
CEREAL_TEMPLATE_SPECIALIZE_V(RepeatInteger);
CEREAL_TEMPLATE_SPECIALIZE_V(RepeatEnumerated);
CEREAL_TEMPLATE_SPECIALIZE_V(RepeatString);
CEREAL_TEMPLATE_SPECIALIZE_V(RepeatDay);
CEREAL_TEMPLATE_SPECIALIZE_V(Repeat);

CEREAL_REGISTER_TYPE(RepeatDate);
CEREAL_REGISTER_TYPE(RepeatDateList);
CEREAL_REGISTER_TYPE(RepeatInteger);
CEREAL_REGISTER_TYPE(RepeatEnumerated);
CEREAL_REGISTER_TYPE(RepeatString);
CEREAL_REGISTER_TYPE(RepeatDay);
