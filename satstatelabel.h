#ifndef SATSTATELABEL_H
#define SATSTATELABEL_H

#include "binaryt.h"

#include <QWidget>

class SatStateLabel : public QWidget
{
    Q_OBJECT

public:
    explicit SatStateLabel(QWidget *p = nullptr);

    struct SatState
    {
        QString name;
        bool valid {false};
        double elev {0};
        double azimuth {0};
    };
    typedef QVector<SatState> satStates_t;

private:
    bool m_gps {false};


    satStates_t m_satStates;

private:
    void paintEvent(QPaintEvent *event) override;

    virtual QSize sizeHint() const override;

public:
    void SetGps(bool on);
    bool paintNoValid {false};

    // Передавать только сообщения типа SatState
    void SetSatStateMessage(const BinaryT::Message &message);
};

#endif // SATSTATELABEL_H
