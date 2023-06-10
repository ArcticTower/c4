#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "signal.h"

#include<QComboBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    cv = NULL;
    oldData = NULL;
    ls = NULL;
    chart = NULL;
    saveName = NULL;
    ui->setupUi(this);
    sig = new signal();
    dot = 0;
    frameN = 1;
    connect(&dl,SIGNAL(showChart()),this,SLOT(showC()));
    rangeFlat =0;
    ui->comboBox->addItems(QStringList() << "flat" << "mountains" << "triangle" );
    ui->comboBox->setCurrentIndex(0);
    mode = 0;
    ui->checkBox->setEnabled(false);
    disable();
}

void MainWindow::disable(){
    ui->comboBox->setEnabled(false);
    ui->horizontalSlider->setEnabled(false);
    ui->horizontalSlider_2->setEnabled(false);
    ui->horizontalSlider_3->setEnabled(false);
    ui->pushButton_7->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    ui->pushButton_4->setEnabled(false);
    ui->pushButton_5->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_6->setEnabled(false);


}

void MainWindow::enable(){
    ui->comboBox->setEnabled(true);
    ui->horizontalSlider->setEnabled(true);
    ui->horizontalSlider_2->setEnabled(true);
    ui->horizontalSlider_3->setEnabled(true);
    ui->pushButton_7->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_4->setEnabled(true);
    ui->pushButton_5->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_6->setEnabled(true);


}

MainWindow::~MainWindow()
{
    delete ui;
}

//#include <QtWidgets/QApplication>
//#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QWidget>
#include <QScatterSeries>
#include <QValueAxis>

void MainWindow::showChart(){
    //![1]
    QLineSeries *series = new QLineSeries();
    //![1]
    ls = series;

    //![2]

    if(sig){
        QList<int> f = sig->getFrame(frameN);
        int c = 0;
        for(int i:f){
            series->append(c++, i);
        }
    }
    //![2]

    //![3]
    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Frame "+ QString::number(frameN));
//    QValueAxis *yAxis = new QValueAxis();
//    qreal maxY = sig->getMaxPower()+77777;
//    yAxis->setRange(0, maxY);
//    chart->addAxis(yAxis, Qt::AlignLeft);

    //![3]
    this->chart = chart;
    //![4]
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    //![4]

    cv = chartView;
    chartView->resize(800,300);
    chartView->show();
    ui->horizontalSlider->setMinimum(0);
    ui->horizontalSlider->setMaximum(sig->getRange());
    ui->horizontalSlider_3->setMinimum(0);
    ui->horizontalSlider_3->setMaximum(0);
    ui->horizontalSlider_2->setMinimum(0);
    ui->horizontalSlider_2->setMaximum((sig->getMaxPower()+300)*2);
    ui->label_4->setText(QString::number(frameN));

    QScatterSeries *dotSeries = new QScatterSeries();
    dotSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle); // Set marker shape
    dotSeries->setMarkerSize(5.0); // Set marker size
    dotSeries->setColor(Qt::red); // Set dot color

    qreal x = 0.0; // X-coordinate of the dot
    qreal y = 3.0; // Y-coordinate of the dot
    dotSeries->append(x, y);
    chart->addSeries(dotSeries);
    chart->createDefaultAxes();
    chart->update();
}

#include <QFileDialog>
#include <QChar>

void MainWindow::showC(){
    showChart();
}

void MainWindow::on_pushButton_clicked()
{
//    open file

    QFileDialog fd;
    QString fname = QFileDialog::getOpenFileName(this, "Open file", QDir::currentPath(), "All files (*.*)");
    ui->lineEdit->setText(fname);
    QFile file(fname);

    if (!file.open(QIODevice::ReadOnly)){
        ui->statusbar->showMessage("Error file read");
        return;
    }
    enable();
    saveName = new QString(fname);
    oldData = new QByteArray();
    file.seek(0);
    *oldData = file.read(file.size());
    file.close();
    if(!ui->checkBox->isChecked()){
        dl.addSig(sig);
        dl.setHex(*oldData);

        dl.show();
        return;
    }



    //TODO: auto
//    file.seek(DATA_OFFSET);
//    qint64 size = file.size()-DATA_OFFSET;
//    QByteArray data = file.read(size);
//    bool inFrame = false;
//    QByteArray frame;
//    int count = 0;
//    for (QByteArray::Iterator it = data.begin();it<data.end()-2;it+=2){
//        QByteArray arr;
//        arr.append(data.at(count++));
//        arr.append(data.at(count++));
//        QByteArray ee;
//        ee.append('\x01');
//        ee.append('\xe4');

//        if((arr.at(0) == ee.at(1) && arr.at(1) == ee.at(0)) || (arr.at(0) == ee.at(0) && arr.at(1) == ee.at(1))){
//            if(!frame.isEmpty()){
//                sig->addFrame(frame);
//                frame.clear();
//                sig->addEnd(frame.size());
//                inFrame=false;
//            }


//        } else{
//            if(!inFrame){
//                inFrame=true;
//                sig->addStart(count-2);
//            }
//            frame.append((arr.at(0)));
//            frame.append((arr.at(1)));

////            inFrame = false;
//        }
//        arr.clear();
//    }

//    file.close();
//    showChart();
}

