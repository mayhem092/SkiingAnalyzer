#include "skiview.h"
#include "ui_skiview.h"

QT_CHARTS_USE_NAMESPACE

SkiView::SkiView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SkiView)
{
    ui->setupUi(this);
    CreateColumns();
    m_view = ui->u_treeView;

    // indexes of the columns that can be soted in the UI
    QVector<int> sortableColumns1 = QVector<int>() << 4 << 5 << 6 << 7 << 9;
    QVector<int> sortableColumns2 = QVector<int>() << 1;

    // Create models for all tabs and views and set them all up.
    m_model = new SkiModel(this);
    m_model->setSortableColumns(sortableColumns1);
    m_model->SetColumns(m_columns);
    m_view->setModel(m_model);
    m_view->setUniformRowHeights(true);
    m_view->setSortingEnabled(true);

    m_compareModel1 = new SkiModel(this);
    m_compareModel1->SetColumns(m_columns);
    m_compareModel1->setSortableColumns(sortableColumns1);
    ui->u_compareView1->setModel(m_compareModel1);
    ui->u_compareView1->setUniformRowHeights(true);
    ui->u_compareView1->setSortingEnabled(true);

    m_compareModel2 = new SkiModel(this);
    m_compareModel2->SetColumns(m_columns);
    m_compareModel2->setSortableColumns(sortableColumns1);
    ui->u_compareView2->setModel(m_compareModel2);
    ui->u_compareView2->setUniformRowHeights(true);
    ui->u_compareView2->setSortingEnabled(true);

    m_bestModel = new SkiModel(this);
    m_bestModel->SetColumns(m_columns);
    m_bestModel->setSortableColumns(sortableColumns1);
    ui->u_bestView->setModel(m_bestModel);
    ui->u_bestView->setUniformRowHeights(true);
    ui->u_bestView->setSortingEnabled(true);

    m_teamsModel = new SkiModel(this);
    m_teamsModel->SetColumns(m_teamsColumns);
    m_teamsModel->setSortableColumns(sortableColumns2);
    ui->u_teamsView->setModel(m_teamsModel);
    ui->u_teamsView->setUniformRowHeights(true);
    ui->u_teamsView->setSortingEnabled(true);

    // Make necessary connects.
    connect(this, &SkiView::AddNewRow, m_model, &SkiModel::AddRow);
    connect(ui->u_tabWidget, &QTabWidget::currentChanged, this, &SkiView::tabHasChanged);

    connect(this, &SkiView::compare1AddRow, m_compareModel1, &SkiModel::AddRow);
    connect(this, &SkiView::compare2AddRow, m_compareModel2, &SkiModel::AddRow);
    connect(this, &SkiView::bestAddRow, m_bestModel, &SkiModel::AddRow);
    connect(this, &SkiView::teamsAddRow, m_teamsModel, &SkiModel::AddRow);

    // Initialize some minor UI elements and set them up.
    ui->u_predictionText->setHidden(true);
    ui->u_predictionText2->setHidden(true);
    ui->u_predictionText3->setHidden(true);

    m_distLayout = new QVBoxLayout(ui->u_distParentWidget);
    ui->u_distParentWidget->setLayout(m_distLayout);
    ui->u_distLabel->setHidden(true);

    m_timesLayout = new QVBoxLayout(ui->u_timesParentWidget);
    ui->u_timesParentWidget->setLayout(m_timesLayout);

    // resize every view
    for(int i = 0; i < m_columns.size(); ++i) {
        m_view->resizeColumnToContents(i);
        ui->u_compareView1->resizeColumnToContents(i);
        ui->u_compareView2->resizeColumnToContents(i);
        ui->u_bestView->resizeColumnToContents(i);
    }

    for(int i = 0; i < m_teamsColumns.size(); ++i) ui->u_teamsView->resizeColumnToContents(i);


}

SkiView::~SkiView()
{
    delete ui;
}

void SkiView::ClearView() { m_model->clearData(); }

void SkiView::clearCompare() {

    m_compareModel1->clearData();
    m_compareModel2->clearData();
    ui->u_c1number->setText("");
    ui->u_c2number->setText("");
}

void SkiView::clearTimes() {

    while (QLayoutItem* item = m_timesLayout->takeAt(0)) {
        if (QWidget* widget = item->widget())
            widget->deleteLater();
    }
}

void SkiView::clearBestAthlete() { m_bestModel->clearData(); }

void SkiView::clearDistribution() {

    ui->u_distLabel->setHidden(true);
    ui->u_distTotalNumber->setText(" ");
    while (QLayoutItem* item = m_distLayout->takeAt(0)) {
        if (QWidget* widget = item->widget())
            widget->deleteLater();
    }
}

void SkiView::clearTeams() { m_teamsModel->clearData(); }


void SkiView::Addrow(QVector<QString> row)
{
    emit AddNewRow(row);
}

void SkiView::showCompareData(QVector<QString> row, int model)
{
    if (model == 1) {
        emit compare1AddRow(row);
    }
    else {
        emit compare2AddRow(row);
    }
}

