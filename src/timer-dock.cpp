#include "timer-dock.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QScrollArea>
#include <QClipboard>
#include <QApplication>
#include <QPropertyAnimation>
#include <QStackedWidget>
#include <QFrame>
#include <QFileDialog>
#include <QStandardPaths>
#include <QStringConverter>
#include <QScreen>
#include <QGuiApplication>

const int TimerDock::DEFAULT_SPEAKER_TIMES[] = {10, 15, 20, 30, 40, 60};
const int TimerDock::DEFAULT_DISCUSSANT_TIMES[] = {5, 10, 15, 20, 30};

TimerDock::TimerDock(QWidget *parent)
    : QDockWidget(parent)
{
    setObjectName("TimerDock");
    setWindowTitle("演讲计时器");
    
    // 设置默认大小
    setMinimumWidth(400);
    setMinimumHeight(300);
    
    // 设置停靠窗口特性
    setAllowedAreas(Qt::AllDockWidgetAreas);  // 允许在所有区域停靠
    setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
    
    // 初始时隐藏窗口
    hide();
    
    setupUI();

    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &TimerDock::updateAllTimes);
    updateTimer->start(1000);

    // 初始化错误提示相关组件
    errorLabel = new QLabel(this);
    errorLabel->setStyleSheet("QLabel { color: #ff4444; background-color: #333333; padding: 8px; border-radius: 4px; }");
    errorLabel->setAlignment(Qt::AlignCenter);
    errorLabel->hide();

    fadeAnimation = new QPropertyAnimation(errorLabel, "windowOpacity", this);
    fadeAnimation->setDuration(500);  // 500ms for fade in/out

    errorTimer = new QTimer(this);
    errorTimer->setSingleShot(true);
    connect(errorTimer, &QTimer::timeout, this, &TimerDock::hideErrorMessage);
    
    // 连接可见性变化信号
    connect(this, &QDockWidget::visibilityChanged, this, &TimerDock::onVisibilityChanged);
}

void TimerDock::onVisibilityChanged(bool visible)
{
    if (visible) {
        // 如果窗口变为可见，且是浮动状态，则移动到合适的位置
        if (isFloating()) {
            // 获取父窗口（OBS Studio主窗口）
            if (QWidget *parentWindow = parentWidget()) {
                QRect parentGeometry = parentWindow->geometry();
                // 设置初始位置为父窗口左上角，留出一些边距
                move(parentGeometry.x() + 20, parentGeometry.y() + 20);
            }
        }
    }
}

TimerDock::~TimerDock()
{
    updateTimer->stop();
    for (int i = 0; i < records.size(); ++i) {
        removeRecordWidgets(i);
    }
}

