//============================================================================
// Copyright 2017 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#include "VRepeatAttr.hpp"

#include <sstream>

#include "VAttributeType.hpp"
#include "VNode.hpp"

std::string VRepeatDateAttr::subType_("date");
std::string VRepeatIntAttr::subType_("integer");
std::string VRepeatStringAttr::subType_("string");
std::string VRepeatEnumAttr::subType_("enumerated");
std::string VRepeatDayAttr::subType_("day");

static long ecf_repeat_date_to_julian(long ddate);
static long ecf_repeat_julian_to_date(long jdate);

long ecf_repeat_julian_to_date(long jdate)
{
    long x,y,d,m,e;
    long day,month,year;

    x = 4 * jdate - 6884477;
    y = (x / 146097) * 100;
    e = x % 146097;
    d = e / 4;

    x = 4 * d + 3;
    y = (x / 1461) + y;
    e = x % 1461;
    d = e / 4 + 1;

    x = 5 * d - 3;
    m = x / 153 + 1;
    e = x % 153;
    d = e / 5 + 1;

    if( m < 11 )
        month = m + 2;
    else
        month = m - 10;


    day = d;
    year = y + m / 11;

    return year * 10000 + month * 100 + day;
}

long ecf_repeat_date_to_julian(long ddate)
{
    long  m1,y1,a,b,c,d,j1;

    long month,day,year;

    year = ddate / 10000;
    ddate %= 10000;
    month  = ddate / 100;
    ddate %= 100;
    day = ddate;

    if (0) {
      a = (14 - month) / 12;
      y1 = year + 4800 - a;
      m1 = month + 12*a - 3;
      j1 = day + (153*m1 + 2)/5 + 365*y1 + y1/4 - y1/100 + y1/400 - 32045;
      return j1 - 0.5;
    }

    if (month > 2)
    {
        m1 = month - 3;
        y1 = year;
    }
    else
    {
        m1 = month + 9;
        y1 = year - 1;
    }
    a = 146097*(y1/100)/4;
    d = y1 % 100;
    b = 1461*d/4;
    c = (153*m1+2)/5+day+1721119;
    j1 = a+b+c;

    return j1;
}

//================================
// VRepeatAttrType
//================================

VRepeatAttrType::VRepeatAttrType() : VAttributeType("repeat")
{
    dataCount_=8;
    searchKeyToData_["repeat_name"]=NameIndex;
    searchKeyToData_["repeat_value"]=ValueIndex;
    searchKeyToData_["name"]=NameIndex;
    scanProc_=VRepeatAttr::scan;
}

QString VRepeatAttrType::toolTip(QStringList d) const
{
    QString t="<b>Type:</b> Repeat";
    if(d.count() == dataCount_)
    {
        t+=" " + d[SubtypeIndex] + "<br>";

        if(d[SubtypeIndex] != "day")
        {
            t+="<b>Name:</b> " + d[NameIndex] + "<br>";
            t+="<b>Value:</b> " + d[ValueIndex] + "<br>";
            t+="<b>Start:</b> " + d[StartIndex] + "<br>";
            t+="<b>End:</b> " + d[EndIndex] + "<br>";
            t+="<b>Step:</b> " + d[StepIndex];
        }
        else
        {
            t+="<b>Step:</b> " + d[StepIndex];
        }
    }

    return t;
}

QString VRepeatAttrType::definition(QStringList d) const
{
    QString t="repeat";
    if(d.count() == dataCount_)
    {
        QString subType=d[SubtypeIndex];

        t+=" " + subType;

        if(subType == "integer" || subType == "date")
        {
            t+=" " + d[NameIndex];
            t+=" " + d[StartIndex];
            t+=" " + d[EndIndex];
            t+=" " + d[StepIndex];
        }
        else if(subType == "string" || subType == "enumerated")
        {
            t+=" " + d[NameIndex];
            t+=" " + d[AllValuesIndex];
        }
        else
        {
            t+=" " + d[StepIndex];
        }       
    }
    return t;
}

