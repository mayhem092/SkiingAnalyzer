#include "skianalyzer.h"
#include <QtCharts>
#include <QRegExp>

SkiAnalyzer::SkiAnalyzer(QObject *parent, bool anonymous) : QObject(parent), m_anonymous(anonymous)
{

}

void SkiAnalyzer::run()
{
    // Initialize the SkiDataRetriever and make necessary connects.
    m_retriever = new SkiDataRetriever(this, m_anonymous);
    connect(this, &SkiAnalyzer::refreshDataStorages, m_retriever, &SkiDataRetriever::UpdateDataBase);
    connect(m_retriever, &SkiDataRetriever::DataReady, this, &SkiAnalyzer::dataReady);
    m_retriever->StartSkiingDataRetrieval();
}

void SkiAnalyzer::handleSearchRequest(const QVector<QString> &searchParams)
{
    QString fromyear = searchParams[0];
    QString toyear = searchParams[1];
    QString comptype = searchParams[2];
    QString forename = searchParams[3];
    QString familyname = searchParams[4];
    QString gender = searchParams[5];
    QString team = searchParams[6];
    QString nationality = searchParams[7];
    QString locality = searchParams[8];
    QString topplacements = searchParams[9];
    QString timefrom = searchParams[10];
    QString timeto = searchParams[11];

    //converting search distance to usable format.
    QString searchdistanceparam = rtrnSearchDistanceParameter(comptype);
    // all available data from searched years/distances
    for(int i = fromyear.toInt(); i <= toyear.toInt(); i++){
        SkiingData data;
        QVector<QVector<QString>> tempcont;
        if(searchdistanceparam != "all"){
            data = m_retriever->GetSkiingData(i, searchdistanceparam);
            for(int j = 0; j < data[searchdistanceparam].count(); j++){
                QVector<QString> temp = createEmit(data, j, searchdistanceparam);
                tempcont << temp;
            }
        }

        else{
            data = m_retriever->GetSkiingData(i);
            foreach(const QString &key, data.keys()){
                for(int j = 0; j < data[key].count(); j++){
                    QVector<QString> temp = createEmit(data, j, key);

                    tempcont << temp;
                }
            }
        }
        // vector containing all data is filtered one search parameter at a time:
        // searching with forename
        if (forename != ""){
            QVector<QVector<QString>> checkvector;
            for(int k = 0; k < tempcont.count(); k++){
                QVector<QString> names = splitName(tempcont[k][5]);
                if(names[1].toLower() == forename.toLower()){
                    checkvector << tempcont[k];
                }
            }
            tempcont = checkvector;
        }

        // searching with familyname
        if (familyname != ""){
            QVector<QVector<QString>> checkvector;
            for(int k = 0; k < tempcont.count(); k++){
                QVector<QString> names = splitName(tempcont[k][5]);
                if(names[0].toLower() == familyname.toLower()){
                    checkvector << tempcont[k];
                }
            }
            tempcont = checkvector;
        }

        // searching with gender
        if(gender != "Both"){
            QString gendercompareparam = "";
            if (gender == "Male"){
                gendercompareparam = "M";
            }
            else if (gender == "Female"){
                gendercompareparam = "F";
            }
            QVector<QVector<QString>> checkvector;
            for(int k = 0; k < tempcont.count(); k++){
                if(tempcont[k][4] == gendercompareparam){
                    checkvector << tempcont[k];
                }
            }
            tempcont = checkvector;
        }

        // searching with team
        if(team != ""){
            QVector<QVector<QString>> checkvector;
            for(int k = 0; k < tempcont.count(); k++){

                if(tempcont[k][9].toLower() == team.toLower()){
                    checkvector << tempcont[k];
                }
            }
            tempcont = checkvector;
        }

        // searching using nationality
        if(nationality != ""){
            QVector<QVector<QString>> checkvector;
            for(int k = 0; k < tempcont.count(); k++){

                if(tempcont[k][7].toLower() == nationality.toLower()){
                    checkvector << tempcont[k];
                }
            }
            tempcont = checkvector;
        }

        // searching using locality
        if(locality != ""){
            QVector<QVector<QString>> checkvector;
            for(int k = 0; k < tempcont.count(); k++){

                if(tempcont[k][6].toLower() == locality.toLower()){
                    checkvector << tempcont[k];
                }
            }
            tempcont = checkvector;
        }

        // filtering by timelimit(s), both are done separately
        if(timefrom != "0"){
            QVector<QVector<QString>> checkvector;
            for(int k = 0; k < tempcont.count(); k++){
                if(timeToInt(tempcont[k][2]) >= timefrom.toFloat()){
                    checkvector << tempcont[k];
                }
            }
            tempcont = checkvector;
        }
        if(timeto != "All"){
            QVector<QVector<QString>> checkvector;
            for(int k = 0; k < tempcont.count(); k++){
                if(timeToInt(tempcont[k][2]) <= timeto.toFloat()){
                    checkvector << tempcont[k];
                }
            }
            tempcont = checkvector;
        }

        if(topplacements == "All"){
            for(int k = 0; k < tempcont.count(); k++){
                emit addNewRow(tempcont[k]);

            }
        }
        else{
            for(int k = 0; k < tempcont.count() && k < topplacements.toInt(); k++){
                emit addNewRow(tempcont[k]);
            }
        }

    }
    emit dataSent(1);
}

