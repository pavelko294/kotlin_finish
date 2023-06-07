#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "portpickerdialog.h"
#include "parserofmessage.h"
#include "crutches.h"

#include <QMessageBox>
#include <QDebug>
#include <QLabel>
#include <QDateTime>
#include <QTableView>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui {new Ui::MainWindow},
    m_statusLabel {new QLabel(this)}
{
    m_ui->setupUi(this);
    statusBar()->addWidget(m_statusLabel);

    m_ui->widget_glonassConstellation->SetGps(false);
    m_ui->widget_gpsConstellation->SetGps(true);

    bool connectOk = true;
    connectOk &= static_cast<bool>(connect(&m_port, SIGNAL(errorOccurred(QSerialPort::SerialPortError)),
                                           this, SLOT(OnSerialPortErrorOccurred(QSerialPort::SerialPortError))));
    connectOk &= static_cast<bool>(connect(&m_port, SIGNAL(readyRead()),
                                           this, SLOT(OnSerialPortReadyRead())));
    /* потом убрать если что */
    //connect(&parser, SIGNAL(writer(QList)), &filer, SLOT(FormingString(QList)));
    // connect(&painter, SIGNAL(noPaint())), this, SLOT(on_pushButton_clicked(bool checked));

    Q_ASSERT(connectOk);

    m_file.setFileName(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"));
    if(!m_file.open(QIODevice::WriteOnly))
    {
        qWarning() << m_file.error() << ": " << m_file.errorString();
        qFatal(":-(");
    }
    OnSerialPortReadyRead();
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::on_action_openPort_triggered()
{
    if(m_port.isOpen())
        m_port.close();

    PortPickerDialog portPicker;

    // Частоты, указанные на с. 10 (на случай, если среди них есть те которые не поддерживаются ОС по умолчанию)
    portPicker.AddMoreBaudRates({4800, 9600, 19200, 38400});

    // TODO: заменить на параметры, на которые настроен блок
    portPicker.SetSettings({38400, QSerialPort::Data8, QSerialPort::OddParity, QSerialPort::OneStop, QSerialPort::NoFlowControl});

    if(portPicker.exec() != QDialog::Accepted) // пользователь нажал отмену
        return;

    QSerialPortInfo chosenPort;
    if(portPicker.GetPortInfo(&chosenPort))
    {
        QMessageBox::warning(nullptr, tr("Ошибка"), tr("Выберите порт."));
        return;
    }

    m_port.setPort(chosenPort);
    m_port.setReadBufferSize(1000000);

    QString culprit;
    if(OpenAndTune(portPicker.GetSettings(), &culprit))
    {
        qWarning() << culprit << ": " << m_port.error() << ": " << m_port.errorString();
        QMessageBox::warning(nullptr, tr("Ошибка"), culprit + ": " + m_port.errorString());
        return;
    }

    qDebug() << "Port " << chosenPort.portName() << " opened with descriptor " << m_port.handle();
}

bool MainWindow::OpenAndTune(const SerialPortSettings &settings, QString *culprit)
{
    if(!m_port.open(QIODevice::ReadWrite))
    {
        (*culprit) = "open";
        return true;
    }
    m_bytesRead = 0;

    if(settings.TunePort(&m_port, culprit))
        return true;

    return false;
}

const MainWindow::TableStuff &MainWindow::GetTableStuff(BinaryT::MessageType messageType)
{
    if(!m_tables.contains(messageType))
    {
        TableStuff newTableStuff;

        newTableStuff.tab = new QWidget();
        newTableStuff.tab->setObjectName(QString("tab %1").arg(::ToString(messageType)));
        m_ui->tabWidget_tables->addTab(newTableStuff.tab, ::ToString(messageType));

        newTableStuff.tableView = new QTableView(newTableStuff.tab);
        newTableStuff.tableView->setObjectName(QString("tableView %1").arg(::ToString(messageType)));

        newTableStuff.verticalLayout = new QVBoxLayout(newTableStuff.tab);
        newTableStuff.verticalLayout->setObjectName(QString("verticalLayout %1").arg(::ToString(messageType)));
        newTableStuff.verticalLayout->addWidget(newTableStuff.tableView);

        newTableStuff.messageModel = new MessageModel(newTableStuff.tableView);

        newTableStuff.tableView->horizontalHeader()->setSectionsMovable(true);
        newTableStuff.tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
        newTableStuff.tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        newTableStuff.tableView->setSortingEnabled(false);
        newTableStuff.tableView->setModel(newTableStuff.messageModel);

        m_tables.insert(messageType, newTableStuff);
    }

    return FindValue(m_tables, messageType);
}

void MainWindow::OnSerialPortErrorOccurred(QSerialPort::SerialPortError error)
{
    if(error != QSerialPort::NoError)
        qWarning() << "Port error " << error;
}

void MainWindow::OnSerialPortReadyRead()
{
    //    forever
    //    {
    //        const qint64 bytesAvailable {m_port.bytesAvailable()};
    //        if(bytesAvailable < 1)
    //            //break;

    //        static constexpr int bufSize {4096};
    //        quint8 buf[bufSize] {};

    //        const qint64 bytesToRead {qMin<qint64>(bufSize, bytesAvailable)};
    //        {
    //            const qint64 ret {m_port.read(reinterpret_cast<char*>(buf), bytesToRead)};
    //            if(ret != bytesToRead)
    //            {
    //                qWarning() << ret << "!=" << bytesToRead << ": " << m_port.error() << ": " << m_port.errorString();
    //                m_port.close();
    //                //return;
    //            }
    //            m_bytesRead += ret;
    //        }

    //        if(m_file.isOpen())
    //        {
    //            const qint64 ret {m_file.write(reinterpret_cast<const char*>(buf), bytesToRead)};
    //            if(ret != bytesToRead)
    //            {
    //                qWarning() << ret << "!=" << bytesToRead << ": " << m_file.error() << ": " << m_file.errorString();
    //                m_file.close();
    //            }
    //        }

   // QFile file("/home/pavelkopytin/2023-04-07-14-11-37");
    QFile file("C:/Users/user/Desktop/dump/2023-04-07-14-11-37");
    if (!file.open(QIODevice::ReadOnly)){
        qDebug() << "я ничего не открыл";
        return;
    }
    qDebug() << "Файл открыт!";

    QByteArray message;
    message = file.readAll();
    char *buffer = message.data();
    int n = message.count();
    quint8 buf[n];
    for(int i = 0; i < n; i++)
    {
        buf[i] = *(buffer+i);
    }

    qDebug() << "Массив создан";

    for(int idx = 0; idx < /* bytesToRead*/ n; idx ++)
    {
        if(m_parser.Feed(buf[idx]))
        {
            quint8 messageId {0};
            const QVector<quint8> payload {m_parser.GetPayload(&messageId)};
            //qDebug() << "Got message " << messageId << ", " << payload.size() << " B payload";
            //parser.Parser(payload, messageId);

            const BinaryT::Message message {BinaryT::Message::Parse(messageId, payload)};
            if(message.IsValid())
            {
                const TableStuff &tableStuff (GetTableStuff(message.GetMessageType()));
                tableStuff.messageModel->SetMessage(message);

                if(message.GetMessageType() == BinaryT::MessageType::SatState)
                {
                    m_ui->widget_glonassConstellation->SetSatStateMessage(message);
                    m_ui->widget_gpsConstellation->SetSatStateMessage(message);
                }
                if(message.GetMessageType() == BinaryT::MessageType::TimeScale)
                {
                    SC_CHECK_EQUALS(message.GetMessageType(), BinaryT::MessageType::TimeScale);
                    freqcorr = ExtractFromVar<double>(message.GetField(BinaryT::TimeScale::Field::frequencyCorr));
                    m_ui->widget_freqcorr->updatePlot(time, freqcorr);
                    time++;
                }


            }
        }
    }

    m_statusLabel->setText(tr("%1 байт получено").arg(m_bytesRead));
}



void MainWindow::on_checkBox_stateChanged(int check)
{
    if (check == 1)
    {
        painter.paintNoValid = true;
        painter.repaint();
    }
    else if(check == 0){
       painter.paintNoValid = false;
       painter.repaint();
    }
}

