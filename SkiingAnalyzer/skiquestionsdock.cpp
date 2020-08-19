#include "skiquestionsdock.h"
#include "ui_skiquestionsdock.h"

SkiQuestionsDock::SkiQuestionsDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::SkiQuestionsDock)
{
    ui->setupUi(this);

    initYearsCombos();

    connect(ui->u_tabs, &QTabWidget::currentChanged, this, &SkiQuestionsDock::tabChanged);

    connect(ui->u_clearButton, &QPushButton::clicked, this, &SkiQuestionsDock::clearView);
    connect(ui->u_clearBest, &QPushButton::clicked, this, &SkiQuestionsDock::clearBest);
    connect(ui->u_clearTeams, &QPushButton::clicked, this, &SkiQuestionsDock::clearTeams);

    connect(ui->u_searchButton, &QPushButton::clicked, this, &SkiQuestionsDock::searchClicked);
    connect(ui->u_compareButton, &QPushButton::clicked, this, &SkiQuestionsDock::compareClicked);
    connect(ui->u_getTimes, &QPushButton::clicked, this, &SkiQuestionsDock::timesClicked);
    connect(ui->u_getBest, &QPushButton::clicked, this, &SkiQuestionsDock::getBestClicked);
    connect(ui->u_getDistribution, &QPushButton::clicked, this, &SkiQuestionsDock::getDistributionClicked);
    connect(ui->u_getTeams, &QPushButton::clicked, this, &SkiQuestionsDock::getTeamsClicked);
    connect(ui->u_getPredictions, &QPushButton::clicked, this, &SkiQuestionsDock::getPredictionClicked);

    connect(ui->u_fromYear, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SkiQuestionsDock::yearIndexChange);
    connect(ui->u_timesFYear, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SkiQuestionsDock::yearIndexChange);
    connect(ui->u_bestFYear, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SkiQuestionsDock::yearIndexChange);
    connect(ui->u_fromTime, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SkiQuestionsDock::timeIndexChange);

    setAttribute( Qt::WA_DeleteOnClose );
    closable = false;

}

SkiQuestionsDock::~SkiQuestionsDock()
{
    delete ui;
}

void SkiQuestionsDock::searchClicked()
{
    QVector<QString> params = QVector<QString>()
                            << QString(ui->u_fromYear->currentText())
                            << QString(ui->u_toYear->currentText())
                            << QString(ui->u_type->currentText())
                            << QString(ui->u_firstName->text())
                            << QString(ui->u_lastName->text())
                            << QString(ui->u_gender->currentText())
                            << QString(ui->u_team->text())
                            << QString(ui->u_nationality->text())
                            << QString(ui->u_locality->text())
                            << QString(ui->u_placement->currentText())
                            << QString(ui->u_fromTime->currentText())
                            << QString(ui->u_toTime->currentText());
    emit search(params);
    lockButtons();
}

void SkiQuestionsDock::compareClicked()
{
    emit clearCompare();
    QVector<QString> params = QVector<QString>()
                            << QString(ui->u_compareType->currentText())
                            << QString(ui->u_compareYear->currentText())
                            << QString(ui->u_CompareType2->currentText())
                            << QString(ui->u_compareYear2->currentText());
    emit compare(params);
    lockButtons();
}

void SkiQuestionsDock::timesClicked()
{
    // If the athlete's name is not specified then enquire the user about it.
    if (ui->u_timesForename->text() == "" && ui->u_timesLastname->text() == "") {
        QMessageBox questionBox;
        questionBox.setText("Please specify a the name of the athlete.");
        questionBox.setStandardButtons(QMessageBox::Ok);
        questionBox.setDefaultButton(QMessageBox::Ok);
        questionBox.exec();
    }
    else {
        QVector<QString> params = QVector<QString>()
                                << QString(ui->u_timesFYear->currentText())
                                << QString(ui->u_timesTYear->currentText())
                                << QString(ui->u_timesForename->text())
                                << QString(ui->u_timesLastname->text());
        emit getTimes(params);
        lockButtons();
    }
}

void SkiQuestionsDock::getBestClicked()
{
    QVector<QString> params = QVector<QString>()
                           << QString(ui->u_bestFYear->currentText())
                           << QString(ui->u_bestTYear->currentText())
                           << QString(ui->u_bestGender->currentText());
    emit getBest(params);
    lockButtons();
}

void SkiQuestionsDock::getDistributionClicked()
{
    emit distribution(QString(ui->u_countriesYear->currentText()));
    lockButtons();
}

void SkiQuestionsDock::getTeamsClicked()
{
    QVector<QString> params = QVector<QString>()
                          << QString(ui->u_teamsYear->currentText())
                          << QString(ui->u_teamsType->currentText());
    emit getTeams(params);
    lockButtons();
}