void SkiAnalyzer::handleCompareRequest(const QVector<QString> &params)
{

    QString type1 = rtrnSearchDistanceParameter(params[0]);
    QString year1 = params[1];
    QString type2 = rtrnSearchDistanceParameter(params[2]);
    QString year2 = params[3];

    int total1 = 0;
    int total2 = 0;

    if(type1 != "All" && type2 != "All"){

        SkiingData data1 = m_retriever->GetSkiingData(year1.toInt(), type1);

        SkiingData data2 = m_retriever->GetSkiingData(year2.toInt(), type2);

        QVector<QVector<QString>> cont1;
        for(int i = 0; i < data1[type1].count(); i++){

            QVector<QString> temp = createEmit(data1, i, type1);

            cont1 << temp;
        }

        QVector<QVector<QString>> cont2;
        for(int i = 0; i < data2[type2].count(); i++){

            QVector<QString> temp = createEmit(data2, i, type2);

            cont2 << temp;
        }


        for(int j = 0; j < cont1.count(); j++){
            total1++;
            emit compareData(cont1[j], 1);
        }

        for(int j = 0; j < cont2.count(); j++){
            total2++;
            emit compareData(cont2[j], 2);
        }

    }

    emit compareNumberOfParticipants(qMakePair(QString().number(total1), QString().number(total2)));

    emit dataSent(2);
}

void SkiAnalyzer::handleTimesRequest(const QVector<QString> &params)
{
    QString fromyear = params[0];
    QString toyear = params[1];
    QString fname = params[2];
    QString lname = params[3];

    QVector<QString> times;


    if(fname.length() > 0 && lname.length() > 0){
        for(int i = fromyear.toInt(); i <= toyear.toInt(); i++){
            SkiingData data;
            QVector<QVector<QString>> tempcont;
            data = m_retriever->GetSkiingData(i);

            foreach(const QString &key, data.keys()){
                for(int j = 0; j < data[key].count(); j++){
                    QVector<QString> temp = createEmit(data, j, key);

                    tempcont << temp;
                }
            }

            if (fname != ""){
                QVector<QVector<QString>> checkvector;
                for(int k = 0; k < tempcont.count(); k++){
                    QVector<QString> names = splitName(tempcont[k][5]);
                    if(names[1].toLower() == fname.toLower()){
                        checkvector << tempcont[k];
                    }
                }
                tempcont = checkvector;
            }

            if (lname != ""){
                QVector<QVector<QString>> checkvector;
                for(int k = 0; k < tempcont.count(); k++){
                    QVector<QString> names = splitName(tempcont[k][5]);
                    if(names[0].toLower() == lname.toLower()){
                        checkvector << tempcont[k];
                    }
                }
                tempcont = checkvector;
            }

            for(int k = 0; k < tempcont.count(); k++){
                times << tempcont[k][0] << tempcont[k][1] << QString().number(timeToInt(tempcont[k][2]));
            }
        }
    }
    emit timesData(times);
    emit dataSent(3);
}

