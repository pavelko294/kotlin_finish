#ifndef BINARYT_H
#define BINARYT_H

#include "scdebug.h"
#include "crutches.h"

#include <QVector>

namespace BinaryT
{
typedef quint8 messageId_t;
typedef int fieldId_t;

// NB: битовые поля (BIN16, BIN32, ...) хранятся в виде quint соотв. размера.



/* Сообщение №149 */

namespace Solution
{
namespace Field
{
enum Type : fieldId_t
{
    // Слово признаков
    hasHeight,
    autoDim,
    useLastHeight,
    offsetByMeasurement,
    autoTimeOffset,
    useLastTimeOffset,
    recursive,
    orthogonalCoords,
    raimOk,
    timeOk,
    timeScaleType,

    // Время
    hour,
    minute,
    second,

    timeFrac,

    // Дата
    year,
    month,
    day,

    lat,
    lon,
    h,

    vLat,
    vLon,
    vH,

    rmsCoord,
    rmsH,
    rmsTime,
    rmsVHoriz,
    rmsVH,

    count
};
} // Field

enum class TimeScaleType
{
    Any,
    UTC_US,
    UTC_Rus,
    GPS,
    GLONASS,
    invalid = -1 // не является валидным членом перечисления
};

QVariant GetField(const QVector<quint8> &payload, fieldId_t field);
QString GetFieldString(const QVector<quint8> &payload, fieldId_t field);

class MetaTypes
{
public:
    static const int TimeScaleType_metaType;
};
} // Solution




/* Сообщение №156 */

namespace SatState
{
namespace HeaderField
{
enum Type : fieldId_t
{
    // Слово состояния
    gnssType,
    satsCount,
    angle,
    visibleSatsCount,

    count
};
} // HeaderField

namespace SatField
{
enum Type : fieldId_t
{
    // NB: структура зависит от gnssType:
    // - для GPS:
    //  - в младших 8 битах номер ПСП, остальное резерв
    // - для GLONASS:
    //  - в младших 5 битах "7 + номер литеры (-7...+24)"
    //  - в битах 5...9 системный номер НКА (0...24), 0 - номер отсутствует
    satNum,


    // Состояние НКА
    // NB: структура зависит от gnssType:
    // - для GPS: копия 8-битовых данных 4-го и 5-го подкадров
    // - для GLONASS: в бите 0 - признак состояния, остальные резерв
    satState,
    satValid,
    hasDiffData,


    almanachAge, // в днях
    elev, // рад, +/-pi/2
    azimuth, // рад, +/-pi

    count
};
} // SatField

enum class GNSSType
{
    GPS,
    Reserve_1,
    GLONASS,
    Reserve_2
};

QVariant GetField(const QVector<quint8> &payload, HeaderField::Type field);
QString GetFieldString(const QVector<quint8> &payload, HeaderField::Type field);
int GetRecsNum(const QVector<quint8> &payload); // -1 - ошибка
QVariant GetField(const QVector<quint8> &payload, SatField::Type field, int satIdx);
QString GetFieldString(const QVector<quint8> &payload, SatField::Type field, int satIdx);

class MetaTypes
{
public:
    static const int GNSSType_metaType;
};
} // SatState




/* Сообщение №154 */

namespace TimeScale
{
namespace Field
{
enum Type : fieldId_t
{
    // Слово признаков
    hasHeight,
    autoDim,
    useLastHeight,
    offsetByMeasurement,
    autoTimeOffset,
    useLastTimeOffset,
    recursive,
    orthogonalCoords,
    raimOk,
    timeOk,
    timeScaleType,

    // Время
    hour,
    minute,
    second,

    timeFrac,

    // Дата
    year,
    month,
    day,

    frequencyCorr,
    rmsTime,
    rmsFrequencyCorr,

    count
};
} // Field

enum class TimeScaleType
{
    Any,
    UTC_US,
    UTC_Rus,
    GPS,
    GLONASS,
    invalid = -1 // не является валидным членом перечисления
};

QVariant GetField(const QVector<quint8> &payload, fieldId_t field);
QString GetFieldString(const QVector<quint8> &payload, fieldId_t field);

class MetaTypes
{
public:
    static const int TimeScaleType_metaType;
};
} // TimeScale - готово




/* Сообщение №155 */

namespace TimeScaleOffset
{
namespace Field
{
enum Type : fieldId_t
{
    // Слово признаков
    methodCorr,

    offset, // в секундах
    rmsOffset, // в наносекундах