void TimerDock::setupUI()
{
    // Create and set the main widget
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // Create scroll area
    QScrollArea *scrollArea = new QScrollArea(mainWidget);
    scrollArea->setWidgetResizable(true);
    QWidget *scrollWidget = new QWidget(scrollArea);
    recordsLayout = new QVBoxLayout(scrollWidget);
    recordsLayout->setContentsMargins(6, 6, 6, 6);
    recordsLayout->setSpacing(6);
    
    // Top controls group
    auto topGroup = new QWidget();
    auto topLayout = new QHBoxLayout(topGroup);
    topLayout->setContentsMargins(10, 0, 10, 0);
    topLayout->setSpacing(6);

    // Speaker min time group
    auto speakerLabel = new QLabel("讲者最低时间:", topGroup);
    speakerLabel->setMinimumWidth(80);
    topLayout->addWidget(speakerLabel);

    speakerMinTimeCombo = new QComboBox(topGroup);
    speakerMinTimeCombo->setMinimumWidth(80);
    for (int time : DEFAULT_SPEAKER_TIMES) {
        speakerMinTimeCombo->addItem(QString("%1分钟").arg(time), time);
    }
    speakerMinTimeCombo->addItem("自定义...", -1);
    topLayout->addWidget(speakerMinTimeCombo);

    // 设置讲者默认时间为30分钟
    int defaultSpeakerIndex = -1;
    for (int i = 0; i < sizeof(DEFAULT_SPEAKER_TIMES)/sizeof(DEFAULT_SPEAKER_TIMES[0]); ++i) {
        if (DEFAULT_SPEAKER_TIMES[i] == 30) {
            defaultSpeakerIndex = i;
            break;
        }
    }
    if (defaultSpeakerIndex != -1) {
        speakerMinTimeCombo->setCurrentIndex(defaultSpeakerIndex);
        customMinTimes[0] = 30;
    }

    // Add spinbox for custom speaker time
    QSpinBox *speakerCustomTime = new QSpinBox(topGroup);
    speakerCustomTime->setMinimum(1);
    speakerCustomTime->setMaximum(999);
    speakerCustomTime->setSuffix("分钟");
    speakerCustomTime->hide();
    topLayout->addWidget(speakerCustomTime);

    topLayout->addSpacing(12);

    // Discussant min time group
    auto discussantLabel = new QLabel("讨论嘉宾最低时间:", topGroup);
    discussantLabel->setMinimumWidth(80);
    topLayout->addWidget(discussantLabel);

    discussantMinTimeCombo = new QComboBox(topGroup);
    discussantMinTimeCombo->setMinimumWidth(80);
    for (int time : DEFAULT_DISCUSSANT_TIMES) {
        discussantMinTimeCombo->addItem(QString("%1分钟").arg(time), time);
    }
    discussantMinTimeCombo->addItem("自定义...", -1);
    topLayout->addWidget(discussantMinTimeCombo);

    // 设置讨论嘉宾默认时间为10分钟
    int defaultDiscussantIndex = -1;
    for (int i = 0; i < sizeof(DEFAULT_DISCUSSANT_TIMES)/sizeof(DEFAULT_DISCUSSANT_TIMES[0]); ++i) {
        if (DEFAULT_DISCUSSANT_TIMES[i] == 10) {
            defaultDiscussantIndex = i;
            break;
        }
    }
    if (defaultDiscussantIndex != -1) {
        discussantMinTimeCombo->setCurrentIndex(defaultDiscussantIndex);
        customMinTimes[1] = 10;
    }

    // Add spinbox for custom discussant time
    QSpinBox *discussantCustomTime = new QSpinBox(topGroup);
    discussantCustomTime->setMinimum(1);
    discussantCustomTime->setMaximum(999);
    discussantCustomTime->setSuffix("分钟");
    discussantCustomTime->hide();
    topLayout->addWidget(discussantCustomTime);
    
    // Connect signals for speaker time
    connect(speakerMinTimeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [speakerCustomTime, this](int index) {
                int value = speakerMinTimeCombo->itemData(index).toInt();
                if (value == -1) {
                    // Show spinbox for custom time
                    speakerCustomTime->show();
                    speakerCustomTime->setValue(customMinTimes[0]);
                } else {
                    speakerCustomTime->hide();
                    customMinTimes[0] = value;
                }
                // 更新所有讲者类型的记录
                for (int i = 0; i < records.size(); ++i) {
                    if (records[i].record.type == SpeakerType::Speaker) {
                        updateTotalTime(i);
                    }
                }
            });
    
    connect(speakerCustomTime, QOverload<int>::of(&QSpinBox::valueChanged),
            [this](int value) {
                customMinTimes[0] = value;
                // 更新所有讲者类型的记录
                for (int i = 0; i < records.size(); ++i) {
                    if (records[i].record.type == SpeakerType::Speaker) {
                        updateTotalTime(i);
                    }
                }
            });

    // Connect signals for discussant time
    connect(discussantMinTimeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [discussantCustomTime, this](int index) {
                int value = discussantMinTimeCombo->itemData(index).toInt();
                if (value == -1) {
                    // Show spinbox for custom time
                    discussantCustomTime->show();
                    discussantCustomTime->setValue(customMinTimes[1]);
                } else {
                    discussantCustomTime->hide();
                    customMinTimes[1] = value;
                }
                // 更新所有讨论嘉宾类型的记录
                for (int i = 0; i < records.size(); ++i) {
                    if (records[i].record.type == SpeakerType::Discussant) {
                        updateTotalTime(i);
                    }
                }
            });
    
    connect(discussantCustomTime, QOverload<int>::of(&QSpinBox::valueChanged),
            [this](int value) {
                customMinTimes[1] = value;
                // 更新所有讨论嘉宾类型的记录
                for (int i = 0; i < records.size(); ++i) {
                    if (records[i].record.type == SpeakerType::Discussant) {
                        updateTotalTime(i);
                    }
                }
            });

    topLayout->addStretch();
    recordsLayout->addWidget(topGroup);

    // Add a stretching spacer
    recordsLayout->addStretch();

    // Bottom buttons group
    auto bottomGroup = new QWidget();
    auto bottomLayout = new QHBoxLayout(bottomGroup);
    bottomLayout->setContentsMargins(10, 0, 10, 0);
    bottomLayout->setSpacing(6);

    // Add record button
    auto addRecordButton = new QPushButton(tr("添加记录"), bottomGroup);
    addRecordButton->setMinimumWidth(80);
    connect(addRecordButton, &QPushButton::clicked, this, &TimerDock::onAddRecord);
    bottomLayout->addWidget(addRecordButton);

    // Export Excel button
    auto exportExcelButton = new QPushButton(tr("导出表格"), bottomGroup);
    exportExcelButton->setMinimumWidth(80);
    connect(exportExcelButton, &QPushButton::clicked, this, &TimerDock::exportToExcel);
    bottomLayout->addWidget(exportExcelButton);

    // Export text button
    auto exportTextButton = new QPushButton(tr("导出文本"), bottomGroup);
    exportTextButton->setMinimumWidth(80);
    connect(exportTextButton, &QPushButton::clicked, this, &TimerDock::exportToText);
    bottomLayout->addWidget(exportTextButton);

    // Appreciation button
    auto appreciationButton = new QPushButton(tr("赞赏我"), bottomGroup);
    appreciationButton->setMinimumWidth(80);
    connect(appreciationButton, &QPushButton::clicked, this, &TimerDock::showAppreciation);
    bottomLayout->addWidget(appreciationButton);

    recordsLayout->addWidget(bottomGroup);

    scrollArea->setWidget(scrollWidget);
    mainLayout->addWidget(scrollArea);
    
    setWidget(mainWidget);

    // 创建默认的记录项
    onAddRecord();
}

