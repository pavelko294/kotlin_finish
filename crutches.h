#ifndef CRUTCHES_H
#define CRUTCHES_H

#include <QVariant>

class QComboBox;

// Вспомогательное барахло

#define SC_CAST_ENUM_TO_UNDERLYING_TYPE(_X) static_cast<std::underlying_type<decltype(_X)>::type>(_X)

#define SC_STRINGIFY(_X) #_X << " = " << _X

#define SC_REGISTER_METATYPE(_NS, _TYPE) \
    const int _NS::MetaTypes::_TYPE ## _metaType {qRegisterMetaType<_NS::_TYPE>()}

// Имена метатипов
QString MetaTypeString(int val);
QString BadMetaTypeString(int neededType, int actualType);

// Вариант, привязанный к текущему элементу comboBox
QVariant ComboBoxCurrentData(QComboBox *comboBox);

// Извлечь значение из варианта (с проверками)
template<typename T>
T ExtractFromVar(const QVariant &var);

// Костыль для Qt 5.14+
template<typename T>
QList<T> ToList(const QSet<T> &set);

// Длина самого длинного слова в списке
int LongestWord(const QStringList &list);

// Выровнять колонки по ширине
QString Align(const QStringList &a, const QStringList &b);

// Стрингифицировать при помощи QTextStream
template<typename T>
QString StringifyUsingTextStream(const T &val);

// Вывести угол в виде "градусы минуты секунды"
QString ToDMSString(double radians);

// Создать маску типа T длиной bits бит
template<typename T>
T MakeMask(int bits);

// true - ошибка
bool ReadBits(const void *srcBufVoid, int srcBufSize,
              void *dstBufVoid, int dstBufSize,
              int bitOffset, int bitsNum);

template<typename T>
QVariant MakeVariant(const T &val)
{
    QVariant var;
    var.setValue(val);
    return var;
}

QString HexRepresentation(const void *buf, int size);

// Не приемлет отсутствия запрошенного ключа
template<typename Key_T, typename Value_T>
const Value_T &FindValue(const QHash<Key_T, Value_T> &hash, const Key_T &key);

#include "crutches.tpp"

#endif // CRUTCHES_H
