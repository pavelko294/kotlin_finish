#include "portpickerdialog.h"
#include "ui_portpickerdialog.h"
#include "crutches.h"

#include <QMetaEnum>
#include <QDebug>
#include <QTimer>

namespace
{
// Заполнить comboBox элементами, соответствующие членам перечисления T с именем enumName
template<typename T>
void Populate(QComboBox *comboBox, const char *enumName)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)) // "This function was introduced in Qt 5.5" © ман QMetaEnum::fromType
    const QMetaEnum metaEnum {QMetaEnum::fromType<T>()};
    Q_UNUSED(enumName);
#else
    const int enumIdx {QSerialPort::staticMetaObject.indexOfEnumerator(enumName)};
    Q_ASSERT(enumIdx >= 0);
    const QMetaEnum metaEnum {QSerialPort::staticMetaObject.enumerator(enumIdx)};
#endif

    for(int i = 0; i < metaEnum.keyCount(); i ++)
    {
        const int val {metaEnum.value(i)};
        if(val < 0) // среди значений перечислений есть -1, которые означают "неизвестно" или "неверно"
            continue;

        QVariant var;
        var.setValue(val);
        comboBox->addItem(QString::fromUtf8(metaEnum.valueToKey(val)), var);
    }
}

// Выбрать в comboBox элемент, соответствующий значению neededVal.
// true - ошибка.
template<typename Var_T, typename Val_T>
bool SetIdx(QComboBox *comboBox, Val_T neededVal)
{
    const int count {comboBox->count()};
    int neededIdx {-1};
    for(int idx = 0; idx < count; idx ++)
    {
        const Var_T curVal {ExtractFromVar<Var_T>(comboBox->itemData(idx))};
        if(static_cast<Val_T>(curVal) == neededVal)
        {
            Q_ASSERT(neededIdx == -1);
            neededIdx = idx;
        }
    }
    if(neededIdx == -1)
        return true;
    Q_ASSERT(0 <= neededIdx && neededIdx < count);
    comboBox->setCurrentIndex(neededIdx);
    return false;
}
} // anon namespace

PortPickerDialog::PortPickerDialog(QWidget *p) :
    QDialog(p),
    m_ui {new Ui::PortPickerDialog}
{
    m_ui->setupUi(this);

    m_ui->tableView_ports->horizontalHeader()->setSectionsMovable(true);
    m_ui->tableView_ports->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_ui->tableView_ports->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_ui->tableView_ports->setSortingEnabled(false);
    m_ui->tableView_ports->setModel(&m_serialPortInfoModel);

    AddMoreBaudRates(QSerialPortInfo::standardBaudRates());

#define SC_POPULATE(_X) \
    Populate<QSerialPort::_X>(m_ui->comboBox_ ## _X, #_X)
    SC_POPULATE(DataBits);
    SC_POPULATE(Parity);
    SC_POPULATE(StopBits);
    SC_POPULATE(FlowControl);
#undef SC_POPULATE

    QTimer::singleShot(1, this, SLOT(on_pushButton_refresh_clicked()));
}

PortPickerDialog::~PortPickerDialog()
{
    {
        // См. ман QAbstractItemView::setModel.
        QItemSelectionModel *const selectionModel {m_ui->tableView_ports->selectionModel()};
        m_ui->tableView_ports->setModel(nullptr);
        if(selectionModel)
            delete selectionModel;
    }

    delete m_ui;
}

void PortPickerDialog::AddMoreBaudRates(const QList<SerialPortSettings::baudRate_t> &nonStandardBaudRates)
{
    QSet<SerialPortSettings::baudRate_t> curBaudRates;

    // Смотрим какие уже есть
    const int count {m_ui->comboBox_BaudRate->count()};
    for(int idx = 0; idx < count; idx ++)
    {
        const SerialPortSettings::baudRate_t curVal {ExtractFromVar<SerialPortSettings::baudRate_t>(m_ui->comboBox_BaudRate->itemData(idx))};
        if(curBaudRates.contains(curVal))
            qCritical() << "Duplicate occurrence of baud rate " << curVal;
        else
            curBaudRates.insert(curVal);
    }

    // Добавляем новые
    foreach (SerialPortSettings::baudRate_t baudRate, nonStandardBaudRates)
    {
        Q_ASSERT(baudRate > 0);
        curBaudRates.insert(baudRate);
    }

    // Сортируем по возрастанию
    QList<SerialPortSettings::baudRate_t> newBaudRates (ToList(curBaudRates));
    std::sort(newBaudRates.begin(), newBaudRates.end());

    // Перестраиваем comboBox
    m_ui->comboBox_BaudRate->clear();
    foreach (SerialPortSettings::baudRate_t baudRate, newBaudRates)
    {
        QVariant var;
        var.setValue(baudRate);
        m_ui->comboBox_BaudRate->addItem(QString::number(baudRate), var);
    }
}

bool PortPickerDialog::SetSettings(const SerialPortSettings &settings)
{
#define SC_SET_SETTING(_X) if(SetIdx<SerialPortSettings::baudRate_t>(m_ui->comboBox_ ## _X, settings._X)) return true
    SC_SET_SETTING(BaudRate);
#undef SC_SET_SETTING

#define SC_SET_SETTING(_X) if(SetIdx<int>(m_ui->comboBox_ ## _X, settings._X)) return true
    SC_SET_SETTING(DataBits);
    SC_SET_SETTING(Parity);
    SC_SET_SETTING(StopBits);
    SC_SET_SETTING(FlowControl);
#undef SC_SET_SETTING

    return false;
}

bool PortPickerDialog::GetPortInfo(QSerialPortInfo *info)
{
    const QItemSelection selection {m_ui->tableView_ports->selectionModel()->selection()};
    if(selection.isEmpty())
        return true;

    // SelectionMode установлено в SingleSelection.
    Q_ASSERT(selection.count() == 1);
    const QItemSelectionRange &range (selection.at(0));
    Q_ASSERT(range.isValid());
    Q_ASSERT(!range.isEmpty());

    const int selectedRow {range.top()};
    Q_ASSERT(selectedRow == range.bottom());

    const QSerialPortInfo &portInfo (m_serialPortInfoModel.GetPortInfo(selectedRow));
    if(info)
        (*info) = portInfo;
    return false;
}

SerialPortSettings PortPickerDialog::GetSettings() const
{
    SerialPortSettings ret;

    ret.BaudRate = ExtractFromVar<SerialPortSettings::baudRate_t>(ComboBoxCurrentData(m_ui->comboBox_BaudRate));

#define SC_GET_SETTING(_X) \
    ret._X = static_cast<QSerialPort::_X>(ExtractFromVar<int>(ComboBoxCurrentData(m_ui->comboBox_ ## _X)))
    SC_GET_SETTING(DataBits);
    SC_GET_SETTING(Parity);
    SC_GET_SETTING(StopBits);
    SC_GET_SETTING(FlowControl);
#undef SC_GET_SETTING

    return ret;
}

void PortPickerDialog::on_pushButton_refresh_clicked()
{
    m_serialPortInfoModel.Refresh();
}
