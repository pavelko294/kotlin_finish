#ifndef SCDEBUG_H
#define SCDEBUG_H

#include <QDebug>

#define scDebug qDebug().noquote().nospace
#define scWarning qWarning().noquote().nospace


#define SC_DECLARE_STREAM_OPERATORS(_TYPE) \
    QDebug operator<<(QDebug debug, _TYPE val); \
    QTextStream &operator<<(QTextStream &s, _TYPE val)

#define SC_DECLARE_TOSTRING_AND_STREAM_OPERATORS(_TYPE) \
    SC_DECLARE_STREAM_OPERATORS(_TYPE); \
    const char *ToString_impl(_TYPE val); \
    const char *ToString(_TYPE val)

#define SC_DEFINE_ENUM_QDEBUG_STREAM_OPERATOR_VIA_TOSTRING(_TYPE) \
    QDebug operator<<(QDebug debug, _TYPE val) \
{ \
    QDebugStateSaver stateSaver (debug); \
    debug.noquote().nospace() << ::ToString(val) << " (" << static_cast<std::underlying_type<_TYPE>::type>(val) << ")"; \
    return debug; \
    }

#define SC_DEFINE_ENUM_TEXT_STREAM_OPERATOR_VIA_TOSTRING(_TYPE) \
    QTextStream &operator<<(QTextStream &s, _TYPE val) \
{ \
    s << ::ToString(val) << " (" << static_cast<std::underlying_type<_TYPE>::type>(val) << ")"; \
    return s; \
    }

#define SC_DEFINE_TOSTRING_VIA_TOSTRING_IMPL(_TYPE) \
    const char *ToString(_TYPE val) \
{ \
    const char *const txt {ToString_impl(val)}; \
    if(txt) \
    return txt; \
    return "<unknown>"; \
    }

#define SC_DEFINE_TOSTRING_AND_ENUM_STREAM_OPERATORS_VIA_TOSTRING_IMPL(_TYPE) \
    SC_DEFINE_TOSTRING_VIA_TOSTRING_IMPL(_TYPE) \
    SC_DEFINE_ENUM_QDEBUG_STREAM_OPERATOR_VIA_TOSTRING(_TYPE) \
    SC_DEFINE_ENUM_TEXT_STREAM_OPERATOR_VIA_TOSTRING(_TYPE)

#define SC_DEFINE_TOQSTRING_VIA_TOQSTRING_METHOD(_TYPE) \
    QString ToString(const _TYPE &val) \
{ \
    return val.ToString(); \
    }

#define SC_DEFINE_QDEBUG_STREAM_OPERATOR_VIA_TOSTRING(_TYPE) \
    QDebug operator<<(QDebug debug, _TYPE val) \
{ \
    QDebugStateSaver stateSaver (debug); \
    debug.noquote().nospace() << ::ToString(val); \
    return debug; \
    }

#define SC_DEFINE_TEXT_STREAM_OPERATOR_VIA_TOSTRING(_TYPE) \
    QTextStream &operator<<(QTextStream &s, _TYPE val) \
{ \
    s << ::ToString(val); \
    return s; \
    }

#define SC_DEFINE_TOQSTRING_AND_STREAM_OPERATORS_VIA_TOQSTRING_METHOD(_TYPE) \
    SC_DEFINE_TOQSTRING_VIA_TOQSTRING_METHOD(_TYPE) \
    SC_DEFINE_QDEBUG_STREAM_OPERATOR_VIA_TOSTRING(const _TYPE &) \
    SC_DEFINE_TEXT_STREAM_OPERATOR_VIA_TOSTRING(const _TYPE &)

#endif // SCDEBUG_H
