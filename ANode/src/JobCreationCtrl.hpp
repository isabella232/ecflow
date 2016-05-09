#ifndef JOBS_GEN_CTRL_HPP_
#define JOBS_GEN_CTRL_HPP_
//============================================================================
// Name        :
// Author      : Avi
// Revision    : $Revision: #4 $ 
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

#include <string>
#include <vector>

#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>

#include "NodeFwd.hpp"

// Used as a utility class for testing Job creation
// Collates data during the node tree traversal
// Note: For testing purposes we do not always want to create jobs
class JobCreationCtrl : public boost::enable_shared_from_this<JobCreationCtrl>, private boost::noncopyable {
public:
	JobCreationCtrl() {}

	void set_node_path( const std::string& absNodePath ) { absNodePath_ = absNodePath;}
	const std::string& node_path() const { return absNodePath_;}

	void generate_temp_dir();
	void set_dir_for_job_creation( const std::string& tempDirForJobGeneration ) { tempDirForJobGeneration_ = tempDirForJobGeneration; }
	const std::string& dir_for_job_creation() { return tempDirForJobGeneration_;}

	std::string& error_msg() { return errorMsg_;}
	const std::string& get_error_msg() const { return errorMsg_;}

	void push_back_failing_submittable(submittable_ptr t) { fail_submittables_.push_back(t); }
	const std::vector<weak_submittable_ptr>& fail_submittables() const { return fail_submittables_;}

private:
	std::string absNodePath_;
   std::string tempDirForJobGeneration_;
 	std::string errorMsg_;
 	std::vector<weak_submittable_ptr> fail_submittables_;
};
#endif
