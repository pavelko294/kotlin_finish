#ifndef FREQUENCYCORRECTIONPLOT_H
#define FREQUENCYCORRECTIONPLOT_H

#include <QWidget>
#include <QApplication>

#include "qcustomplot.h"
#include "binaryt.h"


class frequencycorrectionplot : public QCustomPlot
{
public:
    frequencycorrectionplot(QWidget *parent = nullptr);
    //void SetFrequencyCorrectionMessage(const BinaryT::Message &message);

public slots:
    void updatePlot(double x, double y);

private:
//    double freqcorr;
//    QVector<double> dataforplot;

};

#endif // FREQUENCYCORRECTIONPLOT_H
