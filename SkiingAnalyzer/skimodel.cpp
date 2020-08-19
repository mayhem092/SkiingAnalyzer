#include "skimodel.h"

SkiModel::SkiModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QVariant SkiModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    if (orientation != Qt::Horizontal) return QVariant();
    if (section > m_columns.size()) return QVariant();
    return m_columns.at(section);
}

void SkiModel::SetColumns(QVector<QString> &columns)
{
    layoutAboutToBeChanged();

    m_columns.clear();

    m_columns = columns;

    layoutChanged();
}

int SkiModel::rowCount(const QModelIndex &parent) const
{

    if (parent.isValid())
        return 0;

    return m_data.size();
}

int SkiModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_columns.size();
}

QVariant SkiModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    if (index.row() > m_data.size()) return QVariant();
    if (index.column() > m_columns.size()) return QVariant();

    if (role == Qt::DisplayRole) {
        return m_data.at(index.row()).at(index.column());
    }
    return QVariant();
}

void SkiModel::clearData()
{
    layoutAboutToBeChanged();

    m_data.clear();

    layoutChanged();
}

void SkiModel::sort(int column, Qt::SortOrder order)
{
    if (m_sortableColumns.contains(column)) {
        if (order == Qt::AscendingOrder) {
            QVector<QVector<QString>> newData = QVector<QVector<QString>>();

            for (auto row : m_data) {
                if (newData.isEmpty()) newData.append(row);
                else {
                    for (int i = 0; i < newData.size(); ++i) {
                        if (row[column] <= newData[i][column]) {
                            newData.insert(i, row);
                            break;
                        }
                        else if (i == newData.size() - 1) {
                            newData.append(row);
                            break;
                        }
                    }
                }
            }
            m_data = newData;
        }
        else if (order == Qt::DescendingOrder) {
            QVector<QVector<QString>> newData = QVector<QVector<QString>>();

            for (auto row : m_data) {
                if (newData.isEmpty()) newData.append(row);
                else {
                    for (int i = 0; i < newData.size(); ++i) {
                        if (row[column] >= newData[i][column]) {
                            newData.insert(i, row);
                            break;
                        }
                        else if (i == newData.size() - 1) {
                            newData.append(row);
                            break;
                        }
                    }
                }
            }
            m_data = newData;
        }
    }
}

QModelIndex SkiModel::index(int row, int column, const QModelIndex &) const
{
    return createIndex(row, column);
}

QModelIndex SkiModel::parent(const QModelIndex &) const
{
    return createIndex(-1, -1);
}

void SkiModel::AddRow(QVector<QString> row)
{
    if (row.size() != m_columns.size()) return;
    layoutAboutToBeChanged();
    m_data.append(row);
    layoutChanged();
}

void SkiModel::setSortableColumns(QVector<int> indexes)
{
    m_sortableColumns = indexes;
}
