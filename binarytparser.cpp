#include "binarytparser.h"
#include "crutches.h"
#include "scassert.h"

#include <QtEndian> // формат чисел - LE (с. 12)
#include <QDebug>

namespace BinaryT
{

/* Parser */

Parser::~Parser()
{
    if(m_autoDumpStats)
        scDebug() << "BinaryT::Parser stats:\n" << GetStats();
}

bool Parser::Feed(quint8 byte)
{
    switch (m_state)
    {
    case State::WaitingForHeader_1:
        if(byte != header_1)
        {
            m_counters[Counter_badHeader_1] ++;
            //scDebug() << "badHeader_1: " << byte;
            Reset();
        }
        else
            m_state = State::WaitingForHeader_2;
        return false;

    case State::WaitingForHeader_2:
        if(byte != header_2)
        {
            m_counters[Counter_badHeader_2] ++;
            //scDebug() << "badHeader_2: " << byte;
            Reset();
        }
        else
            m_state = State::WaitingForDataLength;
        return false;

    case State::WaitingForDataLength:
        m_dataLength = byte;
        m_state = State::WaitingForMessageId;
        return false;

    case State::WaitingForMessageId:
        m_messageId = byte;
        m_state = State::WaitingForChecksumLo;
        return false;

    case State::WaitingForChecksumLo:
        m_neededChecksum = byte;
        m_state = State::WaitingForChecksumHi;
        return false;

    case State::WaitingForChecksumHi:
    {
        quint16 t {byte};
        t <<= 8;
        m_neededChecksum |= t;

        ::memset(m_payload, 0x00, sizeof(m_payload));
        m_payloadValidBytes = -1; // если в буфере был валидный пакет, сейчас он уже не валиден

        // Протокол допускает пустые пакеты.
        if(m_dataLength == 0)
        {
            if(CheckChecksum())
            {
                Reset();
                return false;
            }

            m_payloadValidBytes = 0;
            Reset();
            return true;
        }

        // Если данные есть - приступаем к их приёму.
        m_state = State::GettingData;
        return false;
    }

    case State::GettingData:
        Q_ASSERT(m_gotPayloadBytes >= 0 && m_gotPayloadBytes < static_cast<qint64>(sizeof(m_payload)));
        m_payload[m_gotPayloadBytes] = byte;
        m_gotPayloadBytes ++;

        int neededPayloadBytes {m_dataLength};
        neededPayloadBytes *= 2; // длина измеряется в в 16-разрядных словах

        if(m_gotPayloadBytes == neededPayloadBytes) // приняли пакет до конца
        {
            if(CheckChecksum())
            {
                Reset();
                return false;
            }

            m_payloadValidBytes = m_gotPayloadBytes;
            Reset();
            return true;
        }

        Q_ASSERT(m_gotPayloadBytes < neededPayloadBytes);
        return false;
    }
    qFatal("Invalid state");
    return false;
}

QVector<quint8> Parser::GetPayload(messageId_t *messageId) const
{
    Q_ASSERT(m_payloadValidBytes >= 0 && m_payloadValidBytes < static_cast<qint64>(sizeof(m_payload)));

    QVector<quint8> ret (m_payloadValidBytes);
    ::memcpy(ret.data(), m_payload, ret.size());

    if(messageId)
        (*messageId) = m_messageId;
    return ret;
}

quint16 Parser::CalcChecksum(quint8 dataLength, messageId_t messageId, const quint8 *payload)
{
    quint64 checksum {0};

    checksum += header;

    {
        quint16 t {messageId};
        t <<= 8;
        t |= dataLength;
        checksum += t;
    }

    for(int idx = 0; idx < dataLength; idx ++)
        checksum += qFromLittleEndian<quint16>(&(payload[idx * 2]));

    // "Биты переноса с битовой позиции 15, генерируемые в процессе суммирования, складываются
    // с результатом суммирования" (с. 15) <--- это как?? А если после этого сложения они опять вылезут за второй байт?..
    {
        quint64 t {checksum};
        t &= 0xffffffffffff0000u;
        t >>= 16;
        checksum += t;
    }

    checksum &= 0xffffu;

    return checksum;
}

bool Parser::CheckChecksum()
{
    // Вычисляем контрольную сумму.
    const quint64 checksum {CalcChecksum(m_dataLength, m_messageId, m_payload)};

    if(checksum != m_neededChecksum)
    {
        m_counters[Counter_badChecksum] ++;
        scDebug() << "badChecksum: " << checksum << "!=" << m_neededChecksum;
        return true;
    }
    return false;
}

QString Parser::GetStats() const
{
    QString ret;
    {
        QTextStream stream {&ret};
        for(std::underlying_type<Counters>::type i = 0; i < Counters_count; i ++)
        {
            if(!m_counters[i])
                continue;
            if(i)
                stream << "\n";
            stream << ::ToString(static_cast<Counters>(i)) << ": " << m_counters[i];
        }
    }
    return ret;
}

void Parser::Reset()
{
    m_state = State::WaitingForHeader_1;
    //m_messageId = 0;
    m_dataLength = 0;
    m_neededChecksum = 0;
    // NB: не зануляем m_payload
    m_gotPayloadBytes = 0;
    // NB: не сбрасываем m_payloadValidBytes
}
} // BinaryT


/* СТРИНГИФИКАТОРЫ */

const char *ToString_impl(BinaryT::Parser::Counters val)
{
    switch(val)
    {
    case BinaryT::Parser::Counters_count: break;
#define SC_SWITCH_CASE(_X) case BinaryT::Parser::Counter_ ## _X: return #_X
        SC_SWITCH_CASE(badHeader_1);
        SC_SWITCH_CASE(badHeader_2);
        SC_SWITCH_CASE(badChecksum);
#undef SC_SWITCH_CASE
    }
    return nullptr;
}
SC_DEFINE_TOSTRING_VIA_TOSTRING_IMPL(BinaryT::Parser::Counters)
