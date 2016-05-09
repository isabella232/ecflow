#ifndef http_H
#define http_H
//=============================================================================================
// Name        : 
// Author      : 
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
//=============================================================================================


#include <stdio.h>

#ifndef server_H
#include "server.h"
#endif

#ifndef interface_H
#include "interface.h"
#endif


class http : public interface , public server  {
public:

// -- Exceptions
	// None

// -- Contructors

	http(int,int,char**);

// -- Destructor

	~http(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods
	// None

// -- Overridden methods

	// From server

	virtual void serve(int);


	// From interface

	virtual void clear() {}
	virtual void message(const char* p) { fprintf(stderr,"-> %s\n",p); }
	virtual void watch(Boolean) {}

	virtual void add_host(const std::string&) {}
	virtual void remove_host(const std::string&) {}
	virtual void rename_host(const std::string&, const std::string&) {}

	virtual void login(const char*) {}
	virtual void logout(const char*) {}

	virtual Widget top_shell() { return 0; }
	virtual Widget trees()  { return 0; }
	virtual Widget windows()  { return 0; }

	virtual void error(const char*) {}


// -- Class members
	// None

// -- Class methods
	// None

	// Uncomment for persistent, remove otherwise
	// static os_typespec* get_os_typespec();

protected:

// -- Members
	// None

// -- Methods
	
	// void print(ostream&) const; // Change to virtual if base class	

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

private:

// No copy allowed

	http(const http&);
	http& operator=(const http&);

// -- Members
	// None

// -- Methods
	// None

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	//friend ostream& operator<<(ostream& s,const http& p)
	//	{ p.print(s); return s; }

};

inline void destroy(http**) {}

// If persistent, uncomment, otherwise remove
//#ifdef _ODI_OSSG_
//OS_MARK_SCHEMA_TYPE(http);
//#endif

#endif
