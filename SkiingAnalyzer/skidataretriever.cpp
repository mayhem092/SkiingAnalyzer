#include "skidataretriever.h"

SkiDataRetriever::SkiDataRetriever(QObject *parent, bool anonymous) :
    QObject(parent),
    _data(QJsonObject()),
    _manager(new QNetworkAccessManager(this)),
    _postparameters{"", ""},
    _anonymous(anonymous),
    _sentrequests(0),
    _receivedrequests(0)
{
    connect(_manager, &QNetworkAccessManager::finished,
            this, &SkiDataRetriever::HandleRequestReply);

    connect(this, &SkiDataRetriever::ParametersReady,
            this, &SkiDataRetriever::GetSkiDataFromWebServer);

    _data.insert("anonymous", _anonymous);
}

SkiDataRetriever::~SkiDataRetriever()
{}

SkiingData SkiDataRetriever::GetSkiingData(int year, QString distance)
{
    SkiingData data;
    const QString yearString = QString::number(year);

    // Check if year is found from the database
    if(_data.contains(yearString) && _data[yearString].isObject()){
        QJsonObject temp = _data[yearString].toObject();

        QStringList keys = {distance};

        if(distance == ""){
            keys = temp.keys();
        }

        // Loop through selected distance or all distances
        for(const QString &key : keys){
            if(temp[key].isArray()){
                QVector<QHash<QString, QString>> distanceData =
                        distanceDataToVector(temp[key].toArray());
                data.insert(key, distanceData);
            }
        }
    }
    return data;
}

void SkiDataRetriever::StartSkiingDataRetrieval()
{
    // Read database file
    bool fileFound = ReadDataFromFile(_filename, _data);

    if(!fileFound)
        // Start data retrieval
        MakeGetRequest();
    else{
        // Check if anonymous mode in file is different than in database
        QJsonValue temp = _data.value("anonymous");
        if(temp == QJsonValue::Undefined || temp.toBool() != _anonymous)
            UpdateDataBase();
        else
            // Indicate that dataretriever is ready
            emit DataReady(0, 0);
    }
}

void SkiDataRetriever::UpdateDataBase()
{
    // Remove database file if found
    if(QFile::exists(_filename)){
        QFile file(_filename);
        file.remove();
    }

    _data = QJsonObject();
    _data.insert("anonymous", _anonymous);
    MakeGetRequest();
}

void SkiDataRetriever::HandleRequestReply(QNetworkReply *reply)
{
    QString data = reply->readAll();
    if(reply->operation() == 2){
        // Handle get request reply
        HandleGetReply(data);

        if(_postparameters[0] != "" && _postparameters[1] != "")
            emit ParametersReady();
    }
    else if(reply->operation() == 4){
        // Handle post request reply
        HandlePostReply(data);
        ++_receivedrequests;
        // Emit status of the retrieveal
        emit DataReady(_receivedrequests, _sentrequests);
        if(_sentrequests == _receivedrequests){
            // All data is retrieved
            SaveDataToFile(_filename, _data);
            _receivedrequests = 0;
            _sentrequests = 0;
            // Indicate that dataretriever is ready
            emit DataReady(0, 0);
        }
    }

    reply->deleteLater();
}

void SkiDataRetriever::GetSkiDataFromWebServer()
{
    QDate date = QDate::currentDate();
    const int startYear = 1974;
    const int endYear = date.year();

    for(int i = startYear; i <= endYear; ++i) {
        MakePostRequest(i);
        ++_sentrequests;
    }
}

void SkiDataRetriever::MakePostRequest(int year) const
{
    const QVector<QPair<QString, QString>> multipartData = {
        {"\"__EVENTTARGET\"", "dnn$ctr1025$Etusivu$cmdHaeTulokset"},
        {"\"__EVENTARGUMENT\"", ""},
        {"\"__VIEWSTATE\"", _postparameters[0]},
        {"\"__VIEWSTATEGENERATOR\"", "CA0B0334"},
        {"\"__EVENTVALIDATION\"", _postparameters[1]},
        {"\"dnn$ctr1025$Etusivu$ddlVuosi2x\"", QString::number(year)},
        {"\"dnn$ctr1025$Etusivu$ddlMatka2x\"", "kaikki"},
        {"\"dnn$ctr1025$Etusivu$chkLstSukupuoli2\"", "kaikki"},
        {"\"dnn$ctr1025$Etusivu$ddlIkaluokka2\"", "kaikki"},
        {"\"dnn$ctr1025$Etusivu$txtHakuEtunimi2\"", ""},
        {"\"dnn$ctr1025$Etusivu$txtHakuSukunimi2\"", ""},
        {"\"dnn$ctr1025$Etusivu$txtHakuPaikkakunta2\"", ""},
        {"\"dnn$ctr1025$Etusivu$ddlKansalaisuus2x\"", "0"},
        {"\"dnn$ctr1025$Etusivu$txtHakuJoukkue2\"", ""},
        {"\"ScrollTop\"", ""},
        {"\"__dnnVariable\"", ""},
    };
    QHttpMultiPart *multipart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    // Add all parts to the multipart
    for(auto &data: multipartData){
        QHttpPart temp;
        temp.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant("form-data; name=" + data.first));
        temp.setBody(data.second.toUtf8());

        multipart->append(temp);
    }

    // Make request and set headers
    QNetworkRequest request(_url);
    request.setRawHeader("Connection", "keep-alive");
    request.setRawHeader("Upgrade-insecure-requests", "1");
    request.setRawHeader("Origin", "https://www.finlandiahiihto.fi");
    request.setRawHeader("Referer", _url.toUtf8());

    QNetworkReply* reply = _manager->post(request, multipart);
    multipart->setParent(reply);
}

