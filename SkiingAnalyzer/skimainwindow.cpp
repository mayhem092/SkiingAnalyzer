#include "skimainwindow.h"

Q_DECLARE_METATYPE(QVector<QString>);

SkiMainWindow::SkiMainWindow(QWidget *parent): QMainWindow(parent)
{
    qRegisterMetaType<QVector<QString>>();
    qRegisterMetaType<QHash<QString, int>>();
    qRegisterMetaType<QPair<QString,QString>>();
    setAttribute( Qt::WA_DeleteOnClose );

    // Create the SKiView class and set it up.
    m_view = new SkiView(this);
    setCentralWidget(m_view);

    connect(this, &SkiMainWindow::saveTimesChart, m_view, &SkiView::saveTimeChart);
    connect(this, &SkiMainWindow::saveDistChart, m_view, &SkiView::saveDistChart);

    // Create the SkiQuestionsDock class and set it up.
    m_dock = new SkiQuestionsDock(this);
    m_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, m_dock);

    // Make necessary connects.
    connect(m_dock, &SkiQuestionsDock::clearView, m_view, &SkiView::ClearView);
    connect(m_dock, &SkiQuestionsDock::clearCompare, m_view, &SkiView::clearCompare);
    connect(m_dock, &SkiQuestionsDock::clearBest, m_view, &SkiView::clearBestAthlete);
    connect(m_dock, &SkiQuestionsDock::clearTeams, m_view, &SkiView::clearTeams);
    connect(m_dock, &SkiQuestionsDock::tabChanged, m_view, &SkiView::tabChange);
    connect(m_view, &SkiView::tabHasChanged, m_dock, &SkiQuestionsDock::changeTab);
    connect(m_view, &SkiView::getTimesName, m_dock, &SkiQuestionsDock::getTimesName);
    connect(m_view, &SkiView::getDistYear, m_dock, &SkiQuestionsDock::getDistYear);

    m_dock->lockForUpdate();

    // Create Message box that aska the user if they wish to launch the program
    // in anonymous or normal mode.
    QMessageBox questionBox;
    questionBox.setText("Open SkiingAnalyzer in anonymous mode?");
    questionBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    questionBox.setDefaultButton(QMessageBox::No);
    auto answer = questionBox.exec();

    if (answer == QMessageBox::Yes) {
        m_anonymous = true;
    }
    else {
        m_anonymous = false;
    }

    createAnalyzerThread();

    createMenuAndToolBar();

    resize(1200, 800);
}

SkiMainWindow::~SkiMainWindow()
{
    emit stopThread();
}

void SkiMainWindow::openHelpDialog()
{
    QMessageBox helpBox;
    helpBox.setText("Read ReadMe");
    helpBox.setStandardButtons(QMessageBox::Ok);
    helpBox.setDefaultButton(QMessageBox::Ok);
    helpBox.exec();
}

void SkiMainWindow::quitProgram()
{
    close();
}

void SkiMainWindow::retrieverDataReady(int progress, int total)
{
    if (progress == 0 && total == 0) {
        // set the progress bar invisible if the loading is complete.
        m_barAct->setVisible(false);
        m_updateAct->setVisible(true);
        m_dock->releaseAfterUpdate();
    }
    else {
        m_bar->setMaximum(total);
        m_bar->setValue(progress);
    }
}

void SkiMainWindow::updateDataBaseClicked()
{
    m_updateAct->setVisible(false);
    m_barAct->setVisible(true);
    m_dock->lockForUpdate();
    emit refreshData();
}

