#include "parserofmessage.h"

ParserOfMessage::ParserOfMessage()
{

}

void ParserOfMessage::Parser(const QVector<quint8> &payload, quint8 MessageID)
{
    switch(MessageID)
    {
    case 102:
        parser_102(payload);
        break;

    case 149:
        parser_149(payload);
        break;

    case 154:
        parser_154(payload);
        break;

    case 156:
        parser_156(payload);
        break;

    case 157:
        parser_157(payload);
        break;
    case 159:
        parser_159(payload);
        break;
    default:
        qDebug() << "god knows... god knows i want to break free...";
        return;
    }

}


void ParserOfMessage::parser_102(const QVector<quint8>&payload) {
    quint16 StatusMPI = payload[1];

    StatusMPI <<= 8;
    StatusMPI |= payload[0]; // статус МПИ
    data[0] = StatusMPI;
    quint16 NumberTest = payload[3];
    NumberTest <<= 8;
    NumberTest += payload[2];
    data[1] = NumberTest;
    qDebug() << "Номер теста: " << NumberTest;
    quint32 DescriptionOfViolationFirst = payload[7];
    DescriptionOfViolationFirst <<= 8;
    DescriptionOfViolationFirst += payload[6];
    DescriptionOfViolationFirst <<= 8;
    DescriptionOfViolationFirst += payload[5];
    DescriptionOfViolationFirst <<= 8;
    DescriptionOfViolationFirst += payload[4];//описание нарушения, первое слово
    data[3] = DescriptionOfViolationFirst;
    qDebug() << "Описание нарушения, первое слово:" << QString::number(DescriptionOfViolationFirst, 2);
    quint16 DescriptionOfViolationTwo = payload[9];
    DescriptionOfViolationTwo <<= 8;
    DescriptionOfViolationTwo += payload[8];// описание нарушения, второе слово
    data[4] = DescriptionOfViolationTwo;
    qDebug() << "Описание нарушения, второе слово: " << QString::number(DescriptionOfViolationTwo, 2);
    emit writer(data);
}




void ParserOfMessage::parser_149(const QVector<quint8>&payload){
    quint16 SignWord = payload[1];
    SignWord <<= 8;
    SignWord += payload[0];
    data[0] = SignWord;
    qDebug() << "Слово признаков: " << QString::number(SignWord, 2);
    quint8 seconds = payload[4];
    quint8 minutes = payload[3];
    quint8 hours = payload[2];
    data[1] = hours;
    data[2] = minutes;
    data[3] = seconds;
    qDebug() << "Время: "<< hours <<":"<<minutes<<":"<<seconds;
    quint32 fragment_time = payload[9];
    fragment_time <<= 8;
    fragment_time += payload[8];
    fragment_time <<= 8;
    fragment_time += payload[7];
    fragment_time <<= 8;
    fragment_time += payload[6];
    data[4] = fragment_time;
    qDebug() << "Дробная часть времени" << fragment_time;
    quint8 year = payload[10];
    quint8 month = payload[11];
    quint8 day = payload[12];
    data[5] = day;
    data[6] = month;
    data[7] = year;
    qDebug() << "Дата:" << day <<"."<<month<<"."<<year;
    qint64 X = 0;
    qint64 Y = 0;
    qint64 Z = 0;
    int h = 0;
    for(h=0; h<=7; h++){
        X += payload[14+h];
        Y += payload[22+h];
        Z += payload[30+h];
        X <<= 8;
        Y <<= 8;
        Z <<= 8;
    }
    double X_finish = static_cast<double>(X);
    double Y_finish = static_cast<double>(Y);
    double Z_finish = static_cast<double>(Z);
    data[8] = X_finish;
    data[9] = Y_finish;
    data[10] = Z_finish;
    qDebug() << "Latitide" << X_finish;
    qDebug() << "Longitude" <<Y_finish;
    qDebug() << "Height" << Z_finish;
    qint32 CKP_speed_X = 0;
    qint32 CKP_speed_Y = 0;
    qint32 CKP_speed_Z = 0;
    qint32 CKP_coordinates = 0;
    qint32 CKP_height = 0;
    qint32 CKP_time = 0;
    qint32 CKP_plane_speed = 0;
    qint32 CKP_vertical_speed = 0;
    for(int h = 0; h <= 3; h++)
    {
        CKP_speed_X += payload[41-h];
        CKP_speed_Y += payload[45-h];
        CKP_speed_Z += payload[49-h];
        CKP_coordinates += payload[53-h];
        CKP_height += payload[57-h];
        CKP_time += payload[61-h];
        CKP_plane_speed += payload[65-h];
        CKP_vertical_speed += payload[69-h];
        CKP_speed_X <<= 8;
        CKP_speed_Y <<= 8;
        CKP_speed_Z <<= 8;
        CKP_coordinates <<= 8;
        CKP_height <<= 8;
        CKP_time <<= 8;
        CKP_plane_speed <<= 8;
        CKP_vertical_speed <<= 8;
    }
    data[11] = CKP_speed_X;
    data[12] = CKP_speed_Y;
    data[13] = CKP_speed_Z;
    data[14] = CKP_coordinates;
    data[15] = CKP_height;
    data[16] = CKP_time;
    data[17] = CKP_plane_speed;
    data[18] = CKP_vertical_speed;
    qDebug() << "Составляющая скорости по широте: " << static_cast<float>(CKP_speed_X);
    qDebug() << "Составляющая скорости по долготе: " << static_cast<float>(CKP_speed_Y);
    qDebug() << "Составляющая скорости по высоте: " << static_cast<float>(CKP_speed_Z);
    qDebug() << "СКП определения плановых координат: " << static_cast<float>(CKP_coordinates);
    qDebug() << "СКП определения высоты: " << static_cast<float>(CKP_height);
    qDebug() << "СКП определения времени: " << static_cast<float>(CKP_time);
    qDebug() << "СКП определения плановой составляющей скорости: " << static_cast<float>(CKP_plane_speed);
    qDebug() << "СКП определения вертикальной составляющей скорости: " << static_cast<float>(CKP_vertical_speed);
    emit writer(data);
}

