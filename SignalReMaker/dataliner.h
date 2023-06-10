#ifndef DATALINER_H
#define DATALINER_H

#include <QWidget>
#include "signal.h"



namespace Ui {
class DataLiner;
}

class DataLiner : public QWidget
{
    Q_OBJECT

public:
    explicit DataLiner(QWidget *parent = nullptr);
    ~DataLiner();
    void setHex(QByteArray data);
    void addSig(signal* s);
    void clear();

private slots:
    void on_pushButton_3_clicked();

    void on_tableWidget_cellClicked(int row, int column);

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    signal* sig;
    Ui::DataLiner *ui;
    QString hd;
    QList<frs> frameStucts;
    QByteArray bytes;
signals:
    void showChart();
};

#endif // DATALINER_H
