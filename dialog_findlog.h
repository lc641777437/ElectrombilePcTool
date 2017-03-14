#ifndef DIALOG_FINDLOG_H
#define DIALOG_FINDLOG_H

#include <QDialog>
#include <QDateTime>

namespace Ui {
class Dialog_findLog;
}

class Dialog_findLog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_findLog(QWidget *parent = 0);
    ~Dialog_findLog();
    QDateTime starttime;
    QDateTime endtime;

private:
    Ui::Dialog_findLog *ui;

private slots:
    void accept();
    void reject();
};

#endif // DIALOG_FINDLOG_H
