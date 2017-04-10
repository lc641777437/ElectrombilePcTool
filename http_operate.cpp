#include "http_operate.h"

#include <QtNetwork>
#include <QMessageBox>
#include <QApplication>

http_operate::http_operate(){
    return;
}

QString http_operate::httpOperarte(const QString &url, const QString &data, const QString &type, QWidget *parent)
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

    int TIMEOUT = (10 * 1000);
    bool _timeout = false;

    while (!_reply->isFinished()){
        QApplication::processEvents();
        if (_t.elapsed() >= TIMEOUT) {
            _timeout = true;
            break;
        }
    }

    QString _result;
    if(_reply->error() != QNetworkReply::NoError){
        QMessageBox::information(parent, QString("小安提示"), QString("%1\r\n访问失败：%2\r\n").arg(url).arg(_reply->error()));
    }
    else if(_timeout){
        QMessageBox::information(parent, QString("小安提示"), QString("%1\r\n无响应\r\n").arg(url));
    }
    else {
        _result = _reply->readAll();
    }
    _reply->deleteLater();

    return _result;
}

QString http_operate::httpsOperarteLeancloud(const QString &url, const QString &data, const QString &type, QWidget *parent)
{
    QNetworkRequest _request;

    _request.setUrl(QUrl(url));
    QSslConfiguration _sslCon = _request.sslConfiguration();
    _sslCon.setPeerVerifyMode(QSslSocket::VerifyNone);
    _request.setSslConfiguration(_sslCon);
    _request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));
    _request.setRawHeader(QByteArray::fromStdString("X-LC-Id"),QByteArray::fromStdString("5wk8ccseci7lnss55xfxdgj9xn77hxg3rppsu16o83fydjjn"));
    _request.setRawHeader(QByteArray::fromStdString("X-LC-Key"),QByteArray::fromStdString("yovqy5zy16og43zwew8i6qmtkp2y6r9b18zerha0fqi5dqsw"));

    QSslConfiguration conf = _request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    conf.setProtocol(QSsl::TlsV1SslV3);
    _request.setSslConfiguration(conf);

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

    int TIMEOUT = (10 * 1000);
    bool _timeout = false;

    while (!_reply->isFinished()){
        QApplication::processEvents();
        if (_t.elapsed() >= TIMEOUT) {
            _timeout = true;
            break;
        }
    }

    QString _result;
    if(_reply->error() != QNetworkReply::NoError){
        QMessageBox::information(parent, QString("小安提示"), QString("%1\r\n访问失败：%2\r\n").arg(url).arg(_reply->error()));
    }
    else if(_timeout){
        QMessageBox::information(parent, QString("小安提示"), QString("%1\r\n无响应\r\n").arg(url));
    }
    else {
        _result = _reply->readAll();
    }

    _reply->deleteLater();

    return _result;
}
