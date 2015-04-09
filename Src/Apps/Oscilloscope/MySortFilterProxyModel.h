
#ifndef __MYSORTFILTERPROXYMODEL__
#define __MYSORTFILTERPROXYMODEL__
#include <QSortFilterProxyModel>
#include "qfilesystemmodel.h"
#include <QDateTime>

class MySortFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT
public:
	MySortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
	{
		this->setSortRole(Qt::UserRole);
		this->setFilterRole(Qt::UserRole);
	}

	bool lessThan(const QModelIndex &left, const QModelIndex &right) const
	{
	//	QVariant leftData = sourceModel()->data(left);
	//	QVariant rightData = sourceModel()->data(right);
		QFileSystemModel* pfileModel = (QFileSystemModel*)sourceModel();
		QVariant leftData = pfileModel->lastModified(left);
		QVariant rightData = pfileModel->lastModified(right);
		switch (leftData.type()) {
		case QVariant::Int:
			return leftData.toInt() < rightData.toInt();
		case QVariant::String:
			return leftData.toString() < rightData.toString();
		case QVariant::DateTime:
			return leftData.toDateTime() > rightData.toDateTime();
		default:
			return false;
		}
	}
};
#endif