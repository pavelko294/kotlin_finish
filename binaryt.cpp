#include "binaryt.h"

SC_REGISTER_METATYPE(BinaryT::Solution, TimeScaleType);
SC_REGISTER_METATYPE(BinaryT::SatState, GNSSType);
SC_REGISTER_METATYPE(BinaryT::TimeScale, TimeScaleType);
SC_REGISTER_METATYPE(BinaryT::TimeScaleOffset, Method);
SC_REGISTER_METATYPE(BinaryT::ChannelStats, GNSSType);

namespace BinaryT
{
typedef int bitOffset_t; // относительно начала пакета
typedef int bitsNum_t;
struct FieldDescriptor
{
    bitOffset_t bitOffset;
    bitsNum_t bitsNum;
    int type;
};
typedef QVector<FieldDescriptor> fieldDescriptors_t;

typedef QPair<bitsNum_t, int> dietFieldDescriptor_t;
typedef QVector<dietFieldDescriptor_t> dietFieldDescriptors_t;
fieldDescriptors_t GenerateFieldDescriptors_impl(const dietFieldDescriptors_t &fieldDescriptors, bitOffset_t *recSize = nullptr)
{
    fieldDescriptors_t ret;

    bitOffset_t bitOffset {0};
    foreach (const dietFieldDescriptor_t &fieldDescriptor, fieldDescriptors)
    {
        bitsNum_t bitsNum {fieldDescriptor.first};
        const int type {fieldDescriptor.second};

        SC_CHECK_UNEQUALS(bitsNum, 0);
        if(bitsNum < 0)
        {
            SC_CHECK_EQUALS(type, 0);
            bitsNum *= -1;
        }
        else
        {
            SC_CHECK_MORE(type, 0);
            ret.append({bitOffset, bitsNum, type});
        }

        bitOffset += bitsNum;
    }

    if(recSize)
        (*recSize) = bitOffset;

    return ret;
}


#define SC_BINARYT_GETFIELD_BUF(_TYPE) \
    if(fieldDescriptor.type == qMetaTypeId<_TYPE>()) \
{ \
    _TYPE val {}; \
    if(ReadBits(payload, payloadSize, &val, sizeof(val), fieldDescriptor.bitOffset, fieldDescriptor.bitsNum)) \
    return QVariant(); \
    return MakeVariant(val); \
}

#define SC_BINARYT_GETFIELD_VECTOR(_TYPE) \
    if(fieldDescriptor.type == qMetaTypeId<_TYPE>()) \
{ \
    _TYPE val {}; \
    if(ReadBits(payload.constData(), payload.size(), &val, sizeof(val), fieldDescriptor.bitOffset, fieldDescriptor.bitsNum)) \
    return QVariant(); \
    return MakeVariant(val); \
}

QVariant GetField_impl(const quint8 *payload, int payloadSize, const FieldDescriptor &fieldDescriptor)
{
    SC_BINARYT_GETFIELD_BUF(bool);
    SC_BINARYT_GETFIELD_BUF(int);
    SC_BINARYT_GETFIELD_BUF(quint8);
    SC_BINARYT_GETFIELD_BUF(quint16);
    SC_BINARYT_GETFIELD_BUF(quint32);
    SC_BINARYT_GETFIELD_BUF(qint8);
    SC_BINARYT_GETFIELD_BUF(qint16);
    SC_BINARYT_GETFIELD_BUF(qint32);
    SC_BINARYT_GETFIELD_BUF(float);
    SC_BINARYT_GETFIELD_BUF(double);

    SC_THROW(fieldDescriptor.type);
    return QVariant();
}

QVariant GetField_impl(const QVector<quint8> &payload, const FieldDescriptor &fieldDescriptor)
{
    return GetField_impl(payload.constData(), payload.size(), fieldDescriptor);
}

#define SC_BINARYT_STRINGIFY_VARIANT(_TYPE) \
    if(type == qMetaTypeId<_TYPE>()) \
    return StringifyUsingTextStream(var.value<_TYPE>());

QString VariantToString(const QVariant &var)
{
    const int type {var.userType()};

    //SC_BINARYT_STRINGIFY_VARIANT(bool);
    if(type == qMetaTypeId<bool>())
    {
        if(var.value<bool>())
            return "✓";
        else
            return "✗";
    }

    SC_BINARYT_STRINGIFY_VARIANT(int);
    SC_BINARYT_STRINGIFY_VARIANT(quint8);
    SC_BINARYT_STRINGIFY_VARIANT(quint16);
    SC_BINARYT_STRINGIFY_VARIANT(quint32);
    SC_BINARYT_STRINGIFY_VARIANT(qint8);
    SC_BINARYT_STRINGIFY_VARIANT(qint16);
    SC_BINARYT_STRINGIFY_VARIANT(qint32);
    SC_BINARYT_STRINGIFY_VARIANT(float);
    SC_BINARYT_STRINGIFY_VARIANT(double);

    return QString("[unexpected type %1]").arg(MetaTypeString(type));
}


namespace Solution
{
namespace
{
fieldDescriptors_t GenerateFieldDescriptors()
{
    const fieldDescriptors_t ret {GenerateFieldDescriptors_impl(
                    {
                        // Слово признаков
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-2, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-1, 0},
                        {3, qMetaTypeId<TimeScaleType>()},

                        // Время
                        {8, qMetaTypeId<int>()},
                        {8, qMetaTypeId<int>()},
                        {8, qMetaTypeId<int>()},
                        {-8, 0},

                        // Дробная часть времени
                        {32, qMetaTypeId<quint32>()},

                        // Дата
                        {8, qMetaTypeId<int>()},
                        {8, qMetaTypeId<int>()},
                        {8, qMetaTypeId<int>()},
                        {-8, 0},

                        // Координаты
                        {64, qMetaTypeId<double>()},
                        {64, qMetaTypeId<double>()},
                        {64, qMetaTypeId<double>()},

                        // Скорости
                        {32, qMetaTypeId<float>()},
                        {32, qMetaTypeId<float>()},
                        {32, qMetaTypeId<float>()},

                        // СКП
                        {32, qMetaTypeId<float>()},
                        {32, qMetaTypeId<float>()},
                        {32, qMetaTypeId<float>()},
                        {32, qMetaTypeId<float>()},
                        {32, qMetaTypeId<float>()}
                    }
                    )};
    SC_CHECK_EQUALS(ret.size(), Field::count);
    return ret;
}
} // anon namespace

const FieldDescriptor &GetFieldDescriptor(Field::Type field)
{
    static const fieldDescriptors_t fieldDescriptors {GenerateFieldDescriptors()};
    SC_CHECK_MOREOREQ(field, 0);
    SC_CHECK_LESS(field, fieldDescriptors.size());
    return fieldDescriptors.at(field);
}

QVariant GetField(const QVector<quint8> &payload, fieldId_t field)
{
    const FieldDescriptor &fieldDescriptor (GetFieldDescriptor(static_cast<Field::Type>(field)));
    SC_BINARYT_GETFIELD_VECTOR(TimeScaleType);
    return GetField_impl(payload, fieldDescriptor);
}

QString GetFieldString(const QVector<quint8> &payload, fieldId_t fieldInt)
{
    const QVariant var (GetField(payload, fieldInt));
    if(!var.isValid())
        return "[invalid]";

    const Field::Type field {static_cast<Field::Type>(fieldInt)};
    if(field == Field::lat || field == Field::lon)
        return ToDMSString(ExtractFromVar<double>(var));

    const int type {var.userType()};
    SC_BINARYT_STRINGIFY_VARIANT(TimeScaleType);
    return VariantToString(var);
}
} // Solution



