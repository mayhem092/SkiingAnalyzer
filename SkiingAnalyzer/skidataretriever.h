#ifndef SKIDATARETRIEVER_H
#define SKIDATARETRIEVER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QHash>
#include <QPair>
#include <QDate>
#include <QCryptographicHash>

typedef QHash<QString, QVector<QHash<QString, QString>>> SkiingData;

/**
 * @brief The SkiDataRetriever class handles the data retrieval and storage
 *        of skiing data. This class saves the retrieved data to a file. On
 *        subsequent starts dataretriever reads the data from the file if it is
 *        found. This class also provides skiing data to the analyzers.
 */
class SkiDataRetriever : public QObject
{
    Q_OBJECT
public:
    explicit SkiDataRetriever(QObject *parent = nullptr, bool anonymous = false);
    ~SkiDataRetriever();

signals:
    /**
     * @brief DataReady: Notifies that the data is retrieved and retriever is
     * ready to serve data
     * @param progress: Number of received and handeled requests
     * @param total: Total number of requests
     */
    void DataReady(int progress, int total);

    /**
     * @brief ParametersReady: Internal signal to notify that post request
     * parameters are valid
     */
    void ParametersReady();

public slots:

    /**
     * @brief GetSkiingData: This method is used to get skiing data from
     *        specific year. Distance specifies the type of the skiing
     *        competition. If distance is empty then the retreiver returns
     *        every result from the specified year.
     * @param year: Year from which the data is fetched
     * @param distance: Specific distance from which the data is fetched
     * @return Skiing data from a specified year
     * @pre Data is in internal database
     * @post Information found in the database for the year and distance is
     *       returned
     */
    SkiingData GetSkiingData(int year, QString distance = "");

    /**
     * @brief StartSkiingDataRetrieval: Starts the data retrieval
     * @post Data retrieval is started
     */
    void StartSkiingDataRetrieval();

    /**
     * @brief UpdateDataBase: Deletes the local file and starts a new data
     * retrieval and therefore creates the file again with new data.
     * @post Data retrieval is started
     */
    void UpdateDataBase();

private slots:
    /**
     * @brief HandleRequestReply: Handles post and get request replies
     * @param reply: Reply to post or get request
     */
    void HandleRequestReply(QNetworkReply *reply);

    /**
     * @brief GetSkiDataFromWebServer: Makes all post requests to
     *        Finlandiahiihto's servers
     */
    void GetSkiDataFromWebServer();

private:
    /**
     * @brief MakePostRequest: Makes a get request to the server
     * @param year: Year from which the data is fetched from Finlandia hiihto
     */
    void MakePostRequest(int year) const;

    /**
     * @brief MakeGetRequest: Makes a post request to the server
     */
    void MakeGetRequest() const;

    /**
     * @brief HandleGetReply: Handles get request reply
     * @param page: Whole web page in QString
     */
    void HandleGetReply(const QString &page);

    /**
     * @brief HandlePostReply: Handles post request reply
     * @param page: Whole web page in QString
     */
    void HandlePostReply(const QString &page);

    /**
     * @brief SaveDataToFile: Saves retrieved data to file
     * @param filename: Filename of the database file
     * @param data: Database from which a copy is made to a file
     * @return Boolean indicating if saving was successful
     */
    bool SaveDataToFile(const QString &filename, const QJsonObject &data);

    /**
     * @brief ReadDataFromFile: Reads saved data from a file
     * @param filename: Filename of the database file
     * @param data: Object to which the database file is read
     * @return Boolean indicating if reading was successful
     */
    bool ReadDataFromFile(const QString &filename, QJsonObject &data);

    /**
     * @brief SkierDataToHash: Copies skier data from QJsonObject to QHash
     * @param object: QJsonObject containing skier data
     * @return QHash containing skier data
     */
    QHash<QString, QString> SkierDataToHash(QJsonObject object);

    /**
     * @brief distanceDataToVector: Copies distance data from QJsonArray to
     *        QVector
     * @param array: QJsonArray containing distance data
     * @return QVector containing distance data
     */
    QVector<QHash<QString, QString>> distanceDataToVector(QJsonArray array);

    QJsonObject _data;
    const QString _url = "https://www.finlandiahiihto.fi/Tulokset/Tulosarkisto";
    const QString _filename = "data.json";
    QNetworkAccessManager* _manager;
    QString _postparameters[2];
    bool _anonymous;
    int _sentrequests;
    int _receivedrequests;
};

#endif // SKIDATARETRIEVER_H