QWidget *TimerDock::createRecordWidget(int index)
{
    RecordWidgets widgets;
    QWidget *container = new QWidget();
    widgets.container = container;
    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(6);

    // Top row
    QWidget *topWidget = new QWidget(container);
    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(6);

    // Type combo
    widgets.typeCombo = new QComboBox(topWidget);
    widgets.typeCombo->addItem("讲者", static_cast<int>(SpeakerType::Speaker));
    widgets.typeCombo->addItem("讨论嘉宾", static_cast<int>(SpeakerType::Discussant));
    widgets.typeCombo->setFixedWidth(100);
    topLayout->addWidget(widgets.typeCombo);

    // Name edit
    widgets.nameEdit = new QLineEdit(topWidget);
    widgets.nameEdit->setPlaceholderText("姓名");
    widgets.nameEdit->setFixedWidth(100);
    topLayout->addWidget(widgets.nameEdit);

    // Add segment button
    widgets.addButton = new QPushButton("+", topWidget);
    widgets.addButton->setMinimumWidth(80);
    topLayout->addWidget(widgets.addButton);

    // Collapse/Expand button
    QPushButton *expandBtn = new QPushButton("收起", topWidget);
    expandBtn->setMinimumWidth(80);
    expandBtn->setProperty("recordIndex", index);
    expandBtn->setObjectName("expandBtn");
    topLayout->addWidget(expandBtn);

    // Total time label
    widgets.totalLabel = new QLabel("累计: 00:00", topWidget);
    widgets.totalLabel->setMinimumWidth(80);
    widgets.totalLabel->setAlignment(Qt::AlignCenter);
    
    // 获取系统主题颜色
    QPalette pal = palette();
    QColor bgColor = pal.color(QPalette::Button);
    QColor textCol = pal.color(QPalette::ButtonText);
    QColor borderCol = pal.color(QPalette::Mid);
    
    widgets.totalLabel->setStyleSheet(QString("QLabel { padding: 4px; border: 0.5px solid %3; border-radius: 2px; background-color: %1; color: %2; font-weight: bold; font-size: 14px; min-height: 24px; }")
        .arg(bgColor.name())
        .arg(textCol.name())
        .arg(borderCol.name()));
    
    topLayout->addWidget(widgets.totalLabel);

    // Delete button
    widgets.deleteButton = new QPushButton("删除", topWidget);
    widgets.deleteButton->setMinimumWidth(80);
    topLayout->addWidget(widgets.deleteButton);

    containerLayout->addWidget(topWidget);

    // Create a container for segments
    QWidget *segmentsWrapper = new QWidget(container);
    segmentsWrapper->setObjectName("segmentsWrapper");  // 添加对象名称以便查找
    QVBoxLayout *wrapperLayout = new QVBoxLayout(segmentsWrapper);
    wrapperLayout->setContentsMargins(0, 0, 0, 0);
    wrapperLayout->setSpacing(0);
    
    // Segments container
    widgets.segmentsContainer = new QFrame(segmentsWrapper);
    widgets.segmentsLayout = new QVBoxLayout(widgets.segmentsContainer);
    widgets.segmentsLayout->setContentsMargins(216, 0, 0, 0);
    widgets.segmentsLayout->setSpacing(6);
    
    // Add segments container to wrapper
    wrapperLayout->addWidget(widgets.segmentsContainer);
    wrapperLayout->addStretch();
    containerLayout->addWidget(segmentsWrapper);

    // Initialize record data
    widgets.record.isExpanded = true;
    widgets.record.type = SpeakerType::Discussant;

    // Store widgets in records vector
    records.push_back(widgets);

    // Connect signals after adding to records vector
    connect(widgets.addButton, &QPushButton::clicked,
            [this, index]() { onAddSegment(index); });
    connect(widgets.deleteButton, &QPushButton::clicked,
            [this, index]() { onDeleteRecord(index); });
    connect(widgets.typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this, index](int idx) { 
                records[index].record.type = static_cast<SpeakerType>(idx);
                updateTotalTime(index);  // 更新总时间显示，这会重新判断是否达标
            });
    connect(widgets.nameEdit, &QLineEdit::textChanged,
            [this, index](const QString &text) { records[index].record.name = text; });
    connect(expandBtn, &QPushButton::clicked,
            [this, index, expandBtn]() {
                auto &record = records[index];
                record.record.isExpanded = !record.record.isExpanded;
                
                // 查找segmentsWrapper
                QWidget *segmentsWrapper = record.container->findChild<QWidget*>("segmentsWrapper");
                if (segmentsWrapper) {
                    segmentsWrapper->setVisible(record.record.isExpanded);
                    expandBtn->setText(record.record.isExpanded ? "收起" : "展开");
                    
                    // 强制更新布局
                    record.container->adjustSize();
                    if (QWidget *parent = record.container->parentWidget()) {
                        parent->adjustSize();
                    }
                }
            });

    return container;
}

