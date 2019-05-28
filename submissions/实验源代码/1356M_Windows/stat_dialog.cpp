#include "stat_dialog.h"
#include "ui_stat_dialog.h"

stat_dialog::stat_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::stat_dialog)
{
    ui->setupUi(this);
//    QBarSet *set0 = new QBarSet("Books");

//    QString sql_cmd="SELECT booksID,count(*) FROM record_15693 group by booksID";
//    QSqlQuery query;
//    query.exec(sql_cmd);

//    if(!query.next())
//    {
//        return;
//    }
//    /*计算record表中数据行数*/
//    query.last();//跳转到最后一条数据
//    int nRow = query.at() + 1;//取所在行数
//    int row = 0;
//    QStringList categories;

//    query.first();//返回第一条数据
//    do
//    {
//        *set0 << query.value(1).toInt();
//        categories << query.value(0).toString();
//        row++;//行数增加
//    }while(query.next());

//    //*set0 << 1 << 2 << 3 << 4 << 5 << 6;

//    QBarSeries *series = new QBarSeries();
//    series->append(set0);

//    QChart *chart = new QChart();
//    chart->addSeries(series);
//    chart->setTitle("Simple barchart example");
//    chart->setAnimationOptions(QChart::SeriesAnimations);


//    QBarCategoryAxis *axisX = new QBarCategoryAxis();
//    axisX->append(categories);
//    chart->addAxis(axisX, Qt::AlignBottom);
//    series->attachAxis(axisX);

//    QValueAxis *axisY = new QValueAxis();
//    axisY->setRange(0,15);
//    chart->addAxis(axisY, Qt::AlignLeft);
//    series->attachAxis(axisY);

//    chart->legend()->setVisible(true);
//    chart->legend()->setAlignment(Qt::AlignBottom);
//    QChartView *chartView = new QChartView(chart);
//    chartView->setRenderHint(QPainter::Antialiasing);

//    ui->graphicsView->setViewport(chartView);
}

stat_dialog::~stat_dialog()
{
    delete ui;
}