void SkiQuestionsDock::getPredictionClicked()
{
    emit getPrediction(ui->u_predictionType->currentText());
    lockButtons();
}

void SkiQuestionsDock::yearIndexChange(int)
{
    // This method adds or removes options from the signal's sender's pair
    // combobox
    QComboBox* orig = qobject_cast<QComboBox*>(sender());

    QComboBox* targ;
    // Defining the target (the pair) combobox from the name of the signal's
    // sender.
    if (orig->objectName() == "u_fromYear") targ = ui->u_toYear;
    else if (orig->objectName() == "u_timesFYear") targ = ui->u_timesTYear;
    else if (orig->objectName() == "u_bestFYear") targ = ui->u_bestTYear;
    else return;

    // Removing items if needed.
    int limit = orig->currentText().toInt();
    for (int i = (targ->count() - 1); i > 0; --i) {
        if (targ->itemText(i).toInt() < limit) {
            targ->removeItem(i);
        }
        else {
            break;
        }
    }
    // adding items if needed.
    if ( orig->currentIndex() > targ->count() - 1 ) {
        for (int i = (targ->count()); i < (orig->currentIndex() + 1); ++i) {
            targ->addItem(orig->itemText(i));
        }
    }
}

void SkiQuestionsDock::timeIndexChange(int)
{
    // This method removes options from u_totime combobox if the option count
    // in u_fromTime decreases.
    // If the option count in u_fromTime increases then this method adds
    // an appropriate amount of options to u_totime combobox.
    if ((10 - ui->u_fromTime->currentIndex()) > ui->u_toTime->count()) {
        int limit = (10 - ui->u_fromTime->currentIndex() - ui->u_toTime->count());
        for (int i = 0; i < limit; ++i) {
            ui->u_toTime->insertItem(i, QString().number(ui->u_fromTime->itemText(ui->u_fromTime->currentIndex() + i).toInt() + 1));
        }
    }
    else if ((10 - ui->u_fromTime->currentIndex()) == ui->u_toTime->count());
    else {
        int limit = ui->u_fromTime->currentIndex() - (10 - ui->u_toTime->count());
        for (int j = 0; j < limit; ++j) {
            ui->u_toTime->removeItem(0);
        }
    }
}

void SkiQuestionsDock::closeEvent(QCloseEvent *event)
{
    if (closable) event->accept();
    else event->ignore();
}

void SkiQuestionsDock::initYearsCombos()
{
    // Get end year from system clock.
    QDate date = QDate::currentDate();
    const int startYear = 1974;
    const int endYear = date.year();

    for (int i = endYear; i > startYear - 1; --i) {
        ui->u_fromYear->addItem(QString().number(i));
        ui->u_toYear->addItem(QString().number(i));
        ui->u_compareYear->addItem(QString().number(i));
        ui->u_compareYear2->addItem(QString().number(i));
        ui->u_timesFYear->addItem(QString().number(i));
        ui->u_timesTYear->addItem(QString().number(i));
        ui->u_bestFYear->addItem(QString().number(i));
        ui->u_bestTYear->addItem(QString().number(i));
        ui->u_countriesYear->addItem(QString().number(i));
        ui->u_teamsYear->addItem(QString().number(i));
    }
    ui->u_fromYear->setCurrentIndex(ui->u_fromYear->count() - 1);
    ui->u_timesFYear->setCurrentIndex(ui->u_fromYear->count() - 1);
    ui->u_bestFYear->setCurrentIndex(ui->u_fromYear->count() - 1);
}

void SkiQuestionsDock::changeTab(int index)
{
    ui->u_tabs->setCurrentIndex(index);
}

void SkiQuestionsDock::lockForUpdate()
{
    this->setDisabled(true);
}

void SkiQuestionsDock::releaseAfterUpdate()
{
    this->setDisabled(false);
}

void SkiQuestionsDock::getTimesName(QString &name)
{
    name = ui->u_timesForename->text();
    name += QString(" ") + ui->u_timesLastname->text();
}

void SkiQuestionsDock::getDistYear(QString &year)
{
    year = ui->u_countriesYear->currentText();
}

void SkiQuestionsDock::releaseButtons(int)
{
    ui->u_searchButton->setDisabled(false);
    ui->u_compareButton->setDisabled(false);
    ui->u_getTimes->setDisabled(false);
    ui->u_getBest->setDisabled(false);
    ui->u_getDistribution->setDisabled(false);
    ui->u_getTeams->setDisabled(false);
}

void SkiQuestionsDock::lockButtons()
{
    ui->u_searchButton->setDisabled(true);
    ui->u_compareButton->setDisabled(true);
    ui->u_getTimes->setDisabled(true);
    ui->u_getBest->setDisabled(true);
    ui->u_getDistribution->setDisabled(true);
    ui->u_getTeams->setDisabled(true);
}