namespace SatState
{
namespace
{
fieldDescriptors_t GenerateHeaderFieldDescriptors(bitOffset_t *recSize)
{
    const fieldDescriptors_t ret {GenerateFieldDescriptors_impl(
                    {
                        // Слово состояния
                        {-14, 0},
                        {2, qMetaTypeId<GNSSType>()},

                        {16, qMetaTypeId<quint16>()},
                        {32, qMetaTypeId<float>()},
                        {16, qMetaTypeId<quint16>()}
                    }, recSize)};
    SC_CHECK_EQUALS(ret.size(), HeaderField::count);
    return ret;
}

fieldDescriptors_t GenerateSatFieldDescriptors(bitOffset_t *recSize)
{
    const fieldDescriptors_t ret {GenerateFieldDescriptors_impl(
                    {
                        {10, qMetaTypeId<quint16>()},
                        {-6, 0},

                        {8, qMetaTypeId<quint8>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-6, 0},

                        {16, qMetaTypeId<qint16>()},
                        {32, qMetaTypeId<float>()},
                        {32, qMetaTypeId<float>()}
                    }, recSize)};
    SC_CHECK_EQUALS(ret.size(), SatField::count);
    return ret;
}
} // anon namespace

const FieldDescriptor &GetFieldDescriptor(HeaderField::Type field)
{
    static const fieldDescriptors_t fieldDescriptors {GenerateHeaderFieldDescriptors(nullptr)};
    SC_CHECK_MOREOREQ(field, 0);
    SC_CHECK_LESS(field, fieldDescriptors.size());
    return fieldDescriptors.at(field);
}

const FieldDescriptor &GetFieldDescriptor(SatField::Type field)
{
    static const fieldDescriptors_t fieldDescriptors {GenerateSatFieldDescriptors(nullptr)};
    SC_CHECK_MOREOREQ(field, 0);
    SC_CHECK_LESS(field, fieldDescriptors.size());
    return fieldDescriptors.at(field);
}

namespace
{
int GenerateHeaderSize()
{
    bitOffset_t ret {0};
    GenerateHeaderFieldDescriptors(&ret);
    SC_CHECK_MORE(ret, 0);
    SC_CHECK_EQUALS(ret % 8, 0);
    //scDebug() << __FUNCTION__ << ": " << ret;
    return ret / 8;
}

int GenerateSatRecSize()
{
    bitOffset_t ret {0};
    GenerateSatFieldDescriptors(&ret);
    SC_CHECK_MORE(ret, 0);
    SC_CHECK_EQUALS(ret % 8, 0);
    //scDebug() << __FUNCTION__ << ": " << ret;
    return ret / 8;
}
} // anon namespace

int HeaderSize()
{
    static const int ret {GenerateHeaderSize()};
    return ret;
}

int SatRecSize()
{
    static const int ret {GenerateSatRecSize()};
    return ret;
}

QVariant GetField(const QVector<quint8> &payload, HeaderField::Type field)
{
    const FieldDescriptor &fieldDescriptor (GetFieldDescriptor(field));
    SC_BINARYT_GETFIELD_VECTOR(GNSSType);
    return GetField_impl(payload, fieldDescriptor);
}

QString GetFieldString(const QVector<quint8> &payload, HeaderField::Type field)
{
    const QVariant var (GetField(payload, field));
    if(!var.isValid())
        return "[invalid]";
    const int type {var.userType()};
    SC_BINARYT_STRINGIFY_VARIANT(GNSSType);
    return VariantToString(var);
}

int GetRecsNum(const QVector<quint8> &payload)
{
    int size {payload.size()};

    {
        const int headerSize {HeaderSize()};
        if(size < headerSize)
        {
            scWarning() << SC_STRINGIFY(size) << ", " << SC_STRINGIFY(headerSize);
            return -1;
        }
        size -= headerSize;
    }

    const int satRecSize {SatRecSize()};
    if(size % satRecSize)
    {
        scWarning() << SC_STRINGIFY(size) << ", " << SC_STRINGIFY(satRecSize);
        return -1;
    }

    return size / satRecSize;
}

QVariant GetField(const QVector<quint8> &payload, SatField::Type field, int satIdx)
{
    SC_CHECK_MOREOREQ(satIdx, 0);

    {
        const int recsNum {GetRecsNum(payload)};
        SC_CHECK_LESS(satIdx, recsNum);
    }

    const FieldDescriptor &fieldDescriptor (GetFieldDescriptor(field));

    const int recOffset {HeaderSize() + satIdx * SatRecSize()};
    SC_CHECK_LESS(recOffset, payload.size());

    return GetField_impl(&(payload.constData()[recOffset]), payload.size() - recOffset, fieldDescriptor);
}

QString GetFieldString(const QVector<quint8> &payload, SatField::Type field, int satIdx)
{
    const QVariant var (GetField(payload, field, satIdx));
    if(!var.isValid())
        return "[invalid]";
    return VariantToString(var);
}
} // SatState



