#ifndef HTTP_OPERATE_H
#define HTTP_OPERATE_H

#include <QMainWindow>

class http_operate
{
public:
    http_operate();

    static http_operate& instance(){
        static http_operate instance;
        return instance;
    }
    http_operate(http_operate const&)               = delete;
    void operator=(http_operate const&)   = delete;

public:
    QString httpOperarte(const QString &url, const QString &data, const QString &type, QWidget *parent);
    QString httpsOperarteLeancloud(const QString &url, const QString &data, const QString &type, QWidget *parent);

};

#endif // HTTP_OPERATE_H