void ParserOfMessage::parser_154(const QVector<quint8>&payload){
    //так как полезная нагрузка составляет 30 байт, нам понадобится обработать 30 элементов массива payload
    quint16 SignWord = payload[1];
    SignWord <<= 8;
    SignWord += payload[0];
    data[0] = SignWord;
    qDebug() << QString::number(SignWord, 2);
    quint8 seconds = payload[4];
    quint8 minutes = payload[3];
    quint8 hours = payload[2];
    data[1] = hours;
    data[2] = minutes;
    data[3] = seconds;
    qDebug() << "Время: "<< hours <<":"<<minutes<<":"<<seconds;
    quint32 fragment_time = payload[9];
    fragment_time <<= 8;
    fragment_time += payload[8];
    fragment_time <<= 8;
    fragment_time += payload[7];
    fragment_time <<= 8;
    fragment_time += payload[6];
    data[4] = fragment_time;
    qDebug() << "Дробная часть времени:" << fragment_time;
    quint64 frequency_fix = 0;
    int h = 0;
    for(h=0; h<=7; h++){
        frequency_fix <<= 8;
        frequency_fix += payload[17-h];
    }
    double frequency_fix_finish = static_cast<double>(frequency_fix);
    data[5] = frequency_fix_finish;
    qDebug() << "Нормированная поправка по частоте:" << frequency_fix_finish;
    quint32 error_time = 0;
    quint32 error_frequency = 0;
    for(int h = 0; h <= 3; h++)
    {
        error_time = payload[18+h];
        error_frequency = payload[22+h];
        error_time <<= 8;
        error_frequency <<= 8;
    }
    float error_time_finish = static_cast<float>(error_time);
    float error_frequency_finish = static_cast<float>(error_frequency);
    data[6] = error_time_finish;
    data[7] = error_frequency_finish;
    qDebug() << "СКП определения времени: " << error_time_finish;
    emit writer(data);
}


