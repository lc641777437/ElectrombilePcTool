#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog_findlog.h"
#include "dialog_devicelog.h"
#include "dialog_devicelist.h"
#include "dialog_baidumap.h"

#include <QtNetwork>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMenu>
#include <QInputDialog>
#include <QStringList>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("小安宝设备查询软件");
    setWindowIcon(QIcon(" xiaoanbao.ico"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_findDevice_clicked()
{
    if(ui->lineEdit_IMEI->text().isEmpty()){
        QMessageBox::information(this, QString("小安提示"),QString("请输入IMEI号码！\n"));
        return;
    }

    if(ui->lineEdit_IMEI->text().length() != 15){
        QMessageBox::information(this, QString("小安提示"),QString("请输入正确的IMEI号码！\n"));
        return;
    }

    QString url = "http://" + ui->lineEdit_IP->text().toLatin1() +
                  ":"+ui->lineEdit_port->text().toLatin1() +
                  "/v1/imeiData/" + ui->lineEdit_IMEI->text().toLatin1();
    qDebug()<<url;

    QString result = httpsOperarte(url, NULL, "GET");
    qDebug()<<result;
    if(result.isEmpty()){
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(result.toLatin1());
    if( jsonDocument.isNull() )
    {
        qDebug()<< "please check the string "<< result;
        return;
    }
    QJsonObject object = jsonDocument.object();
    ui->tableWidget_deviceState->setColumnCount(1);

    QJsonValue code = object.take("code");
    if(code.isUndefined()){
        QString imei = object.take("imei").toString();
        ui->tableWidget_deviceState->setItem(0,0,new QTableWidgetItem(imei));

        int version = object.take("version").toInt();
        int version_a = version / 65536;
        int version_b = (version % 65536) / 256;
        int version_c = version % 256;
        ui->tableWidget_deviceState->setItem(1,0,new QTableWidgetItem(QString("%1.%2.%3").arg(version_a).arg(version_b).arg(version_c)));

        int state = object.take("state").toInt();
        if(state){
            ui->tableWidget_deviceState->setItem(2,0,new QTableWidgetItem("online"));
            ui->tableWidget_deviceState->item(2, 0)->setForeground(Qt::green);
        }
        else{
            ui->tableWidget_deviceState->setItem(2,0,new QTableWidgetItem("offline"));
            ui->tableWidget_deviceState->item(2, 0)->setForeground(Qt::red);
        }

        QDateTime time = QDateTime::fromTime_t(object.take("timestamp").toInt());
        ui->tableWidget_deviceState->setItem(3,0,new QTableWidgetItem(time.toString("yyyy.MM.dd hh:mm:ss dddd")));

        double lat = object.take("latitude").toDouble();
        double lon = object.take("longitude").toDouble();
        ui->tableWidget_deviceState->setItem(4,0,new QTableWidgetItem(QString::number(lat, 10, 6) + ","+ QString::number(lon, 10, 6)));


        int GSM = object.take("GSM").toInt();
        ui->tableWidget_deviceState->setItem(5,0,new QTableWidgetItem(QString("%1").arg(GSM)));
        if(GSM > 20){
            ui->tableWidget_deviceState->item(5, 0)->setBackground(Qt::green);
        }
        else if(GSM > 10){
            ui->tableWidget_deviceState->item(5, 0)->setBackground(Qt::yellow);
        }
        else{
            ui->tableWidget_deviceState->item(5, 0)->setBackground(Qt::red);
        }

        int MAX_GSM = object.take("MAXGSM").toInt();
        ui->tableWidget_deviceState->setItem(6,0,new QTableWidgetItem(QString("%1").arg(MAX_GSM)));
        if(MAX_GSM > 20){
            ui->tableWidget_deviceState->item(6, 0)->setBackground(Qt::green);
        }
        else if(MAX_GSM > 10){
            ui->tableWidget_deviceState->item(6, 0)->setBackground(Qt::yellow);
        }
        else{
            ui->tableWidget_deviceState->item(6, 0)->setBackground(Qt::red);
        }

        int voltage = object.take("voltage").toInt();
        ui->tableWidget_deviceState->setItem(7,0,new QTableWidgetItem(QString("%1").arg(voltage)));

        int course = object.take("course").toInt();
        ui->tableWidget_deviceState->setItem(8,0,new QTableWidgetItem(QString("%1").arg(course)));

        int speed = object.take("speed").toInt();
        ui->tableWidget_deviceState->setItem(9,0,new QTableWidgetItem(QString("%1").arg(speed)));
    }
    else{
        ui->tableWidget_deviceState->setItem(1,0,new QTableWidgetItem(QString("-")));
        ui->tableWidget_deviceState->setItem(2,0,new QTableWidgetItem("未登录"));
        ui->tableWidget_deviceState->setItem(3,0,new QTableWidgetItem(QString("-")));
        ui->tableWidget_deviceState->setItem(4,0,new QTableWidgetItem(QString("-")));
        ui->tableWidget_deviceState->setItem(5,0,new QTableWidgetItem(QString("-")));
        ui->tableWidget_deviceState->setItem(6,0,new QTableWidgetItem(QString("-")));
        ui->tableWidget_deviceState->setItem(7,0,new QTableWidgetItem(QString("-")));
        ui->tableWidget_deviceState->setItem(8,0,new QTableWidgetItem(QString("-")));
        ui->tableWidget_deviceState->setItem(9,0,new QTableWidgetItem(QString("-")));
    }
    ui->tableWidget_deviceState->resizeColumnsToContents();
}

void MainWindow::on_pushButton_FindDeviceLog_clicked()
{
    if(ui->lineEdit_IMEI->text().isEmpty()){
        QMessageBox::information(this, QString("小安提示"),QString("请输入IMEI号码！\n"));
        return;
    }

    if(ui->lineEdit_IMEI->text().length() != 15){
        QMessageBox::information(this, QString("小安提示"),QString("请输入正确的IMEI号码！\n"));
        return;
    }

    Dialog_findLog eventtime(this);
    if(eventtime.exec() != 0)
    {
        return;
    }
    QString starttime = QString::number(eventtime.starttime.toTime_t());
    QString endtime = QString::number(eventtime.endtime.toTime_t());

    QString url = "http://" + ui->lineEdit_IP->text().toLatin1() +
                  ":"+ui->lineEdit_port->text().toLatin1() +
                  "/v1/deviceEvent/" + ui->lineEdit_IMEI->text().toLatin1() +
                  "?start=" + starttime + "&end=" + endtime;
    qDebug()<<url;

    QString result = httpsOperarte(url, NULL, "GET");
    qDebug()<<result;
    if(!result.isEmpty()){
        Dialog_deviceLog event(this, result);
        event.exec();
    }
}

void MainWindow::on_pushButton_FindDeviceList_clicked()
{
    Dialog_deviceList deviceList(this);
    if(deviceList.exec() != 0)
    {
        return;
    }
}

void MainWindow::on_pushButton_GPS_clicked()
{
    if(ui->lineEdit_IMEI->text().isEmpty()){
        QMessageBox::information(this, QString("小安提示"),QString("请输入IMEI号码！\n"));
        return;
    }

    if(ui->lineEdit_IMEI->text().length() != 15){
        QMessageBox::information(this, QString("小安提示"),QString("请输入正确的IMEI号码！\n"));
        return;
    }

    QString url = "http://" + ui->lineEdit_IP->text().toLatin1() +
                  ":"+ui->lineEdit_port->text().toLatin1() +
                  "/v1/device";
    QString data = QString("{\"imei\":\"%1\",\"cmd\":{\"c\":17}}").arg(ui->lineEdit_IMEI->text());
    QString result = httpsOperarte(url, data, "POST");
    if(result.isEmpty()){
        return;
    }
    QJsonDocument jsonDocument = QJsonDocument::fromJson(result.toLatin1());
    if( jsonDocument.isNull() ){
        qDebug()<< "please check the string "<< result;
        return;
    }
    QJsonObject object = jsonDocument.object();

    QJsonValue gps = object.take("result");
    if(gps.isObject()){
        QJsonObject obj = gps.toObject();
        QMessageBox::information(this, QString("小安提示"),QString("GPSSignal: ") + QString("%1").arg(obj.take("GPSSignal").toDouble()));
    }
}

void MainWindow::on_pushButton_GSM_clicked()
{
    if(ui->lineEdit_IMEI->text().isEmpty()){
        QMessageBox::information(this, QString("小安提示"),QString("请输入IMEI号码！\n"));
        return;
    }

    if(ui->lineEdit_IMEI->text().length() != 15){
        QMessageBox::information(this, QString("小安提示"),QString("请输入正确的IMEI号码！\n"));
        return;
    }

    QString url = "http://" + ui->lineEdit_IP->text().toLatin1() +
                  ":"+ui->lineEdit_port->text().toLatin1() +
                  "/v1/device";
    QString data = QString("{\"imei\":\"%1\",\"cmd\":{\"c\":18}}").arg(ui->lineEdit_IMEI->text());
    QString result = httpsOperarte(url, data, "POST");
    if(result.isEmpty()){
        return;
    }
    QJsonDocument jsonDocument = QJsonDocument::fromJson(result.toLatin1());
    if( jsonDocument.isNull() ){
        qDebug()<< "please check the string "<< result;
        return;
    }
    QJsonObject object = jsonDocument.object();

    QJsonValue gsm = object.take("result");
    if(gsm.isObject()){
        QJsonObject obj = gsm.toObject();
        QMessageBox::information(this, QString("小安提示"),QString("GSMSignal: ") + QString("%1").arg(obj.take("GSMSignal").toDouble()));
    }
}

void MainWindow::on_pushButton_LOG_clicked()
{
    if(ui->lineEdit_IMEI->text().isEmpty()){
        QMessageBox::information(this, QString("小安提示"),QString("请输入IMEI号码！\n"));
        return;
    }

    if(ui->lineEdit_IMEI->text().length() != 15){
        QMessageBox::information(this, QString("小安提示"),QString("请输入正确的IMEI号码！\n"));
        return;
    }

    QString url = "http://" + ui->lineEdit_IP->text().toLatin1() +
                  ":"+ui->lineEdit_port->text().toLatin1() +
                  "/v1/device";
    QString data = QString("{\"imei\":\"%1\",\"cmd\":{\"c\":20}}").arg(ui->lineEdit_IMEI->text());
    QString result = httpsOperarte(url, data, "POST");
    if(result.isEmpty()){
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(result.toLatin1());
    if( jsonDocument.isNull() ){
        qDebug()<< "please check the string "<< result;
        return;
    }
    QJsonObject object = jsonDocument.object();

    QJsonValue log = object.take("result");
    if(log.isObject()){
        QJsonObject obj = log.toObject();
        QMessageBox::information(this, QString("小安提示"), obj.take("log").toString());
    }
}

void MainWindow::on_pushButton_SETTING_clicked()
{
    if(ui->lineEdit_IMEI->text().isEmpty()){
        QMessageBox::information(this, QString("小安提示"),QString("请输入IMEI号码！\n"));
        return;
    }

    if(ui->lineEdit_IMEI->text().length() != 15){
        QMessageBox::information(this, QString("小安提示"),QString("请输入正确的IMEI号码！\n"));
        return;
    }

    QString url = "http://" + ui->lineEdit_IP->text().toLatin1() +
                  ":"+ui->lineEdit_port->text().toLatin1() +
                  "/v1/device";
    QString data = QString("{\"imei\":\"%1\",\"cmd\":{\"c\":16}}").arg(ui->lineEdit_IMEI->text());
    QString result = httpsOperarte(url, data, "POST");
    if(result.isEmpty()){
        return;
    }
    QJsonDocument jsonDocument = QJsonDocument::fromJson(result.toLatin1());
    if( jsonDocument.isNull() ){
        qDebug()<< "please check the string "<< result;
        return;
    }
    QJsonObject object = jsonDocument.object();

    QJsonValue battery = object.take("result");
    if(battery.isObject()){
        QJsonObject obj = battery.toObject();

        QString msg;
        msg += "Server: " + obj.take("server").toString() + "\n";

        if(obj.take("defend").toInt() == 1){
            msg += "Defend: on\n";
        }
        else{
            msg += "Defend: off\n";
        }

        QJsonValue autodefend = obj.take("autodefend");
        QJsonObject autolock = autodefend.toObject();
        if(autolock.take("sw").toInt() == 1){
            msg += "Autolock: on\n";
        }
        else{
            msg += "Autolock: off\n";
        }

        msg+= "Period: " + QString("%1").arg(autolock.take("period").toInt());

        QMessageBox::information(this, QString("小安提示"),msg);
    }
}

void MainWindow::on_pushButton_BATTERY_clicked()
{
    if(ui->lineEdit_IMEI->text().isEmpty()){
        QMessageBox::information(this, QString("小安提示"),QString("请输入IMEI号码！\n"));
        return;
    }

    if(ui->lineEdit_IMEI->text().length() != 15){
        QMessageBox::information(this, QString("小安提示"),QString("请输入正确的IMEI号码！\n"));
        return;
    }

    QString url = "http://" + ui->lineEdit_IP->text().toLatin1() +
                  ":"+ui->lineEdit_port->text().toLatin1() +
                  "/v1/device";
    QString data = QString("{\"imei\":\"%1\",\"cmd\":{\"c\":6}}").arg(ui->lineEdit_IMEI->text());
    QString result = httpsOperarte(url, data, "POST");
    if(result.isEmpty()){
        return;
    }
    QJsonDocument jsonDocument = QJsonDocument::fromJson(result.toLatin1());
    if( jsonDocument.isNull() ){
        qDebug()<< "please check the string "<< result;
        return;
    }
    QJsonObject object = jsonDocument.object();

    QJsonValue battery = object.take("result");
    if(battery.isObject()){
        QJsonObject obj = battery.toObject();
        QMessageBox::information(this, QString("小安提示"),QString("percent: ") + QString("%1").arg(obj.take("percent").toInt()));
    }
}

void MainWindow::on_pushButton_REBOOT_clicked()
{
    if(ui->lineEdit_IMEI->text().isEmpty()){
        QMessageBox::information(this, QString("小安提示"),QString("请输入IMEI号码！\n"));
        return;
    }

    if(ui->lineEdit_IMEI->text().length() != 15){
        QMessageBox::information(this, QString("小安提示"),QString("请输入正确的IMEI号码！\n"));
        return;
    }

    QString url = "http://" + ui->lineEdit_IP->text().toLatin1() +
                  ":"+ui->lineEdit_port->text().toLatin1() +
                  "/v1/device";
    QString data = QString("{\"imei\":\"%1\",\"cmd\":{\"c\":21}}").arg(ui->lineEdit_IMEI->text());
    QString result = httpsOperarte(url, data, "POST");
    if(result.isEmpty()){
        return;
    }
    QMessageBox::information(this, QString("小安提示"),QString("reboot."));
}

void MainWindow::on_pushButton_UPGRADE_clicked()
{
    if(ui->lineEdit_IMEI->text().isEmpty()){
        QMessageBox::information(this, QString("小安提示"),QString("请输入IMEI号码！\n"));
        return;
    }

    if(ui->lineEdit_IMEI->text().length() != 15){
        QMessageBox::information(this, QString("小安提示"),QString("请输入正确的IMEI号码！\n"));
        return;
    }

    QMessageBox::information(this, QString("小安提示"),QString("此功能目前不支持\n"));
}

void MainWindow::on_tableWidget_deviceState_cellDoubleClicked(int row, int column)
{
    qDebug() << "tableWidget_deviceState_cellDoubleClicked:" << row << column;
    QString point = ui->tableWidget_deviceState->item(4,0)->text();
    QStringList sl = point.split(",");
    if(point.length() >= 2){
        double lon = sl.last().toDouble();
        double lat = sl.first().toDouble();
        Dialog_baiduMap baidu(this, lon, lat);
        baidu.exec();
    }
}

QString MainWindow::httpsOperarte(const QString &url, const QString &data, const QString &type)
{
    QNetworkRequest _request;
    _request.setUrl(QUrl(url));
    QSslConfiguration _sslCon = _request.sslConfiguration();
    _sslCon.setPeerVerifyMode(QSslSocket::VerifyNone);
    _request.setSslConfiguration(_sslCon);
    _request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

    QNetworkAccessManager m_NtwkAccManager;
    QNetworkReply *_reply;
    if(type == "POST"){
        _reply = m_NtwkAccManager.post(_request, data.toLatin1());
    }

    if(type == "GET"){
        _reply = m_NtwkAccManager.get(_request);
    }

    if(type == "DELETE"){
        _reply = m_NtwkAccManager.deleteResource(_request);
    }

    if(type == "PUT"){
        _reply = m_NtwkAccManager.put(_request, data.toLatin1());
    }
    _reply->ignoreSslErrors();

    QTime _t;
    _t.start();

    int TIMEOUT = (5 * 1000);
    bool _timeout = false;

    while (!_reply->isFinished()){
        QApplication::processEvents();
        if (_t.elapsed() >= TIMEOUT) {
            _timeout = true;
            break;
        }
    }

    QString _result;
    if (!_timeout && _reply->error() == QNetworkReply::NoError){
        _result = _reply->readAll();
    }
    else{
        QMessageBox::information(this, QString("小安提示"),QString("访问失败：") + QString::number(_reply->error()) + "!\n");
    }
    _reply->deleteLater();

    return _result;
}
