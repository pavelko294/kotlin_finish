#include "messagemodel.h"

MessageModel::MessageModel(QObject *p) :
    QAbstractTableModel(p)
{
}

void MessageModel::SetMessage(const BinaryT::Message &message)
{
    beginResetModel();
    m_message = message;
    endResetModel();
}

int MessageModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    if(m_message.IsValid())
        return m_message.GetFieldsNum();
    return 0;
}

int MessageModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return Column_count;
}

QVariant MessageModel::data(const QModelIndex &index, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

    if(!index.isValid())
        return QVariant();

    if(!m_message.IsValid())
        return QVariant();

    const Column column {static_cast<Column>(index.column())};
    if(column < 0 || column >= Column_count)
        return QVariant();

    const BinaryT::fieldId_t field {index.row()};
    {
        const int rows {m_message.GetFieldsNum()};
        if(field < 0 || field >= rows)
            return QVariant();
    }

    switch (column)
    {
    case Column_count:
        break;

    case Column_name:
        return m_message.GetFieldName(field);

    case Column_value:
        return m_message.GetFieldString(field);
    }
    return QVariant();
}

QVariant MessageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

    if(orientation != Qt::Horizontal)
        return QVariant();

    const Column column {static_cast<Column>(section)};
    switch(column)
    {
    case Column_count: break;
    case Column_name: return tr("Имя");
    case Column_value: return tr("Значение");
    }
    return QVariant();
}
