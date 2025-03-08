#pragma once

#include <QDateTime>
#include <QString>
#include <QTime>
#include <vector>

#ifdef _WIN32
#ifdef EXPORT_LIB
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif
#else
#define EXPORT
#endif

enum class EXPORT SpeakerType {
    Speaker,
    Discussant
};

struct EXPORT TimerSegment {
    QTime startTime;
    QTime endTime;
    bool isRunning;

    TimerSegment() : isRunning(false) {}
};

struct EXPORT TimerRecord {
    QString name;
    SpeakerType type;
    QTime totalTime;
    bool isRunning;
    bool isExpanded;
    std::vector<TimerSegment> segments;

    TimerRecord() : type(SpeakerType::Speaker), isRunning(false), isExpanded(true) {}
}; 