//============================================================================
// Copyright 2014 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#ifndef VIEWER_SRC_ABSTRACTTEXTEDITSEARCHINTERFACE_HPP_
#define VIEWER_SRC_ABSTRACTTEXTEDITSEARCHINTERFACE_HPP_

#include "StringMatchMode.hpp"

#include <QColor>
#include <QTextDocument>

class AbstractTextEditSearchInterface
{
public:
	AbstractTextEditSearchInterface();
	virtual ~AbstractTextEditSearchInterface() {};

	virtual bool findString (QString str, bool highlightAll, QTextDocument::FindFlags findFlags,
			                bool gotoStartOfWord, int iteration,StringMatchMode::Mode matchMode)=0;
	virtual void automaticSearchForKeywords(bool)=0;
	virtual void refreshSearch()=0;
	virtual void clearHighlights()=0;

protected:
	static QColor highlightColour_;
};


#endif /* VIEWER_SRC_ABSTRACTTEXTEDITSEARCHINTERFACE_HPP_ */