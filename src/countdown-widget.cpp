#include "countdown-widget.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTime>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

CountdownWidget::CountdownWidget(QWidget *parent) : OBSDock(parent)
{
    // Initialize UI components
    mainLayout = new QVBoxLayout(this);
    
    // Create buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    addButton = new QPushButton(tr("Add Timer"), this);
    exportExcelButton = new QPushButton(tr("Export Excel"), this);
    exportTextButton = new QPushButton(tr("Export Text"), this);
    
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(exportExcelButton);
    buttonLayout->addWidget(exportTextButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Set up timer
    updateTimer = new QTimer(this);
    updateTimer->setInterval(1000); // Update every second
    updateTimer->start();
    
    // Connect signals
    connect(addButton, &QPushButton::clicked, this, &CountdownWidget::onAddRecord);
    connect(exportExcelButton, &QPushButton::clicked, this, &CountdownWidget::onExportExcel);
    connect(exportTextButton, &QPushButton::clicked, this, &CountdownWidget::onExportText);
    connect(updateTimer, &QTimer::timeout, this, &CountdownWidget::onTimerUpdate);
    
    // Initial setup
    setMinimumSize(300, 200);
}

CountdownWidget::~CountdownWidget()
{
    updateTimer->stop();
}

void CountdownWidget::onAddRecord()
{
    TimerRecord record;
    record.speakerType = tr("Speaker");
    record.name = tr("New Timer");
    record.startTime = QTime::currentTime();
    record.endTime = record.startTime;
    record.totalTime = QTime(0, 0);
    record.isExpanded = true;
    
    records.append(record);
    createTimerRow();
}

void CountdownWidget::onExportExcel()
{
    // TODO: Implement Excel export
    QMessageBox::information(this, tr("Export"), tr("Excel export not implemented yet"));
}

void CountdownWidget::onExportText()
{
    QString content;
    QTextStream stream(&content);
    
    for (const auto &record : records) {
        stream << record.speakerType << "," 
               << record.name << ","
               << record.startTime.toString("hh:mm:ss") << ","
               << record.endTime.toString("hh:mm:ss") << ","
               << record.totalTime.toString("hh:mm:ss") << "\n";
    }
    
    QFile file("timer_export.csv");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << content;
        file.close();
        QMessageBox::information(this, tr("Export"), tr("Data exported to timer_export.csv"));
    } else {
        QMessageBox::warning(this, tr("Export"), tr("Failed to export data"));
    }
}

void CountdownWidget::onTimerUpdate()
{
    for (int i = 0; i < records.size(); ++i) {
        updateTotalTime(i);
    }
}

void CountdownWidget::onToggleExpand(int index)
{
    if (index >= 0 && index < records.size()) {
        records[index].isExpanded = !records[index].isExpanded;
        // TODO: Update UI to reflect expansion state
    }
}

void CountdownWidget::onDeleteRecord(int index)
{
    if (index >= 0 && index < records.size()) {
        records.remove(index);
        // TODO: Update UI to reflect removal
    }
}

void CountdownWidget::onStartStop(int index)
{
    if (index >= 0 && index < records.size()) {
        auto &record = records[index];
        if (record.startTime == record.endTime) {
            record.startTime = QTime::currentTime();
        } else {
            record.endTime = QTime::currentTime();
        }
        updateTotalTime(index);
    }
}

void CountdownWidget::createTimerRow()
{
    // TODO: Implement timer row UI creation
}

void CountdownWidget::updateTotalTime(int index)
{
    if (index >= 0 && index < records.size()) {
        auto &record = records[index];
        int secs = record.startTime.secsTo(record.endTime);
        record.totalTime = QTime(0, 0).addSecs(secs);
    }
}

QString CountdownWidget::formatTime(const QTime &time) const
{
    return time.toString("hh:mm:ss");
} 