void SkiDataRetriever::MakeGetRequest() const
{
    QNetworkRequest request;
    request.setUrl(QUrl(_url));

    request.setRawHeader("Connection", "keep-alive");
    request.setRawHeader("Upgrade-insecure-requests", "1");

    _manager->get(request);
}

void SkiDataRetriever::HandleGetReply(const QString &page)
{
    const int parameters = 2;
    const QString inputs[parameters] = {"__VIEWSTATE", "__EVENTVALIDATION"};
    const QString valuestring = "value=\"";
    const QString quotestring = "\"";

    int index = 0;

    // Search whole page for parameters
    for(int i = 0; i < parameters; ++i){
        index = page.indexOf(inputs[i], index);
        index = page.indexOf(valuestring, index) + valuestring.length();
        int endIndex = page.indexOf(quotestring, index);

        QString parameter = page.mid(index, endIndex-index);

        _postparameters[i] = parameter;
    }
}

void SkiDataRetriever::HandlePostReply(const QString &page)
{
    int infoStartIndex = 0;
    int infoEndIndex = 0;

    const QString infoStart = ">";
    const QString infoEnd = "<";
    const QString yearid = "dnn_ctr1025_Etusivu_ddlVuosi2x";
    const QString selected = "selected=";
    const QString id = "dnn_ctr1025_Etusivu_dgrTulokset_ctl00__";
    const QString cellStart = "<td";

    // Search page for selected year
    infoStartIndex = page.indexOf(yearid, infoStartIndex);
    infoStartIndex = page.indexOf(selected, infoStartIndex);
    infoStartIndex = page.indexOf(infoStart, infoStartIndex) + infoStart.length();
    infoEndIndex = page.indexOf(infoEnd, infoStartIndex);

    QString year = page.mid(infoStartIndex, infoEndIndex-infoStartIndex);

    infoStartIndex = infoEndIndex;

    const QStringList names = {"year", "distance", "time", "placement",
                               "placementMale", "placementFemale", "sex",
                               "name", "locality", "nationality", "birthYear",
                               "team"};

    // Lists for data to censor
    const QStringList censorship {"sex", "placementMale", "placementFemale"};
    const QStringList hashCensorship {"name", "locality", "birthYear"};

    QHash<QString, QJsonArray> temp;

    // Search the page for skier info
    for(int index = 0; true; ++index){
        infoStartIndex = page.indexOf(id + QString::number(index),
                                      infoStartIndex);

        if(infoStartIndex == -1){
            break;
        }

        QJsonObject info;
        QString distance = "";
        for(const QString &name: names){
            infoStartIndex = page.indexOf(cellStart, infoStartIndex);
            infoStartIndex = page.indexOf(infoStart, infoStartIndex) + infoStart.length();
            infoEndIndex = page.indexOf(infoEnd, infoStartIndex);

            QString data = page.mid(infoStartIndex, infoEndIndex-infoStartIndex);
            infoStartIndex = infoEndIndex;

            if(data == "&nbsp;")
                data = "";

            if(name == "distance")
                distance = data;

            if(_anonymous){
                // save anonymized data
                if(censorship.contains(name))
                    data = "[Redacted]";
                else if(hashCensorship.contains(name)){
                    QByteArray hash = QCryptographicHash::hash(data.toUtf8(),
                                                       QCryptographicHash::Md5);
                    const int hashLength = 10;
                    data = QString(hash.toHex()).left(hashLength);
                }
            }

            info[name] = data;
        }

        // Create new distance
        if(!temp.contains(distance)){
            temp.insert(distance, QJsonArray());
        }

        temp[distance].append(info);
    }

    // Transform data from QHash to QJsonObject
    QJsonObject skiers;
    QHash<QString, QJsonArray>::iterator i;
    for (i = temp.begin(); i != temp.end(); ++i) {
        skiers[i.key()] = i.value();
    }

    _data[year] = skiers;
}

bool SkiDataRetriever::SaveDataToFile(const QString &filename,
                                      const QJsonObject &data)
{
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)){
        return false;
    }

    QJsonDocument dataDoc(data);
    file.write(dataDoc.toJson());

    return true;
}

bool SkiDataRetriever::ReadDataFromFile(const QString &filename,
                                        QJsonObject &data)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)){
        return false;
    }

    QJsonDocument dataDoc(QJsonDocument::fromJson(file.readAll()));

    data = dataDoc.object();
    return true;
}

QHash<QString, QString> SkiDataRetriever::SkierDataToHash(QJsonObject object)
{
    QStringList keys = object.keys();

    QHash<QString, QString> data;

    for(const QString &key : keys){
        if(object[key].isString()){
            data.insert(key, object[key].toString());
        }
    }
    return data;
}

QVector<QHash<QString, QString>>
SkiDataRetriever::distanceDataToVector(QJsonArray array)
{
    QVector<QHash<QString, QString>> data;
    const int size = array.size();

    // Loop through all the skiers
    for(int i = 0; i < size; ++i){
        if(array.at(i).isObject()){
            QJsonObject object = array.at(i).toObject();

            QHash<QString, QString> skierData = SkierDataToHash(object);
            data.push_back(skierData);
        }
    }
    return data;
}
