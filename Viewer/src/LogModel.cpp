//============================================================================
// Copyright 2014 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#include "LogModel.hpp"

#include <QDebug>
#include <QStringList>

LogModelLine::LogModelLine(QString s) : type_(NoType)
{
	QString s1=s.section("]",0,0);
	date_=s1.section("[",1);
	entry_=s.section("]",1);

	QString t=s1.section("[",0);

	if(t.contains("MSG:"))
	{
		type_=MessageType;
	}
	else if(t.contains("LOG:"))
	{
		type_=LogType;
	}
	else if(t.contains("ERR:"))
	{
		type_=ErrorType;
	}
}


LogModel::LogModel(QObject *parent) :
          QAbstractItemModel(parent)
{

}

LogModel::~LogModel()
{
}

void LogModel::setData(const std::string& data)
{
	beginResetModel();

	data_.clear();

	QString in=QString::fromStdString(data);
	Q_FOREACH(QString s,in.split("\n"))
	{
		data_ << LogModelLine(s);
	}

	endResetModel();
}

void LogModel::clearData()
{
	beginResetModel();
	data_.clear();
	endResetModel();
}

bool LogModel::hasData() const
{
	return !data_.empty();
}

int LogModel::columnCount( const QModelIndex& /*parent */ ) const
{
   	 return 3;
}

int LogModel::rowCount( const QModelIndex& parent) const
{
	if(!hasData())
		return 0;

	//Parent is the root:
	if(!parent.isValid())
	{
		return data_.size();
	}

	return 0;
}

Qt::ItemFlags LogModel::flags ( const QModelIndex & index) const
{
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant LogModel::data( const QModelIndex& index, int role ) const
{
	if(!index.isValid() || !hasData())
    {
		return QVariant();
	}
	int row=index.row();
	if(row < 0 || row >= data_.size())
		return QVariant();

	if(role == Qt::DisplayRole)
	{
		switch(index.column())
		{
		case 0:
			{
				switch(data_.at(row).type_)
				{
				case LogModelLine::MessageType:
					return "MSG";
				case LogModelLine::LogType:
					return "LOG";
				case LogModelLine::ErrorType:
					return "ERR";
				default:
					QVariant();
				}
			}
			break;
		case 1:
			return data_.at(row).date_;
			break;
		case 2:
			return data_.at(row).entry_;
			break;
		default:
			break;
		}
	}

	return QVariant();
}

QVariant LogModel::headerData( const int section, const Qt::Orientation orient , const int role ) const
{
	if ( orient != Qt::Horizontal || (role != Qt::DisplayRole &&  role != Qt::ToolTipRole))
      		  return QAbstractItemModel::headerData( section, orient, role );

   	if(role == Qt::DisplayRole)
   	{
   		switch ( section )
   		{
   		case 0: return tr("Type");
   		case 1: return tr("Date");
   		case 2: return tr("Entry");
   		default: return QVariant();
   		}
   	}
   	else if(role== Qt::ToolTipRole)
   	{
   		switch ( section )
   		{
   		case 0: return tr("Type");
   		case 1: return tr("Date");
   		case 2: return tr("Entry");
   		default: return QVariant();
   		}
   	}
    return QVariant();
}

QModelIndex LogModel::index( int row, int column, const QModelIndex & parent ) const
{
	if(!hasData() || row < 0 || column < 0)
	{
		return QModelIndex();
	}

	//When parent is the root this index refers to a node or server
	if(!parent.isValid())
	{
		return createIndex(row,column);
	}

	return QModelIndex();

}

QModelIndex LogModel::parent(const QModelIndex &child) const
{
	return QModelIndex();
}