void SkiAnalyzer::handleBestAthleteRequest(const QVector<QString> &params)
{
    QString searchyear = params[0];
    QString searchToYear = params[1];
    QString gender = params[2];

    //Going through the database year by year and race by race.
    //And emiting the best athlete based on which gender was chosen.
    for(int year = searchyear.toInt(); year <= searchToYear.toInt() ; year++){
        SkiingData data = m_retriever->GetSkiingData(year);
        QList<QString> keys = data.uniqueKeys();
        for(int key = 0; key < keys.count() ; key++){
            for(int i = 0; i < data[keys[key]].count() ;i++){
                if(gender == "Male"){
                    if(data[keys[key]][i]["placement"] == "1" && data[keys[key]][i]["sex"] == "M"){
                        QString year (data[keys[key]][i]["year"]);
                        QString name (data[keys[key]][i]["name"]);
                        QString nationality (data[keys[key]][i]["nationality"]);
                        QString team (data[keys[key]][i]["team"]);
                        QString time (data[keys[key]][i]["time"]);
                        QString distance (data[keys[key]][i]["distance"]);
                        QString placement (data[keys[key]][i]["placement"]);
                        QString sex (data[keys[key]][i]["sex"]);
                        QString hometown (data[keys[key]][i]["locality"]);
                        QString birthyear (data[keys[key]][i]["birthYear"]);
                        float race = keys[key].remove(QRegExp(R"([\D])")).toFloat() / timeToInt(time);
                        QString kesk = QString::number(race);
                        if (kesk.lastIndexOf(QChar('.'))){
                            int pos = kesk.lastIndexOf(QChar('.'));
                            kesk = kesk.left(pos+3);
                        }
                        QString avgtime = kesk;

                        QVector<QString> temp = QVector<QString>() << year << distance << time
                                                                   << placement << sex << name << hometown
                                                                   << nationality << birthyear << team << avgtime;

                        emit bestAthleteData(temp);
                    }

                }
                else if(gender == "Female")
                    if(data[keys[key]][i]["placementFemale"] == "1"){
                        QString year (data[keys[key]][i]["year"]);
                        QString name (data[keys[key]][i]["name"]);
                        QString nationality (data[keys[key]][i]["nationality"]);
                        QString team (data[keys[key]][i]["team"]);
                        QString time (data[keys[key]][i]["time"]);
                        QString distance (data[keys[key]][i]["distance"]);
                        QString placement (data[keys[key]][i]["placement"]);
                        QString sex (data[keys[key]][i]["sex"]);
                        QString hometown (data[keys[key]][i]["locality"]);
                        QString birthyear (data[keys[key]][i]["birthYear"]);
                        float race = keys[key].remove(QRegExp(R"([\D])")).toFloat() / timeToInt(time);
                        QString kesk = QString::number(race);
                        if (kesk.lastIndexOf(QChar('.'))){
                            int pos = kesk.lastIndexOf(QChar('.'));
                            kesk = kesk.left(pos+3);
                        }
                        QString avgtime = kesk;

                        QVector<QString> temp = QVector<QString>() << year << distance << time
                                                                   << placement << sex << name << hometown
                                                                   << nationality << birthyear << team << avgtime;
                        emit bestAthleteData(temp);
                    }
            }
        }
    }
    emit dataSent(4);
}

void SkiAnalyzer::handleCountriesRequest(const QString &param)
{
    int searchyear = param.toInt();

    //List contains information of all participated countries and number of their participants
    QHash<QString, int> List;
    SkiingData data = m_retriever->GetSkiingData(searchyear);
    QList<QString> distance = data.uniqueKeys();

    //Going through the chosen years data one race at a time and in it one skiier at a time
    for(int race = 0; race < distance.count(); race++){
        for(int x = 0; x < data[distance[race]].count(); x++){
            if(List.contains(data[distance[race]][x]["nationality"])){
                List.insert(data[distance[race]][x]["nationality"], (List.value(data[distance[race]][x]["nationality"])+1));
            }
            else{
                List.insert(data[distance[race]][x]["nationality"], 1);
            }
        }

    }
    emit nationalityDistributionData(List);
    emit dataSent(5);
}