void ParserOfMessage::parser_156(const QVector<quint8>&payload){
    //так как сообщение содержит неопределенное количество байт, которое зависит от количества видимых НКА, нам понадобится количество элементов массива payload, зависящее от количества видимых спутников НКА
    quint16 StateWord = payload[1];
    StateWord <<= 8;
    StateWord += payload[0];
    data[0] = StateWord;
    qDebug() << "Слово состояния: " << QString::number(StateWord, 2);
    quint16 quantityNKA = payload[3];
    quantityNKA <<= 8;
    quantityNKA += payload[2];
    data[1] = quantityNKA;
    qDebug() << "Количество спутников: " << quantityNKA;
    qint32 angleSee = 0;
    for(int h = 0; h <= 3; h++)
    {
        angleSee += payload[7-h];
        angleSee <<= 8;
    }
    float angleSee_finish = static_cast<float>(angleSee);
    data[2] = angleSee_finish;
    qDebug() << "Угол видимости: " << angleSee_finish;
    quint16 quantitySeeNKA = payload[9];
    quantitySeeNKA <<= 8;
    quantitySeeNKA += payload[8];
    data[3] = quantitySeeNKA;
    qDebug() << "Количество видимых спутников: " << quantitySeeNKA;
    for (int h=0; h <= quantitySeeNKA; h++) // информация об одном НКА содержит 14 байт
    {
        quint16 numberNKA = payload[11+14*h];
        numberNKA <<= 8;
        numberNKA += payload[10+14*h];
        data[4+5*h] = numberNKA;
        qDebug() << "Номер НКА:" << numberNKA;
        quint16 StateNKA = payload[13+14*h];
        StateNKA <<= 8;
        StateNKA += payload[12+14*h];
        data[5+5*h] = StateNKA;
        qDebug() << "Состояние НКА с номером: " << numberNKA << ":" << QString::number(StateNKA, 2);
        qint16 AgeAlmanah = payload[15+14*h];
        AgeAlmanah <<= 8;
        AgeAlmanah += payload[14+14*h];
        data[6+5*h] = AgeAlmanah;
        qDebug() << "Возраст альманаха НКА с номером: " << numberNKA << ":" << AgeAlmanah;
        qint32 AngleOverHorizon = 0;
        qint32 Azimut = 0;
        for (int t = 0; t <= 3; t++)
        {
            AngleOverHorizon += payload[19-t+14*h];
            Azimut += payload[23-t+14*h];
            AngleOverHorizon <<= 8;
            Azimut <<= 8;
        }
        float AngleOverHorizon_finish = static_cast<float>(AngleOverHorizon);
        data[7+5*h] = AngleOverHorizon_finish;
        qDebug() << "Угол возвышения: " << AngleOverHorizon_finish;
        float Azimut_finish = static_cast<float>(Azimut);
        data[8+5*h] = Azimut_finish;
        qDebug() << "Азимут(курс): " << Azimut_finish;
    }
    emit writer(data);
}

void ParserOfMessage::parser_157(const QVector<quint8>&payload){
    quint16 NKAchannel = 0;
    quint16 StateOfChannel = 0;
    qint32 angleNKA = 0;
    qint32 azimut = 0;
    qint32 signal_noise = 0;
    int counter = 1;
    for (int h = 0; h <= 15; h++)
    {
        NKAchannel += payload[1+16*h];
        NKAchannel <<= 8;
        NKAchannel += payload[0+16*h];
        data[0+6*h] = counter;
        data[1+6*h] = NKAchannel;
        qDebug() << "Идентификатор НКА на канале i=" << counter << ": " << QString::number(NKAchannel, 2);
        StateOfChannel += payload[3+16*h];
        StateOfChannel <<= 8;
        StateOfChannel += payload[2+16*h];
        data[2+6*h] = StateOfChannel;
        qDebug() << "Состояние канала i=" << counter << ": " << QString::number(StateOfChannel, 2);
        for (int i = 0; i <= 3; i++)
        {
            angleNKA += payload[7-i+16*h];
            azimut += payload[11-i+16*h];
            signal_noise += payload[15-i+16*h];
            angleNKA <<= 8;
            azimut <<= 8;
            signal_noise <<= 8;
        }
        float angleNKA_finish = static_cast<float>(angleNKA);
        float azimut_finish = static_cast<float>(azimut);
        float signal_noise_finish = static_cast<float>(signal_noise);
        data[3+6*h] = angleNKA_finish;
        data[4+6*h] = azimut_finish;
        data[5+6*h] = signal_noise_finish;
        qDebug() << "Угол места НКА i=" << counter << ": " << angleNKA_finish;
        qDebug() << "Азимут(курс) НКА i=" << counter << ": " << azimut_finish;
        qDebug() << "Отношение сигнал/шум для НКА i=" << counter << ": " << signal_noise_finish;
        i++;
    }
    emit writer(data);
}


