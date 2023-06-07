#ifndef CRUTCHES_TPP
#define CRUTCHES_TPP

#include "crutches.h"
#include "scassert.h"

#include <QTextStream>

template<typename T>
T ExtractFromVar(const QVariant &var)
{
    Q_ASSERT(var.isValid());

    const int neededType {qMetaTypeId<T>()};
    const int actualType {var.userType()};
    if(neededType != actualType)
        qFatal(qPrintable(BadMetaTypeString(neededType, actualType)));

    Q_ASSERT(var.canConvert<T>());
    return var.value<T>();
}

template<typename T>
QList<T> ToList(const QSet<T> &set)
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
    return set.toList();
#else // Note: Since Qt 5.14, range constructors are available for Qt's generic container classes and should be used in place of this method. © ман QSet::toList
    return QList<T>(set.begin(), set.end());
#endif
}

template<typename T>
QString StringifyUsingTextStream(const T &val)
{
    QString ret;
    {
        QTextStream stream (&ret);
        stream << val;
    }
    return ret;
}

template<typename T>
T MakeMask(int bits)
{
    SC_CHECK_MOREOREQ(bits, 0);
    SC_CHECK_LESSOREQ(bits, static_cast<qint64>(sizeof(T) * 8));
    T ret {0};
    for(int i = 0; i < bits; i ++)
    {
        ret <<= 1;
        ret |= 1;
    }
    return ret;
}

template<typename Key_T, typename Value_T>
const Value_T &FindValue(const QHash<Key_T, Value_T> &hash, const Key_T &key)
{
    const typename QHash<Key_T, Value_T>::const_iterator iter {hash.constFind(key)};
    SC_ASSERT(iter != hash.constEnd());
    SC_CHECK_EQUALS(iter.key(), key);
    return iter.value();
}

#endif // CRUTCHES_TPP
