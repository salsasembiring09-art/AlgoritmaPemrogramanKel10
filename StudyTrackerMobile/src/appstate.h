#pragma once
#include <QString>
#include <QList>
#include <QDateTime>
#include <QObject>
#include "styles.h"
 
// ── Struktur data tugas ────────────────────────
struct Task {
    int     id;
    QString title;
    QString matkul;
    QString deadline;       // format "dd/MM/yyyy HH:mm"
    qint64  deadlineTs;     // unix ms, 0 jika tidak ada
    int     priority;       // 0=low, 1=med, 2=high
    bool    isDone;
    QString color;          // warna urgency bar
};

// ── Struktur data teman ────────────────────────
struct Friend {
    QString name;
    QString avatar;
    QString status;
    bool    isOnline;
    int     studySeconds;
};

// ── Struktur pesan chat ────────────────────────
struct ChatMessage {
    QString sender;
    QString message;
    bool    isMe;
};

// ── State global aplikasi ──────────────────────
class AppState : public QObject {
    Q_OBJECT
public:
    static AppState& instance() {
        static AppState s;
        return s;
    }

    // Auth
    QString currentUser;
    QString currentAvatar  = "😀";
    bool    isLoggedIn     = false;

    // Pomodoro / timer state
    bool    timerRunning   = false;
    bool    isBreak        = false;
    int     timerRemaining = 0;      // detik
    int     timerTotal     = 0;      // detik
    int     sessions       = 0;
    int     focusMinutes   = 25;
    int     breakMinutes   = 5;
    int     totalStudySecs = 0;      // akumulasi sesi ini (detik)

    // Tugas
    QList<Task> tasks;
    int nextTaskId = 1;

    // Streak
    QList<bool> streakDays = {false,false,false,false,false,false,false};
    int streakCount = 0;

    // Settings
    bool notifTugas  = true;
    bool notifBelajar= true;
    bool notifTeman  = true;
    int  soundVolume = 80;  // 0-100

    // Tema
    QString themeName = "default";
    ST::Theme theme;

    // Data teman (statis)
    QList<Friend> friends;
    QMap<QString, QList<ChatMessage>> chatHistories;

    // Quotes
    QList<QPair<QString,QString>> quotes;

    void applyTheme(const QString& name) {
        themeName = name;
        if      (name == "black")   theme = ST::blackTheme();
        else if (name == "white")   theme = ST::whiteTheme();
        else if (name == "pink")    theme = ST::pinkTheme();
        else if (name == "laut")    theme = ST::lautTheme();
        else if (name == "adem")    theme = ST::ademTheme();
        else if (name == "vintage") theme = ST::vintageTheme();
        else                        theme = ST::defaultTheme();
        emit themeChanged();
    }

    void initFriendsData() {
        friends = {
            {"Budi Santoso",  "😎", "Belajar Algoritma 💻", true,  3600},
            {"Anisa Putri",   "🥰", "Baca buku 📚",         true,  7200},
            {"Rizky Maulana", "🤓", "Review materi UAS",    false, 0   },
            {"Dewi Kartika",  "😄", "Istirahat dulu ☕",     false, 0   },
            {"Farhan Ahmad",  "🙂", "Ngoding project 💡",   true,  2700},
            {"Maya Sari",     "🥳", "Selesai target hari ini!", false, 10800},
        };
    }

    void initQuotes() {
        quotes = {
            {"Belajar tanpa berpikir itu sia-sia, berpikir tanpa belajar itu berbahaya.", "Konfusius"},
            {"Pendidikan adalah senjata paling ampuh untuk mengubah dunia.", "Nelson Mandela"},
            {"Hari ini harus lebih baik dari kemarin.", "Pepatah"},
            {"Kamu tidak perlu sempurna untuk memulai. Mulailah untuk menjadi sempurna.", "Zig Ziglar"},
            {"Sukses adalah hasil dari kerja keras, bukan keberuntungan.", "Thomas Edison"},
            {"Jangan pernah berhenti belajar, karena hidup tidak pernah berhenti mengajarkan.", "Unknown"},
            {"Setiap detik yang berlalu adalah kesempatan untuk berubah.", "Unknown"},
        };
    }

    int tasksTotal()   const { return tasks.size(); }
    int tasksDone()    const { int n=0; for(auto&t:tasks) if(t.isDone) n++; return n; }
    int tasksPending() const { return tasksTotal() - tasksDone(); }
    int donePercent()  const {
        if(tasks.isEmpty()) return 0;
        return (int)((double)tasksDone()/tasks.size()*100);
    }
    int totalFocusHours()  const { return totalStudySecs / 3600; }
    int totalFocusMins()   const { return (totalStudySecs % 3600) / 60; }

signals:
    void themeChanged();
    void tasksChanged();
    void timerTick();
    void statsChanged();

private:
    AppState() {
        theme = ST::defaultTheme();
        initFriendsData();
        initQuotes();
    }
};