    count

};
} //Field

enum class Method
{
    commandMessage,
    navigSolution,
    acceptPDD,
    diffScales,
    invalid = -1 // не является валидным членом перечисления
};

QVariant GetField(const QVector<quint8> &payload, fieldId_t field);
QString GetFieldString(const QVector<quint8> &payload, fieldId_t field);

class MetaTypes
{
public:
    static const int Method_metaType;
};
} // TimeScaleOffset - нужно проверить, так как сообщения нет в дампе, проект собирается




/* Сообщение №157*/

namespace ChannelStats
{
namespace Field
{
enum Type : fieldId_t
{
    // ID НКА на i-ом канале
    gnssType,
    // NB: структура зависит от gnssType:
    // - для GPS:
    //  - в младших 8 битах номер ПСП, остальное резерв
    // - для GLONASS:
    //  - в младших 5 битах "7 + номер литеры (-7...+24)"
    //  - в битах 5...9 системный номер НКА (0...24), 0 - номер отсутствует
    satNum,

    // Состояние канала
    noSignal,
    failTrackSignal,
    failTrackPhase,
    findSignal,
    trackFreqAssign,
    symbolSync,
    subframeSync,
    flagSignal,
    receiveType,
    measurmentReady,

    elevation,
    azimuth,
    signalNoise,

    count

};
} // Field

enum class GNSSType
{
    GPS,
    Reserve_1,
    GLONASS,
    Reserve_2
};

QVariant GetField(const QVector<quint8> &payload, fieldId_t field);
QString GetFieldString(const QVector<quint8> &payload, fieldId_t field);

class MetaTypes
{
public:
    static const int GNSSType_metaType;
};

} // ChannelStats - проверить, так как сообщения нет в дампе, проект собирается


// ...остальные сообщения...


enum class MessageType
{
    Invalid, // не считается валидным членом перечисления
    Solution,
    SatState,
    TimeScale,
    TimeScaleOffset,
    ChannelStats
};
inline uint qHash(MessageType key, uint seed = 0)
{
    return ::qHash(SC_CAST_ENUM_TO_UNDERLYING_TYPE(key), seed);
}

namespace MessageId
{
enum Type : messageId_t
{
    Invalid,
    Solution = 149,
    TimeScale = 154,
    TimeScaleOffset = 155,
    SatState = 156,
    ChannelStats = 157

};
} // MessageId
MessageType FromId(MessageId::Type id);

class Message
{
private:
    MessageType m_messageType {};
    QVector<quint8> m_data;

public:
    MessageType GetMessageType() const {return m_messageType;}
    bool IsValid() const;

    int GetFieldsNum() const;
    int GetFieldType(fieldId_t field) const;
    QString GetFieldName(fieldId_t field) const;
    QVariant GetField(fieldId_t field) const;
    QString GetFieldString(fieldId_t field) const;
    QString ToString() const;

    // При возврате true описание ошибки уже выведено
    static bool IsValid(messageId_t messageId, const QVector<quint8> &payload, MessageType *messageType);

    static Message Parse(messageId_t messageId, const QVector<quint8> &payload);
};
} // BinaryT
Q_DECLARE_METATYPE(BinaryT::Solution::TimeScaleType)
Q_DECLARE_METATYPE(BinaryT::SatState::GNSSType)
Q_DECLARE_METATYPE(BinaryT::TimeScale::TimeScaleType)
Q_DECLARE_METATYPE(BinaryT::TimeScaleOffset::Method)
Q_DECLARE_METATYPE(BinaryT::ChannelStats::GNSSType)

SC_DECLARE_TOSTRING_AND_STREAM_OPERATORS(BinaryT::Solution::Field::Type);
SC_DECLARE_TOSTRING_AND_STREAM_OPERATORS(BinaryT::Solution::TimeScaleType);

SC_DECLARE_TOSTRING_AND_STREAM_OPERATORS(BinaryT::TimeScale::Field::Type);
SC_DECLARE_TOSTRING_AND_STREAM_OPERATORS(BinaryT::TimeScale::TimeScaleType);

SC_DECLARE_TOSTRING_AND_STREAM_OPERATORS(BinaryT::TimeScaleOffset::Field::Type);
SC_DECLARE_TOSTRING_AND_STREAM_OPERATORS(BinaryT::TimeScaleOffset::Method);

SC_DECLARE_TOSTRING_AND_STREAM_OPERATORS(BinaryT::ChannelStats::Field::Type);
SC_DECLARE_TOSTRING_AND_STREAM_OPERATORS(BinaryT::ChannelStats::GNSSType);

SC_DECLARE_TOSTRING_AND_STREAM_OPERATORS(BinaryT::SatState::HeaderField::Type);
SC_DECLARE_TOSTRING_AND_STREAM_OPERATORS(BinaryT::SatState::SatField::Type);
SC_DECLARE_TOSTRING_AND_STREAM_OPERATORS(BinaryT::SatState::GNSSType);

SC_DECLARE_TOSTRING_AND_STREAM_OPERATORS(BinaryT::MessageType);

SC_DECLARE_STREAM_OPERATORS(const BinaryT::Message &);

#endif // BINARYT_H
