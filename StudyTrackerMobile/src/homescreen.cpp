#include "homescreen.h"
#include <QHBoxLayout>
#include <QFrame>    
#include <QScrollArea>
#include <QTime>
#include <QTimer>
#include <algorithm>

HomeScreen::HomeScreen(QWidget* parent) : QWidget(parent) {
    buildUi();
    applyStyle();

    // Clock tick
    auto* clk = new QTimer(this);
    clk->setInterval(1000);
    connect(clk, &QTimer::timeout, this, [this](){
        lblStatusTime->setText(QTime::currentTime().toString("HH:mm"));
    });
    clk->start();

    connect(&AppState::instance(), &AppState::timerTick, this, [this](){
        updatePomadoroDisplay();
    });
    connect(&AppState::instance(), &AppState::tasksChanged, this, [this](){
        updateTaskPreview();
        refresh();
    });
}

void HomeScreen::buildUi() {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(0);

    // ── Status bar ──
    auto* statusBar = new QWidget(this);
    statusBar->setObjectName("statusBar");
    statusBar->setFixedHeight(44);
    auto* sbLay = new QHBoxLayout(statusBar);
    sbLay->setContentsMargins(20,0,20,0);
    lblStatusTime = new QLabel(QTime::currentTime().toString("HH:mm"), statusBar);
    lblStatusTime->setObjectName("statusTime");
    auto* sbMid = new QLabel("Semangat belajar!", statusBar);
    sbMid->setObjectName("labelMuted");
    auto* sbRight = new QLabel("📶 🔋", statusBar);
    sbLay->addWidget(lblStatusTime);
    sbLay->addStretch();
    sbLay->addWidget(sbMid);
    sbLay->addStretch();
    sbLay->addWidget(sbRight);
    root->addWidget(statusBar);

    // ── Scrollable content ──
    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    auto* content = new QWidget();
    auto* cl = new QVBoxLayout(content);
    cl->setContentsMargins(0,0,0,16);
    cl->setSpacing(0);
    scroll->setWidget(content);
    root->addWidget(scroll, 1);

    // ── Header ──
    auto* hdr = new QWidget(content);
    hdr->setObjectName("homeHeader");
    auto* hdrLay = new QHBoxLayout(hdr);
    hdrLay->setContentsMargins(20,20,20,16);
    auto* greetArea = new QWidget(hdr);
    auto* greetLay  = new QVBoxLayout(greetArea);
    greetLay->setContentsMargins(0,0,0,0); greetLay->setSpacing(3);
    auto* lbHello = new QLabel("Halo,", greetArea);
    lbHello->setObjectName("labelMuted");
    lblName   = new QLabel("Pengguna", greetArea);
    lblName->setObjectName("homeName");
    lblStatus = new QLabel("Semangat Belajar!", greetArea);
    lblStatus->setObjectName("homeStatus");
    greetLay->addWidget(lbHello);
    greetLay->addWidget(lblName);
    greetLay->addWidget(lblStatus);
    lblAvatar = new QLabel("😀", hdr);
    lblAvatar->setObjectName("avatarCircle");
    lblAvatar->setFixedSize(46,46);
    lblAvatar->setAlignment(Qt::AlignCenter);
    lblAvatar->setCursor(Qt::PointingHandCursor);
    hdrLay->addWidget(greetArea, 1);
    hdrLay->addWidget(lblAvatar);
    cl->addWidget(hdr);

    // ── Streak bar ──
    auto* streakBar = new QWidget(content);
    streakBar->setObjectName("streakBar");
    auto* stLay = new QHBoxLayout(streakBar);
    stLay->setContentsMargins(20,0,20,12);
    stLay->setSpacing(6);
    for (int i = 0; i < 7; i++) {
        auto* dot = new QLabel(streakBar);
        dot->setObjectName("streakDot");
        dot->setFixedSize(30, 30);
        dot->setAlignment(Qt::AlignCenter);
        streakDots.append(dot);
        stLay->addWidget(dot);
    }
    lblStreakCount = new QLabel("0 hari", streakBar);
    lblStreakCount->setObjectName("labelMuted");
    stLay->addWidget(lblStreakCount);
    stLay->addStretch();
    cl->addWidget(streakBar);

    // ── Quick stats ──
    auto* qsRow = new QWidget(content);
    auto* qsLay = new QHBoxLayout(qsRow);
    qsLay->setContentsMargins(20,0,20,16);
    qsLay->setSpacing(10);

    auto makeChip = [&](QLabel*& val, const QString& lbl, QWidget* parent) -> QWidget* {
        auto* chip = new QFrame(parent);
        chip->setObjectName("statChip");
        auto* chLay = new QVBoxLayout(chip);
        chLay->setContentsMargins(14,10,14,10);
        chLay->setSpacing(2);
        val = new QLabel("0", chip);
        val->setObjectName("statVal");
        val->setAlignment(Qt::AlignCenter);
        auto* l = new QLabel(lbl, chip);
        l->setObjectName("statLbl");
        l->setAlignment(Qt::AlignCenter);
        chLay->addWidget(val);
        chLay->addWidget(l);
        return chip;
    };
    qsLay->addWidget(makeChip(lblSessions, "Sesi",    qsRow));
    qsLay->addWidget(makeChip(lblFocus,    "Fokus",   qsRow));
    qsLay->addWidget(makeChip(lblTasks,    "Tugas",   qsRow));
    qsLay->addWidget(makeChip(lblDone,     "Selesai", qsRow));
    cl->addWidget(qsRow);

    // ── Pomodoro card ──
    auto* pomCard = new QFrame(content);
    pomCard->setObjectName("pomCard");
    auto* pomLay = new QVBoxLayout(pomCard);
    pomLay->setContentsMargins(18,18,18,18);
    pomLay->setSpacing(10);

    auto* pomHdr = new QHBoxLayout();
    auto* pomTitle = new QLabel("⏱ Timer Belajar", pomCard);
    pomTitle->setObjectName("pomTitle");
    lblPomBadge = new QLabel("● Siap", pomCard);
    lblPomBadge->setObjectName("pomBadge");
    pomHdr->addWidget(pomTitle);
    pomHdr->addStretch();
    pomHdr->addWidget(lblPomBadge);
    pomLay->addLayout(pomHdr);

    lblPomDisplay = new QLabel("00:00", pomCard);
    lblPomDisplay->setObjectName("pomDisplay");
    lblPomDisplay->setAlignment(Qt::AlignCenter);
    pomLay->addWidget(lblPomDisplay);

    lblPomSessions = new QLabel("Sesi: 0 / 4", pomCard);
    lblPomSessions->setObjectName("labelMuted");
    lblPomSessions->setAlignment(Qt::AlignCenter);
    pomLay->addWidget(lblPomSessions);

    auto* pomCtrl = new QHBoxLayout();
    pomCtrl->setSpacing(10);
    auto* btnStart = new QPushButton("▶ Mulai", pomCard);
    btnStart->setObjectName("btnPomStart");
    auto* btnStop  = new QPushButton("■ Stop",  pomCard);
    btnStop->setObjectName("btnPomStop");
    auto* btnSkip  = new QPushButton("⏭ Skip",  pomCard);
    btnSkip->setObjectName("btnPomSkip");
    connect(btnStart, &QPushButton::clicked, this, &HomeScreen::timerStartRequested);
    connect(btnStop,  &QPushButton::clicked, this, &HomeScreen::timerStopRequested);
    connect(btnSkip,  &QPushButton::clicked, this, &HomeScreen::timerSkipRequested);
    pomCtrl->addWidget(btnStart, 2);
    pomCtrl->addWidget(btnStop, 1);
    pomCtrl->addWidget(btnSkip, 1);
    pomLay->addLayout(pomCtrl);

    auto* pomWrap = new QWidget(content);
    auto* pwLay = new QVBoxLayout(pomWrap);
    pwLay->setContentsMargins(16,0,16,16);
    pwLay->addWidget(pomCard);
    cl->addWidget(pomWrap);

    // ── Task preview ──
    auto* secTitle = new QHBoxLayout();
    secTitle->setContentsMargins(20,0,20,10);
    auto* stLbl = new QLabel("Tugas Mendatang", content);
    stLbl->setObjectName("sectionTitle");
    auto* btnSeeAll = new QPushButton("Lihat Semua →", content);
    btnSeeAll->setObjectName("btnSeeAll");
    btnSeeAll->setFlat(true);
    connect(btnSeeAll, &QPushButton::clicked, this, [this](){ emit navigateTo("tasks"); });
    secTitle->addWidget(stLbl, 1);
    secTitle->addWidget(btnSeeAll);
    auto* secTitleW = new QWidget(content);
    secTitleW->setLayout(secTitle);
    cl->addWidget(secTitleW);

    taskPreviewContainer = new QWidget(content);
    taskPreviewLayout = new QVBoxLayout(taskPreviewContainer);
    taskPreviewLayout->setContentsMargins(16,0,16,0);
    taskPreviewLayout->setSpacing(10);
    cl->addWidget(taskPreviewContainer);

    // ── Quote ──
    auto* quoteCard = new QFrame(content);
    quoteCard->setObjectName("quoteCard");
    auto* qcLay = new QVBoxLayout(quoteCard);
    qcLay->setContentsMargins(18,16,18,16);
    qcLay->setSpacing(6);
    auto* qcLbl = new QLabel("💡 Quote Hari Ini", quoteCard);
    qcLbl->setObjectName("quoteLbl");
    lblQuoteText   = new QLabel("", quoteCard);
    lblQuoteText->setObjectName("quoteText");
    lblQuoteText->setWordWrap(true);
    lblQuoteAuthor = new QLabel("", quoteCard);
    lblQuoteAuthor->setObjectName("quoteAuthor");
    qcLay->addWidget(qcLbl);
    qcLay->addWidget(lblQuoteText);
    qcLay->addWidget(lblQuoteAuthor);

    auto* qWrap = new QWidget(content);
    auto* qwLay = new QVBoxLayout(qWrap);
    qwLay->setContentsMargins(16,16,16,0);
    qwLay->addWidget(quoteCard);
    cl->addWidget(qWrap);
    cl->addStretch();
}

