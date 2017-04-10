#include "dialog_baidumap.h"
#include "ui_dialog_baidumap.h"
#include "mainwindow.h"

#include <QDir>
#include <QDebug>
#include <QUrlQuery>
#include <QWebFrame>
#include <QTextCodec>

Dialog_baiduMap::Dialog_baiduMap(QWidget *parent, double longitude, double latitude) :
    QDialog(parent),
    ui(new Ui::Dialog_baiduMap)
{
    ui->setupUi(this);
    connect(ui->webView->page()->mainFrame(), SIGNAL(loadFinished(bool)),this, SLOT(baiduMapLoadFinish()));
    ui->webView->page()->mainFrame()->addToJavaScriptWindowObject("ReinforcePC", this);
    lat = latitude;
    lon = longitude;

    QString filePath = QFileInfo("./mapFile/baiduMap.html").absoluteFilePath();
    QUrl url = "file:///" + filePath;
    qDebug()<<url;
    ui->webView->setUrl(url);
    ui->webView->show();
}

Dialog_baiduMap::~Dialog_baiduMap()
{
    delete ui;
}

void Dialog_baiduMap::baiduMapLoadFinish()
{
    QString cmd = QString("showAddress(\"%1\",\"%2\")").arg(lon).arg(lat);
    ui->webView->page()->mainFrame()->evaluateJavaScript(cmd);
}
