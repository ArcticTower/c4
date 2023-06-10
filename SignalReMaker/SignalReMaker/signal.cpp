#include "signal.h"

signal::signal()
{
    signalData = new QList<QList<int>>;
    ends = new QList<int>;
    starts = new QList<int>;
    frameCount = 0;
}
signal::~signal(){
    delete signalData;
    delete ends;
    delete starts;
}

void signal::addFrame(QByteArray data){
    QList<int> frame;
    for(QByteArray::Iterator it = data.begin();it<data.end();){
        quint8 byte1 = static_cast<quint8>(*it);
        ++it;
        quint8 byte2 = static_cast<quint8>(*it);
        ++it;
        int value = (byte1 << 8) | byte2;
        frame.append(value);
    }
    signalData->append(frame);
    frameCount++;
}

void signal::addEnd(int i){
    ends->append(i);
}

QList<int> signal::getEnds(){
    return *ends;
}

void signal::addStart(int i){
    starts->append(i);
}

QList<int> signal::getStarts(){
    return *starts;
}

QList<QList<int>> signal::getSigData(){
    return *signalData;
}

int signal::GetCurPower(int frame,int dot){
    return signalData->at(frame-1).at(dot);
}

QList<int> signal::getFrame(int i){
    if (signalData->size()<i-1){
        QList<int> res;
        return res;
    }
    return signalData->at(i-1);
}

int signal::getMaxPower(){
    QList<int>::const_iterator maxElement = std::max_element(signalData->at(0).constBegin(), signalData->at(0).constEnd());
    if (maxElement != (signalData->at(0).constEnd())){
        int maxValue = *maxElement;
        return maxValue;
    }
    return 255;
}

int signal::getRange(){
    if (signalData->isEmpty()){
        return 0;
    }
    return signalData->at(0).size()-1;
}

int signal::getFrameCount(){
    if (signalData->isEmpty()){
        return 0;
    }
    return signalData->size();
}

void signal::setPower(int point, int power, int frame){
    if (signalData->isEmpty()){
        return;
    }
    if (signalData->size()<frame-1){
        return;
    }
    if(signalData->at(frame-1).size()<point){
        return;
    }
    QList b = signalData->at(frame-1);
    b.replace(point,power);
    signalData->replace(frame-1,b);
}
