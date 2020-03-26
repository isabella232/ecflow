//============================================================================
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#ifndef TRIGGERGRAPHMODEL_HPP
#define TRIGGERGRAPHMODEL_HPP

#include <QAbstractItemModel>
#include <QColor>

#include "VInfo.hpp"
#include "TriggerCollector.hpp"

#include "Aspect.hpp"

class NodeQueryResult;

class TriggerGraphModel : public QAbstractItemModel
{
public:
    enum Mode {TriggerMode,TriggeredMode,NodeMode};

    explicit TriggerGraphModel(Mode mode,QObject *parent=nullptr);
    ~TriggerGraphModel() override;

    //The custom roles must have the same numerical value as in AbstractNodeModel.hpp because the
    //core delegate was written to only handle the custom roles it defines!
    enum CustomItemRole {FilterRole = Qt::UserRole+1, IconRole = Qt::UserRole+2,
                         NodeTypeRole = Qt::UserRole + 13,
                         NodeTypeForegroundRole = Qt::UserRole + 14,
                         NodePointerRole = Qt::UserRole + 17};

    int columnCount (const QModelIndex& parent = QModelIndex() ) const override;
    int rowCount (const QModelIndex& parent = QModelIndex() ) const override;

    Qt::ItemFlags flags ( const QModelIndex & index) const override;
    QVariant data (const QModelIndex& , int role = Qt::DisplayRole ) const override;
    QVariant headerData(int,Qt::Orientation,int role = Qt::DisplayRole ) const override;

    QModelIndex index (int, int, const QModelIndex& parent = QModelIndex() ) const override;
    QModelIndex parent (const QModelIndex & ) const override;

    //TriggerTableCollector* triggerCollector() const {return tc_;}
    void setTriggerCollectors(TriggerTableCollector *triggerTc,
                              TriggerTableCollector *triggeredTv);

    void setNode(VInfo_ptr);

    void clearData();
    bool hasData() const;
    bool hasTrigger() const;

    void beginUpdate();
    void endUpdate();

    TriggerTableItem* indexToItem(const QModelIndex&) const;
    VInfo_ptr nodeInfo(const QModelIndex&);
    QModelIndex itemToIndex(TriggerTableItem*);

    void nodeChanged(const VNode* node, const std::vector<ecf::Aspect::Type>&);

protected:
    TriggerTableItem* node_ {nullptr};
    TriggerTableCollector* triggerTc_ {nullptr};
    TriggerTableCollector* triggeredTc_ {nullptr};

    Mode mode_;
};

#endif // TRIGGERGRAPHMODEL_HPP
