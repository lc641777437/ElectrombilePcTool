#include "dialog_devicelist.h"
#include "ui_dialog_devicelist.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtNetwork>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMenu>
#include <QInputDialog>
#include <QFileDialog>
#include <QStandardItem>

Dialog_deviceList::Dialog_deviceList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_deviceList)
{
    ui->setupUi(this);
    this->setWindowTitle("小安宝设备查询软件");
    connect(ui->tableWidget->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(slotHeaderClicked(int)));
    qDebug()<<"Dialog_deviceList";
}

Dialog_deviceList::~Dialog_deviceList()
{
    delete ui;
}

void Dialog_deviceList::slotHeaderClicked(int column)
{
    qDebug() << "slotHeaderClicked" << column;
    ui->tableWidget->sortByColumn(column);
}

void Dialog_deviceList::keyPressEvent(QKeyEvent *event)
{
    if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_F)
    {
        qDebug() << "get keyPressEvent ctrl+f";
        bool isOK;
        QString text = QInputDialog::getText(NULL,"小安宝设备查询软件","", QLineEdit::Normal,NULL,&isOK);
        if(text.isEmpty())return;
        if(ui->tableWidget->findItems(text, Qt::MatchContains).isEmpty()){
            qDebug() << "failed to find:" << text;
        }
        else
        {
            qDebug() << "succeed to find:" << text;
            int column = ui->tableWidget->findItems(text, Qt::MatchContains).first()->column();
            int row = ui->tableWidget->findItems(text, Qt::MatchContains).first()->row();
            ui->tableWidget->setCurrentCell(row, column, QItemSelectionModel::Select);
        }
    }
}

void Dialog_deviceList::on_pushButton_ReadFile_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, tr("小安宝设备查询软件"), ".", tr("Text Files(*.txt)"));
    if(!path.isEmpty())
    {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::warning(this, tr("小安宝设备查询软件"), tr("文件打开失败:\n%1").arg(path));
            return;
        }
        QTextStream stream(&file);
        QString line;
        ui->tableWidget->setRowCount(0);//clear the table
        while(!stream.atEnd())
        {
            line = stream.readLine(); // 一行一行读取，不包括“/n”的一行文本
            int rowNum = ui->tableWidget->rowCount();
            ui->tableWidget->setRowCount(rowNum+1);

            ui->tableWidget->setItem(rowNum, 0, new QTableWidgetItem(line));
            ui->tableWidget->item(rowNum, 0)->setForeground(Qt::blue);
        }
        ui->tableWidget->resizeColumnsToContents();

        file.close();
    }
}

void Dialog_deviceList::on_pushButton_StartLoad_clicked()
{
    static int row = 0;
    qDebug()<<"on_pushButton_StartLoad_clicked";
    if(!isLoading && ui->tableWidget->rowCount() != 0)
    {
        isLoading = true;
        ui->pushButton_StartLoad->setText("停止查询");
        for(; row < ui->tableWidget->rowCount() && isLoading ; row++)
        {
            findDeviceStatuswithRow(row);
        }

        if(row == ui->tableWidget->rowCount())
        {
            isLoading = false;
            ui->pushButton_StartLoad->setText("开始查询");
        }
    }
    else
    {
        row--;
        isLoading = false;
        ui->pushButton_StartLoad->setText("继续查询");
    }
}

