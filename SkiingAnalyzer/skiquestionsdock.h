#ifndef SKIQUESTIONSDOCK_H
#define SKIQUESTIONSDOCK_H

#include <QDockWidget>
#include <QCloseEvent>
#include <QMessageBox>
#include <QDate>

namespace Ui {
class SkiQuestionsDock;
}

/**
 * @brief The SkiQuestionsDock class creates a new dock widget UI element that
 *        lets the user interact with the software.
 */
class SkiQuestionsDock : public QDockWidget
{
    Q_OBJECT


public:
    explicit SkiQuestionsDock(QWidget *parent = nullptr);
    ~SkiQuestionsDock();

public slots:

    /**
     * @brief releaseButtons slot enables all buttons in the tab indicated by
     *        the param index.
     * @param index: the index of the tab to enable.
     * @post buttons are enabled.
     */
    void releaseButtons(int index);

    /**
     * @brief changeTab slot changes the current tab to the one indicated by
     *        param index.
     * @param index: the index of the tab to change to.
     * @pre  parameter is valid tab index.
     * @post tab has changed to desired one.
     */
    void changeTab(int index);

    /**
     * @brief lockForUpdate slot disables the dock widget.
     * @post dock widget is disabled.
     */
    void lockForUpdate();

    /**
     * @brief releaseAfterUpdate slot enables the dock widget.
     * @post dock widget is enabled.
     */
    void releaseAfterUpdate();

    /**
     * @brief getTimesName slot gets the athlete's name used in the time
     *        development chart from theUI and saves it to the referenced
     *        parameter name.
     * @param name: reference to QString where the name is saved.
     */
    void getTimesName(QString& name);

    /**
     * @brief getDistYear slot gets the year used in the nationality
     *        distribution chart from the UI and saves it to the referenced
     *        parameter name.
     * @param year: reference to QString where the year is saved.
     */
    void getDistYear(QString& year);

private slots:

    void searchClicked();
    void compareClicked();
    void timesClicked();
    void getBestClicked();
    void getDistributionClicked();
    void getTeamsClicked();
    void getPredictionClicked();

    /**
     * @brief yearIndexChange index of a year combobox has changed and some
     *        options need to be hidden or revealed.
     * @param index of the combobox.
     */
    void yearIndexChange(int index);

    /**
     * @brief timeIndexChange index of a time combobox has changed and some
     *        options need to be hidden or revealed.
     * @param index of the combobox.
     */
    void timeIndexChange(int index);

signals:
    /**
     * @brief clearView signal clears search view from SkiView.
     */
    void clearView();

    /**
     * @brief search signal sends search parameters to SkiAnalyzer.
     * @param params includes the needed parameters given by the user.
     */
    void search(const QVector<QString> &params);

    /**
     * @brief compare signal sends compare parameters to SkiAnalyzer.
     * @param params includes the needed parameters given by the user.
     */
    void compare(const QVector<QString> &params);

    /**
     * @brief clearCompare signal clears compare view from SkiView.
     */
    void clearCompare();

    /**
     * @brief getTimes signal sends time progression parameters to SkiAnalyzer.
     * @param params includes the needed parameters given by the user.
     */
    void getTimes(const QVector<QString> &params);

    /**
     * @brief getBest signal sends best athlete parameters to SkiAnalyzer.
     * @param params includes the needed parameters given by the user.
     */
    void getBest(const QVector<QString> &params);

    /**
     * @brief clearBest signal clears best athlete view from SkiView.
     */
    void clearBest();

    /**
     * @brief distribution signal sends nationality distribution parameters to
     *        SkiAnalyzer.
     * @param param includes the needed parameters given by the user.
     */
    void distribution(const QString &param);

    /**
     * @brief getTeams signal sends team search parameters to SkiAnalyzer.
     * @param params includes the needed parameters given by the user.
     */
    void getTeams(const QVector<QString> &params);

    /**
     * @brief getPrediction signal sends prediction calculation data to
     *        SkiAnalyzer.
     * @param param includes the needed parameters given by the user.
     */
    void getPrediction(const QString &param);

    /**
     * @brief clearTeams signal clears teams view from SkiView.
     */
    void clearTeams();

    /**
     * @brief tabChanged signal informs SkiView about tab change.
     * @param index: index of the new tab.
     */
    void tabChanged(int index);

protected:
    /**
     * @brief closeEvent method prevents the user from closing the dock widget.
     * @param event is the close event.
     */
    void closeEvent(QCloseEvent *event) override;

private:

    /**
     * @brief initYearsCombos method initializes the values of the years
     *        comboboxes.
     */
    void initYearsCombos();

    /**
     * @brief lockButtons method disables all buttons in the dock widget.
     */
    void lockButtons();

    Ui::SkiQuestionsDock *ui;
    bool                  closable;
};

#endif // SKIQUESTIONSDOCK_H
