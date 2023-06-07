#include "serialportinfomodel.h"

SerialPortInfoModel::SerialPortInfoModel(QObject *p) :
    QAbstractTableModel(p)
{
}

void SerialPortInfoModel::Refresh()
{
    beginResetModel();
    m_availablePorts = QSerialPortInfo::availablePorts();
    endResetModel();
}

const QSerialPortInfo &SerialPortInfoModel::GetPortInfo(int idx)
{
    Q_ASSERT(0 <= idx && idx < m_availablePorts.size());
    return m_availablePorts.at(idx);
}

int SerialPortInfoModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    return m_availablePorts.size();
}

int SerialPortInfoModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    return Column_count;
}

QVariant SerialPortInfoModel::data(const QModelIndex &index, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

    if(!index.isValid())
        return QVariant();

    const Column column {static_cast<Column>(index.column())};
    if(column < 0 || column >= Column_count)
        return QVariant();

    const qint64 idx {index.row()};
    if(idx < 0 || idx >= m_availablePorts.size())
        return QVariant();

    // Пробуем извлечь данные из кэша
    const QSerialPortInfo &portInfo {m_availablePorts.at(idx)};
    switch (column)
    {
    case Column_count:
        break;

#define SC_SWITCH_CASE(_X) \
    case Column_ ## _X: return portInfo._X()
        SC_SWITCH_CASE(portName);
        SC_SWITCH_CASE(description);
        SC_SWITCH_CASE(manufacturer);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)) // "This function was introduced in Qt 5.3" © ман QSerialPortInfo::serialNumber
        SC_SWITCH_CASE(serialNumber);
#else
    case Column_serialNumber: return "[N/A]";
#endif

        SC_SWITCH_CASE(systemLocation);

#undef SC_SWITCH_CASE

    case Column_productIdentifier:
        if(portInfo.hasProductIdentifier())
            return QString("0x%1").arg(portInfo.productIdentifier(), 4, 16, QChar('0'));
        return "[N/A]";

    case Column_vendorIdentifier:
        if(portInfo.hasVendorIdentifier())
            return QString("0x%1").arg(portInfo.vendorIdentifier(), 4, 16, QChar('0'));
        return "[N/A]";
    }
    return QVariant();
}

QVariant SerialPortInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

    if(orientation != Qt::Horizontal)
        return QVariant();

    const Column column {static_cast<Column>(section)};
    switch(column)
    {
    case Column_count: break;
#define SC_TOSTRING_SWITCH_CASE(_X) \
    case Column_ ## _X: return QString::fromUtf8(#_X)

        SC_TOSTRING_SWITCH_CASE(portName);
        SC_TOSTRING_SWITCH_CASE(description);
        SC_TOSTRING_SWITCH_CASE(manufacturer);
        SC_TOSTRING_SWITCH_CASE(serialNumber);
        SC_TOSTRING_SWITCH_CASE(systemLocation);
        SC_TOSTRING_SWITCH_CASE(productIdentifier);
        SC_TOSTRING_SWITCH_CASE(vendorIdentifier);

#undef SC_TOSTRING_SWITCH_CASE
    }
    return QVariant();
}