QWidget *TimerDock::createSegmentWidget(int recordIndex, int segmentIndex)
{
    SegmentWidgets widgets;
    QWidget *container = new QWidget();
    widgets.container = container;
    QHBoxLayout *layout = new QHBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);

    // Start button
    widgets.startButton = new QPushButton("记录开始时间", container);
    widgets.startButton->setMinimumWidth(80);
    widgets.startButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout->addWidget(widgets.startButton);

    // End button
    widgets.endButton = new QPushButton("记录结束时间", container);
    widgets.endButton->setMinimumWidth(80);
    widgets.endButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    widgets.endButton->setEnabled(false);
    layout->addWidget(widgets.endButton);

    // Duration label
    widgets.durationLabel = new QLabel("00:00", container);
    widgets.durationLabel->setMinimumWidth(80);
    widgets.durationLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    widgets.durationLabel->setAlignment(Qt::AlignCenter);
    
    // 获取系统主题颜色
    QPalette pal = palette();
    QColor bgColor = pal.color(QPalette::Button);
    QColor textCol = pal.color(QPalette::ButtonText);
    QColor borderCol = pal.color(QPalette::Mid);
    
    widgets.durationLabel->setStyleSheet(QString("QLabel { padding: 4px; border: 0.5px solid %3; border-radius: 2px; background-color: %1; color: %2; font-weight: bold; font-size: 14px; min-height: 24px; }")
        .arg(bgColor.name())
        .arg(textCol.name())
        .arg(borderCol.name()));
    
    layout->addWidget(widgets.durationLabel);

    // Delete button
    widgets.deleteButton = new QPushButton("删除", container);
    widgets.deleteButton->setMinimumWidth(80);
    widgets.deleteButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout->addWidget(widgets.deleteButton);

    // Connect signals
    connect(widgets.startButton, &QPushButton::clicked,
            [this, recordIndex, segmentIndex]() { onStartSegment(recordIndex, segmentIndex); });
    connect(widgets.endButton, &QPushButton::clicked,
            [this, recordIndex, segmentIndex]() { onEndSegment(recordIndex, segmentIndex); });
    connect(widgets.deleteButton, &QPushButton::clicked,
            [this, recordIndex, segmentIndex]() { onDeleteSegment(recordIndex, segmentIndex); });

    // Add to record's segments
    records[recordIndex].segments.push_back(widgets);
    records[recordIndex].record.segments.push_back(TimerSegment());

    return container;
}

void TimerDock::removeRecordWidgets(int index)
{
    if (index >= 0 && index < records.size()) {
        auto &widgets = records[index];
        
        // 先断开所有信号连接
        widgets.addButton->disconnect();
        widgets.deleteButton->disconnect();
        widgets.typeCombo->disconnect();
        widgets.nameEdit->disconnect();
        
        // 找到并断开展开/收起按钮的连接
        QPushButton *expandBtn = widgets.container->findChild<QPushButton*>("expandBtn");
        if (expandBtn) {
            expandBtn->disconnect();
        }
        
        // 从布局中移除并删除容器
        recordsLayout->removeWidget(widgets.container);
        delete widgets.container;
        records.erase(records.begin() + index);
        
        // 更新剩余记录的索引
        for (int i = index; i < records.size(); i++) {
            auto &record = records[i];
            
            // 重新连接信号
            connect(record.addButton, &QPushButton::clicked,
                    [this, i]() { onAddSegment(i); });
            connect(record.deleteButton, &QPushButton::clicked,
                    [this, i]() { onDeleteRecord(i); });
            connect(record.typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                    [this, i](int idx) { records[i].record.type = static_cast<SpeakerType>(idx); });
            connect(record.nameEdit, &QLineEdit::textChanged,
                    [this, i](const QString &text) { records[i].record.name = text; });
            
            // 重新连接展开/收起按钮
            QPushButton *expandBtn = record.container->findChild<QPushButton*>("expandBtn");
            if (expandBtn) {
                connect(expandBtn, &QPushButton::clicked,
                        [this, i, expandBtn]() {
                            auto &record = records[i];
                            record.record.isExpanded = !record.record.isExpanded;
                            
                            QWidget *segmentsWrapper = record.container->findChild<QWidget*>("segmentsWrapper");
                            if (segmentsWrapper) {
                                segmentsWrapper->setVisible(record.record.isExpanded);
                                expandBtn->setText(record.record.isExpanded ? "收起" : "展开");
                                
                                record.container->adjustSize();
                                if (QWidget *parent = record.container->parentWidget()) {
                                    parent->adjustSize();
                                }
                            }
                        });
            }
        }
    }
}

