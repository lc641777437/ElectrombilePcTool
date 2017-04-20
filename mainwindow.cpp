#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog_findlog.h"
#include "dialog_devicelog.h"
#include "dialog_devicelist.h"
#include "dialog_baidumap.h"
#include "http_operate.h"

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

    if(ui->lineEdit_IMEI->text().length() != IMEI_LEN){
        QMessageBox::information(this, QString("小安提示"),QString("请输入正确的IMEI号码！\n"));
        return;
    }

    QString url = "http://" + ui->lineEdit_IP->text().toLatin1() +
                  ":"+ui->lineEdit_port->text().toLatin1() +
                  "/v1/imeiData/" + ui->lineEdit_IMEI->text().toLatin1();
    qDebug()<<url;
    QString result = http_operate::instance().httpOperarte(url, NULL, "GET", this);
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

    if(ui->lineEdit_IMEI->text().length() != IMEI_LEN){
        QMessageBox::information(this, QString("小安提示"),QString("请输入正确的IMEI号码！\n"));
        return;
    }

    Dialog_deviceLog event(this);
    event.exec();
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

    if(ui->lineEdit_IMEI->text().length() != IMEI_LEN){
        QMessageBox::information(this, QString("小安提示"),QString("请输入正确的IMEI号码！\n"));
        return;
    }

    QString url = "http://" + ui->lineEdit_IP->text().toLatin1() +
                  ":"+ui->lineEdit_port->text().toLatin1() +
                  "/v1/device";
    QString data = QString("{\"imei\":\"%1\",\"cmd\":{\"c\":17}}").arg(ui->lineEdit_IMEI->text());
    QString result = http_operate::instance().httpOperarte(url, data, "POST", this);
    qDebug()<<result;
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

    if(ui->lineEdit_IMEI->text().length() != IMEI_LEN){
        QMessageBox::information(this, QString("小安提示"),QString("请输入正确的IMEI号码！\n"));
        return;
    }

    QString url = "http://" + ui->lineEdit_IP->text().toLatin1() +
                  ":"+ui->lineEdit_port->text().toLatin1() +
                  "/v1/device";
    QString data = QString("{\"imei\":\"%1\",\"cmd\":{\"c\":18}}").arg(ui->lineEdit_IMEI->text());
    QString result = http_operate::instance().httpOperarte(url, data, "POST", this);
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

    if(ui->lineEdit_IMEI->text().length() != IMEI_LEN){
        QMessageBox::information(this, QString("小安提示"),QString("请输入正确的IMEI号码！\n"));
        return;
    }

    QString url = "http://" + ui->lineEdit_IP->text().toLatin1() +
                  ":"+ui->lineEdit_port->text().toLatin1() +
                  "/v1/device";
    QString data = QString("{\"imei\":\"%1\",\"cmd\":{\"c\":20}}").arg(ui->lineEdit_IMEI->text());
    QString result = http_operate::instance().httpOperarte(url, data, "POST", this);
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

    if(ui->lineEdit_IMEI->text().length() != IMEI_LEN){
        QMessageBox::information(this, QString("小安提示"),QString("请输入正确的IMEI号码！\n"));
        return;
    }

    QString url = "http://" + ui->lineEdit_IP->text().toLatin1() +
                  ":"+ui->lineEdit_port->text().toLatin1() +
                  "/v1/device";
    QString data = QString("{\"imei\":\"%1\",\"cmd\":{\"c\":16}}").arg(ui->lineEdit_IMEI->text());
    QString result = http_operate::instance().httpOperarte(url, data, "POST", this);
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

    if(ui->lineEdit_IMEI->text().length() != IMEI_LEN){
        QMessageBox::information(this, QString("小安提示"),QString("请输入正确的IMEI号码！\n"));
        return;
    }

    QString url = "http://" + ui->lineEdit_IP->text().toLatin1() +
                  ":"+ui->lineEdit_port->text().toLatin1() +
                  "/v1/device";
    QString data = QString("{\"imei\":\"%1\",\"cmd\":{\"c\":6}}").arg(ui->lineEdit_IMEI->text());
    QString result = http_operate::instance().httpOperarte(url, data, "POST", this);
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

    if(ui->lineEdit_IMEI->text().length() != IMEI_LEN){
        QMessageBox::information(this, QString("小安提示"),QString("请输入正确的IMEI号码！\n"));
        return;
    }

    QString url = "http://" + ui->lineEdit_IP->text().toLatin1() +
                  ":"+ui->lineEdit_port->text().toLatin1() +
                  "/v1/device";
    QString data = QString("{\"imei\":\"%1\",\"cmd\":{\"c\":21}}").arg(ui->lineEdit_IMEI->text());
    QString result = http_operate::instance().httpOperarte(url, data, "POST", this);
    if(result.isEmpty()){
        return;
    }
    QMessageBox::information(this, QString("小安提示"),QString("reboot."));
}

