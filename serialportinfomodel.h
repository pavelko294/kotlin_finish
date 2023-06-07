#ifndef SERIALPORTINFOMODEL_H
#define SERIALPORTINFOMODEL_H

#include <QAbstractTableModel>
#include <QSerialPortInfo>

class SerialPortInfoModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit SerialPortInfoModel(QObject *p = nullptr);

    enum Column
    {
        Column_portName,
        Column_description,
        Column_manufacturer,
        Column_serialNumber,
        Column_systemLocation,
        Column_productIdentifier,
        Column_vendorIdentifier,

        Column_count
    };

private:
    QList<QSerialPortInfo> m_availablePorts;

public:
    void Refresh(void);
    const QSerialPortInfo &GetPortInfo(int idx);

    /* QAbstractTableModel */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
};

#endif // SERIALPORTINFOMODEL_H
