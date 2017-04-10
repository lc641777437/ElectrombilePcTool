#include "dialog_devicelist.h"
#include "ui_dialog_devicelist.h"
#include "mainwindow.h"
#include "dialog_baidumap.h"
#include "ui_mainwindow.h"
#include "http_operate.h"

#include <QtNetwork>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMenu>
#include <QInputDialog>
#include <QFileDialog>
#include <QStandardItem>
#include <QTextEdit>

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
        row = 0;
        isFirst = false;

        file.close();
    }
}

void Dialog_deviceList::on_pushButton_StartLoad_clicked()
{
    qDebug()<<"on_pushButton_StartLoad_clicked";
    if(ui->tableWidget->rowCount() == 0){
        QMessageBox::information(this, QString("小安提示"), QString("请先导入IMEI列表！\n"));
    }

    if(!isLoading)
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
        ui->tableWidget->setItem(row,9,new QTableWidgetItem(QString("-")));
    }
}

void Dialog_deviceList::on_tableWidget_cellDoubleClicked(int row, int column)
{
    qDebug() << "tableWidget_deviceState_cellDoubleClicked:" << row << column;

    if(ui->tableWidget->item(row,4) == NULL){
        return;
    }

    QString point = ui->tableWidget->item(row,4)->text();
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

void Dialog_deviceList::on_pushButton_ClearData_clicked()
{
    QMessageBox message(QMessageBox::Warning, QString("小安提示"),
                        QString("确定要清除所有设备的数据？\r\n"),
                        QMessageBox::Yes|QMessageBox::No, NULL);
    if (message.exec()==QMessageBox::No){
        return;
    }

    MainWindow *ptr = (MainWindow*)parentWidget();

    int row = this->ui->tableWidget->rowCount();
    for(int i = 0; i < row; i++) {
        if(ui->tableWidget->item(i,0) == NULL){
            continue;
        }
        QString imei = ui->tableWidget->item(i,0)->text();
        QString url = QString("https://api.leancloud.cn/1.1/classes/Bindings?where={\"IMEI\":\"%1\"}")
                      .arg(imei);
        qDebug()<<url;
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
            QMessageBox::information(this, QString("小安提示"),QString("查询Leancloud数据失败:%1\n").arg(imei));
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
            http_operate http;
            result = http.httpsOperarteLeancloud(url, NULL, "DELETE", this);
            if(result.isEmpty()){
                QMessageBox::information(this, QString("小安提示"),QString("清理Leancloud数据失败：%1\n").arg(objectid));
                continue;
            }
        }
        url = "http://" + ptr->ui->lineEdit_IP->text().toLatin1() +
                ":"+ptr->ui->lineEdit_port->text().toLatin1() +
                "/v1/imeiData/" + imei;
        qDebug()<< url;
        result = http_operate::instance().httpOperarte(url, NULL, "DELETE", this);
        qDebug()<<result;

        if(result.isEmpty()){
            continue;
        }

        jsonDocument = QJsonDocument::fromJson(result.toLatin1());
        if( jsonDocument.isNull() ){
            qDebug()<< "please check the string "<< result;
            continue;
        }
        object = jsonDocument.object();
        QJsonValue code = object.take("code");
        if(code.toInt() != 0){
            QMessageBox::information(this, QString("小安提示"),QString("清理数据库数据失败\n%1")
                                     .arg(ui->tableWidget->item(i,0)->text()));
            continue;
        }
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(QString("-")));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem("已清除"));
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(QString("-")));
        ui->tableWidget->setItem(i,4,new QTableWidgetItem(QString("-")));
        ui->tableWidget->setItem(i,5,new QTableWidgetItem(QString("-")));
        ui->tableWidget->setItem(i,6,new QTableWidgetItem(QString("-")));
        ui->tableWidget->setItem(i,7,new QTableWidgetItem(QString("-")));
        ui->tableWidget->setItem(i,8,new QTableWidgetItem(QString("-")));
        ui->tableWidget->setItem(i,9,new QTableWidgetItem(QString("-")));
    }
}

void Dialog_deviceList::on_pushButton_DeriveFile_clicked()
{
    QString filepath = QFileDialog::getSaveFileName (this,tr("导出数据"),"",tr("表格 (*.csv)"));

    QFile file(filepath);
    if(!file.open(QFile::WriteOnly | QIODevice::Text)){
        QMessageBox::information(this, QString("小安提示"),QString("导出失败！\n"));
        return;
    }

    int row = this->ui->tableWidget->rowCount();
    int col = this->ui->tableWidget->columnCount();
    QTextStream stream(&file);

    qDebug() << filepath << row << col;

    for(int i = 0;i < col;i++){
        stream << "\"" + this->ui->tableWidget->horizontalHeaderItem(i)->text() + "\"" + ",";
    }
    stream << endl;

    for(int i = 0; i < row; i++)
    {
        for(int j = 0;j < col; j++){
            if(this->ui->tableWidget->item(i,j) != NULL){
                stream << "\"\t" + this->ui->tableWidget->item(i,j)->text() + "\"" + ",";
            }else{
                stream << "-,";
            }
        }
        stream << endl;
    }

    file.close();
    QMessageBox::information(this, QString("小安提示"),QString("导出成功！\n"));
}

void Dialog_deviceList::on_checkBox_ClearData_stateChanged(int arg1)
{
    if(arg1 != 0){
        QMessageBox::information(this, QString("小安提示"),QString("已经开启清除历史数据功能！\n"));
        ui->pushButton_ClearData->setEnabled(true);
    }else{
        ui->pushButton_ClearData->setEnabled(false);
    }
}
