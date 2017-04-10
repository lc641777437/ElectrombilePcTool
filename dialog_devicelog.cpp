#include "dialog_devicelog.h"
#include "ui_dialog_devicelog.h"
#include "dialog_findlog.h"
#include "http_operate.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QDateTime>

Dialog_deviceLog::Dialog_deviceLog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_deviceLog)
{
    ui->setupUi(this);
    this->setWindowTitle("小安宝设备查询软件");

    MainWindow *ptr = (MainWindow*)parentWidget();

    Dialog_findLog eventtime(this);
    if(eventtime.exec() != 0){
        return;
    }
    starttime = QString::number(eventtime.starttime.toTime_t());
    endtime = QString::number(eventtime.endtime.toTime_t());
    QString url = "http://" + ptr->ui->lineEdit_IP->text().toLatin1() +
                  ":"+ptr->ui->lineEdit_port->text().toLatin1() +
                  "/v1/deviceEvent/" + ptr->ui->lineEdit_IMEI->text().toLatin1() +
                  "?start=" + starttime + "&end=" + endtime;
    qDebug()<<url;
    QString result = http_operate::instance().httpOperarte(url, NULL, "GET", this);
    qDebug()<< result;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(result.toLatin1());
    if( jsonDocument.isNull() ){
        qDebug()<< "please check the string "<< result;
        return;
    }
    QJsonArray array = jsonDocument.array();

    int rowNum = 0;
    for(int i = array.size() - 1;i >= 0;i--){
        QJsonValue obj_value = array.at(i);
        if(obj_value.isObject()){
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

void Dialog_deviceLog::on_pushButton_Refresh_clicked()
{
    MainWindow *ptr = (MainWindow*)parentWidget();
    qDebug()<< "refresh";
    ui->label_Refresh->setText("正在刷新...");
    QString url = "http://" + ptr->ui->lineEdit_IP->text().toLatin1() +
                  ":"+ptr->ui->lineEdit_port->text().toLatin1() +
                  "/v1/deviceEvent/" + ptr->ui->lineEdit_IMEI->text().toLatin1() +
                  "?start=" + starttime + "&end=" + endtime;
    qDebug()<<url;
    QString result = http_operate::instance().httpOperarte(url, NULL, "GET", this);
    qDebug()<< result;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(result.toLatin1());
    if( jsonDocument.isNull() ){
        qDebug()<< "please check the string "<< result;
        return;
    }
    QJsonArray array = jsonDocument.array();

    int rowNum = 0;
    for(int i = array.size() - 1;i >= 0;i--){
        QJsonValue obj_value = array.at(i);
        if(obj_value.isObject()){
            QJsonObject obj = obj_value.toObject();
            ui->tableWidget->setRowCount(rowNum + 1);
            ui->tableWidget->setItem(rowNum, 0, new QTableWidgetItem(QDateTime::fromTime_t(obj.take("timestamp").toInt()).toString("yyyy.MM.dd hh:mm:ss dddd")));
            ui->tableWidget->setItem(rowNum, 1, new QTableWidgetItem(obj.take("event").toString()));
            rowNum++;
        }
    }
    ui->tableWidget->resizeColumnsToContents();
    ui->label_Refresh->setText("刷新成功");
}