namespace TimeScale
{
namespace
{
fieldDescriptors_t GenerateFieldDescriptors()
{
    const fieldDescriptors_t ret {GenerateFieldDescriptors_impl(
                    {
                        // Слово признаков
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-2, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-1, 0},
                        {3, qMetaTypeId<TimeScaleType>()},

                        // Время
                        {8, qMetaTypeId<int>()},
                        {8, qMetaTypeId<int>()},
                        {8, qMetaTypeId<int>()},
                        {-8, 0},

                        // Дробная часть времени
                        {32, qMetaTypeId<quint32>()},

                        // Дата
                        {8, qMetaTypeId<int>()},
                        {8, qMetaTypeId<int>()},
                        {8, qMetaTypeId<int>()},
                        {-8, 0},

                        // Нормированная поправка по частоте
                        {64, qMetaTypeId<double>()},

                        // СКП
                        {32, qMetaTypeId<float>()},
                        {32, qMetaTypeId<float>()},

                    }
                    )};
    SC_CHECK_EQUALS(ret.size(), Field::count);
    return ret;
}
} // anon namespace

const FieldDescriptor &GetFieldDescriptor(Field::Type field)
{
    static const fieldDescriptors_t fieldDescriptors {GenerateFieldDescriptors()};
    SC_CHECK_MOREOREQ(field, 0);
    SC_CHECK_LESS(field, fieldDescriptors.size());
    return fieldDescriptors.at(field);
}

QVariant GetField(const QVector<quint8> &payload, fieldId_t field)
{
    const FieldDescriptor &fieldDescriptor (GetFieldDescriptor(static_cast<Field::Type>(field)));
    SC_BINARYT_GETFIELD_VECTOR(TimeScaleType);
    return GetField_impl(payload, fieldDescriptor);
}

QString GetFieldString(const QVector<quint8> &payload, fieldId_t fieldInt)
{
    const QVariant var (GetField(payload, fieldInt));
    if(!var.isValid())
        return "[invalid]";


    const int type {var.userType()};
    SC_BINARYT_STRINGIFY_VARIANT(TimeScaleType);
    return VariantToString(var);
}
} // TimeScale




namespace TimeScaleOffset
{
namespace
{
fieldDescriptors_t GenerateFieldDescriptors()
{
    const fieldDescriptors_t ret {GenerateFieldDescriptors_impl(
                    {
                        // Слово признаков
                        {3, qMetaTypeId<Method>()},
                        {-14, 0},

                        // Смещение между времеными шкалами
                        {64, qMetaTypeId<double>()},

                        // СКП смещения
                        {32, qMetaTypeId<float>()},

                    }
                    )};
    SC_CHECK_EQUALS(ret.size(), Field::count);
    return ret;
}
} // anon namespace

const FieldDescriptor &GetFieldDescriptor(Field::Type field)
{
    static const fieldDescriptors_t fieldDescriptors {GenerateFieldDescriptors()};
    SC_CHECK_MOREOREQ(field, 0);
    SC_CHECK_LESS(field, fieldDescriptors.size());
    return fieldDescriptors.at(field);
}

QVariant GetField(const QVector<quint8> &payload, fieldId_t field)
{
    const FieldDescriptor &fieldDescriptor (GetFieldDescriptor(static_cast<Field::Type>(field)));
    SC_BINARYT_GETFIELD_VECTOR(Method);
    return GetField_impl(payload, fieldDescriptor);
}

QString GetFieldString(const QVector<quint8> &payload, fieldId_t fieldInt)
{
    const QVariant var (GetField(payload, fieldInt));
    if(!var.isValid())
        return "[invalid]";


    const int type {var.userType()};
    SC_BINARYT_STRINGIFY_VARIANT(Method);
    return VariantToString(var);
}
} // TimeScaleOffset



namespace ChannelStats
{
namespace
{

fieldDescriptors_t GenerateFieldDescriptors()
{
    const fieldDescriptors_t ret {GenerateFieldDescriptors_impl(
                    {
                        // ID канала 1
                        {10, qMetaTypeId<quint16>()},
                        {-4, 0},
                        {3, qMetaTypeId<GNSSType>()},

                        // Состояние канала 1
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-4, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-1, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},

                        // Угол места НКА
                        {32, qMetaTypeId<float>()},
                        // Азимут
                        {32, qMetaTypeId<float>()},
                        // Отношение сигнал/шум
                        {32, qMetaTypeId<float>()},


                        // ID канала 2
                        {10, qMetaTypeId<quint16>()},
                        {-4, 0},
                        {3, qMetaTypeId<GNSSType>()},


                        // Состояние канала 2
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-4, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-1, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},

                        // Угол места НКА
                        {32, qMetaTypeId<float>()},
                        // Азимут
                        {32, qMetaTypeId<float>()},
                        // Отношение сигнал/шум
                        {32, qMetaTypeId<float>()},


                        // ID канала 3
                        {10, qMetaTypeId<quint16>()},
                        {-4, 0},
                        {3, qMetaTypeId<GNSSType>()},

                        // Состояние канала 3
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-4, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-1, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},

                        // Угол места НКА
                        {32, qMetaTypeId<float>()},
                        // Азимут
                        {32, qMetaTypeId<float>()},
                        // Отношение сигнал/шум
                        {32, qMetaTypeId<float>()},


                        // ID канала 4
                        {10, qMetaTypeId<quint16>()},
                        {-4, 0},
                        {3, qMetaTypeId<GNSSType>()},

                        // Состояние канала 4
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-4, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-1, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},