void VRepeatAttrType::encode(const Repeat& r,QStringList& data,const std::string& type,QString allValues) const
{
    //We try to avoid creating a VRepeat object everytime we are here
    //std::string type=VRepeat::type(r);

    data << qName_ << QString::fromStdString(type) <<
         QString::fromStdString(r.name()) <<
         QString::fromStdString(r.valueAsString()) <<
         QString::fromStdString(r.value_as_string(r.start())) <<
         QString::fromStdString(r.value_as_string(r.end())) <<
         QString::number(r.step()) <<
         allValues;
}

//=====================================================
//
// VRepeatAttr
//
//=====================================================

VRepeatAttr::VRepeatAttr(VNode *parent) : VAttribute(parent,0)
{
    //name_=e.name_or_number();
}

VAttributeType* VRepeatAttr::type() const
{
    static VAttributeType* atype=VAttributeType::find("repeat");
    return atype;
}

int VRepeatAttr::step() const
{
    if(node_ptr node=parent_->node())
    {
        const Repeat& r=node->repeat();
        return r.step();
    }
    return 0;
}

QStringList VRepeatAttr::data(bool /*firstLine*/) const
{
    static VRepeatAttrType* atype=static_cast<VRepeatAttrType*>(type());
    QStringList s;
    if(parent_->node_)
    {
        const Repeat& r=parent_->node_->repeat();
        atype->encode(r,s,subType(),allValues());
    }
    return s;
}

std::string VRepeatAttr::strName() const
{
    if(parent_->node_)
    {
        const Repeat& r=parent_->node_->repeat();
        if(r.empty() == false)
            return r.name();
    }
    return std::string();
}

void VRepeatAttr::scan(VNode* vnode,std::vector<VAttribute*>& vec)
{
    if(vnode->node_)
    {
        const Repeat& r=vnode->node_->repeat();
        if(r.empty() == false)
        {
            std::string t=r.toString();

            VRepeatAttr *a=0;

            if(t.find("date") != std::string::npos)
                a=new VRepeatDateAttr(vnode);
            else if(t.find("integer") != std::string::npos)
                a=new VRepeatIntAttr(vnode);
            else if(t.find("string") != std::string::npos)
                a=new VRepeatStringAttr(vnode);
            else if(t.find("enumerated") != std::string::npos)
                a=new VRepeatEnumAttr(vnode);
            else if(t.find("day") != std::string::npos)
                a=new VRepeatDayAttr(vnode);

            if(a)
                vec.push_back(a);
        }
    }
}

QString VRepeatAttr::allValues() const
{
   return QString();
}

//=====================================================
//
// VRepeatDateAttr
//
//=====================================================

int VRepeatDateAttr::endIndex() const
{
    if(node_ptr node=parent_->node())
    {
        const Repeat& r=node->repeat();
        if(r.step() >0)
        {
            long jStart=ecf_repeat_date_to_julian(r.start());
            long jEnd=ecf_repeat_date_to_julian(r.end());

            int index=(jEnd-jStart)/r.step();
            long val=jStart + index*r.step();
            while(val > jEnd && index >=1)
            {
                index--;
                val=jStart + index*r.step();
            }
            return index;
        }
    }
    return 0;
}

int VRepeatDateAttr::currentIndex() const
{
    if(node_ptr node=parent_->node())
    {
        const Repeat& r=node->repeat();
        int cur=(ecf_repeat_date_to_julian(r.index_or_value()) -
                ecf_repeat_date_to_julian(r.start())) / r.step();
        return cur;
    }
    return 0;
}

std::string VRepeatDateAttr::value(int index) const
{
    std::stringstream ss;
    if(node_ptr node=parent_->node())
    {
        const Repeat& r=node->repeat();
        ss << (ecf_repeat_julian_to_date
          (ecf_repeat_date_to_julian(r.start()) + index * r.step()));
    }

    return ss.str();
}

