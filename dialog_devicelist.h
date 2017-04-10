#ifndef DIALOG_DEVICELIST_H
#define DIALOG_DEVICELIST_H

#include <QDialog>
#include <QKeyEvent>

namespace Ui {
class Dialog_deviceList;
}

class Dialog_deviceList : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_deviceList(QWidget *parent = 0);
    ~Dialog_deviceList();

private:
    Ui::Dialog_deviceList *ui;
    int row = 0;
    bool isLoading = false;
    bool isFirst = false;
    void findDeviceStatuswithRow(const int row);

private slots:
    void on_pushButton_StartLoad_clicked();
    void on_pushButton_ReadFile_clicked();
    void slotHeaderClicked(int column);
    void keyPressEvent(QKeyEvent *event);
    void on_tableWidget_cellDoubleClicked(int row, int column);
    void on_pushButton_ClearData_clicked();
    void on_pushButton_DeriveFile_clicked();
    void on_checkBox_ClearData_stateChanged(int arg1);
};

#endif // DIALOG_DEVICELIST_H