void MainWindow::updateChart()
{
//    QLineSeries* ls = static_cast<QLineSeries*>(cv->chart()->series().at(0));
    cv->chart()->update();


}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    //dot
    dot = position;
    ui->label_6->setText(QString::number(dot));
    int pw = sig->GetCurPower(frameN,dot);
    ui->horizontalSlider_2->setValue(pw);
    ui->horizontalSlider_3->setValue(0);
    rangeFlat = 0;
    ui->label_9->setText("0");
    QList<int> fr = sig->getFrame(frameN);
    ui->horizontalSlider_3->setMaximum(fr.size()-dot);
    ui->label_7->setText(QString::number(pw));
    QScatterSeries* dotSeries = static_cast<QScatterSeries*>(cv->chart()->series().at(1));
    dotSeries->remove(0);
    qreal newX = position;
    qreal y = 0.0;
    dotSeries->append(newX, y);
    updateChart();
}

#include <QRandomGenerator>
void MainWindow::on_horizontalSlider_2_sliderMoved(int position)
{
    //power
    if (rangeFlat == 0){
        int power = position;
        sig->setPower(dot,power,frameN);
        QLineSeries* ls = static_cast<QLineSeries*>(cv->chart()->series().at(0));
        ls->clear();
        QList<int> d = sig->getFrame(frameN);
        int c =0;
        for(int i :d){
            ls->append(c++,i);
        }
        updateChart();
        ui->label_7->setText(QString::number(power));
    } else{
        if (mode == 0){//flat
            int power = position;
            for(int i = 0;i<rangeFlat;i++){
                sig->setPower(dot+i,power,frameN);
            }
            QLineSeries* ls = static_cast<QLineSeries*>(cv->chart()->series().at(0));
            ls->clear();
            QList<int> d = sig->getFrame(frameN);
            int c =0;
            for(int i :d){
                ls->append(c++,i);
            }
            updateChart();
            ui->label_7->setText(QString::number(power));
        }
        if (mode == 1){//mountains
            int power = position;
            for(int i = 0;i<rangeFlat;i++){
                sig->setPower(dot+i,power+QRandomGenerator::global()->bounded(-300, 301),frameN);
            }

            QLineSeries* ls = static_cast<QLineSeries*>(cv->chart()->series().at(0));
            ls->clear();
            QList<int> d = sig->getFrame(frameN);
            int c =0;
            for(int i :d){
                ls->append(c++,i);
            }
            updateChart();
            ui->label_7->setText(QString::number(power));
        }
        if (mode == 2){//triangle
            int power = position;
            int half = rangeFlat/2;
            int a=1;
            for(int i = 0;i<half;i++){
                sig->setPower(dot+i,power+a,frameN);
                a+=35;
            }
            for(int i = half;i<rangeFlat;i++){
                sig->setPower(dot+i,power+a,frameN);
                a-=35;
            }
            QLineSeries* ls = static_cast<QLineSeries*>(cv->chart()->series().at(0));
            ls->clear();
            QList<int> d = sig->getFrame(frameN);
            int c =0;
            for(int i :d){
                ls->append(c++,i);
            }
            updateChart();
            ui->label_7->setText(QString::number(power));
        }
    }

}


