#include "satstatelabel.h"
#include "crutches.h"

#include <QPainter>
#include <QPaintEvent>
#include <QtMath>

SatStateLabel::SatStateLabel(QWidget *p) :
    QWidget(p)
{
}

void SatStateLabel::paintEvent(QPaintEvent *event)
{
    QPainter painter {this};

    const QFontMetricsF fontMetrics (painter.fontMetrics());

    const double outerCircleR {qMin(width(), height()) / 2.0};
    if(outerCircleR <= 0)
        return;

    // Размер кружка для обозн. спутника как доля от большого круга
    const double satCircleRelR {0.1};


    const QPointF center (outerCircleR, outerCircleR);

    painter.fillRect(event->rect(), Qt::black);

    painter.save();
    painter.translate(center);

    painter.setPen(Qt::white);
    painter.drawEllipse(QPointF(), outerCircleR, outerCircleR);
    painter.drawEllipse(QPointF(), outerCircleR / 2, outerCircleR / 2);

    foreach (const SatState &satState, m_satStates)
    {
        double elev {satState.elev};
        const bool belowHorizon {elev < 0};
        if(belowHorizon)
            elev *= -1;

        painter.save();

        painter.rotate(qRadiansToDegrees(satState.azimuth));

        double elevPixels = M_PI_2;
        elevPixels -= elev;
        elevPixels /= M_PI_2;
        elevPixels *= outerCircleR;

        painter.translate(QPointF(0, -elevPixels));
        painter.rotate(-qRadiansToDegrees(satState.azimuth));


        QColor color;
        if(satState.valid)
        {
            if(m_gps != true)
                color = QColor(Qt::blue);
            else
                color = QColor(Qt::green);
        }
        else
            color = QColor(Qt::red);

        if(belowHorizon){
            if (paintNoValid == false)
                color.setAlphaF(0.0);
            else if (paintNoValid == true)
                color.setAlphaF(0.5);
        };
        painter.setPen(color);

        {
            const QRectF boundingRect {fontMetrics.boundingRect(satState.name)};
            const double w {boundingRect.width()};
            const double h {boundingRect.height()};
            painter.drawText(QRectF(-w / 2, -h / 2, w, h), Qt::TextDontClip, satState.name);
        }

        painter.drawEllipse(QPointF(), outerCircleR * satCircleRelR, outerCircleR * satCircleRelR);

        painter.restore();
    }

    painter.restore();
}

QSize SatStateLabel::sizeHint() const
{
    return QSize(400, 400);
}

void SatStateLabel::SetGps(bool on)
{
    m_gps = on;
}

void SatStateLabel::SetSatStateMessage(const BinaryT::Message &message)
{
    SC_CHECK_EQUALS(message.GetMessageType(), BinaryT::MessageType::SatState);

    bool isGps {false};
    {
        const BinaryT::SatState::GNSSType gnssType {ExtractFromVar<BinaryT::SatState::GNSSType>(message.GetField(BinaryT::SatState::HeaderField::gnssType))};
        switch (gnssType)
        {
        case BinaryT::SatState::GNSSType::GPS:
            isGps = true;
            break;

        case BinaryT::SatState::GNSSType::GLONASS:
            isGps = false;
            break;

        default:
            scWarning() << "Unsupported GNSS type " << gnssType;
            return;
        }
    }

    if(m_gps != isGps)
        return;

    m_satStates.clear();

    int recsNum {message.GetFieldsNum()};
    recsNum -= BinaryT::SatState::HeaderField::count;
    SC_CHECK_MOREOREQ(recsNum, 0);

    SC_CHECK_EQUALS(recsNum % BinaryT::SatState::SatField::count, 0);
    recsNum /= BinaryT::SatState::SatField::count;

    for(int recIdx = 0; recIdx < recsNum; recIdx ++)
    {
        SatState newSatState;

        const int fieldOffset {recIdx * BinaryT::SatState::SatField::count + BinaryT::SatState::HeaderField::count};

        {
            const quint16 satNum {ExtractFromVar<quint16>(message.GetField(fieldOffset + BinaryT::SatState::SatField::satNum))};
            if(isGps)
                newSatState.name = QString::number(satNum & 0xff);
            else
            {
                quint16 literal {satNum};
                literal &= 0x1f;
                int literal_s = literal;
                literal_s -= 7;

                quint16 id {satNum};
                id >>= 5;
                id &= 0x1f;

                newSatState.name = QString("%1 (%2)").arg(id).arg(literal);
            }
        }

        newSatState.valid = ExtractFromVar<bool>(message.GetField(fieldOffset + BinaryT::SatState::SatField::satValid));

        newSatState.elev = ExtractFromVar<float>(message.GetField(fieldOffset + BinaryT::SatState::SatField::elev));
        if(newSatState.elev > M_PI_2 || newSatState.elev < -M_PI_2)
            scWarning() << newSatState.name << ": " << SC_STRINGIFY(newSatState.elev);

        newSatState.azimuth = ExtractFromVar<float>(message.GetField(fieldOffset + BinaryT::SatState::SatField::azimuth));
        if(newSatState.azimuth > M_PI || newSatState.azimuth < -M_PI)
            scWarning() << newSatState.name << ": " << SC_STRINGIFY(newSatState.azimuth);

        m_satStates.append(newSatState);
    }

    //scDebug() << m_satStates.size() << " satellites";

    update();
}