void SkiMainWindow::createAnalyzerThread()
{
    QThread* thread = new QThread;
    m_analyzer = new SkiAnalyzer(nullptr, m_anonymous);
    m_analyzer->moveToThread(thread);

    connect(thread, &QThread::started, m_analyzer, &SkiAnalyzer::run);
    connect(this, &SkiMainWindow::stopThread, thread, &QThread::quit);
    connect(this, &SkiMainWindow::stopThread, m_analyzer, &SkiAnalyzer::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(m_analyzer, &SkiAnalyzer::addNewRow, m_view, &SkiView::Addrow);
    connect(m_analyzer, &SkiAnalyzer::dataReady, this, &SkiMainWindow::retrieverDataReady);

    connect(m_dock, &SkiQuestionsDock::search, m_analyzer, &SkiAnalyzer::handleSearchRequest);
    connect(m_dock, &SkiQuestionsDock::compare, m_analyzer, &SkiAnalyzer::handleCompareRequest);
    connect(m_dock, &SkiQuestionsDock::getTimes, m_analyzer, &SkiAnalyzer::handleTimesRequest);
    connect(m_dock, &SkiQuestionsDock::getBest, m_analyzer, &SkiAnalyzer::handleBestAthleteRequest);
    connect(m_dock, &SkiQuestionsDock::distribution, m_analyzer, &SkiAnalyzer::handleCountriesRequest);
    connect(m_dock, &SkiQuestionsDock::getTeams, m_analyzer, &SkiAnalyzer::handleTeamsRequest);
    connect(m_dock, &SkiQuestionsDock::getPrediction, m_analyzer, &SkiAnalyzer::handlePredictionRequest);

    connect(m_analyzer, &SkiAnalyzer::dataSent, m_dock, &SkiQuestionsDock::releaseButtons);
    connect(m_analyzer, &SkiAnalyzer::dataSent, m_view, &SkiView::dataReady);
    connect(this, &SkiMainWindow::refreshData, m_analyzer, &SkiAnalyzer::refreshDataStorages);

    connect(m_analyzer, &SkiAnalyzer::compareData, m_view, &SkiView::showCompareData);
    connect(m_analyzer, &SkiAnalyzer::compareNumberOfParticipants, m_view, &SkiView::showCompareNumberOfParticipants);
    connect(m_analyzer, &SkiAnalyzer::bestAthleteData, m_view, &SkiView::showBestAthleteData);
    connect(m_analyzer, &SkiAnalyzer::teamsData, m_view, &SkiView::showTeamsData);
    connect(m_analyzer, &SkiAnalyzer::timesData, m_view, &SkiView::showTimesData);
    connect(m_analyzer, &SkiAnalyzer::nationalityDistributionData, m_view, &SkiView::showNationalityDistributionData);
    connect(m_analyzer, &SkiAnalyzer::predictionData, m_view, &SkiView::showPredictionData);

    thread->start();
}

void SkiMainWindow::createMenuAndToolBar()
{
    QMenu* menu = menuBar()->addMenu(tr("&Menu"));
    QToolBar* toolBar = addToolBar(tr("Menu"));

    const QIcon saveIcon = QIcon::fromTheme("document-new", QIcon(":/floppy.png"));
    QAction *act = new QAction(saveIcon, tr("&Save time development chart"), this);
    connect(act, &QAction::triggered, this, &SkiMainWindow::saveTimesChart);
    menu->addAction(act);

    QAction *act2 = new QAction(saveIcon, tr("&Save nationality distribution chart"), this);
    connect(act2, &QAction::triggered, this, &SkiMainWindow::saveDistChart);
    menu->addAction(act2);

    const QIcon updateIcon = QIcon::fromTheme("document-new", QIcon(":/refreshlogo.png"));
    m_updateAct = new QAction(updateIcon, tr("&Refresh skiing data"), this);
    connect(m_updateAct, &QAction::triggered, this, &SkiMainWindow::updateDataBaseClicked);
    toolBar->addAction(m_updateAct);
    menu->addAction(m_updateAct);

    const QIcon closeIcon = QIcon::fromTheme("document-new", QIcon(":/closelogo.png"));
    QAction *closeAct = new QAction(closeIcon, tr("&Close"), this);
    connect(closeAct, &QAction::triggered, this, &SkiMainWindow::quitProgram);
    menu->addAction(closeAct);

    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
    const QIcon helpIcon = QIcon::fromTheme("document-new", QIcon(":/questionlogo.png"));
    QAction *helpAct = new QAction(helpIcon, tr("&Help"), this);
    connect(helpAct, &QAction::triggered, this, &SkiMainWindow::openHelpDialog);
    helpMenu->addAction(helpAct);

    m_barAct = new QWidgetAction(this);
    m_bar = new QProgressBar(this);
    m_bar->setFixedWidth(400);
    m_bar->setMinimum(0);
    m_bar->setMaximum(0);

    m_barAct->setDefaultWidget(m_bar);
    toolBar->addAction(m_barAct);
    m_updateAct->setVisible(false);
}





