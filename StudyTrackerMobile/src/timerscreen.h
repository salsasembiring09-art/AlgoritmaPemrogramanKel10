#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QTimer>
#include <QPainter>
#include "appstate.h"

class TimerRing : public QWidget {
    Q_OBJECT
public:  
    explicit TimerRing(QWidget* parent=nullptr);
    void setProgress(double p, bool running);
protected:
    void paintEvent(QPaintEvent*) override;
private:
    double  progress = 0.0; // 0..1
    bool    running  = false;
};

class TimerScreen : public QWidget {
    Q_OBJECT
public:
    explicit TimerScreen(QWidget* parent = nullptr);
    void refresh();
    void applyStyle();
    void start();
    void stop();
    void skip();

signals:
    void sessionCompleted(int sessions);
    void notifyRequested(const QString& icon, const QString& title, const QString& body);

private:
    TimerRing*   ring;
    QLabel*      lblTime;
    QLabel*      lblBadge;
    QLabel*      lblSessions;
    QLabel*      lblStatusTime;
    QSpinBox*    spFocus;
    QSpinBox*    spBreak;
    QLabel*      lbTotalFocus;
    QLabel*      lbLongest;
    QLabel*      lbSessions;
    QLabel*      lblClosestTask;
    QTimer*      ticker;

    void buildUi();
    void tick();
    void updateUI();
    void applySessionStyle();
    void updateClosestTask();
};
