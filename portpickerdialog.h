#ifndef PORTPICKERDIALOG_H
#define PORTPICKERDIALOG_H

#include "serialportinfomodel.h"
#include "serialportsettings.h"

#include <QDialog>

namespace Ui { class PortPickerDialog; }

class PortPickerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PortPickerDialog(QWidget *p = nullptr);
    ~PortPickerDialog();

private:
    Ui::PortPickerDialog *const m_ui;
    SerialPortInfoModel m_serialPortInfoModel;

public:
    void AddMoreBaudRates(const QList<SerialPortSettings::baudRate_t> &nonStandardBaudRates);
    bool SetSettings(const SerialPortSettings &settings);

    bool GetPortInfo(QSerialPortInfo *info); // true - ошибка (порт не выбран)
    SerialPortSettings GetSettings(void) const;

public slots:
    void on_pushButton_refresh_clicked(void);
};

#endif // PORTPICKERDIALOG_H