void SkiAnalyzer::handleTeamsRequest(const QVector<QString> &params)
{
    int searchyear = params[0].toInt();
    QString distance = params[1];
    QString race = rtrnSearchDistanceParameter(distance);
    SkiingData data = m_retriever->GetSkiingData(searchyear);

    //List containing every team in a race and a vector of the teams times
    QHash<QString, QVector<QString>> List;

    //Going through the chosen year and race skiier by skiier
    for(int x = 0; x < data[race].count(); x++){
        if(List.contains(data[race][x]["team"])){
            List[(data[race][x]["team"])].push_back(data[race][x]["time"]);
        }
        else if(data[race][x]["team"] != ""){
            QVector<QString> team;
            team.push_back(data[race][x]["time"]);
            List.insert(data[race][x]["team"], team);
        }
    }
    //top10 map has only teams that meet the requirements. Teams total time, team name.
    //Sorted by time
    QMap<QString, QString> top10;

    //Going through all the of the races team data and parsing the data for top10.
    for(const QVector<QString> &teams : List){
        QString temptime;
        int tempHours = 0;
        int tempMin = 0;
        int tempSec = 0;
        int tempSadas = 0;
        if(teams.size() >= 4){
            for(int skiier = 0 ; skiier < 4 ; skiier++){
                QString splitTime = teams.at(skiier);
                QStringList timeList = splitTime.split(":");
                tempHours = tempHours + timeList.at(0).toInt();
                tempMin = tempMin + timeList.at(1).toInt();
                if(timeList.at(2).contains(".")){
                    QStringList seconds = timeList.at(2).split(".");
                    tempSec = tempSec + seconds.at(0).toInt();
                    tempSadas = tempSadas + seconds.at(1).toInt();
                }
                else{
                    tempSec = tempSec + timeList.at(2).toInt();
                }

            }
            tempSec = tempSec + (tempSadas / 100);
            tempSadas = tempSadas % 100;
            tempMin = tempMin + (tempSec / 60);
            tempSec = tempSec % 60;
            tempHours = tempHours + (tempMin / 60);
            tempMin = tempMin % 60;

            QString hours = QString::number(tempHours);
            QString minutes = QString::number(tempMin);
            QString seconds = QString::number(tempSec);
            QString sadas = QString::number(tempSadas);

            if(tempHours < 10){
                hours = "0"+hours;
            }
            if(tempMin < 10){
                minutes = "0"+minutes;
            }
            if(tempSec < 10){
                seconds = "0"+seconds;
            }
            if(tempSadas < 10){
                sadas = "0"+sadas;
            }

            temptime = hours + ":" + minutes + ":" + seconds + "." + sadas;
            top10.insert(temptime, List.key(teams));
        }
    }
    QList<QString> keys = top10.uniqueKeys();
    int counter = 0;

    //Going through top10 map to get the top10 teams and emiting the data
    for(const QString &key : keys){
        counter++;
        float avgtime = timeToInt(key) / 4;

        QString time = floatToTimeString(avgtime);

        QString counterstring = QString::number(counter);
        QString team = top10[key];
        QVector<QString> temp = QVector<QString>() << counterstring << team << params[0]
                                                   << distance << key << time;
        emit teamsData(temp);
        if(counter == 10){
            break;
        }
    }
    emit dataSent(6);
}

void SkiAnalyzer::handlePredictionRequest(const QString &param)
{
    QString race = rtrnSearchDistanceParameter(param);

    //List containing the winners of past years races. Name, number of wins.
    QHash<QString, int> winnerList;

    QVector<QString> totalTime;

    //Going through the race data year by year and storing the winner to List.
    //If the winner had won before int just goes up by 1.
    for(int year = 2014; year < 2020; year++){
        SkiingData data = m_retriever->GetSkiingData(year);
        if(data[race].size() > 0){
            if(winnerList.contains(data[race][0]["name"])){
                winnerList.insert(data[race][0]["name"], (winnerList.value(data[race][0]["name"])+1));
                totalTime.push_back(data[race][0]["time"]);
            }
            if(!winnerList.contains(data[race][0]["name"])){
                winnerList.insert(data[race][0]["name"], 1);
                totalTime.push_back(data[race][0]["time"]);
            }
        }
    }
    //mostwins contains the name of the predicted winner
    QString mostwins = "";

    //Checking who had the most wins. If there is a tie it just gives the first one from the list.
    QList<QString> keys = winnerList.uniqueKeys();
    for(const QString &key : keys){
        if(mostwins == ""){
            mostwins = key;
        }
        else if(winnerList.value(key) > winnerList.value(mostwins)){
            mostwins = key;
        }
    }
    float magicTime = 0;
    for(const QString &time : totalTime){
        magicTime = magicTime + timeToInt(time);
    }

    magicTime = magicTime / totalTime.size();

    QString time = floatToTimeString(magicTime);

    emit predictionData(QVector<QString>() << mostwins << param << time);
    emit dataSent(7);
}