void HomeScreen::refresh() {
    auto& app = AppState::instance();
    lblName->setText(app.currentUser.isEmpty() ? "Pengguna" : app.currentUser);
    lblAvatar->setText(app.currentAvatar);

    // Stats
    lblSessions->setText(QString::number(app.sessions));
    int h = app.totalStudySecs / 3600;
    int m = (app.totalStudySecs % 3600) / 60;
    lblFocus->setText(h > 0 ? QString("%1j").arg(h) : QString("%1m").arg(m));
    lblTasks->setText(QString::number(app.tasksTotal()));
    lblDone->setText(QString("%1%").arg(app.donePercent()));

    // Streak
    updateStreak();

    // Pomodoro
    updatePomadoroDisplay();

    // Tasks
    updateTaskPreview();

    // Quote
    if (!app.quotes.isEmpty()) {
        auto q = app.quotes[0];
        lblQuoteText->setText(QString("\"%1\"").arg(q.first));
        lblQuoteAuthor->setText(QString("— %1").arg(q.second));
    }
}

void HomeScreen::updatePomadoroDisplay() {
    auto& app = AppState::instance();
    int mins = app.timerRemaining / 60;
    int secs = app.timerRemaining % 60;
    lblPomDisplay->setText(QString("%1:%2")
        .arg(mins, 2, 10, QChar('0'))
        .arg(secs, 2, 10, QChar('0')));
    lblPomSessions->setText(QString("Sesi: %1 / 4").arg(app.sessions));

    auto& t = app.theme;
    if (app.timerRunning) {
        lblPomDisplay->setStyleSheet(QString("color:%1; font-family:'Courier New'; font-size:54px; font-weight:700;").arg(t.accent));
        lblPomBadge->setText("● Berjalan");
        lblPomBadge->setStyleSheet(QString("font-size:10px;padding:3px 10px;border-radius:10px;background:rgba(5,214,5,38);color:%1;border:1px solid rgba(5,214,5,89);font-weight:700;").arg(t.green));
    } else {
        lblPomDisplay->setStyleSheet("font-family:'Courier New'; font-size:54px; font-weight:700;");
        lblPomBadge->setText("● Siap");
        lblPomBadge->setStyleSheet(QString("font-size:10px;padding:3px 10px;border-radius:10px;background:%1;color:%2;border:1px solid %3;font-weight:700;").arg(
            "rgba(255,204,0,38)", t.accent, "rgba(255,204,0,89)"));
    }
}

