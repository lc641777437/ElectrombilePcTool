#include "dialog_findlog.h"
#include "ui_dialog_findlog.h"
#include <QDebug>
#include <QDateTime>

Dialog_findLog::Dialog_findLog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_findLog)
{
    ui->setupUi(this);
    this->setWindowTitle("小安宝设备查询软件");
    QDateTime time = QDateTime::currentDateTime();
    ui->dateTimeEdit_start->setDateTime(time);
    ui->dateTimeEdit_end->setDateTime(time);
    ui->dateTimeEdit_start->setTime(QTime::fromMSecsSinceStartOfDay(0));
}

Dialog_findLog::~Dialog_findLog()
{
    delete ui;
}

void Dialog_findLog::accept()
{
    starttime = ui->dateTimeEdit_start->dateTime();
    endtime = ui->dateTimeEdit_end->dateTime();
    Dialog_findLog::done(0);
}

void Dialog_findLog::reject()
{
    Dialog_findLog::done(-1);
}