void SkiView::showCompareNumberOfParticipants(QPair<QString, QString> numbers)
{
    ui->u_c1number->setText(numbers.first);
    ui->u_c2number->setText(numbers.second);
}

void SkiView::showTimesData(QVector<QString> row)
{
    clearTimes();

    QBarSeries *series = new QBarSeries();
    QString categories;
    if(row.length() == 0){
        categories = "No data found";
    }
    else{
        categories = "Search results";
    }


    for(int i = 0; i < row.count(); i = i+3){
        QBarSet *set = new QBarSet(row.at(i) + ", " + row.at(i+1));
        *set << row.at(i+2).toFloat();
        series->append(set);

    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Time development chart");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0,10);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QtCharts::QChartView *chartview = new QtCharts::QChartView(chart, ui->u_timesParentWidget);
    chartview->setRenderHint(QPainter::Antialiasing);
    chartview->resize(1500, 1200);

    m_timesLayout->addWidget(chartview);
}

void SkiView::showBestAthleteData(QVector<QString> row)
{
    emit bestAddRow(row);
}

void SkiView::showNationalityDistributionData(QHash<QString, int> row)
{

    clearDistribution();

    QPieSeries *series = new QPieSeries();
    QList<QString> keys = row.uniqueKeys();
    for(const QString &key : keys){
        series->append(key, row[key]);
    }

    int total = series->sum();

    QChart *chart = new QChart();
    chart->addSeries(series);

    chart->setTheme(QChart::ChartThemeBlueNcs);

    for(auto i : series->slices()){
        i->setPen(QPen(Qt::black, 1));
        i->setLabel(i->label() + " " + QString::number(i->value()));
        i->setLabelVisible(true);
    }


    chart->legend()->setAlignment(Qt::AlignLeft);

    QtCharts::QChartView *chartview = new QtCharts::QChartView(chart, nullptr);

    m_distLayout->addWidget(chartview);
    ui->u_distLabel->setHidden(false);
    ui->u_distTotalNumber->setText(QString::number(total));
}

void SkiView::showTeamsData(QVector<QString> row)
{
    emit teamsAddRow(row);
}

void SkiView::showPredictionData(QVector<QString> row)
{
    ui->u_predictionText->setHidden(false);
    ui->u_predictionText2->setHidden(false);
    ui->u_predictionText3->setHidden(false);

    ui->u_predictionTime->setText(row[2]);

    if(row[0] == "") {
        ui->u_predictionName->setText("unknown as there isn't enough data to make the prediction");
        ui->u_predictionTime->setText("");
        ui->u_predictionText3->setHidden(true);
    }
    else{
        ui->u_predictionName->setText(row[0]);
    }
    ui->u_predictionType->setText(row[1]);
}

void SkiView::dataReady(int index)
{
    // Rezising only the views that have new data.
    if (index == 1) for(int i = 0; i < m_columns.size(); ++i) m_view->resizeColumnToContents(i);
    else if (index == 2)
        for(int i = 0; i < m_columns.size(); ++i) {
        ui->u_compareView1->resizeColumnToContents(i);
        ui->u_compareView2->resizeColumnToContents(i);
    }
    else if (index == 4) for(int i = 0; i < m_columns.size(); ++i) ui->u_bestView->resizeColumnToContents(i);
    else if (index == 6) for(int i = 0; i < m_teamsColumns.size(); ++i) ui->u_teamsView->resizeColumnToContents(i);

}

void SkiView::tabChange(int index)
{
    ui->u_tabWidget->setCurrentIndex(index);
}

void SkiView::saveTimeChart()
{
    if (m_timesLayout->count() > 0){
        QLayoutItem* item = m_timesLayout->itemAt(0);
        if (QChartView* chart = qobject_cast<QChartView*>(item->widget())) {
            QString name = "";
            emit getTimesName(name);
            name = name + ".png";
            QPixmap p = chart->grab();
            p.save(name, "PNG");
        }
    }
}

void SkiView::saveDistChart()
{
    if (m_distLayout->count() > 0){
        QLayoutItem* item = m_distLayout->itemAt(0);
        if (QChartView* chart = qobject_cast<QChartView*>(item->widget())) {
            QString year = "";
            emit getDistYear(year);
            year += ".png";
            QPixmap p = chart->grab();
            p.save(year, "PNG");
        }
    }
}

void SkiView::CreateColumns()
{
    m_columns.append("Year");
    m_columns.append("Type");
    m_columns.append("Time");
    m_columns.append("Ranking");
    m_columns.append("Gender");
    m_columns.append("Name");
    m_columns.append("Locality");
    m_columns.append("Nationality");
    m_columns.append("Year of birth");
    m_columns.append("Team");
    m_columns.append("Average speed (km/h)");

    m_teamsColumns.append("Placement");
    m_teamsColumns.append("Name of the team");
    m_teamsColumns.append("Year");
    m_teamsColumns.append("Type");
    m_teamsColumns.append("Combined time");
    m_teamsColumns.append("Average time");

}