void TimerDock::removeSegmentWidgets(int recordIndex, int segmentIndex)
{
    if (recordIndex >= 0 && recordIndex < records.size()) {
        auto &record = records[recordIndex];
        if (segmentIndex >= 0 && segmentIndex < record.segments.size()) {
            auto &widgets = record.segments[segmentIndex];
            record.segmentsLayout->removeWidget(widgets.container);
            delete widgets.container;
            record.segments.erase(record.segments.begin() + segmentIndex);
            
            // 更新剩余时间段的按钮连接
            for (int i = segmentIndex; i < record.segments.size(); i++) {
                auto &segWidgets = record.segments[i];
                
                // 断开旧连接
                segWidgets.startButton->disconnect();
                segWidgets.endButton->disconnect();
                segWidgets.deleteButton->disconnect();
                
                // 重新连接信号
                connect(segWidgets.startButton, &QPushButton::clicked,
                        [this, recordIndex, i]() { onStartSegment(recordIndex, i); });
                connect(segWidgets.endButton, &QPushButton::clicked,
                        [this, recordIndex, i]() { onEndSegment(recordIndex, i); });
                connect(segWidgets.deleteButton, &QPushButton::clicked,
                        [this, recordIndex, i]() { onDeleteSegment(recordIndex, i); });
            }
        }
    }
}

void TimerDock::updateTotalTime(int recordIndex)
{
    if (recordIndex >= 0 && recordIndex < records.size()) {
        auto &record = records[recordIndex];
        QTime totalTime(0, 0);

        for (const auto &segment : record.record.segments) {
            if (!segment.startTime.isNull()) {
                QTime endTime = segment.endTime.isNull() ? QTime::currentTime() : segment.endTime;
                int secs = segment.startTime.secsTo(endTime);
                totalTime = totalTime.addSecs(secs);
            }
        }

        record.record.totalTime = totalTime;  // 保存总时间
        record.totalLabel->setText(QString("累计: %1").arg(formatTime(totalTime)));

        // 根据时间是否为0和是否达标设置不同的背景色
        bool isZero = totalTime == QTime(0, 0);
        bool isReached = isMinTimeReached(recordIndex);
        QString backgroundColor;
        QString textColor;
        QString borderColor;
        
        if (isZero) {
            // 获取系统主题颜色
            QPalette pal = palette();
            QColor bgColor = pal.color(QPalette::Button);
            QColor textCol = pal.color(QPalette::ButtonText);
            QColor borderCol = pal.color(QPalette::Mid);
            
            backgroundColor = bgColor.name();
            textColor = textCol.name();
            borderColor = borderCol.name();
        } else if (isReached) {
            backgroundColor = "#548235";  // 达标时的背景色改为深绿色
            textColor = "white";
            borderColor = "white";
        } else {
            backgroundColor = "#c00000";
            textColor = "white";
            borderColor = "white";
        }
        
        record.totalLabel->setStyleSheet(QString("QLabel { padding: 4px; border: 0.5px solid %3; border-radius: 2px; background-color: %1; color: %2; font-weight: bold; font-size: 14px; min-height: 24px; }")
            .arg(backgroundColor)
            .arg(textColor)
            .arg(borderColor));
    }
}

void TimerDock::onAddRecord()
{
    // 如果有现有记录，收起最后一条记录
    if (!records.empty()) {
        int lastIndex = records.size() - 1;
        auto &lastRecord = records[lastIndex];
        lastRecord.record.isExpanded = false;
        
        // 查找并隐藏segmentsWrapper
        QWidget *segmentsWrapper = lastRecord.container->findChild<QWidget*>("segmentsWrapper");
        if (segmentsWrapper) {
            segmentsWrapper->setVisible(false);
        }
        
        // 更新按钮文本
        QPushButton *expandBtn = lastRecord.container->findChild<QPushButton*>("expandBtn");
        if (expandBtn) {
            expandBtn->setText("展开");
        }
    }

    int newIndex = records.size();
    QWidget *recordWidget = createRecordWidget(newIndex);
    if (recordsLayout->count() > 1) {
        recordsLayout->insertWidget(recordsLayout->count() - 2, recordWidget);
    } else {
        recordsLayout->insertWidget(0, recordWidget);
    }

    // 为每个新增的记录项自动添加一个时间段
    onAddSegment(newIndex);

    // 更新所有记录项的删除按钮可见性
    updateRecordDeleteButtonsVisibility();
}