void Dialog_deviceList::findDeviceStatuswithRow(const int row){

    MainWindow *ptr = (MainWindow*)parentWidget();

    QString imeiString = ui->tableWidget->item(row, 0)->text();
    QString url = "http://" + ptr->ui->lineEdit_IP->text().toLatin1() +
                  ":"+ptr->ui->lineEdit_port->text().toLatin1() +
                  "/v1/imeiData/" + imeiString;
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

    QJsonValue code = object.take("code");
    if(code.isUndefined()){
        int version = object.take("version").toInt();
        int version_a = version / 65536;
        int version_b = (version % 65536) / 256;
        int version_c = version % 256;
        ui->tableWidget->setItem(row,1,new QTableWidgetItem(QString("%1.%2.%3").arg(version_a).arg(version_b).arg(version_c)));

        int state = object.take("state").toInt();

        if(state){
            ui->tableWidget->setItem(row,2,new QTableWidgetItem("在   线"));
            ui->tableWidget->item(row, 2)->setForeground(Qt::green);
        }
        else{
            ui->tableWidget->setItem(row,2,new QTableWidgetItem("不在线"));
            ui->tableWidget->item(row, 2)->setForeground(Qt::red);
        }

        QDateTime time = QDateTime::fromTime_t(object.take("timestamp").toInt());
        ui->tableWidget->setItem(row,3,new QTableWidgetItem(time.toString("yyyy.MM.dd hh:mm:ss dddd")));

        double lat = object.take("latitude").toDouble();
        double lon = object.take("longitude").toDouble();
        ui->tableWidget->setItem(row,4,new QTableWidgetItem(QString::number(lat, 10, 6) + "," + QString::number(lon, 10, 6)));

        int GSM = object.take("GSM").toInt();
        QTableWidgetItem *item_GSM = new QTableWidgetItem;
        item_GSM->setData(Qt::DisplayRole, GSM);
        ui->tableWidget->setItem(row, 5, item_GSM);
        if(GSM > 20){
            ui->tableWidget->item(row, 5)->setBackground(Qt::green);
        }
        else if(GSM > 10){
            ui->tableWidget->item(row, 5)->setBackground(Qt::yellow);
        }
        else{
            ui->tableWidget->item(row, 5)->setBackground(Qt::red);
        }

        int MAX_GSM = object.take("MAXGSM").toInt();
        QTableWidgetItem *item_MAXGSM = new QTableWidgetItem;
        item_MAXGSM->setData(Qt::DisplayRole, MAX_GSM);
        ui->tableWidget->setItem(row, 6, item_MAXGSM);
        if(MAX_GSM >= 20){
            ui->tableWidget->item(row, 6)->setBackground(Qt::green);
        }
        else if(MAX_GSM >= 10){
            ui->tableWidget->item(row, 6)->setBackground(Qt::yellow);
        }
        else{
            ui->tableWidget->item(row, 6)->setBackground(Qt::red);
        }

        int voltage = object.take("voltage").toInt();
        QTableWidgetItem *item_voltage = new QTableWidgetItem;
        item_voltage->setData(Qt::DisplayRole, voltage);
        ui->tableWidget->setItem(row, 7, item_voltage);

        int course = object.take("course").toInt();
        ui->tableWidget->setItem(row,8,new QTableWidgetItem(QString("%1").arg(course)));

        int speed = object.take("speed").toInt();
        ui->tableWidget->setItem(row,9,new QTableWidgetItem(QString("%1").arg(speed)));

        if(!isFirst){
            isFirst = true;
            ui->tableWidget->resizeColumnsToContents();
        }
    }
    else{
        ui->tableWidget->setItem(row,1,new QTableWidgetItem(QString("-")));
        ui->tableWidget->setItem(row,2,new QTableWidgetItem("未登录"));
        ui->tableWidget->setItem(row,3,new QTableWidgetItem(QString("-")));
        ui->tableWidget->setItem(row,4,new QTableWidgetItem(QString("-")));
        ui->tableWidget->setItem(row,5,new QTableWidgetItem(QString("-")));
        ui->tableWidget->setItem(row,6,new QTableWidgetItem(QString("-")));
        ui->tableWidget->setItem(row,7,new QTableWidgetItem(QString("-")));
        ui->tableWidget->setItem(row,8,new QTableWidgetItem(QString("-")));
    }
}

QString Dialog_deviceList::httpsOperarte(const QString &url, const QString &data, const QString &type)
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
