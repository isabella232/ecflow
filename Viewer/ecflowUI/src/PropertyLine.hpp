//============================================================================
// Copyright 2009-2018 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#ifndef PROPERTYLINE_INC_
#define PROPERTYLINE_INC_

#include <string>

#include <QFont>
#include <QObject>
#include <QSpinBox>
#include <QVariant>

class  QComboBox;
class  QCheckBox;
class  QLabel;
class  QLineEdit;
class  QSpinBox;
class  QToolButton;
class  QWidget;
class  ComboMulti;

#include "VProperty.hpp"

class PropertyLine;

class FontSizeSpin : public QSpinBox
{
public:
	FontSizeSpin(QWidget* parent=nullptr);
	void setFamily(QString);

protected:
	QString textFromValue(int value) const override;

	QList<int> vals_;

};



//-------------------------------------
// Factory
//------------------------------------

class PropertyLineFactory
{
public:
	explicit PropertyLineFactory(VProperty::GuiType);
	virtual ~PropertyLineFactory();

	virtual PropertyLine* make(VProperty* p,bool,QWidget* w) = 0;
	static PropertyLine* create(VProperty* p,bool,QWidget* w);

private:
	explicit PropertyLineFactory(const PropertyLineFactory&) = delete;
	PropertyLineFactory& operator=(const PropertyLineFactory&) = delete;

};

template<class T>
class PropertyLineMaker : public PropertyLineFactory
{
	PropertyLine* make(VProperty* p,bool addLabel,QWidget* w) override { return new T(p,addLabel,w); }
public:
	explicit PropertyLineMaker(VProperty::GuiType t) : PropertyLineFactory(t) {}
};


//-------------------------------------
// Abstract property line editor
//------------------------------------

class PropertyLine: public QObject
{
 Q_OBJECT

public:
	PropertyLine(VProperty*,bool addLabel,QWidget* parent=nullptr);
	~PropertyLine() override;

    QLabel* label() {return label_;}
    QLabel* suffixLabel() {return suffixLabel_;}
	virtual QWidget* item()=0;
	virtual QWidget* button()=0;
    QToolButton* defaultTb() {return defaultTb_;}
    QToolButton* masterTb() {return masterTb_;}
	VProperty* property() const {return prop_;}
	VProperty* guiProperty() const {return guiProp_;}
    VProperty* ruleProperty();
    void addRuleLine(PropertyLine*);
    virtual bool canExpand() const {return false;}

	void addHelper(PropertyLine*);

	void init();
	virtual bool applyChange()=0;
	virtual QVariant currentValue()=0;

public Q_SLOTS:
	virtual void slotReset(QVariant)=0;
    virtual void slotReset(VProperty*,QVariant);
    virtual void slotEnabled(QVariant);

protected Q_SLOTS:
	void slotResetToDefault(bool);
	void slotMaster(bool b);
	void checkState();
    void slotRule();

Q_SIGNALS:
	void changed(QVariant);
	void masterChanged(bool);
	void changed();

protected:
	virtual void setEnabledEditable(bool)=0;
	bool applyMaster();
	void valueChanged();

	VProperty* prop_;
	VProperty* guiProp_;
	QLabel* label_;
	QLabel* suffixLabel_;
	QToolButton* defaultTb_;
	QToolButton* masterTb_;
	bool enabled_;
	QVariant oriVal_;
	bool doNotEmitChange_;
	QMap<QString,PropertyLine*> helpers_;
    PropertyLine* ruleLine_;
    QString ruleValue_;
};

//-------------------------------------
// String editor
//------------------------------------

class StringPropertyLine : public PropertyLine
{
	Q_OBJECT

public:
	StringPropertyLine(VProperty* vProp,bool addLabel,QWidget * parent=nullptr);
	QWidget* item() override;
	QWidget* button() override;
	bool applyChange() override;
    QVariant currentValue() override;
    bool canExpand() const override {return true;}

public Q_SLOTS:
	void slotEdited(QString);
	void slotReset(QVariant) override;

protected:
	void setEnabledEditable(bool) override;

private:
	QLineEdit* le_;
};

//-------------------------------------
// Colour editor
//------------------------------------

