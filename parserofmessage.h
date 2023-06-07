#ifndef PARSEROFMESSAGE_H
#define PARSEROFMESSAGE_H
#include <QByteArray>
#include <QFile>
#include <QtEndian>
#include <QList>
#include <QDebug>


class ParserOfMessage: public QObject
{
    Q_OBJECT
public:
    ParserOfMessage();
    void Parser(const QVector<quint8>&payload, quint8 MessageID);
    void parser_102(const QVector<quint8>&payload);
    void parser_149(const QVector<quint8>&payload);
    void parser_152(const QVector<quint8>&payload);
    void parser_154(const QVector<quint8>&payload);
    void parser_156(const QVector<quint8>&payload);
    void parser_157(const QVector<quint8>&payload);
    void parser_159(const QVector<quint8>&payload);

    enum ID {
           id_102 = 0x66,
           id_149 = 0x95,
           id_154 = 0x9A,
           id_156 = 0x9C,
           id_157 = 0x9D,
           id_159 = 0x9F
        };



signals:
   void writer(QList<QVariant> &data);

private:
    int i;
    int n;
    int f;
    static constexpr quint8 header_1 {0x57};
    static constexpr quint8 header_2 {0xf1};
    ID msg = id_102;
    QString results;
    QList<QVariant> data;
};



#endif // PARSEROFMESSAGE_H
