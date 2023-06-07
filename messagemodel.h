#ifndef MESSAGEMODEL_H
#define MESSAGEMODEL_H

#include "binaryt.h"

#include <QAbstractTableModel>

class MessageModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit MessageModel(QObject *p = nullptr);

    enum Column
    {
        Column_name,
        Column_value,

        Column_count
    };

private:
    BinaryT::Message m_message;

public:
    void SetMessage(const BinaryT::Message &message);

    /* QAbstractTableModel */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
};

#endif // MESSAGEMODEL_H