void MainWindow::on_pushButton_2_clicked()
{
    //save file
    QFile file(*saveName);
    if (!file.open(QIODevice::WriteOnly)){
        ui->statusbar->showMessage("Error file save");
        return;
    }

    file.seek(0);
    file.write(*oldData);
    QList<int> starts = sig->getStarts();
    QList<int> lens = sig->getEnds();
    QList<QList<int>> sd = sig->getSigData();
    int c =0;
    for(QList<int> list: sd){
        QByteArray ba = convertIntListToByteArray(list);
        file.seek(starts.at(c));
        file.write(ba);
        c++;
    }
//    int a = file.size();
//    for(int i = 0;i<a;i++){
//        if(oldData->at(i)=='\xe4' || oldData->at(i)=='\x01'){
//            file.seek(i);
//            QByteArray t;
//            t.append(oldData->at(i));
//            file.write(t);
//        }
//    }
//    for(int i = 0;i<a;i++){
//        if(oldData->at(i)=='\x00' && oldData->at(i+1)=='\x23'){
//            file.seek(i);
//            QByteArray t;
//            t.append(oldData->at(i));
//            file.write(t);
//        }
//    }
//    int cc =0;
//    while (oldData->at(cc)!='\x00'&&oldData->at(cc)!='\x23') {
//        file.seek(cc);
//        QByteArray t;
//        t.append(oldData->at(cc));
//        file.write(t);
//        cc++;
//    }

    file.close();
    ui->statusbar->showMessage("SAVED",1000);

}

QByteArray MainWindow::convertIntListToByteArray(const QList<int>& list)
{
    QByteArray byteArray;
    for (int value : list) {
        if (value > 0xFFFF) {
            // value exceeds 2 bytes, set it to maximum 2 bytes value
            byteArray.append(0xFF);
            byteArray.append(0xFF);
        } else {
            // value fits in 2 bytes, just append it
            byteArray.append((value >> 8) & 0xFF); // append high byte
            byteArray.append(value & 0xFF); // append low byte
        }
    }
    return byteArray;
}


void MainWindow::on_pushButton_3_clicked()
{
    //prev frame
    if(frameN<2){
        return;
    }
    frameN--;
    QLineSeries* ls = static_cast<QLineSeries*>(cv->chart()->series().at(0));
    ls->clear();
    QList<int> d = sig->getFrame(frameN);
    int c =0;
    for(int i :d){
        ls->append(c++,i);
    }
    ui->horizontalSlider->setValue(0);
    ui->horizontalSlider->setMaximum(d.size()-1);
    chart->setTitle("Frame "+ QString::number(frameN));
    updateChart();
    ui->label_4->setText(QString::number(frameN));
}


void MainWindow::on_pushButton_4_clicked()
{
    //next frame
    int all = sig->getFrameCount();
    if(frameN+1>all){
        return;
    }
    frameN++;
    QLineSeries* ls = static_cast<QLineSeries*>(cv->chart()->series().at(0));
    ls->clear();
    QList<int> d = sig->getFrame(frameN);
    int c =0;
    for(int i :d){
        ls->append(c++,i);
    }
    ui->horizontalSlider->setValue(0);
    ui->horizontalSlider->setMaximum(d.size()-1);
    chart->setTitle("Frame "+ QString::number(frameN));
    updateChart();
    ui->label_4->setText(QString::number(frameN));

}


void MainWindow::on_pushButton_5_clicked()
{
    //set all
    auto fr = sig->getFrame(frameN);
    int cc = 0;
    for(auto a: fr){
        sig->setPower(cc,0,frameN);
        cc++;
    }

    QLineSeries* ls = static_cast<QLineSeries*>(cv->chart()->series().at(0));
    ls->clear();
    QList<int> d = sig->getFrame(frameN);
    int c =0;
    for(int i :d){
        ls->append(c++,i);
    }
    updateChart();
    ui->label_7->setText(QString::number(0));
    updateChart();
}


void MainWindow::on_pushButton_6_clicked()
{
    if(cv){
        cv->close();
        delete cv;
        cv = NULL;
    }
    if(oldData){
        delete oldData;
        oldData = NULL;
    }
    if(sig){
        delete sig;
        sig = NULL;
        sig = new signal();
    }
    if(saveName){
        delete saveName;
        saveName = NULL;
    }
    ui->lineEdit->clear();
    ui->label_4->setText("0/0");
    dl.close();
    dl.clear();
    disable();
}


void MainWindow::on_pushButton_7_clicked()
{
    if(cv){
           cv->show();
    }

}


void MainWindow::on_horizontalSlider_3_sliderMoved(int position)
{
    rangeFlat = position;
    ui->label_9->setText(QString::number(rangeFlat));
}


void MainWindow::on_comboBox_activated(int index)
{
    mode = index;
}

