#ifndef SKIMODEL_H
#define SKIMODEL_H

#include <QAbstractItemModel>

/**
 * @brief The SkiModel class is inherited from QAbstractItemModel. It works
 *        as a model for the QtreeViews of the UI.
 */
class SkiModel : public QAbstractItemModel
{
    Q_OBJECT

public slots:

    /**
     * @brief AddRow slot adds a new row to the m_data vector.
     * @param row: the row of data to be added.
     * @pre  Parameter is valid data.
     * @post Parameter has been added.
     */
    void AddRow(QVector<QString> row);

    /**
     * @brief setSortableColumns slot saves the indexes of columns that can be
     *        sorted
     * @param indexes the indexes to be saved.
     * @pre  Parameter is valid and includes valid data.
     * @post Parameter has been saved.
     */
    void setSortableColumns(QVector<int> indexes);

public:
    explicit SkiModel(QObject *parent = nullptr);

    /**
     * @brief headerData method sends header data to the QtreeView this model
     *        is attached to.
     * @param section tells which section of the header.
     * @param orientation can be horizontal, vertical...
     * @param role: Qt role that can be used to create custom features.
     * @return header data used by QTreeView.
     * @pre  Parameters are valid.
     * @post Header data is returned.
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /**
     * @brief SetColumns method saves the columns given as parameters.
     * @param columns: the columns to be saved.
     * @pre  Parameter is valid data for column names.
     * @post Parameter has been saved.
     */
    void SetColumns(QVector<QString> &columns);

    /**
     * @brief rowCount method returns the amount of rows in the models
     *        datastructure
     * @param parent: check QT documentation for more info.
     * @return row count.
     * @pre  parent is not valid.
     * @post row count has been returned.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief columnCount method returns the amount of saved columns.
     * @param parent: check QT documentation for more info.
     * @return column count.
     * @pre  parent is not valid.
     * @post column count has been returned.
     */
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief data method returns the data indicated by the index.
     * @param index: the index that indicates the desired data.
     * @param role: Qt role that can be used to create custom features.
     * @return data used by the QTreeView
     * @pre  parameter index is valid.
     * @post data has been returned.
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /**
     * @brief clearData method clears the models data structure.
     * @post data has been removed.
     */
    void clearData();

    /**
     * @brief sort method sorts the models data according to the given
     *        parameters.
     * @param column: the column to sort by.
     * @param order: ascending or descending.
     * @pre  parameter column is valid and can be found from sortable columns.
     * @post data has been sorted.
     */
    void sort(int column, Qt::SortOrder order) override;

    /**
     * @brief index method creates a index with the desired row and column.
     * @param row: the row of the desired index.
     * @param column: the column of the desired index.
     * @param parent: check QT documentation for more info.
     * @return QModelIndex with the desired row and column.
     * @pre  row and column are valid and data exists at that location.
     * @post valid index has been returned.
     */
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief parent method returns the parent of the given parameter index.
     * @param child which parent should be returned
     * @return the parent of the given child.
     * @post invalid parent is returned.
     */
    QModelIndex parent(const QModelIndex &child) const override;

private:

    QVector<QString>            m_columns;
    QVector<QVector<QString>>   m_data;
    QVector<int>                m_sortableColumns;
};

#endif // SKIMODEL_H
