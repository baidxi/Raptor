/**
 *
 * Copyright (C) 2024 凉州刺史. All rights reserved.
 *
 * This file is part of Raptor.
 *
 * $RAPTOR_BEGIN_LICENSE$
 *
 * GNU General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl.html.
 *
 * $RAPTOR_END_LICENSE$
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#include "RaptorShareViewModel.h"

RaptorShareViewModel::RaptorShareViewModel(QObject *qParent) : QAbstractTableModel(qParent)
{
    invokeInstanceInit();
}

QVariant RaptorShareViewModel::headerData(int qSection,
                                          Qt::Orientation qOrientation,
                                          int qRole) const
{
    if (qOrientation != Qt::Horizontal)
    {
        return QVariant();
    }

    switch (qRole)
    {
        case Qt::DisplayRole:
            if (qSection > 0 && qSection <= _Headers.length())
            {
                return _Headers[qSection - 1];
            }
            return QVariant();
        case Qt::TextAlignmentRole:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        default:
            return QVariant();
    }
}

int RaptorShareViewModel::rowCount(const QModelIndex &qIndex) const
{
    if (qIndex.isValid())
    {
        return 0;
    }

    return _Items.length();
}

int RaptorShareViewModel::columnCount(const QModelIndex &qIndex) const
{
    if (qIndex.isValid())
    {
        return 0;
    }

    return _ColumnCount;
}

QVariant RaptorShareViewModel::data(const QModelIndex &qIndex, int qRole) const
{
    if (!qIndex.isValid())
    {
        return QVariant();
    }

    const auto item = _Items[qIndex.row()];
    switch (qRole)
    {
        case Qt::UserRole:
            return QVariant::fromValue<RaptorShareItem>(item);
        case Qt::DisplayRole:
        {
            switch (qIndex.column())
            {
                case 1:
                    return item._Name;
                case 2:
                    return item._Expired.split(' ')[0];
                default:
                    return QVariant();
            }
        }
        case Qt::TextAlignmentRole:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        default:
            return QVariant();
    }
}

bool RaptorShareViewModel::setData(const QModelIndex &qIndex, const QVariant &qVariant, int qRole)
{
    if (!qIndex.isValid())
    {
        return QAbstractTableModel::setData(qIndex, qVariant, qRole);
    }

    if (qRole != Qt::EditRole)
    {
        return QAbstractTableModel::setData(qIndex, qVariant, qRole);
    }

    if (!qVariant.canConvert<RaptorShareItem>())
    {
        return QAbstractTableModel::setData(qIndex, qVariant, qRole);
    }

    const auto item = qVariant.value<RaptorShareItem>();
    _Items.replace(qIndex.row(), item);
    Q_EMIT dataChanged(qIndex, qIndex);
    return QAbstractTableModel::setData(qIndex, qVariant, qRole);
}

bool RaptorShareViewModel::removeRow(int qRow, const QModelIndex &qIndex)
{
    if (qRow < 0 || qRow > _Items.length())
    {
        return false;
    }

    beginRemoveRows(qIndex, qRow, qRow);
    _Items.removeAt(qRow);
    endRemoveRows();
    return true;
}

void RaptorShareViewModel::sort(int qColumn, Qt::SortOrder qOrder)
{
    if (qColumn == 0)
    {
        return;
    }

    beginResetModel();
    switch (qColumn)
    {
        case 1:
        {
            if (qOrder == Qt::AscendingOrder)
            {
                std::sort(_Items.begin(), _Items.end(), std::bind(invokeItemsByNameAscSort, this, std::placeholders::_1, std::placeholders::_2));
            } else
            {
                std::sort(_Items.begin(), _Items.end(), std::bind(invokeItemsByNameDescSort, this, std::placeholders::_1, std::placeholders::_2));
            }
            break;
        }
        case 2:
        {
            if (qOrder == Qt::AscendingOrder)
            {
                std::sort(_Items.begin(), _Items.end(), std::bind(invokeItemsByExpiredAscSort, this, std::placeholders::_1, std::placeholders::_2));
            } else
            {
                std::sort(_Items.begin(), _Items.end(), std::bind(invokeItemsByExpiredDescSort, this, std::placeholders::_1, std::placeholders::_2));
            }
            break;
        }
        default:
            break;
    }

    endResetModel();
}

void RaptorShareViewModel::invokeHeaderSet(const QVector<QString> &qHeader)
{
    _Headers = qHeader;
}

void RaptorShareViewModel::invokeColumnCountSet(const quint16 &qCount)
{
    _ColumnCount = qCount;
}

void RaptorShareViewModel::invokeItemAppend(const RaptorShareItem &item)
{
    beginInsertRows(QModelIndex(), _Items.length(), _Items.length());
    _Items << item;
    endInsertRows();
}

void RaptorShareViewModel::invokeItemsAppend(const QVector<RaptorShareItem> &items)
{
    if (items.isEmpty())
    {
        return;
    }

    beginInsertRows(QModelIndex(), _Items.length(), _Items.length() + items.length() - 1);
    _Items << items;
    endInsertRows();
}

void RaptorShareViewModel::invokeItemsClear()
{
    beginResetModel();
    _Items.clear();
    endResetModel();
}

void RaptorShareViewModel::invokeInstanceInit()
{
    qCollator = QCollator(QLocale::Chinese);
    qCollator.setNumericMode(false);
    qCollator.setIgnorePunctuation(true);
}

bool RaptorShareViewModel::invokeItemsByNameAscSort(const RaptorShareItem &item, const RaptorShareItem &iten) const
{
    return qCollator.compare(item._Name, iten._Name) < 0;
}

bool RaptorShareViewModel::invokeItemsByNameDescSort(const RaptorShareItem &item, const RaptorShareItem &iten) const
{
    return qCollator.compare(item._Name, iten._Name) > 0;
}

bool RaptorShareViewModel::invokeItemsByExpiredAscSort(const RaptorShareItem &item, const RaptorShareItem &iten) const
{
    return qCollator.compare(item._Expired, iten._Expired) < 0;
}

bool RaptorShareViewModel::invokeItemsByExpiredDescSort(const RaptorShareItem &item, const RaptorShareItem &iten) const
{
    return qCollator.compare(item._Expired, iten._Expired) > 0;
}