QString SkiAnalyzer::rtrnSearchDistanceParameter(const QString distance)
{
    if (distance == "50 km traditional"){
        return QString("P50");
    }
    else if (distance == "50 km freestyle"){
        return QString("V50");
    }
    else if (distance == "100km traditional"){
        return QString("P100");
    }
    else if (distance == "32km traditional"){
        return QString("P32");
    }
    else if (distance == "20km freestyle"){
        return QString("V20");
    }
    else if (distance == "32km freestyle"){
        return QString("V32");
    }
    else if (distance == "20km freestyle, juniors"){
        return QString("V20jun");
    }
    else if (distance == "42km traditional"){
        return QString("P42");
    }
    else if (distance == "42km freestyle"){
        return QString("V42");
    }
    else if (distance == "32km freestyle (2014)"){
        return QString("V32");
    }
    else if (distance == "20km traditional (2014)"){
        return QString("P20");
    }
    else if (distance == "30km traditional (2002-2005)"){
        return QString("P30");
    }
    else if (distance == "44km traditional (2002)"){
        return QString("P44");
    }
    else if (distance == "60km traditional (2003-2005)"){
        return QString("P60");
    }
    else if (distance == "62km traditional (2006)"){
        return QString("P62");
    }
    else if (distance == "25km traditional"){
        return QString("P25");
    }
    else if (distance == "32km traditional"){
        return QString("P32");
    }
    else if (distance == "35km traditional"){
        return QString("P35");
    }
    else if (distance == "45km traditional"){
        return QString("P45");
    }
    else if (distance == "52km traditional"){
        return QString("P52");
    }
    else if (distance == "53km traditional"){
        return QString("P53");
    }
    else if (distance == "75km traditional"){
        return QString("P75");
    }
    else if (distance == "30km freestyle"){
        return QString("V30");
    }
    else if (distance == "45km freestyle"){
        return QString("V45");
    }
    else if (distance == "53km freestyle"){
        return QString("V53");
    }
    else if (distance == "75km freestyle"){
        return QString("V75");
    }
    else if (distance == "All types"){
        return QString("all");
    }
    return QString("");
}

QString SkiAnalyzer::floatToTimeString(const float time){
    int avghours = time / 1;
    int avgmin = (time-avghours) * 60 * 60 / 60;
    int avgsec = (time-avghours-(avgmin/60))*60;

    QString tempavghours = QString::number(avghours);
    QString tempavgminutes = QString::number(avgmin);
    QString tempavgseconds = QString::number(avgsec);

    if(avghours < 10){
        tempavghours = "0"+tempavghours;
    }
    if(avgmin < 10){
        tempavgminutes = "0"+tempavgminutes;
    }
    if(avgsec < 10){
        tempavgseconds = "0"+tempavgseconds;
    }

    QString avgtimestring = tempavghours + ":" + tempavgminutes + ":" + tempavgseconds;

    return avgtimestring;
}

float SkiAnalyzer::timeToInt (const QString time)
{
    float total;
    if(time.length() < 4){
        total = 0;
        return total;
    }

    QStringList list;
    list = time.split(":");
    QString h, m, s;
    float hi = 0;
    float mi = 0;
    float si = 0;

    if(list.count() == 3){
        h = list[0];
        m = list[1];
        s = list[2];
    }

    else{
        m = list[0];
        s = list[1];
    }

    hi = h.toFloat();
    mi = m.toFloat();
    si = s.toFloat();
    total = hi + mi/60 + si/(60*60);

    return total;

}

QVector<QString> SkiAnalyzer::createEmit(QHash<QString, QVector<QHash<QString, QString>>> data, int index, QString parameter)
{

    QString vuosi = (data[parameter][index]["year"]);
    QString nimi = (data[parameter][index]["name"]);
    QString kans = (data[parameter][index]["nationality"]);
    QString joukkue = (data[parameter][index]["team"]);
    QString aika = (data[parameter][index]["time"]);
    QString tyyppi = (data[parameter][index]["distance"]);
    QString sijoitus = (data[parameter][index]["placement"]);
    QString spuoli = (data[parameter][index]["sex"]);
    QString paikkakunta = (data[parameter][index]["locality"]);
    QString syntymavuosi = (data[parameter][index]["birthYear"]);

    float matka = tyyppi.remove(QRegExp(R"([\D])")).toFloat();
    QString kesk;

    if (aika != 0){
        float keskinopeus = matka/timeToInt(aika);
        kesk = QString::number(keskinopeus);

        // rounding average speed to 2 decimals.
        if (kesk.lastIndexOf(QChar('.'))){
            int pos = kesk.lastIndexOf(QChar('.'));
            kesk = kesk.left(pos+3);
        }
    }
    else{
        kesk = "not available";
    }

    QVector<QString> temp = QVector<QString>() << vuosi << parameter << aika
                                               << sijoitus << spuoli << nimi << paikkakunta
                                               << kans << syntymavuosi << joukkue << kesk;
    return temp;
}

QVector<QString>SkiAnalyzer::splitName(QString name){
    QStringList list;
    list = name.split(" ");
    QVector<QString> rtrn;

    if(list.length() == 2){
        rtrn << list[0];
        rtrn << list[1];
    }
    // in case of incomplete data or typo:
    else{
        rtrn << list[0] << "";
    }
    return rtrn;
}