                        // Угол места НКА
                        {32, qMetaTypeId<float>()},
                        // Азимут
                        {32, qMetaTypeId<float>()},
                        // Отношение сигнал/шум
                        {32, qMetaTypeId<float>()},


                        // ID канала 5
                        {10, qMetaTypeId<quint16>()},
                        {-4, 0},
                        {3, qMetaTypeId<GNSSType>()},

                        // Состояние канала 5
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-4, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-1, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},

                        // Угол места НКА
                        {32, qMetaTypeId<float>()},
                        // Азимут
                        {32, qMetaTypeId<float>()},
                        // Отношение сигнал/шум
                        {32, qMetaTypeId<float>()},


                        // ID канала 6
                        {10, qMetaTypeId<quint16>()},
                        {-4, 0},
                        {3, qMetaTypeId<GNSSType>()},

                        // Состояние канала 6
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-4, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-1, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},

                        // Угол места НКА
                        {32, qMetaTypeId<float>()},
                        // Азимут
                        {32, qMetaTypeId<float>()},
                        // Отношение сигнал/шум
                        {32, qMetaTypeId<float>()},


                        // ID канала 7
                        {10, qMetaTypeId<quint16>()},
                        {-4, 0},
                        {3, qMetaTypeId<GNSSType>()},

                        // Состояние канала 7
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-4, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-1, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},

                        // Угол места НКА
                        {32, qMetaTypeId<float>()},
                        // Азимут
                        {32, qMetaTypeId<float>()},
                        // Отношение сигнал/шум
                        {32, qMetaTypeId<float>()},


                        // ID канала 8
                        {10, qMetaTypeId<quint16>()},
                        {-4, 0},
                        {3, qMetaTypeId<GNSSType>()},

                        // Состояние канала 8
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-4, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-1, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},

                        // Угол места НКА
                        {32, qMetaTypeId<float>()},
                        // Азимут
                        {32, qMetaTypeId<float>()},
                        // Отношение сигнал/шум
                        {32, qMetaTypeId<float>()},


                        // ID канала 9
                        {10, qMetaTypeId<quint16>()},
                        {-4, 0},
                        {3, qMetaTypeId<GNSSType>()},

                        // Состояние канала 9
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-4, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-1, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},

                        // Угол места НКА
                        {32, qMetaTypeId<float>()},
                        // Азимут
                        {32, qMetaTypeId<float>()},
                        // Отношение сигнал/шум
                        {32, qMetaTypeId<float>()},


                        // ID канала 10
                        {10, qMetaTypeId<quint16>()},
                        {-4, 0},
                        {3, qMetaTypeId<GNSSType>()},

                        // Состояние канала 10
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-4, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-1, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},

                        // Угол места НКА
                        {32, qMetaTypeId<float>()},
                        // Азимут
                        {32, qMetaTypeId<float>()},
                        // Отношение сигнал/шум
                        {32, qMetaTypeId<float>()},


                        // ID канала 11
                        {10, qMetaTypeId<quint16>()},
                        {-4, 0},
                        {3, qMetaTypeId<GNSSType>()},

                        // Состояние канала 11
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-4, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-1, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},

                        // Угол места НКА
                        {32, qMetaTypeId<float>()},
                        // Азимут
                        {32, qMetaTypeId<float>()},
                        // Отношение сигнал/шум
                        {32, qMetaTypeId<float>()},


                        // ID канала 12
                        {10, qMetaTypeId<quint16>()},
                        {-4, 0},
                        {3, qMetaTypeId<GNSSType>()},

                        // Состояние канала 12
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-4, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-1, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},

                        // Угол места НКА
                        {32, qMetaTypeId<float>()},
                        // Азимут
                        {32, qMetaTypeId<float>()},
                        // Отношение сигнал/шум
                        {32, qMetaTypeId<float>()},


                        // ID канала 13
                        {10, qMetaTypeId<quint16>()},
                        {-4, 0},
                        {3, qMetaTypeId<GNSSType>()},

                        // Состояние канала 13
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-4, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-1, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},

                        // Угол места НКА
                        {32, qMetaTypeId<float>()},
                        // Азимут
                        {32, qMetaTypeId<float>()},
                        // Отношение сигнал/шум
                        {32, qMetaTypeId<float>()},


                        // ID канала 14
                        {10, qMetaTypeId<quint16>()},
                        {-4, 0},
                        {3, qMetaTypeId<GNSSType>()},

                        // Состояние канала 14
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-4, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-1, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},

                        // Угол места НКА
                        {32, qMetaTypeId<float>()},
                        // Азимут
                        {32, qMetaTypeId<float>()},
                        // Отношение сигнал/шум
                        {32, qMetaTypeId<float>()},


                        // ID канала 15
                        {10, qMetaTypeId<quint16>()},
                        {-4, 0},
                        {3, qMetaTypeId<GNSSType>()},

                        // Состояние канала 15
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-4, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-1, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},

                        // Угол места НКА
                        {32, qMetaTypeId<float>()},
                        // Азимут
                        {32, qMetaTypeId<float>()},
                        // Отношение сигнал/шум
                        {32, qMetaTypeId<float>()},


                        // ID канала 16
                        {10, qMetaTypeId<quint16>()},
                        {-4, 0},
                        {3, qMetaTypeId<GNSSType>()},

                        // Состояние канала 16
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-4, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {-1, 0},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},
                        {1, qMetaTypeId<bool>()},

                        // Угол места НКА
                        {32, qMetaTypeId<float>()},
                        // Азимут
                        {32, qMetaTypeId<float>()},
                        // Отношение сигнал/шум
                        {32, qMetaTypeId<float>()},

                    }
                    )};
    SC_CHECK_EQUALS(ret.size(), Field::count);
    return ret;
}
} // anon namespace

