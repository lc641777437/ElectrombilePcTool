#include "dialog_devicelog.h"
#include "ui_dialog_devicelog.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QDateTime>

Dialog_deviceLog::Dialog_deviceLog(QWidget *parent, QString result) :
    QDialog(parent),
    ui(new Ui::Dialog_deviceLog)
{
    ui->setupUi(this);
    this->setWindowTitle("小安宝设备查询软件");
    QJsonDocument jsonDocument = QJsonDocument::fromJson(result.toLatin1());
    if( jsonDocument.isNull() )
    {
        qDebug()<< "please check the string "<< result;
    }
    QJsonArray array = jsonDocument.array();

    int rowNum = 0;
    for(int i = array.size() - 1;i >= 0;i--)
    {
        QJsonValue obj_value = array.at(i);
        if(obj_value.isObject())
        {
            QJsonObject obj = obj_value.toObject();
            ui->tableWidget->setRowCount(rowNum + 1);
            ui->tableWidget->setItem(rowNum, 0, new QTableWidgetItem(QDateTime::fromTime_t(obj.take("timestamp").toInt()).toString("yyyy.MM.dd hh:mm:ss dddd")));
            ui->tableWidget->setItem(rowNum, 1, new QTableWidgetItem(obj.take("event").toString()));
            rowNum++;
        }
    }
    ui->tableWidget->resizeColumnsToContents();
}

Dialog_deviceLog::~Dialog_deviceLog()
{
    delete ui;
}
