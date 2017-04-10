#ifndef DIALOG_DEVICELOG_H
#define DIALOG_DEVICELOG_H

#include <QDialog>

namespace Ui {
class Dialog_deviceLog;
}

class Dialog_deviceLog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_deviceLog(QWidget *parent);
    ~Dialog_deviceLog();

private slots:
    void on_pushButton_Refresh_clicked();

private:
    Ui::Dialog_deviceLog *ui;
    QString starttime;
    QString endtime;
};

#endif // DIALOG_DEVICELOG_H