void TimerDock::onDeleteRecord(int index)
{
    // 如果删除的是最后一条记录，不需要特殊处理
    if (records.size() <= 1) {
        removeRecordWidgets(index);
        updateRecordDeleteButtonsVisibility();
        return;
    }

    // 如果删除的是最后一条记录，展开倒数第二条记录
    if (index == records.size() - 1) {
        int prevIndex = index - 1;
        auto &prevRecord = records[prevIndex];
        prevRecord.record.isExpanded = true;
        
        // 查找并显示segmentsWrapper
        QWidget *segmentsWrapper = prevRecord.container->findChild<QWidget*>("segmentsWrapper");
        if (segmentsWrapper) {
            segmentsWrapper->setVisible(true);
        }
        
        // 更新按钮文本
        QPushButton *expandBtn = prevRecord.container->findChild<QPushButton*>("expandBtn");
        if (expandBtn) {
            expandBtn->setText("收起");
        }
    }

    removeRecordWidgets(index);
    updateRecordDeleteButtonsVisibility();
}

void TimerDock::onAddSegment(int recordIndex)
{
    if (recordIndex >= 0 && recordIndex < records.size()) {
        auto &record = records[recordIndex];
        
        // 检查是否有未使用的时间段或正在计时的时间段
        bool hasUnusedSegment = false;
        bool hasRunningSegment = false;
        for (const auto &segment : record.record.segments) {
            if (segment.startTime.isNull()) {
                hasUnusedSegment = true;
                break;
            }
            if (segment.isRunning) {
                hasRunningSegment = true;
                break;
            }
        }
        
        if (hasUnusedSegment) {
            showErrorMessage("请先使用现有的时段");
            return;
        }
        
        if (hasRunningSegment) {
            showErrorMessage("请先结束当前正在计时的时段");
            return;
        }

        int segmentIndex = record.segments.size();
        QWidget *segmentWidget = createSegmentWidget(recordIndex, segmentIndex);
        record.segmentsLayout->addWidget(segmentWidget);

        // 更新时间段删除按钮的可见性
        updateSegmentDeleteButtonsVisibility(recordIndex);
    }
}

void TimerDock::onDeleteSegment(int recordIndex, int segmentIndex)
{
    if (recordIndex >= 0 && recordIndex < records.size()) {
        auto &record = records[recordIndex];
        if (segmentIndex >= 0 && segmentIndex < record.segments.size()) {
            // 获取要删除的时间段
            const auto &segment = record.record.segments[segmentIndex];
            
            // 如果时间段有开始和结束时间，从总时间中减去这段时间
            if (!segment.startTime.isNull() && !segment.endTime.isNull()) {
                int secs = segment.startTime.secsTo(segment.endTime);
                record.record.totalTime = record.record.totalTime.addSecs(-secs);
            }
            
            // 删除时间段组件
            removeSegmentWidgets(recordIndex, segmentIndex);
            record.record.segments.erase(record.record.segments.begin() + segmentIndex);
            
            // 更新总计时间显示
            updateTotalTime(recordIndex);
            
            // 更新时间段删除按钮的可见性
            updateSegmentDeleteButtonsVisibility(recordIndex);
        }
    }
}

void TimerDock::onStartSegment(int recordIndex, int segmentIndex)
{
    if (recordIndex >= 0 && recordIndex < records.size()) {
        auto &record = records[recordIndex];
        if (segmentIndex >= 0 && segmentIndex < record.segments.size()) {
            auto &segment = record.record.segments[segmentIndex];
            auto &widgets = record.segments[segmentIndex];
            
            QTime currentTime = QTime::currentTime();
            segment.startTime = currentTime;
            segment.isRunning = true;
            
            widgets.startButton->setEnabled(false);
            widgets.startButton->setText(currentTime.toString("HH:mm:ss"));
            widgets.endButton->setEnabled(true);
        }
    }
}

void TimerDock::onEndSegment(int recordIndex, int segmentIndex)
{
    if (recordIndex >= 0 && recordIndex < records.size()) {
        auto &record = records[recordIndex];
        if (segmentIndex >= 0 && segmentIndex < record.segments.size()) {
            auto &segment = record.record.segments[segmentIndex];
            auto &widgets = record.segments[segmentIndex];
            
            QTime currentTime = QTime::currentTime();
            segment.endTime = currentTime;
            segment.isRunning = false;
            
            widgets.startButton->setEnabled(false);
            widgets.endButton->setEnabled(false);
            widgets.endButton->setText(currentTime.toString("HH:mm:ss"));
            
            updateSegmentDisplay(recordIndex, segmentIndex);
            updateTotalTime(recordIndex);
        }
    }
}