class ColourPropertyLine : public PropertyLine
{
Q_OBJECT

public:
	ColourPropertyLine(VProperty* vProp,bool addLabel,QWidget * parent=nullptr);
	QWidget* item() override;
	QWidget* button() override;
	bool applyChange() override;
	QVariant currentValue() override;

private Q_SLOTS:
	void slotEdit(bool);
	void slotReset(QVariant) override;

protected:
	void setEnabledEditable(bool) override;

private:
	QToolButton* cb_;
	QColor currentCol_;
	QString styleSheet_;
};

//-------------------------------------
// Font editor
//------------------------------------

class FontPropertyLine : public PropertyLine
{
Q_OBJECT

public:
	FontPropertyLine(VProperty* vProp,bool addLabel,QWidget * parent=nullptr);
	QWidget* item() override;
	QWidget* button() override;
	bool applyChange() override;
	QVariant currentValue() override;

private Q_SLOTS:
	void slotEdit(bool);
	void slotReset(QVariant) override;
	void slotFamilyChanged(int);
	void slotSizeChanged(int);

protected:
	void setEnabledEditable(bool) override;

private:
	QWidget* holderW_;
	QComboBox* familyCb_;
	QSpinBox* sizeSpin_;
	QLabel* lName_;
	QToolButton *tbEdit_;
	QFont font_;
};

//-------------------------------------
// Int editor
//------------------------------------

class IntPropertyLine : public PropertyLine
{
	Q_OBJECT

public:
	IntPropertyLine(VProperty* vProp,bool addLabel,QWidget * parent=nullptr);
	QWidget* item() override;
	QWidget* button() override;
	bool applyChange() override;
	QVariant currentValue() override;

public Q_SLOTS:
	void slotEdited(QString);
	void slotReset(QVariant) override;

protected:
	void setEnabledEditable(bool) override;

private:
	QLineEdit* le_;
};

//-------------------------------------
// Boolean editor
//------------------------------------

class BoolPropertyLine : public PropertyLine
{
	Q_OBJECT

public:
	BoolPropertyLine(VProperty* vProp,bool addLabel,QWidget * parent=nullptr);
	QWidget* item() override;
	QWidget* button() override;
	bool applyChange() override;
	QVariant currentValue() override;

public Q_SLOTS:
	void slotStateChanged(int);
	void slotReset(QVariant) override;

protected:
	void setEnabledEditable(bool) override;

private:
	QCheckBox* cb_;
};

//-------------------------------------
// Combo box editor
//------------------------------------

class ComboPropertyLine : public PropertyLine
{
	Q_OBJECT

public:
	ComboPropertyLine(VProperty* vProp,bool addLabel,QWidget * parent=nullptr);
	QWidget* item() override;
	QWidget* button() override;
	bool applyChange() override;
	QVariant currentValue() override;

public Q_SLOTS:
	void slotCurrentChanged(int);
	void slotReset(QVariant) override;

protected:
	void setEnabledEditable(bool) override;

protected:
	QComboBox* cb_;
};


//-------------------------------------
// Combo box editor
//------------------------------------

class ComboMultiPropertyLine : public PropertyLine
{
	Q_OBJECT

public:
	ComboMultiPropertyLine(VProperty* vProp,bool addLabel,QWidget * parent=nullptr);
	QWidget* item() override;
	QWidget* button() override;
	bool applyChange() override;
	QVariant currentValue() override;
	bool canExpand() const override {return true;}

public Q_SLOTS:
    void slotSelectionChanged();
	void slotReset(QVariant);

protected:
	void setEnabledEditable(bool) override;

protected:
	ComboMulti* cb_;
};

//-------------------------------------
// Combo box editor
//------------------------------------

class SoundComboPropertyLine : public ComboPropertyLine
{
	Q_OBJECT

public:
	SoundComboPropertyLine(VProperty* vProp,bool addLabel,QWidget * parent=nullptr);
	QWidget* item() override;
	QWidget* button() override;

public Q_SLOTS:
	void slotPlay(bool);

protected:
	void setEnabledEditable(bool) override;

private:
	QToolButton* playTb_;

};



#endif

