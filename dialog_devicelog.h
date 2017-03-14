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
    explicit Dialog_deviceLog(QWidget *parent, QString result);
    ~Dialog_deviceLog();

private:
    Ui::Dialog_deviceLog *ui;
};

#endif // DIALOG_DEVICELOG_H
