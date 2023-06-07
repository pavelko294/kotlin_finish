#ifndef FILEWRITER_H
#define FILEWRITER_H

#include <QFile>

#include "parserofmessage.h"

class FileWriter: public QObject
{
    Q_OBJECT
public:
    FileWriter();
    void WriteToFile(const QString &results);
    void File_102(QList<QVariant> &data);
    void File_149(QList<QVariant> &data);
    void File_154(QList<QVariant> &data);
    void File_156(QList<QVariant> &data);
    void File_157(QList<QVariant> &data);
    void File_159(QList<QVariant> &data);

public slots:

   void FormingString( QList<QVariant> &data, quint8 MessageID);

private:
  QString results;
};

#endif // FILEWRITER_H