const FieldDescriptor &GetFieldDescriptor(Field::Type field)
{
    static const fieldDescriptors_t fieldDescriptors {GenerateFieldDescriptors()};
    SC_CHECK_MOREOREQ(field, 0);
    SC_CHECK_LESS(field, fieldDescriptors.size());
    return fieldDescriptors.at(field);
}

QVariant GetField(const QVector<quint8> &payload, fieldId_t field)
{
    const FieldDescriptor &fieldDescriptor (GetFieldDescriptor(static_cast<Field::Type>(field)));
    SC_BINARYT_GETFIELD_VECTOR(GNSSType);
    return GetField_impl(payload, fieldDescriptor);
}

QString GetFieldString(const QVector<quint8> &payload, fieldId_t fieldInt)
{
    const QVariant var (GetField(payload, fieldInt));
    if(!var.isValid())
        return "[invalid]";

    const int type {var.userType()};
    SC_BINARYT_STRINGIFY_VARIANT(GNSSType);
    return VariantToString(var);
}

} // ChannelStats



MessageType FromId(MessageId::Type id)
{
    switch (id)
    {
    case MessageId::Invalid: break;
#define SC_SWITCH_CASE(_X) case MessageId::_X: return MessageType::_X
        SC_SWITCH_CASE(Solution);
        SC_SWITCH_CASE(SatState);
        SC_SWITCH_CASE(TimeScale);
        SC_SWITCH_CASE(TimeScaleOffset);
        SC_SWITCH_CASE(ChannelStats);

#undef SC_SWITCH_CASE
    }
    return MessageType::Invalid;
}


/* Message */

bool Message::IsValid() const
{
    return m_messageType != MessageType::Invalid;
}

int Message::GetFieldsNum() const
{
    switch (m_messageType)
    {
    case MessageType::Invalid: break;
#define SC_SWITCH_CASE(_X) case MessageType::_X: return _X::Field::count
        SC_SWITCH_CASE(Solution);
#undef SC_SWITCH_CASE

    case MessageType::SatState:
    {
        int ret {SatState::GetRecsNum(m_data)};
        ret *= SatState::SatField::count;
        ret += SatState::HeaderField::count;
        return ret;
    }
#define SC_SWITCH_CASE(_X) case MessageType::_X: return _X::Field::count
        SC_SWITCH_CASE(TimeScale);
#undef SC_SWITCH_CASE

#define SC_SWITCH_CASE(_X) case MessageType::_X: return _X::Field::count
        SC_SWITCH_CASE(TimeScaleOffset);
#undef SC_SWITCH_CASE

#define SC_SWITCH_CASE(_X) case MessageType::_X: return _X::Field::count
        SC_SWITCH_CASE(ChannelStats);
#undef SC_SWITCH_CASE
    }
    SC_THROW(m_messageType);
    return -1;
}

int Message::GetFieldType(fieldId_t field) const
{
    switch (m_messageType)
    {
    case MessageType::Invalid: break;
#define SC_SWITCH_CASE(_X) case MessageType::_X: return _X::GetFieldDescriptor(static_cast<_X::Field::Type>(field)).type
        SC_SWITCH_CASE(Solution);
#undef SC_SWITCH_CASE

    case MessageType::SatState:
    {
        SC_CHECK_MOREOREQ(field, 0);
        if(field < SatState::HeaderField::count)
            return SatState::GetFieldDescriptor(static_cast<SatState::HeaderField::Type>(field)).type;

        field -= SatState::HeaderField::count;
        field %= SatState::SatField::count;
        return SatState::GetFieldDescriptor(static_cast<SatState::SatField::Type>(field)).type;
    }
#define SC_SWITCH_CASE(_X) case MessageType::_X: return _X::GetFieldDescriptor(static_cast<_X::Field::Type>(field)).type
        SC_SWITCH_CASE(TimeScale);
#undef SC_SWITCH_CASE

#define SC_SWITCH_CASE(_X) case MessageType::_X: return _X::GetFieldDescriptor(static_cast<_X::Field::Type>(field)).type
        SC_SWITCH_CASE(TimeScaleOffset);
#undef SC_SWITCH_CASE

#define SC_SWITCH_CASE(_X) case MessageType::_X: return _X::GetFieldDescriptor(static_cast<_X::Field::Type>(field)).type
        SC_SWITCH_CASE(ChannelStats);
#undef SC_SWITCH_CASE
    }
    SC_THROW(m_messageType);
    return -1;
}

QString Message::GetFieldName(fieldId_t field) const
{
    switch (m_messageType)
    {
    case MessageType::Invalid: break;
#define SC_SWITCH_CASE(_X) case MessageType::_X: return ::ToString(static_cast<_X::Field::Type>(field))
        SC_SWITCH_CASE(Solution);
#undef SC_SWITCH_CASE

    case MessageType::SatState:
    {
        SC_CHECK_MOREOREQ(field, 0);
        if(field < SatState::HeaderField::count)
            return ::ToString(static_cast<SatState::HeaderField::Type>(field));

        field -= SatState::HeaderField::count;
        field %= SatState::SatField::count;
        return ::ToString(static_cast<SatState::SatField::Type>(field));
    }
#define SC_SWITCH_CASE(_X) case MessageType::_X: return ::ToString(static_cast<_X::Field::Type>(field))
        SC_SWITCH_CASE(TimeScale);
#undef SC_SWITCH_CASE

#define SC_SWITCH_CASE(_X) case MessageType::_X: return ::ToString(static_cast<_X::Field::Type>(field))
        SC_SWITCH_CASE(TimeScaleOffset);
#undef SC_SWITCH_CASE

#define SC_SWITCH_CASE(_X) case MessageType::_X: return ::ToString(static_cast<_X::Field::Type>(field))
        SC_SWITCH_CASE(ChannelStats);
#undef SC_SWITCH_CASE

    }
    SC_THROW(m_messageType);
    return "WTF?";
}

