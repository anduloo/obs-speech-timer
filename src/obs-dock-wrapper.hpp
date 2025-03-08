#pragma once

#include <QDockWidget>

#ifdef _WIN32
#ifdef EXPORT_LIB
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif
#else
#define EXPORT
#endif

class EXPORT OBSDock : public QDockWidget {
    Q_OBJECT

public:
    explicit OBSDock(QWidget *parent = nullptr) : QDockWidget(parent) {}
    virtual ~OBSDock() {}
}; 