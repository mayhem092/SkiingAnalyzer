#ifndef SKIVIEW_H
#define SKIVIEW_H

#include <QTreeView>
#include <QVBoxLayout>
#include <QtCharts>

#include "skimodel.h"
#include "skiquestionsdock.h"

namespace Ui {
class SkiView;
}

/**
 * @brief The SkiView class is the UI element which shows data to the user. It
 *        is the central widget in the softwares main window. It has 7 tabs
 *        which all contain some data for the user.
 */
class SkiView : public QWidget
{
    Q_OBJECT

public:
    explicit SkiView(QWidget *parent = nullptr);
    ~SkiView();

public slots:

    /**
     * @brief ClearView slot clears all data from the search tab.
     * @post search view has been cleared.
     */
    void ClearView();

    /**
     * @brief clearCompare slot clears all data from the compare tab.
     * @post compare views have been cleared.
     */
    void clearCompare();

    /**
     * @brief clearTimes slot clears all data from the times tab.
     * @post times view has been cleared.
     */
    void clearTimes();

    /**
     * @brief clearBestAthlete slot clears all data from the best tab.
     * @post best view has been cleared.
     */
    void clearBestAthlete();

    /**
     * @brief clearDistribution slot clears all data from the countries tab.
     * @post distribution view has been cleared.
     */
    void clearDistribution();

    /**
     * @brief clearTeams slot clears all data from the teams tab.
     * @post teams view has been cleared.
     */
    void clearTeams();

    /**
     * @brief Addrow slot receives data from SkiAnalyzer and shows it in the
     *        view.
     * @param row: the row of data to be added to the view.
     * @post model has been notified about new data.
     */
    void Addrow(QVector<QString> row);

    /**
     * @brief showCompareData slot shows compare data.
     * @param row: the row of data to be added to the view.
     * @param model tells which model should take the row's data
     * @pre  param model has to be 1 or 2.
     * @post the right model has been notified about new data.
     */
    void showCompareData(QVector<QString> row, int model);

    /**
     * @brief showCompareNumberOfParticipants slot shows the number of
     *        participants above the compare view.
     * @param numbers: includes the numbers that should be shown in the UI.
     * @pre  params are valid.
     * @post params are displayed in the UI.
     */
    void showCompareNumberOfParticipants(QPair<QString, QString> numbers);

    /**
     * @brief showTimesData slot shows time progression of a single athlete.
     * @param row: the row of data to be added to the view.
     * @pre  param data is valid.
     * @post a chart is shown in the UI.
     */
    void showTimesData(QVector<QString> row);

    /**
     * @brief showBestAthleteData slot shows the best athlete's data.
     * @param row: the row of data to be added to the view.
     * @post model has been notified about new data.
     */
    void showBestAthleteData(QVector<QString> row);

    /**
     * @brief showNationalityDistributionData slot shows nationality
     *        distribution graph.
     * @param row: the row of data to be added to the view.
     * @pre  param data is valid.
     * @post a chart is shown in the UI.
     */
    void showNationalityDistributionData(QHash<QString, int> row);

    /**
     * @brief showTeamsData slot shows top ten teams.
     * @param row: the row of data to be added to the view.
     * @post model has been notified about new data.
     */
    void showTeamsData(QVector<QString> row);

    /**
     * @brief showPredictionData slot shows the predicted winner.
     * @param row: the row of data to be added to the view.
     * @pre  param is valid.
     * @post data is shown in the UI.
     */
    void showPredictionData(QVector<QString> row);

    /**
     * @brief dataReady slot resizes columns to contents.
     * @param index of the tab that has new and ready data.
     */
    void dataReady(int index);

    /**
     * @brief tabChange slot changes tab to the one indicated by the parameter
     *        index.
     * @param index of the tab to change to.
     * @pre  param is vlid tab index.
     * @post tab has changed to the desired one.
     */
    void tabChange(int index);


    /**
     * @brief saveTimeChart slot saves the current time development chart as
     *        png file. If there is no chart to save then this slot does
     *        nothing.
     */
    void saveTimeChart();

    /**
     * @brief saveDistChart slot saves the current nationality distribution
     *        chart as png file. If there is no chart to save then this slot
     *        does nothing.
     */
    void saveDistChart();

signals:
    /**
     * @brief AddNewRow signal sends new row data to search tab's SkiModel.
     * @param row: the row of data to be added to the model.
     */
    void AddNewRow(QVector<QString> row);

    /**
     * @brief compare1AddRow signal sends new row data to compares tab's first
     *        SkiModel.
     * @param row: the row of data to be added to the model.
     */
    void compare1AddRow(QVector<QString> row);

    /**
     * @brief compare2AddRow signal sends new row data to compares tab's
     *        second SkiModel.
     * @param row: the row of data to be added to the model.
     */
    void compare2AddRow(QVector<QString> row);

    /**
     * @brief bestAddRow signal sends new row data to best tab's SkiModel.
     * @param row: the row of data to be added to the model.
     */
    void bestAddRow(QVector<QString> row);

    /**
     * @brief teamsAddRow signal sends new row data to teams tab's SkiModel.
     * @param row: the row of data to be added to the model.
     */
    void teamsAddRow(QVector<QString> row);

    /**
     * @brief tabHasChanged signal informs SkiQuestionsDock about tab change.
     * @param index of the new tab.
     */
    void tabHasChanged(int index);

    /**
     * @brief lockDockWidget signal orders the dock widget to disable itself
     *        for the duration of the update.
     */
    void lockDockWidget();

    /**
     * @brief releaseDockWidget signal tells the dock widget to enable itself
     *        after an update has completed.
     */
    void releaseDockWidget();


    /**
     * @brief getTimesName signal orders the SkiQuestionsDock to return the
     *        name of the athlete used to create the time development chart.
     * @param name: a reference to QString which should get the athlete's name.
     */
    void getTimesName(QString& name);

    /**
     * @brief getDistYear signal orders the SkiQuestionsDock to return the
     *        year used to create the nationality distribution chart.
     * @param year: a reference to QString which should get the year.
     */
    void getDistYear(QString& year);

private:

    /**
     * @brief CreateColumns method fills the m_columns and m_teamsColumns
     *        vectors with column name strings
     */
    void CreateColumns();

    Ui::SkiView*      ui;
    QTreeView*        m_view;
    SkiModel*         m_model;
    SkiModel*         m_compareModel1;
    SkiModel*         m_compareModel2;
    SkiModel*         m_bestModel;
    SkiModel*         m_teamsModel;
    QVector<QString>  m_columns;
    QVector<QString>  m_teamsColumns;
    SkiQuestionsDock* m_dock;
    QVBoxLayout*      m_distLayout;
    QVBoxLayout*      m_timesLayout;

};

#endif // SKIVIEW_H