void MainWindow::on_pushButton_DELETE_clicked()
{
    if(ui->lineEdit_IMEI->text().isEmpty()){
        QMessageBox::information(this, QString("小安提示"),QString("请输入IMEI号码！\n"));
        return;
    }

    if(ui->lineEdit_IMEI->text().length() != IMEI_LEN){
        QMessageBox::information(this, QString("小安提示"),QString("请输入正确的IMEI号码！\n"));
        return;
    }

    QMessageBox message(QMessageBox::Warning, QString("小安提示"),
                        QString("确定要清除该设备的数据？\r\n%1").arg(ui->lineEdit_IMEI->text()),
                        QMessageBox::Yes|QMessageBox::No, NULL);
    if (message.exec()==QMessageBox::No){
        return;
    }

    QString url = QString("https://api.leancloud.cn/1.1/classes/Bindings?where={\"IMEI\":\"%1\"}").arg(ui->lineEdit_IMEI->text());
    QString result = http_operate::instance().httpsOperarteLeancloud(url, NULL, "GET", this);
    qDebug()<<result;
    if(result.isEmpty()){
        return;
    }
    QJsonDocument jsonDocument = QJsonDocument::fromJson(result.toLatin1());
    if( jsonDocument.isNull() ){
        QMessageBox::information(this, QString("小安提示"),QString("查询Leancloud数据错误\n"));
        return;
    }
    QJsonObject object = jsonDocument.object();
    QJsonValue results = object.take("results");
    if(!results.isArray()){
        QMessageBox::information(this, QString("小安提示"),QString("查询Leancloud数据失败:%1\n").arg(ui->lineEdit_IMEI->text()));
        return;
    }
    QJsonArray array = results.toArray();
    for(int i = 0; i < array.size(); i++){
        QJsonValue obj = array.at(i);
        if(!obj.isObject()){
            continue;
        }
        QJsonObject tmp = obj.toObject();
        QJsonValue objectID = tmp.take("objectId");
        QString objectid = objectID.toString();
        url = QString("https://api.leancloud.cn/1.1/classes/Bindings/%1").arg(objectid);
        qDebug()<<url;
        result = http_operate::instance().httpsOperarteLeancloud(url, NULL, "DELETE", this);
        if(result.isEmpty()){
            QMessageBox::information(this, QString("小安提示"),QString("清理Leancloud数据失败：%1\n").arg(objectid));
            continue;
        }
    }
    url = "http://" + ui->lineEdit_IP->text().toLatin1() +
            ":"+ui->lineEdit_port->text().toLatin1() +
            "/v1/imeiData/" + ui->lineEdit_IMEI->text();
    qDebug()<< url;
    result = http_operate::instance().httpOperarte(url, NULL, "DELETE", this);
    if(result.isEmpty()){
        return;
    }

    jsonDocument = QJsonDocument::fromJson(result.toLatin1());
    if( jsonDocument.isNull() ){
        qDebug()<< "please check the string "<< result;
        return;
    }
    object = jsonDocument.object();
    QJsonValue code = object.take("code");
    if(code.toInt() != 0){
        QMessageBox::information(this, QString("小安提示"),QString("清理数据库数据失败\n"));
        return;
    }

    QMessageBox::information(this, QString("小安提示"),QString("清理数据库数据成功\n"));
}

void MainWindow::on_pushButton_UPGRADE_clicked()
{
    QMessageBox message(QMessageBox::Warning, QString("小安提示"),
                        QString("确定要将该设备立即升到最新版本？\r\n%1").arg(ui->lineEdit_IMEI->text()),
                        QMessageBox::Yes|QMessageBox::No, NULL);
    if (message.exec()==QMessageBox::No){
        return;
    }
    QString url = "http://" + ui->lineEdit_IP->text().toLatin1() +
            ":"+ui->lineEdit_port->text().toLatin1() +
            "/v1/server";
    qDebug()<< url;
    QString data = QString("{\"imei\":\"%1\",\"cmd\":0}").arg(ui->lineEdit_IMEI->text());

    QString result = http_operate::instance().httpOperarte(url, data, "POST", this);
    if(result.isEmpty()){
        return;
    }
    QJsonDocument jsonDocument = QJsonDocument::fromJson(result.toLatin1());
    if( jsonDocument.isNull() ){
        qDebug()<< "please check the string "<< result;
        return;
    }
    QJsonObject object = jsonDocument.object();

    QJsonValue code = object.take("code");
    if(code.toInt() == 0){
        QMessageBox::information(this, QString("小安提示"),QString("设备正在升级，请稍后确认！"));
    }
    else{
        QMessageBox::information(this, QString("小安提示"),QString("设备升级升级失败！"));
    }
}

