//============================================================================
// Copyright 2014 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#ifndef CHANGENOTIFYDIALOG_HPP_
#define CHNAGENOTIFYDIALOG_HPP_

#include <QDialog>
#include <QLinearGradient>
#include <QWidget>

#include <map>

#include "ui_ChangeNotifyDialog.h"
#include "ui_ChangeNotifyDialogWidget.h"

class ChangeNotify;
class VProperty;

class QLabel;

class ChangeNotifyDialogWidget : public QWidget, protected Ui::ChangeNotifyDialogWidget
{
public:
	explicit ChangeNotifyDialogWidget(QWidget* parent=0);
	~ChangeNotifyDialogWidget() {};

	void init(ChangeNotify*);
	void update(ChangeNotify*);
};


class ChangeNotifyDialog : public QDialog, protected Ui::ChangeNotifyDialog
{
Q_OBJECT

public:
	explicit ChangeNotifyDialog(QWidget *parent=0);
	~ChangeNotifyDialog();

	void addTab(ChangeNotify*);
	void setCurrentTab(ChangeNotify*);
	void setEnabledTab(ChangeNotify*,bool b);
	void updateSettings(ChangeNotify*);

public Q_SLOTS:
	void on_tab__currentChanged(int);
	void on_closePb__clicked(bool b);
	void on_clearPb__clicked(bool b);

protected:
	ChangeNotify* tabToNtf(int tabIdx);
	int ntfToTab(ChangeNotify*);
	void decorateTabs();
	void decorateTab(int,VProperty*);
	void updateStyleSheet(VProperty *currentProp);
	void closeEvent(QCloseEvent*);
	void writeSettings();
	void readSettings();

	std::map<ChangeNotify*,int> ntfToTabMap_;
	std::map<int,ChangeNotify*> tabToNtfMap_;
	std::map<int,ChangeNotifyDialogWidget*> tabWidgets_;
	bool ignoreCurrentChange_;
	QLinearGradient grad_;
};

#endif
