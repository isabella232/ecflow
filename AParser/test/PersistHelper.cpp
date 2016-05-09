//============================================================================
// Name        :
// Author      : Avi
// Revision    : $Revision$ 
//
// Copyright 2009-2016 ECMWF. 
// This software is licensed under the terms of the Apache Licence version 2.0 
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
// In applying this licence, ECMWF does not waive the privileges and immunities 
// granted to it by virtue of its status as an intergovernmental organisation 
// nor does it submit to any jurisdiction. 
//
// Description :
//============================================================================
#include <boost/archive/tmpdir.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include <iostream>
#include <fstream>

#include "PersistHelper.hpp"
#include "DefsStructureParser.hpp"
#include "PrintStyle.hpp"
#include "Defs.hpp"
#include "Ecf.hpp"

namespace fs = boost::filesystem;
using namespace std;
using namespace ecf;

bool PersistHelper::test_persist_and_reload( const Defs& theInMemoryDefs, PrintStyle::Type_t file_type_on_disk)
{
 	// Write parsed file to disk, and reload, then compare defs, they should be the same
	errorMsg_.clear();
	file_size_ = 0;

#ifdef DEBUG
	std::string tmpFilename = "tmp_d.def";
#else
	std::string tmpFilename = "tmp.def";
#endif
	{
	   // The file MUST be written in the *SAME* form that it was read
	   // Otherwise they will not compare:
	   PrintStyle style(file_type_on_disk);
		std::ofstream ofs( tmpFilename.c_str() );
		ofs << theInMemoryDefs;
	}

	// Reload the file we just persisted and compare with in memory defs
	Defs savedDef;
	return reload_from_defs_file(theInMemoryDefs,savedDef,tmpFilename);
}


bool PersistHelper::test_checkpt_and_reload( const Defs& theInMemoryDefs, bool do_compare, ecf::Archive::Type at)
{
 	errorMsg_.clear();
 	file_size_ = 0;

   // Save in memory defs as a check pt file, then restore and compare
 	Defs reloaded_defs;
 	return reload_from_checkpt_file(theInMemoryDefs,reloaded_defs,do_compare,at);
}


bool PersistHelper::test_state_persist_and_reload_with_checkpt(const Defs& theInMemoryDefs )
{
   // Write Defs to disk, and reload, then compare defs relaoded checkpt file, they should be the same
   errorMsg_.clear();
   file_size_ = 0;

#ifdef DEBUG
   std::string tmpFilename = "tmp_d.def";
#else
   std::string tmpFilename = "tmp.def";
#endif
   {
      // The file MUST be written in the *SAME* form that it was read
      // Otherwise they will not compare:
      PrintStyle style(PrintStyle::MIGRATE); // will save edit history
      std::ofstream ofs( tmpFilename.c_str() );
      ofs << theInMemoryDefs;
   }

   // Reload the file we just persisted and compare with in memory defs
   Defs reloaded_defs;
   if (!reload_from_defs_file(theInMemoryDefs,reloaded_defs,tmpFilename)) {
      return false;
   }

   // Save in memory defs as a check pt file, then restore and compare
   Defs reloaded_checkPt_defs;
   if (!reload_from_checkpt_file(theInMemoryDefs,reloaded_checkPt_defs,true,ecf::Archive::default_archive())) {
      return false;
   }

   // Make sure reloading def's file with state is same as the checkpt file
   Ecf::set_debug_equality(true);
   bool match = reloaded_defs == reloaded_checkPt_defs;
   Ecf::set_debug_equality(false);

   if (!match) {
      std::stringstream ss;
      ss << "\nPersistHelper::test_state_persist_and_reload_with_checkpt\n";
      ss << "In reloaded_defs_file and reloaded_checkPt_defs don't match\n";
      ss << "+++++++++++++ in memory defs  ++++++++++++++++++++++++++++\n";
      PrintStyle style(PrintStyle::MIGRATE); // will save edit history
      ss << theInMemoryDefs;
      ss << "+++++++++++++ reloaded_defs  ++++++++++++++++++++++++++++\n";
      ss << reloaded_defs;
      ss << "++++++++++++++ reloaded_checkPt_defs  ++++++++++++++++++++++++++++\n";
      ss << reloaded_checkPt_defs;
      errorMsg_ += ss.str();
   }
   else {
      if (compare_edit_history_ && !reloaded_defs.compare_edit_history(reloaded_checkPt_defs)) {
         std::stringstream ss;
         ss << "\nPersistHelper::test_state_persist_and_reload_with_checkpt  compare_edit_history_\n";
         ss << "In reloaded_defs_file and reloaded_checkPt_defs edit history don't match\n";
         ss << "+++++++++++++ in memory defs  ++++++++++++++++++++++++++++\n";
         PrintStyle style(PrintStyle::MIGRATE); // will save edit history
         ss << theInMemoryDefs;
         ss << "+++++++++++++ reloaded_defs  ++++++++++++++++++++++++++++\n";
         ss << reloaded_defs;
         ss << "++++++++++++++ reloaded_checkPt_defs  ++++++++++++++++++++++++++++\n";
         ss << reloaded_checkPt_defs;
         errorMsg_ += ss.str();
      }
   }
   return errorMsg_.empty();
}