void MainWindow::on_pushButton_SERVER_clicked()
{
    if(ui->lineEdit_IMEI->text().isEmpty()){
        QMessageBox::information(this, QString("小安提示"),QString("请输入IMEI号码！\n"));
        return;
    }

    if(ui->lineEdit_IMEI->text().length() != IMEI_LEN){
        QMessageBox::information(this, QString("小安提示"),QString("请输入正确的IMEI号码！\n"));
        return;
    }

    QString url = "http://" + ui->lineEdit_IP->text().toLatin1() +
                  ":"+ui->lineEdit_port->text().toLatin1() +
                  "/v1/device";
    qDebug()<<url;
    bool isOK;
    QString text = QInputDialog::getText(NULL,"小安提示","请输入服务器地址：", QLineEdit::Normal,"www.xiaoan110.com:9880",&isOK);
    if(text.isEmpty() || !isOK){
        return;
    }
    qDebug()<<text;
    QRegExp domain(tr("^([a-zA-Z0-9][-a-zA-Z0-9]{0,62})(\.[a-zA-Z0-9][-a-zA-Z0-9]{0,62})(\.[a-zA-Z0-9][-a-zA-Z0-9]{0,62})(\:[0-9]{1,5})$"));
    QRegExp ip(tr("^([0-9]{1,3})(\.[0-9]{1,3}){3}(\:[0-9]{1,5})$"));
    if(!domain.exactMatch(text) && !ip.exactMatch(text)){
        QMessageBox::information(this, QString("小安提示"),QString("服务器地址错误."));
        return;
    }
    int type = 0;
    QString data = QString("{\"imei\":\"%1\",\"cmd\":{\"c\":11,\"param\":{\"type\":%2,\"server\":\"%3\"}}}")
            .arg(ui->lineEdit_IMEI->text())
            .arg(type)
            .arg(text);
    QString result = http_operate::instance().httpOperarte(url, data, "POST", this);
    if(result.isEmpty()){
        return;
    }
    qDebug()<<result;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(result.toLatin1());
    if( jsonDocument.isNull() ){
        qDebug()<< "please check the string "<< result;
        return;
    }
    QJsonObject object = jsonDocument.object();

    QJsonValue code = object.take("code");
    if(code.isUndefined()){
        QMessageBox::information(this, QString("小安提示"),QString("设置服务器失败."));
        return;
    }

    if(code.toInt() == 0 ||code.toInt() == 108 ){
        QMessageBox::information(this, QString("小安提示"),QString("设置服务器成功."));
    }
}


void MainWindow::on_tableWidget_deviceState_cellDoubleClicked(int row, int column)
{
    qDebug() << "tableWidget_deviceState_cellDoubleClicked:" << row << column;
    if(ui->tableWidget_deviceState->item(4,0) == NULL){
        return;
    }

    QString point = ui->tableWidget_deviceState->item(4,0)->text();
    if(point.isEmpty()){
        return;
    }
    QStringList sl = point.split(",");
    if(point.length() >= 2){
        double lon = sl.last().toDouble();
        double lat = sl.first().toDouble();
        Dialog_baiduMap baidu(this, lon, lat);
        baidu.exec();
    }
}

void MainWindow::on_pushButton_SETMOTOR_clicked()
{
    if(ui->lineEdit_IMEI->text().isEmpty()){
        QMessageBox::information(this, QString("小安提示"),QString("请输入IMEI号码！\n"));
        return;
    }

    if(ui->lineEdit_IMEI->text().length() != IMEI_LEN){
        QMessageBox::information(this, QString("小安提示"),QString("请输入正确的IMEI号码！\n"));
        return;
    }

    QMessageBox message(QMessageBox::Warning, QString("小安提示"),
                        QString("\t%1\r\nReset：注销为电动车\r\nApply：注册为摩托车").arg(ui->lineEdit_IMEI->text()),
                        QMessageBox::Apply|QMessageBox::Reset|QMessageBox::Cancel, NULL);
    int res = message.exec();
    if (res == QMessageBox::Cancel){
        return;
    }

    QString url = "http://" + ui->lineEdit_IP->text().toLatin1() +
            ":"+ui->lineEdit_port->text().toLatin1() +
            "/v1/motorcycle/" + ui->lineEdit_IMEI->text();
    qDebug()<< url;
    QString result;

    if (res == QMessageBox::Reset){
        QString url = "http://" + ui->lineEdit_IP->text().toLatin1() +
                ":"+ui->lineEdit_port->text().toLatin1() +
                "/v1/motorcycle/" + ui->lineEdit_IMEI->text();
        result = http_operate::instance().httpOperarte(url, NULL, "DELETE", this);
    }else{
        QString url = "http://" + ui->lineEdit_IP->text().toLatin1() +
                ":"+ui->lineEdit_port->text().toLatin1() +
                "/v1/motorcycle";
        QString data = QString("{\"imei\":\"%1\"}").arg(ui->lineEdit_IMEI->text());
        result = http_operate::instance().httpOperarte(url, data, "POST", this);
    }

    if(result.isEmpty()){
        return;
    }
    qDebug()<<result;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(result.toLatin1());
    if( jsonDocument.isNull() ){
        qDebug()<< "please check the string "<< result;
        return;
    }
    QJsonObject object = jsonDocument.object();
    QJsonValue code = object.take("code");
    if(code.toInt() != 0){
        if (res == QMessageBox::Reset){
            QMessageBox::information(this, QString("小安提示"),QString("注销失败\n"));
        }else{
            QMessageBox::information(this, QString("小安提示"),QString("注册失败\n"));
        }
    }else if (res == QMessageBox::Reset){
        QMessageBox::information(this, QString("小安提示"),QString("注销成功\n"));

    }else{
        QMessageBox::information(this, QString("小安提示"),QString("注册成功\n"));

    }
}
