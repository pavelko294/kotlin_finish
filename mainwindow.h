#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "serialportsettings.h"
#include "binarytparser.h"
#include "filewriter.h"
#include "messagemodel.h"
#include "satstatelabel.h"

#include <QMainWindow>
#include <QFile>
#include <QHash>

class QLabel;
class QVBoxLayout;
class QTableView;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

//public slots:
//void WriteToWindow(QStringList &names, QStringList &values);

private:
    Ui::MainWindow *const m_ui;
    QLabel *const m_statusLabel;
    QSerialPort m_port;
    qint64 m_bytesRead {0};
    BinaryT::Parser m_parser;
    QFile m_file;
    //ParserOfMessage parser;
    //BinaryT::Solution::Msg message;
    FileWriter filer;
    SatStateLabel painter;

    struct TableStuff
    {
        QWidget *tab {nullptr};
        QVBoxLayout *verticalLayout {nullptr};
        QTableView *tableView {nullptr};
        MessageModel *messageModel {nullptr};
    };
    QHash<BinaryT::MessageType, TableStuff> m_tables;

private:
    bool OpenAndTune(const SerialPortSettings &settings, QString *culprit);
    const TableStuff &GetTableStuff(BinaryT::MessageType messageType);

    double time = 1;
    double freqcorr;

private slots:
    void on_action_openPort_triggered();
    void OnSerialPortErrorOccurred(QSerialPort::SerialPortError error);
    void OnSerialPortReadyRead();
    void on_checkBox_stateChanged(int check);
};

#endif // MAINWINDOW_H