void ParserOfMessage::parser_159(const QVector<quint8>&payload){
    quint16 SignWord = payload[1];
    SignWord <<= 8;
    SignWord += payload[0];
    data[0] = SignWord;
    qDebug() << QString::number(SignWord, 2);
    quint16 AntennaStatus = payload[3];
    AntennaStatus <<= 8;
    AntennaStatus += payload[2];
    data[1] = AntennaStatus;
    qDebug() << "Статус антенны" << QString::number(AntennaStatus, 2);
    quint8 seconds = payload[6];
    quint8 minutes = payload[5];
    quint8 hours = payload[4];
    data[2] = hours;
    data[3] = minutes;
    data[4] = seconds;
    qDebug() << "Время: "<< hours <<":"<<minutes<<":"<<seconds;
    quint32 fragment_time = 0;
    for (int h = 0; h <= 3; h++)
    {
        fragment_time += payload[11-h];
        fragment_time <<= 8;
    }
    data[5] = fragment_time;
    qDebug() << "Дробная часть времени:" << fragment_time;
    quint8 year = payload[12];
    quint8 month = payload[13];
    quint8 day = payload[14];
    data[6] = day;
    data[7] = month;
    data[8] = year;
    qDebug() << "Дата:" << day <<"."<<month<<"."<<year;
    qint64 X = 0;
    qint64 Y = 0;
    qint64 Z = 0;
    int h = 0;
    for(h=0; h<=7; h++){
        X += payload[15+h];
        Y += payload[23+h];
        Z += payload[31+h];
        X <<= 8;
        Y <<= 8;
        Z <<= 8;
    }
    double X_finish = static_cast<double>(X);
    double Y_finish = static_cast<double>(Y);
    double Z_finish = static_cast<double>(Z);
    data[9] = X_finish;
    data[10] = Y_finish;
    data[11] = Z_finish;
    qDebug() << "Latitide =" << qToBigEndian(X_finish);
    qDebug() << "Longitude =" << qToBigEndian(Y_finish);
    qDebug() << "Height =" << qToBigEndian(Z_finish);
    quint32 SeeNKA_GPS = 0;
    quint32 SeeNKA_GLONASS = 0;
    quint32 Navigation_NKA_GPS = 0;
    quint32 Navigation_NKA_GLONASS = 0;
    for (int h = 0; h <= 3; h++)
    {
        quint32 SeeNKA_GPS = payload[39+h];
        SeeNKA_GPS <<= 8;
        quint32 SeeNKA_GLONASS = payload[43+h];
        SeeNKA_GLONASS <<= 8;
        quint32 Navigation_NKA_GPS = payload[47+h];
        Navigation_NKA_GPS <<= 8;
        quint32 Navigation_NKA_GLONASS = payload[51+h];
        Navigation_NKA_GLONASS <<= 8;
    }
    data[12] = SeeNKA_GPS;
    data[13] = SeeNKA_GLONASS;
    data[14] = Navigation_NKA_GPS;
    data[15] = Navigation_NKA_GLONASS;
    qDebug() << "Видимые НКА GPS: " << QString::number(SeeNKA_GPS, 2);
    qDebug() << "Видимые НКА ГЛОНАСС: " << QString::number(SeeNKA_GLONASS, 2);
    qDebug() << "Навигационные НКА GPS: " << QString::number(Navigation_NKA_GPS, 2);
    qDebug() << "Навигационные НКА ГЛОНАСС: " << QString::number(Navigation_NKA_GLONASS, 2);

    quint16 NKAchannel = 0;
    quint16 StateOfChannel = 0;
    qint32 angleNKA = 0;
    qint32 azimut = 0;
    qint32 signal_noise = 0;
    int counter = 1;
    for (int h = 0; h <= 15; h++)
    {
        NKAchannel += payload[56+16*h];
        NKAchannel <<= 8;
        NKAchannel += payload[55+16*h];
        data[16+6*h] = counter;
        data[17+6*h] = NKAchannel;
        qDebug() << "Идентификатор НКА на канале i=" << counter << ": " << QString::number(NKAchannel, 2);
        int id = NKAchannel & 0x1100000000000000;
        if (id == 0){ // GPS
            quint16 liter_GPS = NKAchannel & 0x0000000011111111;
            qDebug() << liter_GPS;
            data[18+8*h] = liter_GPS;
            data[19+8*h] = "у GPS тут ничего нет";
        }
        else if (id==2){
            quint16 id_glonass = NKAchannel & 0x0000001111100000;
            quint16 liter_glonass = NKAchannel & 0x0000000000011111;
            qDebug() << id_glonass;
            qDebug() << liter_glonass;
            data[18+8*h] = id_glonass;
            data[19+8*h] = liter_glonass;
        }
        StateOfChannel += payload[58+16*h];
        StateOfChannel <<= 8;
        StateOfChannel += payload[57+16*h];
        data[20+8*h] = StateOfChannel;
        qDebug() << "Состояние канала i=" << counter << ": " << QString::number(StateOfChannel, 2);
        for (int i = 0; i <= 3; i++)
        {
            angleNKA += payload[62-i+16*h];
            azimut += payload[66-i+16*h];
            signal_noise += payload[70-i+16*h];
            angleNKA <<= 8;
            azimut <<= 8;
            signal_noise <<= 8;
        }
        float angleNKA_finish = static_cast<float>(angleNKA);
        float azimut_finish = static_cast<float>(azimut);
        float signal_noise_finish = static_cast<float>(signal_noise);
        data[21+8*h] = angleNKA_finish;
        data[22+8*h] = azimut_finish;
        data[23+8*h] = signal_noise_finish;
        qDebug() << "Угол места НКА i=" << counter << ": " << angleNKA_finish;
        qDebug() << "Азимут(курс) НКА i=" << counter << ": " << azimut_finish;
        qDebug() << "Отношение сигнал/шум для НКА i=" << counter << ": " << signal_noise_finish;
        i++;
    }
    emit writer(data);
}
