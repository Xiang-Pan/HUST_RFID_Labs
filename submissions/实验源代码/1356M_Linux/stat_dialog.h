#ifndef STAT_DIALOG_H
#define STAT_DIALOG_H

#include <QDialog>
#include <QChartView>
#include <QSet>
#include <QBarSet>
#include <QChart>
#include <QBarSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QWindow>
#include "sqlite.h"

using namespace QtCharts;

namespace Ui {
class stat_dialog;
}

class stat_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit stat_dialog(QWidget *parent = nullptr);
    Sqlite *sql;
    ~stat_dialog();

private:
    Ui::stat_dialog *ui;
};

#endif // STAT_DIALOG_H
