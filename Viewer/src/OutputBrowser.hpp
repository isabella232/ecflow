//============================================================================
// Copyright 2014 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================
#ifndef VIEWER_SRC_OUTPUTBROWSER_HPP_
#define VIEWER_SRC_OUTPUTBROWSER_HPP_

#include <QStackedWidget>

class Highlighter;
class TextEditSearchLine;
class PlainTextEdit;
class PlainTextSearchInterface;
class TextPagerWidget;
class VProperty;

class OutputBrowser : public QWidget
{
public:
	OutputBrowser(QWidget* parent);
	~OutputBrowser();

	void clear();
	void loadFile(QString fileName);
	void loadText(QString text);
	void adjustHighlighter(QString fileName);
	void setFontProperty(VProperty* p);
	void updateFont();
	void gotoLine();
	void showSearchLine();
	bool searchOnReload(bool userClickedReload);
	void zoomIn();
	void zoomOut();

private:
	enum IndexType {BasicIndex=0,PagerIndex=1};
	void changeIndex(IndexType indexType);

	QStackedWidget *stacked_;
	PlainTextEdit* textEdit_;
	TextPagerWidget* textPager_;
	TextEditSearchLine* searchLine_;
	Highlighter* jobHighlighter_;
	PlainTextSearchInterface *textEditSearchInterface_;
	//PagerTextSearchInterface *pagerSearchInterface_;

};

#endif /* VIEWER_SRC_OUTPUTBROWSER_HPP_ */