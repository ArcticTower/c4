#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include "signal.h"
#include "dataliner.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void showChart();
    void updateChart();
    void disable();
    void enable();

private slots:
    void on_pushButton_clicked();

    void on_horizontalSlider_sliderMoved(int position);

    void on_horizontalSlider_2_sliderMoved(int position);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void showC();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_horizontalSlider_3_sliderMoved(int position);

    void on_comboBox_activated(int index);

private:
    QByteArray convertIntListToByteArray(const QList<int>& list);
    QByteArray* oldData;
    Ui::MainWindow *ui;
//    QFile *file;
    QLineSeries *ls;
    QChart *chart;
    QChartView *cv;
    signal *sig;
    int dot;
    int frameN;
    QString* saveName;
    DataLiner dl;
    int rangeFlat;
    int mode;
};
#endif // MAINWINDOW_H
