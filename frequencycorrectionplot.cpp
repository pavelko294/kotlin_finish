#include "frequencycorrectionplot.h"

frequencycorrectionplot::frequencycorrectionplot(QWidget *parent):
    QCustomPlot(parent)
{
    this->addGraph();
    this->xAxis->setLabel("Marks");
    this->yAxis->setLabel("Frequency correction");
}



//void frequencycorrectionplot::SetFrequencyCorrectionMessage(const BinaryT::Message &message)
//{
//    SC_CHECK_EQUALS(message.GetMessageType(), BinaryT::MessageType::TimeScale);

//    freqcorr = ExtractFromVar<double>(message.GetField(BinaryT::TimeScale::Field::frequencyCorr));
//    dataforplot.append(freqcorr);
//}

void frequencycorrectionplot::updatePlot(double x, double y)
{
    this->graph(0)->addData(x, y);
    this->replot();
    this->xAxis->rescale();
    this->yAxis->rescale();
}

