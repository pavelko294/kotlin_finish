#include "serialportsettings.h"

SerialPortSettings::SerialPortSettings()
{
}

SerialPortSettings::SerialPortSettings(SerialPortSettings::baudRate_t g_BaudRate,
                                       QSerialPort::DataBits g_DataBits,
                                       QSerialPort::Parity g_Parity,
                                       QSerialPort::StopBits g_StopBits,
                                       QSerialPort::FlowControl g_FlowControl) :

    #define SC_INIT_MEMBER(_X) _X {g_ ## _X}
    SC_INIT_MEMBER(BaudRate),
    SC_INIT_MEMBER(DataBits),
    SC_INIT_MEMBER(Parity),
    SC_INIT_MEMBER(StopBits),
    SC_INIT_MEMBER(FlowControl)
  #undef SC_INIT_MEMBER
{
}

bool SerialPortSettings::TunePort(QSerialPort *port, QString *culprit) const
{
    Q_ASSERT(port);

#define SC_SET_SETTING(_X) \
    if(!port->set ## _X(_X)) \
    { \
    if(culprit) \
    (*culprit) = #_X; \
    return true; \
}
    SC_SET_SETTING(BaudRate);
    SC_SET_SETTING(DataBits);
    SC_SET_SETTING(Parity);
    SC_SET_SETTING(StopBits);
    SC_SET_SETTING(FlowControl);

#undef SC_SET_SETTING

    return false;
}
