#include "dataliner.h"
#include "ui_dataliner.h"

DataLiner::DataLiner(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataLiner)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(4);
    QStringList headers;
    headers << "Start" << "Len" << "Bytes"<<" ";
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

DataLiner::~DataLiner()
{
    delete ui;
    for(frs fs: frameStucts){
        delete fs.bytes;
    }
}

void DataLiner::setHex(QByteArray data){
    QString hexData = data.toHex(' ');
    hexData.append(' ');

    QTextCharFormat format;
        format.setForeground(Qt::black);
        QTextDocument *document = ui->textEdit->document();
        QTextCursor cursor(document);
        cursor.select(QTextCursor::Document);
        cursor.mergeCharFormat(format);
        ui->textEdit->setTextCursor(cursor);

    ui->textEdit->setPlainText(hexData);
    ui->textEdit->setReadOnly(true);
    hd = hexData;
    bytes = data;
}

void DataLiner::addSig(signal* s){
    sig = s;
}

#include <QMessageBox>

void DataLiner::on_pushButton_3_clicked()
{
    QTextCursor cursor = ui->textEdit->textCursor();
    if (!cursor.hasSelection()){
        QMessageBox msgWarning;
           msgWarning.setText("No selected text");
           msgWarning.setIcon(QMessageBox::Warning);
           msgWarning.setWindowTitle("Caution");
           msgWarning.exec();
           return;
    }
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();
    if(start == end || start+1 == end){
        QMessageBox msgWarning;
           msgWarning.setText("Invalid frame");
           msgWarning.setIcon(QMessageBox::Warning);
           msgWarning.setWindowTitle("Caution");
           msgWarning.exec();
           return;
    }
    QString selectedText  = cursor.selectedText();
    if(selectedText.at(1)==' '|| selectedText.at(selectedText.size()-1-1)==' '){
        QMessageBox msgWarning;
        msgWarning.setText("Invalid selection");
        msgWarning.setIcon(QMessageBox::Warning);
        msgWarning.setWindowTitle("Caution");
        msgWarning.exec();
        return;
    }
    selectedText.remove(' ');

    if(start+1 == hd.size()||start==hd.size()){
        start-=2;
    }
    if(end == hd.size()){
        end--;
    }
    if(hd.at(start)==' '){
        start++;
    }

    if(hd.at(end)==' '){
        end--;
    }
    QByteArray newFrame = bytes.sliced(start/3, selectedText.size()/2);
    QString hexData = newFrame.toHex(' ');

    frs frame;
    frame.start = start/3;
    frame.len = selectedText.size()/2;
    frame.bytes = new QByteArray(newFrame);

    frameStucts.append(frame);

    int newRow = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(newRow);

    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(frame.start));
    QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(frame.len));
    QTableWidgetItem *item3 = new QTableWidgetItem(frame.bytes->toHex(' '));
    QTableWidgetItem *item4 = new QTableWidgetItem("Delete");

    ui->tableWidget->setItem(newRow, 0, item1);
    ui->tableWidget->setItem(newRow, 1, item2);
    ui->tableWidget->setItem(newRow, 2, item3);
    ui->tableWidget->setItem(newRow, 3, item4);
}


void DataLiner::on_tableWidget_cellClicked(int row, int column)
{
    if(column==3){
        ui->tableWidget->removeRow(row);
        frameStucts.remove(row);
    }
    if(column == 2){
        QMessageBox msgWarning;
        msgWarning.setText(frameStucts.at(row).bytes->toHex(' '));
        msgWarning.setIcon(QMessageBox::NoIcon);
        msgWarning.setWindowTitle("Bytes");
        msgWarning.exec();
        return;
    }

}


void DataLiner::on_pushButton_2_clicked()
{
    //done
    for(frs fr: frameStucts){
        sig->addFrame(*fr.bytes);
        sig->addStart(fr.start);
        sig->addEnd(fr.len);
    }
    this->close();
    emit showChart();
}

void DataLiner::clear(){
    frameStucts.clear();
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    ui->textEdit->clear();
}

#include <QStringList>
#include <QPair>
#include <QList>

void DataLiner::on_pushButton_clicked()
{
    //auto detect
    QStringList tokens = hd.split(' ');
    int c=0;
    bool e4flag = false;
    bool spframe = false;
    int startspf = 0;
    QList<QPair<int,int>> spframes;
    for (const QString& token : tokens){
        if (token == "e4"){
            e4flag = true;
        } else
        if (token == "01"){
            if(e4flag){
                if(!spframe){
                    spframe=true;
                    startspf = c-1;
                }
            }
        } else
        {
            if(!spframe){
                e4flag=false;

            }else{
                QPair<int,int> p(startspf,c-1);
                spframes.append(p);
                spframe = false;
            }
        }

        c++;
    }
    for(QPair<int,int> p: spframes){
        int len = p.second - p.first;
        int fend = p.first-1;
        while(
              (tokens.at(fend) == "80" && tokens.at(fend-1) == "00") ||
              (tokens.at(fend) == "00" && tokens.at(fend+1) == "80")
              ){
            fend--;
        }
        int fstart = fend - len+1;
        QTextCursor cursor = ui->textEdit->textCursor();
        cursor.setPosition(fstart*3);
        cursor.setPosition(fend*3, QTextCursor::KeepAnchor);
        QTextCharFormat format;
            format.setForeground(Qt::red);
            cursor.mergeCharFormat(format);
            ui->textEdit->setTextCursor(cursor);


            frs frame;
            frame.start = fstart;


            QByteArray newFrame = bytes.sliced(fstart, len-1);
            frame.bytes = new QByteArray(newFrame);
                        frame.len = newFrame.size();

            frameStucts.append(frame);

            int newRow = ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(newRow);

            QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(frame.start));
            QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(newFrame.size()));
            QTableWidgetItem *item3 = new QTableWidgetItem(frame.bytes->toHex(' '));
            QTableWidgetItem *item4 = new QTableWidgetItem("Delete");

            ui->tableWidget->setItem(newRow, 0, item1);
            ui->tableWidget->setItem(newRow, 1, item2);
            ui->tableWidget->setItem(newRow, 2, item3);
            ui->tableWidget->setItem(newRow, 3, item4);

    }
}