void TimerDock::exportToText()
{
    // 计算最大列宽
    int roleWidth = 12;      // "讨论嘉宾" 长度为4个汉字
    int nameWidth = 8;       // 至少保留8个字符的宽度
    int timeWidth = 8;       // "HH:mm:ss" 长度为8
    int durationWidth = 5;   // "mm:ss" 长度为5
    int statusWidth = 4;     // "是/否" 长度为2个汉字

    // 遍历所有记录以找到最长的名字
    for (const auto &record : records) {
        nameWidth = qMax(nameWidth, record.record.name.length());
    }

    // 创建表头
    QString text = QString("%1\t%2\t%3\t%4\t%5\t%6\n")
        .arg("角色", -roleWidth)
        .arg("姓名", -nameWidth)
        .arg("开始时间", -timeWidth)
        .arg("结束时间", -timeWidth)
        .arg("累计时间", -durationWidth)
        .arg("是否达标", -statusWidth);
    
    for (int i = 0; i < records.size(); ++i) {
        const auto &record = records[i];
        QString role = record.typeCombo->currentText();  // 从typeCombo获取当前选择的角色
        QString name = record.record.name.isEmpty() ? "(未填写)" : record.record.name;
        bool isReached = record.record.totalTime.minute() >= getMinTime(i);
        
        for (const auto &segment : record.record.segments) {
            if (!segment.startTime.isNull()) {
                QString startTime = segment.startTime.toString("HH:mm:ss");
                QString endTime = segment.endTime.isNull() ? "进行中" : segment.endTime.toString("HH:mm:ss");
                QString duration = formatTime(segment.endTime.isNull() ? 
                    QTime(0, 0).addSecs(segment.startTime.secsTo(QTime::currentTime())) :
                    QTime(0, 0).addSecs(segment.startTime.secsTo(segment.endTime)));
                
                text += QString("%1\t%2\t%3\t%4\t%5\t%6\n")
                    .arg(role, -roleWidth)
                    .arg(name, -nameWidth)
                    .arg(startTime, -timeWidth)
                    .arg(endTime, -timeWidth)
                    .arg(duration, -durationWidth)
                    .arg(isReached ? "是" : "否", -statusWidth);
            }
        }
    }

    // 生成默认文件名（使用当前日期时间）
    QString defaultFileName = QString("Speech_Timer_%1.txt")
        .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));

    // 获取系统文档目录作为默认保存位置
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString filePath = QFileDialog::getSaveFileName(this,
        tr("保存文本文件"),
        defaultPath + "/" + defaultFileName,
        tr("文本文件 (*.txt)"));

    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream.setEncoding(QStringConverter::Utf8);
            stream << text;
            file.close();
            showErrorMessage("文件已保存");
        } else {
            showErrorMessage("保存文件失败");
        }
    }
}

void TimerDock::exportToExcel()
{
    QString csv = "角色,姓名,开始时间,结束时间,累计时间,是否达标\n";
    
    for (int i = 0; i < records.size(); ++i) {
        const auto &record = records[i];
        QString role = record.typeCombo->currentText();  // 从typeCombo获取当前选择的角色
        QString name = record.record.name.isEmpty() ? "(未填写)" : record.record.name;
        bool isReached = record.record.totalTime.minute() >= getMinTime(i);
        
        for (const auto &segment : record.record.segments) {
            if (!segment.startTime.isNull()) {
                QString startTime = segment.startTime.toString("HH:mm:ss");
                QString endTime = segment.endTime.isNull() ? "进行中" : segment.endTime.toString("HH:mm:ss");
                QString duration = formatTime(segment.endTime.isNull() ? 
                    QTime(0, 0).addSecs(segment.startTime.secsTo(QTime::currentTime())) :
                    QTime(0, 0).addSecs(segment.startTime.secsTo(segment.endTime)));
                
                // 处理CSV中的特殊字符
                if (name.contains(",")) {
                    name = QString("\"%1\"").arg(name);
                }
                
                csv += QString("%1,%2,%3,%4,%5,%6\n")
                    .arg(role)
                    .arg(name)
                    .arg(startTime)
                    .arg(endTime)
                    .arg(duration)
                    .arg(isReached ? "是" : "否");
            }
        }
    }

    // 生成默认文件名（使用当前日期时间）
    QString defaultFileName = QString("Speech_Timer_%1.csv")
        .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));

    // 获取系统文档目录作为默认保存位置
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString filePath = QFileDialog::getSaveFileName(this,
        tr("保存CSV文件"),
        defaultPath + "/" + defaultFileName,
        tr("CSV文件 (*.csv)"));

    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            // 写入 UTF-8 BOM，以确保Excel正确识别中文
            file.write("\xEF\xBB\xBF");
            
            QTextStream stream(&file);
            stream.setEncoding(QStringConverter::Utf8);
            stream << csv;
            file.close();
            showErrorMessage("文件已保存");
        } else {
            showErrorMessage("保存文件失败");
        }
    }
}

