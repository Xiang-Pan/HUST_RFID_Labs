#ifndef STAT_FORM_H
#define STAT_FORM_H

#include <QWidget>
#include <QChartView>
#include <QSet>
#include <QBarSet>
#include <QChart>
#include <QBarSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QWindow>

using namespace QtCharts;

namespace Ui {
class stat_form;
}

class stat_form : public QWidget
{
    Q_OBJECT

public:
    explicit stat_form(QWidget *parent = nullptr);
    ~stat_form();

private:
    Ui::stat_form *ui;
};

#endif // STAT_FORM_H
