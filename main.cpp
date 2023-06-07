#include "mainwindow.h"
#include "scdebug.h"
#include "crutches.h"

#include <QApplication>

int main(int argc, char *argv[])
{
#if 0
    {
        //QFile file("C:/Users/user/Desktop/dump/2023-04-07-14-11-37");
        QFile file("/home/pavelkopytin/2023-04-07-14-11-37");
        if (!file.open(QIODevice::ReadOnly))
        {
            scWarning() << file.error() << ": " << file.errorString();
            return -1;
        }

        const QByteArray data {file.readAll()};
        scDebug() << data.size() << " bytes read";

        BinaryT::Parser parser;
        parser.SetAutoDumpStats(true);

        const quint8 *const bytes {reinterpret_cast<const quint8*>(data.constData())};
        for(int idx = 0; idx < data.size(); idx ++)
        {
            if(parser.Feed(bytes[idx]))
            {
                quint8 messageId {0};
                const QVector<quint8> payload {parser.GetPayload(&messageId)};
                scDebug() << "Got message " << messageId << ", " << payload.size() << " B payload";
                //parser.Parser(payload, messageId);

                const BinaryT::Message message {BinaryT::Message::Parse(messageId, payload)};
                scDebug() << message;
            }
        }

        return 0;
    }
#endif

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