//=====================================================
//
// VRepeatIntAttr
//
//=====================================================

int VRepeatIntAttr::endIndex() const
{
    if(node_ptr node=parent_->node())
    {
        const Repeat& r=node->repeat();
        if(r.step() >0)
        {
            int index=(r.end() - r.start()) / r.step();
            int val=r.start() + index*r.step();
            while(val > r.end() && index >=1)
            {
                index--;
                val=r.start() + index*r.step();
            }
            return index;
        }
    }
    return 0;
}

int VRepeatIntAttr::currentIndex() const
{
    if(node_ptr node=parent_->node())
    {
        const Repeat& r=node->repeat();
        if(r.step() >0)
            return (r.index_or_value() - r.start())/r.step();
    }
    return 0;
}

std::string VRepeatIntAttr::value(int index) const
{
    std::stringstream ss;
    if(node_ptr node=parent_->node())
    {
        const Repeat& r=node->repeat();
        ss << r.start() + index*r.step();
    }
    return ss.str();
}

//=====================================================
//
// VRepeatDayAttr
//
//=====================================================

std::string VRepeatDayAttr::value(int /*index*/) const
{
    std::stringstream ss;
    if(node_ptr node=parent_->node())
    {
        const Repeat& r=node->repeat();
        ss << r.step();
    }
    return ss.str();
}

//=====================================================
//
// VRepeatEnumAttr
//
//=====================================================

std::string VRepeatEnumAttr::value(int index) const
{
    if(node_ptr node=parent_->node())
    {
        const Repeat& r=node->repeat();
        return r.value_as_string(index);
    }
    return std::string();
}

int VRepeatEnumAttr::endIndex() const
{
    if(node_ptr node=parent_->node())
    {
        const Repeat& r=node->repeat();
        return r.end();
    }
    return 0;
}

int VRepeatEnumAttr::currentIndex() const
{
    if(node_ptr node=parent_->node())
    {
        const Repeat& r=node->repeat();
        return r.index_or_value();
    }
    return 0;
}

QString VRepeatEnumAttr::allValues() const
{
    QString vals;

    if(node_ptr node=parent_->node())
    {
        const Repeat& r=node->repeat();
        int start=r.start();
        int end=r.end();

        if(end <= start)
        {
            return QString();
        }

        if(end-start >1)
        {
            for(int i=start; i <= end; i++)
            {
                if(!vals.isEmpty()) vals+=" ";
                vals+="\"" + QString::fromStdString(r.value_as_string(i)) + "\"";
            }
            return vals;
        }

    }
    return vals;
}


//=====================================================
//
// VRepeatStringAttr
//
//=====================================================

std::string VRepeatStringAttr::value(int index) const
{
    if(node_ptr node=parent_->node())
    {
        const Repeat& r=node->repeat();
        return r.value_as_string(index);
    }
    return std::string();
}

int VRepeatStringAttr::endIndex() const
{
    if(node_ptr node=parent_->node())
    {
        const Repeat& r=node->repeat();
        return r.end();
    }
    return 0;
}

int VRepeatStringAttr::currentIndex() const
{
    if(node_ptr node=parent_->node())
    {
        const Repeat& r=node->repeat();
        return r.index_or_value();
    }
    return 0;
}

QString VRepeatStringAttr::allValues() const
{
    QString vals;

    if(node_ptr node=parent_->node())
    {
        const Repeat& r=node->repeat();
        int start=r.start();
        int end=r.end();

        if(end <= start)
        {
            return QString();
        }

        if(end-start >1)
        {
            for(int i=start; i <= end; i++)
            {
                if(!vals.isEmpty()) vals+=" ";
                vals+="\"" + QString::fromStdString(r.value_as_string(i)) + "\"";
            }
            return vals;
        }

    }
    return vals;
}