QVariant Message::GetField(fieldId_t field) const
{
    switch (m_messageType)
    {
    case MessageType::Invalid: break;
#define SC_SWITCH_CASE(_X) case MessageType::_X: return _X::GetField(m_data, field)
        SC_SWITCH_CASE(Solution);
#undef SC_SWITCH_CASE

    case MessageType::SatState:
    {
        SC_CHECK_MOREOREQ(field, 0);
        if(field < SatState::HeaderField::count)
            return SatState::GetField(m_data, static_cast<SatState::HeaderField::Type>(field));

        field -= SatState::HeaderField::count;
        const int satIdx {field / SatState::SatField::count};
        field %= SatState::SatField::count;
        return SatState::GetField(m_data, static_cast<SatState::SatField::Type>(field), satIdx);
    }
#define SC_SWITCH_CASE(_X) case MessageType::_X: return _X::GetField(m_data, field)
        SC_SWITCH_CASE(TimeScale);
#undef SC_SWITCH_CASE

#define SC_SWITCH_CASE(_X) case MessageType::_X: return _X::GetField(m_data, field)
        SC_SWITCH_CASE(TimeScaleOffset);
#undef SC_SWITCH_CASE

#define SC_SWITCH_CASE(_X) case MessageType::_X: return _X::GetField(m_data, field)
        SC_SWITCH_CASE(ChannelStats);
#undef SC_SWITCH_CASE
    }
    SC_THROW(m_messageType);
    return QVariant();
}

QString Message::GetFieldString(fieldId_t field) const
{
    switch (m_messageType)
    {
    case MessageType::Invalid: break;
#define SC_SWITCH_CASE(_X) case MessageType::_X: return _X::GetFieldString(m_data, field)
        SC_SWITCH_CASE(Solution);
#undef SC_SWITCH_CASE

    case MessageType::SatState:
    {
        SC_CHECK_MOREOREQ(field, 0);
        if(field < SatState::HeaderField::count)
            return SatState::GetFieldString(m_data, static_cast<SatState::HeaderField::Type>(field));

        field -= SatState::HeaderField::count;
        const int satIdx {field / SatState::SatField::count};
        field %= SatState::SatField::count;
        return SatState::GetFieldString(m_data, static_cast<SatState::SatField::Type>(field), satIdx);
    }
#define SC_SWITCH_CASE(_X) case MessageType::_X: return _X::GetFieldString(m_data, field)
        SC_SWITCH_CASE(TimeScale);
#undef SC_SWITCH_CASE

#define SC_SWITCH_CASE(_X) case MessageType::_X: return _X::GetFieldString(m_data, field)
        SC_SWITCH_CASE(TimeScaleOffset);
#undef SC_SWITCH_CASE

#define SC_SWITCH_CASE(_X) case MessageType::_X: return _X::GetFieldString(m_data, field)
        SC_SWITCH_CASE(ChannelStats);
#undef SC_SWITCH_CASE
}
    SC_THROW(m_messageType);
    return "WTF?";

}

QString Message::ToString() const
{
    if(m_messageType == MessageType::Invalid)
        return "[invalid]";

    QStringList names;
    QStringList values;

    const int fieldsNum {GetFieldsNum()};
    for(fieldId_t field = 0; field < fieldsNum; field ++)
    {
        names.append(GetFieldName(field));
        values.append(GetFieldString(field));
    }

    return Align(names, values);
}

bool Message::IsValid(messageId_t messageId, const QVector<quint8> &payload, MessageType *messageTypePtr)
{
    const MessageType messageType {FromId(static_cast<MessageId::Type>(messageId))};
    if(messageTypePtr)
        (*messageTypePtr) = messageType;

    switch (messageType)
    {
    case MessageType::Invalid:
        return false;

    case MessageType::Solution:
    {
        const int expectedSize {70};
        if(payload.size() != expectedSize)
        {
            scWarning() << messageType << ": " << payload.size() << " != " << expectedSize;
            return false;
        }

        return true;
    }

    case MessageType::SatState:
    {
        if(SatState::GetRecsNum(payload) < 0)
            return false;
        return true;
    }
    case MessageType::TimeScale:
    {
        const int expectedSize {30};
        if(payload.size() != expectedSize)
        {
            scWarning() << messageType << ": " << payload.size() << " != " << expectedSize;
            return false;
        }
        return true;
    }
        case MessageType::TimeScaleOffset:
    {
        const int expectedSize {14};
        if(payload.size() != expectedSize)
        {
            scWarning() << messageType << ": " << payload.size() << " != " << expectedSize;
            return false;
        }
        return true;
    }
    case MessageType::ChannelStats:
    {
        const int expectedSize {256};
        if(payload.size() != expectedSize)
        {
            scWarning() << messageType << ": " << payload.size() << " != " << expectedSize;
            return false;
        }
        return true;
    }

    }

    SC_THROW(messageType);
    return false;
}

Message Message::Parse(messageId_t messageId, const QVector<quint8> &payload)
{
    MessageType messageType {};
    if(!IsValid(messageId, payload, &messageType))
        return Message();

    Message msg;
    msg.m_messageType = messageType;
    msg.m_data = payload;
    return msg;
}
} // BinaryT


/* СТРИНГИФИКАТОРЫ */

