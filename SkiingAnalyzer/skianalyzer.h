#ifndef SKIANALYZER_H
#define SKIANALYZER_H

#include <QObject>

#include "skidataretriever.h"


/**
 * @brief The SkiAnalyzer class takes users inputs from SkiQuestionsDock and
 *        asks data from SkiDataRetriever class. It analyses this data and
 *        sends the analysis results to SkiView. It represents the "controller"
 *        part of the software but it doesn't quite fullfil the right MVC
 *        controller role. Nevertheless all data analysis happens in this
 *        class.
 */
class SkiAnalyzer : public QObject
{
    Q_OBJECT
public:
    explicit SkiAnalyzer(QObject *parent = nullptr, bool anonymous = false);

public slots:
    /**
     * @brief run slot starts the new thread that includes this class and the
     *        SkiDataRetriever class.
     */
    void run();

    /**
     * @brief handleSearchRequest searches the database and filters out all unneeded entries.
     * @param searchParams: the parameters the user has input.
     * @pre parameters are valid, data is accessible.
     * @post the searched data has been emitted.
     */
    void handleSearchRequest(const QVector<QString> &searchParams);

    /**
     * @brief handleCompareRequest seaches database twice and show the result side-by-side.
     * @param params: user input
     * @pre data is accessible.
     * @post both searches have concluded and data has been emitted.
     */
    void handleCompareRequest(const QVector<QString> &params);
    /**
     * @brief handleTimesRequest searches the results of a single athlete
     * @param params: the name of the athelete and years to be searched.
     * @pre data is accessible, user has input a name.
     * @post data is emitted.
     */

    void handleTimesRequest(const QVector<QString> &params);

    /**
     * @brief Gives best athlete of chosen gender for each race for the wanted years.
     * @param params contains: Starting year, end year, gender.
     * @pre   params needs to be QVector<QString> and contain three parameters. Start year and end year need to be numbers.
     * @post  Emits bestAthleteData. If placement "1" is not found function emits nothing for that race.
     */
    void handleBestAthleteRequest(const QVector<QString> &params);

    /**
     * @brief Counts all participants by country
     * @param searchyear
     * @pre   param needs to be a number
     * @post  Emits nationalityDistributionData
     */
    void handleCountriesRequest(const QString &param);

    /**
     * @brief Goes through data for given year and race and analyzes the top 10 teams for that race.
     *        Comparison is done by counting the 4 best times of all teams.
     * @param Search year, race.
     * @pre   params has to be a QVector with 2 parameters and params[0] has to be a number.
     * @post  Emits teamsData.
     */
    void handleTeamsRequest(const QVector<QString> &params);

    /**
     * @brief Gives a prediction of the next years winner by checking past winners of given race.
     * @param Race.
     * @pre   param data is valid.
     * @post  Emits predictionData.
     */
    void handlePredictionRequest(const QString &param);

signals:

    /**
     * @brief addNewRow signal sends search result data to SkiView
     * @param row: the row of new data to be shown.
     */
    void addNewRow(QVector<QString> row);

    /**
     * @brief compareData sends compare result data to SkiView.
     * @param row: the row of new data to be shown.
     * @param view: the index of the view that should show the data.
     */
    void compareData(QVector<QString> row, int view);

    /**
     * @brief compareNumberOfParticipants signal sends the number of
     *        participants to SkiView.
     * @param numbers: the participant amount that should be shown.
     */
    void compareNumberOfParticipants(QPair<QString, QString> numbers);

    /**
     * @brief timesData signal sends time progression of a single athlete to
     *        SkiView
     * @param row: the row of new data to be shown.
     */
    void timesData(QVector<QString> row);

    /**
     * @brief addNewRow signal sends the best athlete's data to SkiView.
     * @param row: the row of new data to be shown.
     */
    void bestAthleteData(QVector<QString> row);

    /**
     * @brief nationalityDistributionData sends parameters for nationality
     *        distribution graph to SkiView.
     * @param row: the row of new data to be shown.
     */
    void nationalityDistributionData(QHash<QString, int> row);

    /**
     * @brief teamsData signal sends top ten team data to SkiView.
     * @param row: the row of new data to be shown.
     */
    void teamsData(QVector<QString> row);

    /**
     * @brief predictionData signal sends the calculated prediction data to
     *        SkiView.
     * @param row: the row of new data to be shown.
     */
    void predictionData(QVector<QString> row);

    /**
     * @brief dataSent signal informs other classes that the data analysis is
     *        ready.
     * @param index of the tab which contains the new data.
     */
    void dataSent(int index);

    /**
     * @brief refreshDataStorages signal tells the data retriever to update its
     *        data.
     */
    void refreshDataStorages();

    /**
     * @brief dataReady informs the main window that the SkiDataRetriever has
     *        completed some of the data retrieval.
     * @param progress: the amount of progress made.
     * @param total: the total amount the progress param can get.
     */
    void dataReady(int progress, int total);

private:

    SkiDataRetriever* m_retriever;
    bool              m_anonymous;

    /**
     * @brief rtrnSearchDistanceParameter converts the name of the competition
     *        type into a code that is used to search the data
     * @param distance as it is presented in the ui.
     * @return returns the competition type as a code.
     */
    QString rtrnSearchDistanceParameter (QString distance);

    /**
     * @brief timeToInt converts timevalues in the data to float.
     * @param time: the time of an individual athelete in hh:mm:ss-form.
     * @return time in hours as float.
     */
    float timeToInt (QString time);

    /**
     * @brief createEmit creates the signal to be emitted in search-function.
     * @param data is the entirety of data from which the emits are created.
     * @param index of the data.
     * @param parameter is the type of the competition.
     * @return returns a vector of required values for addNewRow-signal.
     */
    QVector<QString> createEmit(QHash<QString, QVector<QHash<QString, QString>>> data, int index, QString parameter);

    /**
     * @brief splitName splits the first and last name of the athlete into separate entities.
     * @param name of the athelete separated with a space.
     * @return vector containing first and last name.
     */
    QVector<QString> splitName(QString name);

    /**
     * @brief Converts float time (in hours) to QString
     * @param time
     * @return QString of time in correct format
     */
    QString floatToTimeString(const float time);
};

#endif // SKIANALYZER_H
