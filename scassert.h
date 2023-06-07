#ifndef SCASSERT_H
#define SCASSERT_H

#include <QString>
#include <QDebug>

template<typename X_T, typename Y_T>
void SCThrowBadRelation(const X_T &x, const Y_T &y,
                        const char* xName, const char* yName,
                        const char* operatorName,
                        const char* file, int line)
{
    QString str;
    {
        QDebug debug {&str};
        debug.nospace().noquote() << file << ":" << line
                                  << ": condition "
                                  << xName << operatorName << yName << " is not met: "
                                  << xName << " = " << x << "; " << yName << " = " << y;
    }
    qFatal(qPrintable(str));
}

#define SC_CHECK_BASE(_X, _Y, _COND) \
{ \
    const decltype(_X) x = _X; \
    const decltype(_Y) y = _Y; \
    if(!(x _COND y)) \
    SCThrowBadRelation(x, y, #_X, #_Y, #_COND, __FILE__, __LINE__); \
    }

#define SC_CHECK_EQUALS(_X, _Y) SC_CHECK_BASE(_X, _Y, ==)
#define SC_CHECK_UNEQUALS(_X, _Y) SC_CHECK_BASE(_X, _Y, !=)
#define SC_CHECK_MORE(_X, _Y) SC_CHECK_BASE(_X, _Y, >)
#define SC_CHECK_LESS(_X, _Y) SC_CHECK_BASE(_X, _Y, <)
#define SC_CHECK_MOREOREQ(_X, _Y) SC_CHECK_BASE(_X, _Y, >=)
#define SC_CHECK_LESSOREQ(_X, _Y) SC_CHECK_BASE(_X, _Y, <=)

template<typename X_T>
void SCThrowInvalidValue(const X_T &x,
                         const char* xName,
                         const char* file, int line)
{
    QString str;
    {
        QDebug debug {&str};
        debug.nospace().noquote() << "Invalid value of " << xName << ": " << x
                                  << " @ " << file << ":" << line;
    }
    qFatal(qPrintable(str));
}

#define SC_THROW(_X) \
    SCThrowInvalidValue(_X, #_X, __FILE__, __LINE__)

#define SC_ASSERT(_X) \
{ \
    const decltype(_X) x = _X; \
    if(!x) \
    SCThrowInvalidValue(x, #_X, __FILE__, __LINE__); \
    }

#endif // SCASSERT_H