bool PersistHelper::reload_from_defs_file(const Defs& theInMemoryDefs, Defs& reloaded_defs, const std::string& tmpFilename )
{
   std::string warningMsg;
   DefsStructureParser defsParser( &reloaded_defs, tmpFilename );
   bool theParse = defsParser.doParse(errorMsg_,warningMsg);
   if (!theParse) {
      std::stringstream ss;
      ss << "RE-PARSE failed for " << tmpFilename << "\n";
      errorMsg_ += ss.str();
      return false;
   }

   // Make sure the file we just parsed match's the one we persisted
   Ecf::set_debug_equality(true);
   bool match = reloaded_defs == theInMemoryDefs;
   Ecf::set_debug_equality(false);

   if (!match) {
      std::stringstream ss;
      ss << "\nPersistHelper::reload_from_defs_file\n";
      ss << "In memory and reloaded def's don't match\n";
      ss << "+++++++++++++ Saved/reloaded_defs  ++++++++++++++++++++++++++++\n";
      PrintStyle style(PrintStyle::STATE);
      ss << reloaded_defs;
      ss << "++++++++++++++ In memory def ++++++++++++++++++++++++++++\n";
      ss << theInMemoryDefs;
      errorMsg_ += ss.str();
   }
   else {
      if (compare_edit_history_ && !reloaded_defs.compare_edit_history(theInMemoryDefs)) {
         std::stringstream ss;
         ss << "\nPersistHelper::reload_from_defs_file compare_edit_history_\n";
         ss << "In memory and reloaded def's don't match\n";
         ss << "+++++++++++++ Saved/reloaded_defs  ++++++++++++++++++++++++++++\n";
         PrintStyle style(PrintStyle::MIGRATE);
         ss << reloaded_defs;
         ss << "++++++++++++++ In memory def ++++++++++++++++++++++++++++\n";
         ss << theInMemoryDefs;
         errorMsg_ += ss.str();
      }
   }

   file_size_ = fs::file_size(tmpFilename);
   std::remove(tmpFilename.c_str());
   return errorMsg_.empty();
}


bool PersistHelper::reload_from_checkpt_file(const Defs& theInMemoryDefs,
                                             Defs& reloaded_defs,
                                             bool do_compare ,
                                             ecf::Archive::Type at)
{
   // make sure edit history is saved
#ifdef DEBUG
   std::string tmpCheckPt_file = "tmp.check_debug";
#else
   std::string tmpCheckPt_file = "tmp.check";
#endif
   theInMemoryDefs.save_as_checkpt(tmpCheckPt_file,at);

   try  {
      // Parse the file we just persisted and load the defs file into memory.
      reloaded_defs.restore_from_checkpt(tmpCheckPt_file,at);

      if (do_compare ) {
         // Make sure the checkpoint file file we just parsed match's the one we persisted
         bool match = reloaded_defs == theInMemoryDefs;
         if (!match) {
            std::stringstream ss;
            ss << "\nPersistHelper::reload_from_checkpt_file\n";
            ss << "In memory and reloaded def's don't match\n";
            ss << "+++++++++++++ Saved/reloaded check pt file ++++++++++++++++++++++++++++\n";
            PrintStyle style(PrintStyle::STATE);
            ss << reloaded_defs;
            ss << "++++++++++++++ In memory def ++++++++++++++++++++++++++++\n";
            ss << theInMemoryDefs;
            errorMsg_ += ss.str();
         }
         else {
            if (compare_edit_history_ && !reloaded_defs.compare_edit_history(theInMemoryDefs)) {
               std::stringstream ss;
               ss << "\nPersistHelper::reload_from_checkpt_file  compare_edit_history_\n";
               ss << "In reloaded_defs_file and reloaded_checkPt_defs edit history don't match\n";
               ss << "+++++++++++++ Saved/reloaded check pt file ++++++++++++++++++++++++++++\n";
               PrintStyle style(PrintStyle::MIGRATE);
               ss << reloaded_defs;
               ss << "++++++++++++++ theInMemoryDefs  ++++++++++++++++++++++++++++\n";
               ss << theInMemoryDefs;
               errorMsg_ += ss.str();
            }
         }
      }
   }
   catch (std::exception& e) {
      errorMsg_ =  "PersistHelper::reload_from_checkpt_file: " + string(e.what());
   }

   file_size_ = fs::file_size(tmpCheckPt_file);
   std::remove(tmpCheckPt_file.c_str());

   return errorMsg_.empty();
}