const char *ToString_impl(BinaryT::Solution::Field::Type val)
{
    switch(val)
    {
    case BinaryT::Solution::Field::count: break;
#define SC_SWITCH_CASE(_X) case BinaryT::Solution::Field::_X: return #_X
        SC_SWITCH_CASE(hasHeight);
        SC_SWITCH_CASE(autoDim);
        SC_SWITCH_CASE(useLastHeight);
        SC_SWITCH_CASE(offsetByMeasurement);
        SC_SWITCH_CASE(autoTimeOffset);
        SC_SWITCH_CASE(useLastTimeOffset);
        SC_SWITCH_CASE(recursive);
        SC_SWITCH_CASE(orthogonalCoords);
        SC_SWITCH_CASE(raimOk);
        SC_SWITCH_CASE(timeOk);
        SC_SWITCH_CASE(timeScaleType);

        SC_SWITCH_CASE(hour);
        SC_SWITCH_CASE(minute);
        SC_SWITCH_CASE(second);

        SC_SWITCH_CASE(timeFrac);

        SC_SWITCH_CASE(year);
        SC_SWITCH_CASE(month);
        SC_SWITCH_CASE(day);

        SC_SWITCH_CASE(lat);
        SC_SWITCH_CASE(lon);
        SC_SWITCH_CASE(h);

        SC_SWITCH_CASE(vLat);
        SC_SWITCH_CASE(vLon);
        SC_SWITCH_CASE(vH);

        SC_SWITCH_CASE(rmsCoord);
        SC_SWITCH_CASE(rmsH);
        SC_SWITCH_CASE(rmsTime);
        SC_SWITCH_CASE(rmsVHoriz);
        SC_SWITCH_CASE(rmsVH);
#undef SC_SWITCH_CASE
    }
    return nullptr;
}

SC_DEFINE_TOSTRING_AND_ENUM_STREAM_OPERATORS_VIA_TOSTRING_IMPL(BinaryT::Solution::Field::Type)

const char *ToString_impl(BinaryT::Solution::TimeScaleType val)
{
    switch(val)
    {
    case BinaryT::Solution::TimeScaleType::invalid: break;
#define SC_SWITCH_CASE(_X) case BinaryT::Solution::TimeScaleType::_X: return #_X
        SC_SWITCH_CASE(Any);
        SC_SWITCH_CASE(UTC_US);
        SC_SWITCH_CASE(UTC_Rus);
        SC_SWITCH_CASE(GPS);
        SC_SWITCH_CASE(GLONASS);
#undef SC_SWITCH_CASE
    }
    return nullptr;
}
SC_DEFINE_TOSTRING_AND_ENUM_STREAM_OPERATORS_VIA_TOSTRING_IMPL(BinaryT::Solution::TimeScaleType)

const char *ToString_impl(BinaryT::SatState::HeaderField::Type val)
{
    switch(val)
    {
    case BinaryT::SatState::HeaderField::count: break;
#define SC_SWITCH_CASE(_X) case BinaryT::SatState::HeaderField::_X: return #_X
        SC_SWITCH_CASE(gnssType);
        SC_SWITCH_CASE(satsCount);
        SC_SWITCH_CASE(angle);
        SC_SWITCH_CASE(visibleSatsCount);
#undef SC_SWITCH_CASE
    }
    return nullptr;
}
SC_DEFINE_TOSTRING_AND_ENUM_STREAM_OPERATORS_VIA_TOSTRING_IMPL(BinaryT::SatState::HeaderField::Type)

const char *ToString_impl(BinaryT::SatState::SatField::Type val)
{
    switch(val)
    {
    case BinaryT::SatState::SatField::count: break;
#define SC_SWITCH_CASE(_X) case BinaryT::SatState::SatField::_X: return #_X
        SC_SWITCH_CASE(satNum);
        SC_SWITCH_CASE(satState);
        SC_SWITCH_CASE(satValid);
        SC_SWITCH_CASE(hasDiffData);
        SC_SWITCH_CASE(almanachAge);
        SC_SWITCH_CASE(elev);
        SC_SWITCH_CASE(azimuth);
#undef SC_SWITCH_CASE
    }
    return nullptr;
}
SC_DEFINE_TOSTRING_AND_ENUM_STREAM_OPERATORS_VIA_TOSTRING_IMPL(BinaryT::SatState::SatField::Type)

const char *ToString_impl(BinaryT::SatState::GNSSType val)
{
    switch(val)
    {
    case BinaryT::SatState::GNSSType::Reserve_1:
    case BinaryT::SatState::GNSSType::Reserve_2:
        break;
#define SC_SWITCH_CASE(_X) case BinaryT::SatState::GNSSType::_X: return #_X
        SC_SWITCH_CASE(GPS);
        SC_SWITCH_CASE(GLONASS);
#undef SC_SWITCH_CASE
    }
    return nullptr;
}
SC_DEFINE_TOSTRING_AND_ENUM_STREAM_OPERATORS_VIA_TOSTRING_IMPL(BinaryT::SatState::GNSSType)

const char *ToString_impl(BinaryT::MessageType val)
{
    switch(val)
    {
    case BinaryT::MessageType::Invalid: break;
#define SC_SWITCH_CASE(_X) case BinaryT::MessageType::_X: return #_X
        SC_SWITCH_CASE(Solution);
        SC_SWITCH_CASE(SatState);
        SC_SWITCH_CASE(TimeScale);
        SC_SWITCH_CASE(TimeScaleOffset);
        SC_SWITCH_CASE(ChannelStats);
#undef SC_SWITCH_CASE
    }
    return nullptr;
}