void TimerDock::showAppreciation()
{
    AppreciationDialog *dialog = new AppreciationDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void TimerDock::updateAllTimes()
{
    for (int i = 0; i < records.size(); ++i) {
        auto &record = records[i];
        for (int j = 0; j < record.segments.size(); ++j) {
            if (record.record.segments[j].isRunning) {
                updateSegmentDisplay(i, j);
            }
        }
        updateTotalTime(i);
    }
}

void TimerDock::updateSegmentDisplay(int recordIndex, int segmentIndex)
{
    if (recordIndex >= 0 && recordIndex < records.size()) {
        auto &record = records[recordIndex];
        if (segmentIndex >= 0 && segmentIndex < record.segments.size()) {
            auto &segment = record.record.segments[segmentIndex];
            auto &widgets = record.segments[segmentIndex];

            if (!segment.startTime.isNull()) {
                QTime endTime = segment.endTime.isNull() ? QTime::currentTime() : segment.endTime;
                int secs = segment.startTime.secsTo(endTime);
                QTime duration = QTime(0, 0).addSecs(secs);
                widgets.durationLabel->setText(formatTime(duration));

                // 根据时间是否为0和是否达标设置不同的背景色
                bool isZero = duration == QTime(0, 0);
                int minTime = getMinTime(recordIndex);
                bool isReached = duration.minute() >= minTime;
                QString backgroundColor;
                QString textColor;
                QString borderColor;
                
                if (isZero) {
                    // 获取系统主题颜色
                    QPalette pal = palette();
                    QColor bgColor = pal.color(QPalette::Button);
                    QColor textCol = pal.color(QPalette::ButtonText);
                    QColor borderCol = pal.color(QPalette::Mid);
                    
                    backgroundColor = bgColor.name();
                    textColor = textCol.name();
                    borderColor = borderCol.name();
                } else if (isReached) {
                    backgroundColor = "#548235";  // 达标时的背景色改为深绿色
                    textColor = "white";
                    borderColor = "white";
                } else {
                    backgroundColor = "#c00000";
                    textColor = "white";
                    borderColor = "white";
                }
                
                widgets.durationLabel->setStyleSheet(QString("QLabel { padding: 4px; border: 0.5px solid %3; border-radius: 2px; background-color: %1; color: %2; font-weight: bold; font-size: 14px; min-height: 24px; }")
                    .arg(backgroundColor)
                    .arg(textColor)
                    .arg(borderColor));
            }
        }
    }
}

bool TimerDock::isMinTimeReached(int recordIndex) const
{
    if (recordIndex >= 0 && recordIndex < records.size()) {
        const auto &record = records[recordIndex];
        int minTime = getMinTime(recordIndex);
        return record.record.totalTime.minute() >= minTime;
    }
    return false;
}

int TimerDock::getMinTime(int recordIndex) const
{
    if (recordIndex >= 0 && recordIndex < records.size()) {
        const auto &record = records[recordIndex];
        return record.record.type == SpeakerType::Speaker ? customMinTimes[0] : customMinTimes[1];
    }
    return 0;
}

QString TimerDock::formatTime(const QTime &time) const
{
    return time.toString("mm:ss");
}

void TimerDock::showErrorMessage(const QString &message)
{
    errorLabel->setText(message);
    errorLabel->adjustSize();
    
    // 将错误标签放置在窗口中央
    QPoint center = this->rect().center();
    errorLabel->move(center.x() - errorLabel->width() / 2,
                    center.y() - errorLabel->height() / 2);
    
    // 设置淡入效果
    fadeAnimation->setStartValue(0.0);
    fadeAnimation->setEndValue(1.0);
    
    errorLabel->show();
    fadeAnimation->start();
    
    // 3秒后开始淡出
    errorTimer->start(3000);
}

void TimerDock::hideErrorMessage()
{
    // 设置淡出效果
    fadeAnimation->setStartValue(1.0);
    fadeAnimation->setEndValue(0.0);
    connect(fadeAnimation, &QPropertyAnimation::finished, errorLabel, &QLabel::hide);
    fadeAnimation->start();
}

void TimerDock::updateRecordDeleteButtonsVisibility()
{
    // 如果只有一个记录项，隐藏其删除按钮
    if (records.size() == 1) {
        records[0].deleteButton->hide();
    } else {
        // 否则显示所有记录项的删除按钮
        for (auto &record : records) {
            record.deleteButton->show();
        }
    }
}

void TimerDock::updateSegmentDeleteButtonsVisibility(int recordIndex)
{
    if (recordIndex >= 0 && recordIndex < records.size()) {
        auto &record = records[recordIndex];
        
        // 如果只有一个时间段，隐藏其删除按钮
        if (record.segments.size() == 1) {
            record.segments[0].deleteButton->hide();
        } else {
            // 否则显示所有时间段的删除按钮
            for (auto &segment : record.segments) {
                segment.deleteButton->show();
            }
        }
    }
} 