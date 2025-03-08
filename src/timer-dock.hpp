#pragma once

#include <QDockWidget>
#include <QTimer>
#include <QTime>
#include <QVector>
#include <QPropertyAnimation>
#include <QLabel>
#include <vector>
#include "timer-record.hpp"
#include <QDialog>

class QComboBox;
class QLineEdit;
class QPushButton;
class QVBoxLayout;
class QFrame;

struct SegmentWidgets {
    QWidget *container;
    QPushButton *startButton;
    QPushButton *endButton;
    QLabel *durationLabel;
    QPushButton *deleteButton;

    SegmentWidgets() : container(nullptr), startButton(nullptr), 
                      endButton(nullptr), durationLabel(nullptr),
                      deleteButton(nullptr) {}
};

struct RecordWidgets {
    QWidget *container;
    QComboBox *typeCombo;
    QLineEdit *nameEdit;
    QPushButton *addButton;
    QPushButton *deleteButton;
    QLabel *totalLabel;
    QFrame *segmentsContainer;
    QVBoxLayout *segmentsLayout;
    QVector<SegmentWidgets> segments;
    TimerRecord record;
};

// 赞赏窗口类
class AppreciationDialog : public QDialog {
    Q_OBJECT
public:
    explicit AppreciationDialog(QWidget *parent = nullptr);
};

class TimerDock : public QDockWidget {
    Q_OBJECT

public:
    explicit TimerDock(QWidget *parent = nullptr);
    ~TimerDock();

private:
    void setupUI();
    void updateAllTimes();
    void updateTotalTime(int recordIndex);
    void updateSegmentDisplay(int recordIndex, int segmentIndex);
    void updateRecordDeleteButtonsVisibility();
    void updateSegmentDeleteButtonsVisibility(int recordIndex);
    void showErrorMessage(const QString &message);
    void hideErrorMessage();
    void onVisibilityChanged(bool visible);
    QWidget *createRecordWidget(int index);
    QWidget *createSegmentWidget(int recordIndex, int segmentIndex);
    void removeRecordWidgets(int index);
    void removeSegmentWidgets(int recordIndex, int segmentIndex);
    bool isMinTimeReached(int recordIndex) const;
    int getMinTime(int recordIndex) const;
    QString formatTime(const QTime &time) const;

    // 新增导出函数
    void exportToText();
    void exportToExcel();
    void showAppreciation();

    QVBoxLayout *recordsLayout;
    QComboBox *speakerMinTimeCombo;
    QComboBox *discussantMinTimeCombo;
    QTimer *updateTimer;
    QVector<RecordWidgets> records;
    int customMinTimes[2] = {10, 5};  // 默认讲者10分钟，讨论嘉宾5分钟

    // 错误提示相关
    QLabel *errorLabel;
    QPropertyAnimation *fadeAnimation;
    QTimer *errorTimer;

    static const int DEFAULT_SPEAKER_TIMES[];
    static const int DEFAULT_DISCUSSANT_TIMES[];

private Q_SLOTS:
    void onAddRecord();
    void onDeleteRecord(int index);
    void onAddSegment(int recordIndex);
    void onDeleteSegment(int recordIndex, int segmentIndex);
    void onStartSegment(int recordIndex, int segmentIndex);
    void onEndSegment(int recordIndex, int segmentIndex);
    void onExportText() { exportToText(); }
}; 