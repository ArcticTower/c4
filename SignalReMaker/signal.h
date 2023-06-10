#ifndef SIGNAL_H
#define SIGNAL_H

#include <QObject>

#define DATA_OFFSET 0x10E

typedef struct frs{
    int start;
    int len;
    QByteArray* bytes;
}frs;

class signal
{
private:
    int frameCount;
    QList<QList<int>>* signalData;
    QList<int> *ends;
    QList<int> *starts;
public:
    signal();
    ~signal();
//    void init(QList<int> vec);
    void addFrame(QByteArray data);
    QList<int> getFrame(int i);
    int getRange();
    int getMaxPower();
    void addEnd(int i);
    void addStart(int i);
    QList<int> getEnds();
    QList<int> getStarts();
    int GetCurPower(int frame,int dot);
    int getFrameCount();
    void setPower(int point, int power, int frame);

    QList<QList<int>> getSigData();

};

#endif // SIGNAL_H
