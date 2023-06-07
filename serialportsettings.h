#ifndef SERIALPORTSETTINGS_H
#define SERIALPORTSETTINGS_H

#include <QSerialPort>

// Данная структура инкапсулирует набор настроек последовательного порта.
struct SerialPortSettings
{
    typedef qint32 baudRate_t;

    SerialPortSettings(void);
    SerialPortSettings(baudRate_t g_BaudRate,
                       QSerialPort::DataBits g_DataBits,
                       QSerialPort::Parity g_Parity,
                       QSerialPort::StopBits g_StopBits,
                       QSerialPort::FlowControl g_FlowControl);

    baudRate_t BaudRate {115200};
    QSerialPort::DataBits DataBits {QSerialPort::Data8};
    QSerialPort::Parity Parity {QSerialPort::NoParity};
    QSerialPort::StopBits StopBits {QSerialPort::OneStop};
    QSerialPort::FlowControl FlowControl {QSerialPort::NoFlowControl};

    // При возврате true имя вызвавшего ошибку поля будет записано в culprit.
    // NB: в старых версиях Qt класс не умел настраивать параметры неоткрытого порта,
    // так что лучше передавать сюда уже открытый порт.
    bool TunePort(QSerialPort *port, QString *culprit) const;
};

#endif // SERIALPORTSETTINGS_H