const char *ToString_impl(BinaryT::TimeScale::Field::Type val)
{
    switch(val)
    {
    case BinaryT::TimeScale::Field::count: break;
#define SC_SWITCH_CASE(_X) case BinaryT::TimeScale::Field::_X: return #_X
        SC_SWITCH_CASE(hasHeight);
        SC_SWITCH_CASE(autoDim);
        SC_SWITCH_CASE(useLastHeight);
        SC_SWITCH_CASE(offsetByMeasurement);
        SC_SWITCH_CASE(autoTimeOffset);
        SC_SWITCH_CASE(useLastTimeOffset);
        SC_SWITCH_CASE(recursive);
        SC_SWITCH_CASE(orthogonalCoords);
        SC_SWITCH_CASE(raimOk);
        SC_SWITCH_CASE(timeOk);
        SC_SWITCH_CASE(timeScaleType);

        SC_SWITCH_CASE(hour);
        SC_SWITCH_CASE(minute);
        SC_SWITCH_CASE(second);

        SC_SWITCH_CASE(timeFrac);

        SC_SWITCH_CASE(year);
        SC_SWITCH_CASE(month);
        SC_SWITCH_CASE(day);

        SC_SWITCH_CASE(frequencyCorr);
        SC_SWITCH_CASE(rmsTime);
        SC_SWITCH_CASE(rmsFrequencyCorr);

#undef SC_SWITCH_CASE
    }
    return nullptr;
}

SC_DEFINE_TOSTRING_AND_ENUM_STREAM_OPERATORS_VIA_TOSTRING_IMPL(BinaryT::TimeScale::Field::Type)

const char *ToString_impl(BinaryT::TimeScale::TimeScaleType val)
{
    switch(val)
    {
    case BinaryT::TimeScale::TimeScaleType::invalid: break;
#define SC_SWITCH_CASE(_X) case BinaryT::TimeScale::TimeScaleType::_X: return #_X
        SC_SWITCH_CASE(Any);
        SC_SWITCH_CASE(UTC_US);
        SC_SWITCH_CASE(UTC_Rus);
        SC_SWITCH_CASE(GPS);
        SC_SWITCH_CASE(GLONASS);
#undef SC_SWITCH_CASE
    }
    return nullptr;
}
SC_DEFINE_TOSTRING_AND_ENUM_STREAM_OPERATORS_VIA_TOSTRING_IMPL(BinaryT::TimeScale::TimeScaleType)


const char *ToString_impl(BinaryT::TimeScaleOffset::Field::Type val)
{
    switch(val)
    {
    case BinaryT::TimeScaleOffset::Field::count: break;
#define SC_SWITCH_CASE(_X) case BinaryT::TimeScaleOffset::Field::_X: return #_X
        SC_SWITCH_CASE(methodCorr);

        SC_SWITCH_CASE(offset);
        SC_SWITCH_CASE(rmsOffset);

#undef SC_SWITCH_CASE
    }
    return nullptr;
}

SC_DEFINE_TOSTRING_AND_ENUM_STREAM_OPERATORS_VIA_TOSTRING_IMPL(BinaryT::TimeScaleOffset::Field::Type)

const char *ToString_impl(BinaryT::TimeScaleOffset::Method val)
{
    switch(val)
    {
    case BinaryT::TimeScaleOffset::Method::invalid: break;
#define SC_SWITCH_CASE(_X) case BinaryT::TimeScaleOffset::Method::_X: return #_X
        SC_SWITCH_CASE(commandMessage);
        SC_SWITCH_CASE(navigSolution);
        SC_SWITCH_CASE(acceptPDD);
        SC_SWITCH_CASE(diffScales);
#undef SC_SWITCH_CASE
    }
    return nullptr;
}
SC_DEFINE_TOSTRING_AND_ENUM_STREAM_OPERATORS_VIA_TOSTRING_IMPL(BinaryT::TimeScaleOffset::Method)


const char *ToString_impl(BinaryT::ChannelStats::Field::Type val)
{
    switch(val)
    {
    case BinaryT::ChannelStats::Field::count: break;
#define SC_SWITCH_CASE(_X) case BinaryT::ChannelStats::Field::_X: return #_X
        SC_SWITCH_CASE(gnssType);
        SC_SWITCH_CASE(satNum);
        SC_SWITCH_CASE(noSignal);
        SC_SWITCH_CASE(failTrackSignal);
        SC_SWITCH_CASE(failTrackPhase);
        SC_SWITCH_CASE(findSignal);
        SC_SWITCH_CASE(trackFreqAssign);
        SC_SWITCH_CASE(symbolSync);
        SC_SWITCH_CASE(subframeSync);
        SC_SWITCH_CASE(flagSignal);
        SC_SWITCH_CASE(receiveType);
        SC_SWITCH_CASE(measurmentReady);

        SC_SWITCH_CASE(elevation);
        SC_SWITCH_CASE(azimuth);
        SC_SWITCH_CASE(signalNoise);

#undef SC_SWITCH_CASE
    }
    return nullptr;
}
SC_DEFINE_TOSTRING_AND_ENUM_STREAM_OPERATORS_VIA_TOSTRING_IMPL(BinaryT::ChannelStats::Field::Type)

const char *ToString_impl(BinaryT::ChannelStats::GNSSType val)
{
    switch(val)
    {
    case BinaryT::ChannelStats::GNSSType::Reserve_1:
    case BinaryT::ChannelStats::GNSSType::Reserve_2:
        break;
#define SC_SWITCH_CASE(_X) case BinaryT::ChannelStats::GNSSType::_X: return #_X
        SC_SWITCH_CASE(GPS);
        SC_SWITCH_CASE(GLONASS);
#undef SC_SWITCH_CASE
    }
    return nullptr;
}
SC_DEFINE_TOSTRING_AND_ENUM_STREAM_OPERATORS_VIA_TOSTRING_IMPL(BinaryT::ChannelStats::GNSSType)


SC_DEFINE_TOSTRING_AND_ENUM_STREAM_OPERATORS_VIA_TOSTRING_IMPL(BinaryT::MessageType)

SC_DEFINE_TOQSTRING_AND_STREAM_OPERATORS_VIA_TOQSTRING_METHOD(BinaryT::Message)