void HomeScreen::updateTaskPreview() {
    // Clear
    QLayoutItem* child;
    while ((child = taskPreviewLayout->takeAt(0)) != nullptr) {
        if (child->widget()) child->widget()->deleteLater();
        delete child;
    }

    auto& app = AppState::instance();
    auto& t   = app.theme;

    // Sort by deadline
    auto tasks = app.tasks;
    std::sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b){
        if (a.isDone != b.isDone) return !a.isDone;
        if (a.deadlineTs && b.deadlineTs) return a.deadlineTs < b.deadlineTs;
        return a.id < b.id;
    });

    int shown = 0;
    for (auto& task : tasks) {
        if (task.isDone) continue;
        if (shown >= 3) break;
        shown++;

        auto* card = new QFrame(taskPreviewContainer);
        card->setObjectName("taskCardPreview");
        auto* cl = new QHBoxLayout(card);
        cl->setContentsMargins(18,12,14,12);
        cl->setSpacing(12);

        auto* infoW = new QWidget(card);
        auto* il = new QVBoxLayout(infoW);
        il->setContentsMargins(0,0,0,0); il->setSpacing(3);

        auto* lbName = new QLabel(task.title, card);
        lbName->setObjectName("taskPreviewName");
        auto* lbMeta = new QLabel(task.deadline.isEmpty() ? task.matkul : task.matkul + " · " + task.deadline, card);
        lbMeta->setObjectName("taskPreviewMeta");

        il->addWidget(lbName);
        il->addWidget(lbMeta);

        // Urgency bar
        QString urg = task.priority == 2 ? t.red : (task.priority == 1 ? "#ffa500" : t.green);
        card->setStyleSheet(QString(R"(
            QFrame#taskCardPreview {
                background:%1; border:1.5px solid %2;
                border-radius:14px;
                border-left: 4px solid %3;
            }
        )").arg(t.bgSecondary, t.border, urg));

        cl->addWidget(infoW, 1);
        taskPreviewLayout->addWidget(card);
    }

    if (shown == 0) {
        auto* empty = new QLabel("Belum ada tugas. Tambah dulu!", taskPreviewContainer);
        empty->setObjectName("labelMuted");
        empty->setAlignment(Qt::AlignCenter);
        empty->setStyleSheet("padding: 20px;");
        taskPreviewLayout->addWidget(empty);
    }
}

