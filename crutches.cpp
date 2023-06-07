#include "crutches.h"
#include "scassert.h"
#include "scdebug.h"

#include <QComboBox>
#include <QtMath>

QString MetaTypeString(int val)
{
    return QString("%1 (%2)").arg(QMetaType::typeName(val)).arg(val);
}

QString BadMetaTypeString(int neededType, int actualType)
{
    return QString("needed %1, got %2")
            .arg(MetaTypeString(neededType))
            .arg(MetaTypeString(actualType));
}

QVariant ComboBoxCurrentData(QComboBox *comboBox)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)) // "This property was introduced in Qt 5.2" © ман QComboBox::currentData
    return comboBox->currentData();
#else
    return comboBox->itemData(comboBox->currentIndex());
#endif
}

int LongestWord(const QStringList &list)
{
    int ret {0};
    foreach (const QString &str, list)
    {
        const int len {str.length()};
        if(ret < len)
            ret = len;
    }
    return ret;
}

QString Align(const QStringList &a, const QStringList &b)
{
    const int rows {a.size()};
    SC_CHECK_EQUALS(rows, b.size());

    const int aWidth {LongestWord(a)};
    QString ret;
    for(int row = 0; row < rows; row ++)
    {
        if(row)
            ret.append("\n");
        const QString str {a.at(row)};
        ret.append(str);
        ret.append(": ");
        ret.append(QString(aWidth - str.length(), QChar(' ')));
        ret.append(b.at(row));
    }
    return ret;
}

QString ToDMSString(double radians)
{
    double degrees {qRadiansToDegrees(radians)};
    QString ret;
    if(degrees < 0)
    {
        degrees *= -1;
        ret.append("-");
    }

    {
        const qint64 degreesInt = degrees;
        ret.append(QString::number(degreesInt));
        ret.append("°");
        degrees -= degreesInt;
    }

    degrees *= 60;
    {
        const qint64 minutesInt = degrees;
        ret.append(QString::number(minutesInt));
        ret.append("′");
        degrees -= minutesInt;
    }

    degrees *= 60;
    {
        ret.append(QString::number(degrees));
        ret.append("″");
    }

    return ret;
}

// true - ошибка
bool ReadBits(const void *srcBufVoid, int srcBufSize,
              void *dstBufVoid, int dstBufSize,
              int bitOffset, int bitsNum)
{
    SC_ASSERT(srcBufVoid);
    SC_CHECK_MORE(srcBufSize, 0);
    SC_ASSERT(dstBufVoid);
    SC_CHECK_MORE(dstBufSize, 0);
    SC_CHECK_MOREOREQ(bitOffset, 0);
    SC_CHECK_MORE(bitsNum, 0);

    const quint8 *const srcBuf {static_cast<const quint8*>(srcBufVoid)};
    quint8 *const dstBuf {static_cast<quint8*>(dstBufVoid)};

    for(int dstBitIdx = 0; dstBitIdx < bitsNum; dstBitIdx ++)
    {
        const int srcBitIdx {bitOffset + dstBitIdx};

        const int srcByteIdx {srcBitIdx / 8};
        //SC_CHECK_LESS(srcByteIdx, srcBufSize);
        if(srcByteIdx >= srcBufSize)
        {
            scWarning() << "Too small src buf: " << SC_STRINGIFY(srcBufSize)
                        << ", " << SC_STRINGIFY(bitOffset)
                        << ", " << SC_STRINGIFY(bitsNum);
            return true;
        }
        const quint8 srcByte {srcBuf[srcByteIdx]};

        const int bitInSrcByteIdx {srcBitIdx % 8};
        quint8 srcBit {1};
        srcBit <<= bitInSrcByteIdx;
        if(srcByte & srcBit)
        {
            const int dstByteIdx {dstBitIdx / 8};
            //SC_CHECK_LESS(dstByteIdx, dstBufSize);
            if(dstByteIdx >= dstBufSize)
            {
                scWarning() << "Too small dst buf: " << SC_STRINGIFY(dstBufSize)
                            << ", " << SC_STRINGIFY(bitOffset)
                            << ", " << SC_STRINGIFY(bitsNum);
                return true;
            }
            quint8 &dstByte {dstBuf[dstByteIdx]};

            const int bitInDstByteIdx {dstBitIdx % 8};
            quint8 dstBit {1};
            dstBit <<= bitInDstByteIdx;

            dstByte |= dstBit;
        }
    }

    return false;
}

QString HexRepresentation(const void *bufVoid, int size)
{
    QString ret;
    const quint8 *const buf {static_cast<const quint8*>(bufVoid)};
    for(int idx = 0; idx < size; idx ++)
    {
        if(idx)
            ret.append(" ");
        ret.append(QString("%1").arg(buf[idx], 2, 16, QChar('0')));
    }
    return ret;
}
