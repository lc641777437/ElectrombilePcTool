#ifndef DIALOG_BAIDUMAP_H
#define DIALOG_BAIDUMAP_H

#include <QDialog>

namespace Ui {
class Dialog_baiduMap;
}

class Dialog_baiduMap : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_baiduMap(QWidget *parent, double lon, double lat);
    ~Dialog_baiduMap();

private:
    Ui::Dialog_baiduMap *ui;
    double lat = 0.0;
    double lon = 0.0;

private slots:
    void baiduMapLoadFinish();
};

#endif // DIALOG_BAIDUMAP_H