void HomeScreen::updateStreak() {
    auto& app = AppState::instance();
    auto& t   = app.theme;
    QStringList days = {"S","S","R","K","J","S","M"};
    for (int i = 0; i < 7 && i < streakDots.size(); i++) {
        bool active = (i < app.streakDays.size()) ? app.streakDays[i] : false;
        streakDots[i]->setText(days[i]);
        streakDots[i]->setStyleSheet(active
            ? QString("background:%1;color:%2;border-radius:15px;font-size:11px;font-weight:800;").arg(t.accent, t.bgDeep)
            : QString("background:%1;color:%2;border-radius:15px;font-size:11px;border:1px solid %3;").arg(t.bgSecondary, t.textMuted, t.border));
    }
    lblStreakCount->setText(QString("%1 hari 🔥").arg(app.streakCount));
}

void HomeScreen::applyStyle() {
    auto& t = AppState::instance().theme;
    setStyleSheet(QString(R"(
        HomeScreen { background-color: %1; }
        QWidget#statusBar { background-color: %1; border-bottom: 1px solid %2; }
        QLabel#statusTime { font-family:'Courier New'; font-size:13px; font-weight:700; color:%3; }
        QWidget#homeHeader { background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 %4, stop:1 %1); }
        QLabel#homeName    { font-size:22px; font-weight:900; color:%3; }
        QLabel#homeStatus  { font-size:12px; color:%5; }
        QLabel#avatarCircle { background:%6; border:2px solid %7; border-radius:23px; font-size:24px; }
        QFrame#statChip { background:%4; border:1px solid %2; border-radius:14px; min-width:80px; }
        QLabel#statVal { font-family:'Courier New'; font-size:20px; font-weight:700; color:%5; }
        QLabel#statLbl { font-size:10px; color:%8; }
        QFrame#pomCard { background:%4; border:1px solid %2; border-radius:20px; }
        QLabel#pomTitle { font-size:12px; font-weight:700; color:%5; }
        QLabel#pomDisplay { font-family:'Courier New'; font-size:54px; font-weight:700; color:%3; }
        QLabel#sectionTitle { font-size:13px; font-weight:800; color:%8; }
        QPushButton#btnSeeAll { color:%5; font-size:12px; background:transparent; border:none; }
        QFrame#quoteCard { background:%4; border:1px solid %2; border-radius:20px; }
        QLabel#quoteLbl  { font-size:11px; font-weight:700; color:%5; }
        QLabel#quoteText { font-size:13px; color:%3; font-style:italic; }
        QLabel#quoteAuthor { font-size:11px; color:%8; }
        QLabel#labelMuted { font-size:12px; color:%8; }
        QLabel#taskPreviewName { font-size:14px; font-weight:700; color:%3; }
        QLabel#taskPreviewMeta { font-size:11px; color:%8; }
    )").arg(t.bgPrimary, t.border, t.textPrimary, t.bgSecondary,
            t.accent, "rgba(255,204,0,38)", "rgba(255,204,0,89)",
            t.textMuted));
}
