#ifndef SKIMAINWINDOW_H
#define SKIMAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QThread>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QMessageBox>

#include "skiview.h"
#include "skiquestionsdock.h"
#include "skianalyzer.h"

/**
 * @brief The SkiMainWindow class is the main UI element of the Skiing Analyzer
 *        software. It creates other UI elements and initializes the
 *        SkiAnalyzer class.
 */
class SkiMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    SkiMainWindow(QWidget *parent = 0);
    ~SkiMainWindow();

public slots:

    /**
     * @brief openHelpDialog slot opens a new dialog that displays some
     *        guidance
     */
    void openHelpDialog();

    /**
     * @brief quitProgram slot closes the program.
     */
    void quitProgram();

    /**
     * @brief retrieverDataReady slot is invoked by SkiDataRetriever. The slot
     *        updates the status of the UI's progress bar.
     * @param progress: amount of progress the SkiDataRetriever has done.
     * @param total: The max number of the progress integer.
     * @pre  Parameters are positive integers.
     * @post Progress bar has been updated.
     */
    void retrieverDataReady(int progress, int total);

    /**
     * @brief updateDataBaseClicked slot is invoked when update button is
     *        clicked. The slot orders the SkiDataRetriever to update its
     *        databases.
     * @post Signal has been emitted.
     */
    void updateDataBaseClicked();

signals:
    /**
     * @brief stopThread signal stops the thread that runs SkiAnalyzer.
     */
    void stopThread();

    /**
     * @brief refreshData signal orders the SkiDataRetriever to update its
     *        databases.
     */
    void refreshData();

    /**
     * @brief saveTimesChart signal tells the SkiView to save the current time
     *        development chart as png.
     */
    void saveTimesChart();

    /**
     * @brief saveDistChart signal tells the SkiView to save the current
     *        nationality distribution chart as png.
     */
    void saveDistChart();

private:

    /**
     * @brief createAnalyzerThread method creates the SkiAnalyzer thread and
     *        makes all necessary connects.
     */
    void createAnalyzerThread();

    /**
     * @brief createMenuAndToolBar method creates the menu and toolbar to the
     *        UI.
     */
    void createMenuAndToolBar();

    SkiView*          m_view;
    SkiQuestionsDock* m_dock;
    SkiAnalyzer*      m_analyzer;
    QProgressBar*     m_bar;
    QWidgetAction*    m_barAct;
    QAction*          m_updateAct;
    bool              m_anonymous;
};

#endif // SKIMAINWINDOW_H
