#include "filewriter.h"

#include <QDateTime>

FileWriter::FileWriter()
{

}

void FileWriter::FormingString(QList<QVariant> &data, quint8 MessageID){
    switch(MessageID)
    {
    case 102:
       File_102(data);
       WriteToFile(results);
        break;

    case 149:
        File_149(data);
        WriteToFile(results);
        break;

    case 154:
        File_154(data);
        WriteToFile(results);
        break;

    case 156:
       File_156(data);
       WriteToFile(results);
        break;

    case 157:
       File_157(data);
       WriteToFile(results);
        break;
    case 159:
        File_159(data);
        WriteToFile(results);
        break;
    default:
        qDebug() << "god knows... god knows i want to break free...";
        return;
    }

}

void FileWriter::File_102(QList<QVariant> &data){
   results += "\nСообщение ID=102:\n";
   results += "Статус МПИ:" + data.at(0).toString() + "\n";
   QString slovo;
       if(data.at(0) == 0){
           slovo = "МПИ исправен, в процессе контроля сбои не зафиксированы.\n";
       }
       else if (data.at(0) == 4){
           slovo = "МПИ исправен, в процессе контроля зафиксированы сбои.\n";
       }
       else if (data.at(0) == 1){
           slovo = "МПИ неисправен, в процессе контроля сбои не зафиксированы.\n";
       }
       else if (data.at(0) == 5){
           slovo = "МПИ неисправен, в процессе контроля зафиксированы сбои.\n";
       }
       else if (data.at(0) == 3){
           slovo = "Отказ МПИ, в процессе контроля сбои не зафиксированы.\n";
       }
       else if (data.at(0) == 7){
           slovo = "Отказ МПИ, в процессе контроля зафиксированы сбои.\n";
       }
       else{
           slovo = "биты не распознаны.\n";
       }
       results += slovo;
       results += "Номер теста: " + data.at(1).toString() + "\n";
       results += "Описание нарушения, первое слово:" + data.at(3).toString() + "\n";
       results += "Описание нарушения, первое слово:" + data.at(4).toString() + "\n";
}

void FileWriter::File_149(QList<QVariant> &data){
    results += "\nСообщение ID=149\n";
    results += "Слово признаков: " + data.at(0).toString() + "\n" +
            "Время: " + data.at(1).toString() + ":" + data.at(2).toString() + ":" + data.at(3).toString() + "\n" +
            "Дробная часть времени: " + data.at(4).toString() + "\n" +
            "Дата: " + data.at(5).toString() + ":" + data.at(6).toString() + ":" + data.at(7).toString() + "\n" +
            "Latitide: " + data.at(8).toString() + "\n" +
            "Longitude: " + data.at(9).toString() + "\n" +
            "Height: " + data.at(10).toString() + "\n" +
            "Составляющая скорости по широте: " + data.at(11).toString() + "\n" +
            "Составляющая скорости по долготе: " + data.at(12).toString() + "\n" +
            "Составляющая скорости по высоте: " + data.at(13).toString()+ "\n" +
            "СКП определения плановых координат: " + data.at(14).toString() + "\n" +
            "СКП определения высоты: " + data.at(15).toString() + "\n" +
            "СКП определения времени: " + data.at(16).toString() + "\n" +
            "СКП определения плановой составляющей скорости: " + data.at(17).toString() + "\n" +
            "СКП определения вертикальной составляющей скорости: " + data.at(18).toString() + "\n";
}

void FileWriter::File_154(QList<QVariant> &data){
    results += "\nСообщение ID=154\n";
    results += "Слово признаков:" + QVariant(data.at(0)).toString() + "\n" +
            "Время: " + data.at(1).toString() + ":" + data.at(2).toString() + ":" + data.at(3).toString() + "\n" +
            "Дробная часть времени: " + data.at(4).toString() + "\n" +
            "Нормированная поправка по частоте: " + data.at(5).toString() + "\n" +
            "СКП определения времени: " + data.at(6).toString() + "\n" +
            "СКП нормированной поправки по частоте: " + data.at(7).toString() + "\n";
}

void FileWriter::File_156(QList<QVariant> &data){
    results += "\nСообщение ID=156\n";
    results += "Слово состояния: " + data.at(0).toString() + "\n";
    results += "Количество спутников: " + data.at(1).toString() + "\n";
    results += "Угол видимости: " + data.at(2).toString() + "\n";
    results += "Количество видимых спутников: " + data.at(3).toString() + "\n";
    int quantity = data.at(3).toInt();
    for (int h=0; h <= quantity; h++){
        results += "Номер НКА: " + data.at(4+5*h).toString() + "\n";
        results += "Состояние НКА с номером: " + data.at(4+5*h).toString() + ":" + data.at(5+5*h).toString() + "\n";
        results += "Возраст альманаха НКА с номером: " + data.at(4+5*h).toString() + ":" + data.at(6+5*h).toString() + "\n";
        results += "Угол возвышения НКА с номером: " + data.at(4+5*h).toString() + ":" + data.at(7+5*h).toString() + "\n";
        results += "Азимут(курс) НКА с номером: " + data.at(4+5*h).toString() + ":" + data.at(8+5*h).toString() + "\n";
    }
}

void FileWriter::File_157(QList<QVariant> &data){
     results += "\nСообщение ID=157\n";
     for (int h = 0; h <= 15; h++){
         results += "Идентификатор НКА на канале i=" + data.at(0+6*h).toString() + ":" + data.at(1+6*h).toString() + "\n";
         results += "Состояние канала i=" + data.at(0+6*h).toString() + ":" + data.at(2+6*h).toString() + "\n";
         results += "Угол места НКА i=" + data.at(0+6*h).toString() + ": " + data.at(3+6*h).toString() + "\n";
         results += "Азимут(курс) НКА i=" + data.at(0+6*h).toString() + ": " + data.at(4+6*h).toString() + "\n";
         results += "Отношение сигнал/шум для НКА i=" + data.at(0+6*h).toString() + ": " + data.at(5+6*h).toString() + "\n";
     }
}

void FileWriter::File_159(QList<QVariant> &data){
    results += "\nСообщение ID=159\n";
    results += "Слово признаков: " + data.at(0).toString() + "\n" +
            "Статус антенны: " + data.at(1).toString() + "\n" +
            "Время: " + data.at(2).toString() + ":" + data.at(3).toString() + ":" + data.at(4).toString() + "\n" +
            "Дробная часть времени: " + data.at(5).toString() + "\n" +
            "Дата: " + data.at(6).toString() + ":" + data.at(7).toString() + ":" + data.at(8).toString() + "\n" +
            "Latitide: " + data.at(9).toString() + "\n" +
            "Longitude: " + data.at(10).toString() + "\n" +
            "Height: " + data.at(11).toString() + "\n" +
            "Видимые НКА GPS: " + data.at(12).toString() + "\n" +
            "Видимые НКА ГЛОНАСС: " + data.at(13).toString() + "\n" +
            "Навигационные НКА GPS: " + data.at(14).toString() + "\n" +
            "Навигационные НКА ГЛОНАСС: " + data.at(15).toString() + "\n";
}


void FileWriter::WriteToFile(const QString &results)
{
    QFile file;
    file.setFileName("results" + QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss") + ".txt");
    if (!file.open(QIODevice::WriteOnly))
        return;
    file.write(results.toUtf8());
    file.close();
}
