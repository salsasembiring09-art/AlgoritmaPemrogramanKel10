#include "friendscreen.h"
#include <QScrollArea>
#include <QFrame>
#include <QPushButton>
#include <QTime>
#include <QListWidgetItem>
#include <cstdlib>
          
// ─────────────────────────────────────────────────
//  ChatDialog
// ─────────────────────────────────────────────────
ChatDialog::ChatDialog(const Friend& f, QWidget* p)
    : QDialog(p), fr(f)
{
    setWindowTitle(f.name);
    setMinimumSize(320, 480);

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0,0,0,0);
    lay->setSpacing(0);

    // Header
    auto* hdr  = new QWidget(this); hdr->setObjectName("chatHdr");
    auto* hdrL = new QHBoxLayout(hdr);
    hdrL->setContentsMargins(16,12,16,12); hdrL->setSpacing(12);

    auto* avLbl = new QLabel(f.avatar, hdr);
    avLbl->setStyleSheet("font-size:28px;");

    auto* vv = new QVBoxLayout(); vv->setSpacing(2);
    auto* nm = new QLabel(f.name, hdr); nm->setObjectName("chatName");
    auto* st = new QLabel(f.isOnline ? "● Online" : "○ Offline", hdr);
    st->setObjectName(f.isOnline ? "chatOnline" : "chatOffline");
    vv->addWidget(nm); vv->addWidget(st);

    hdrL->addWidget(avLbl); hdrL->addLayout(vv, 1);
    lay->addWidget(hdr);

    // Messages
    msgList = new QListWidget(this);
    msgList->setObjectName("chatMsgList");
    msgList->setSelectionMode(QAbstractItemView::NoSelection);
    lay->addWidget(msgList, 1);

    // Input row
    auto* inputW  = new QWidget(this);
    auto* inputLay = new QHBoxLayout(inputW);
    inputLay->setContentsMargins(12,8,12,12); inputLay->setSpacing(8);

    edtInput = new QLineEdit(inputW);
    edtInput->setPlaceholderText("Ketik pesan...");
    edtInput->setFixedHeight(44);

    auto* btnSend = new QPushButton("➤", inputW);
    btnSend->setObjectName("btnSend");
    btnSend->setFixedSize(44, 44);

    connect(btnSend, &QPushButton::clicked, this, [this](){
        sendMessage(edtInput->text().trimmed());
    });
    connect(edtInput, &QLineEdit::returnPressed, this, [this](){
        sendMessage(edtInput->text().trimmed());
    });

    inputLay->addWidget(edtInput, 1);
    inputLay->addWidget(btnSend);
    lay->addWidget(inputW);

    // Greeting (delayed)
    if (f.isOnline) {
        QTimer::singleShot(500, this, [this](){
            QStringList greets = {
                "Halo! Lagi belajar apa nih?",
                "Hai! Tumben chat, ada apa?",
                "Yoo! Gimana kabar hari ini?"
            };
            addBubble(greets[rand() % greets.size()], false, fr.name);
        });
    } else {
        addBubble(f.name + " sedang offline.", false, "Sistem");
    }

    applyStyle();
}

void ChatDialog::sendMessage(const QString& msg) {
    if (msg.isEmpty()) return;
    edtInput->clear();
    addBubble(msg, true, "Saya");

    auto& hist = AppState::instance().chatHistories[fr.name];
    hist.append({QString("Saya"), msg, true});

    if (fr.isOnline) {
        QTimer::singleShot(800 + rand() % 1200, this, [this, msg](){
            QString reply = getAutoReply(msg);
            addBubble(reply, false, fr.name);
            AppState::instance().chatHistories[fr.name].append({fr.name, reply, false});
        });
    }
}

void ChatDialog::addBubble(const QString& text, bool isMe, const QString& /*sender*/) {
    auto& t = AppState::instance().theme;

    auto* item = new QListWidgetItem(msgList);
    auto* w    = new QWidget(msgList);
    auto* lay  = new QHBoxLayout(w);
    lay->setContentsMargins(8, 4, 8, 4);

    auto* bubble = new QLabel(text, w);
    bubble->setWordWrap(true);
    bubble->setMaximumWidth(220);
    bubble->setStyleSheet(isMe
        ? QString("background:%1; color:%2; border-radius:14px; padding:10px 14px; font-size:13px;")
              .arg(t.accent, t.bgDeep)
        : QString("background:%1; color:%2; border:1px solid %3; border-radius:14px; padding:10px 14px; font-size:13px;")
              .arg(t.bgCard, t.textPrimary, t.border));

    if (isMe) { lay->addStretch(); lay->addWidget(bubble); }
    else      { lay->addWidget(bubble); lay->addStretch(); }

    item->setSizeHint(QSize(msgList->width(), bubble->sizeHint().height() + 24));
    msgList->setItemWidget(item, w);
    msgList->scrollToBottom();
}

QString ChatDialog::getAutoReply(const QString& msg) {
    QString lc = msg.toLower();
    if (lc.contains("halo") || lc.contains("hai") || lc.contains("hi"))
        return QStringList{"Halo halo! Lagi ngapain?","Hai! Kabar gimana?","Yoo!"}[rand()%3];
    if (lc.contains("tugas") || lc.contains("deadline") || lc.contains("ujian"))
        return QStringList{"Aduh deadline lagi? Sabar!","Skripsi emang monster, semangat!","Mulai dari yang paling gampang."}[rand()%3];
    if (lc.contains("belajar") || lc.contains("fokus") || lc.contains("malas"))
        return QStringList{"Mood belajar fluktuatif, istirahat dulu.","Mulai 5 menit aja, nanti ngalir.","Pomodoro aja!"}[rand()%3];
    QStringList def = {
        "Ooh gitu! Cerita dong.", "Wkwk ada-ada aja.",
        "Hmm menarik, terus gimana?", "Iya nih, setuju banget!",
        "Tetep semangat ya!"
    };
    return def[rand() % def.size()];
}

void ChatDialog::applyStyle() {
    auto& t = AppState::instance().theme;
    setStyleSheet(QString(R"(
        QDialog            { background:%1; }
        QWidget#chatHdr    { background:%2; border-bottom:1px solid %3; }
        QLabel#chatName    { font-size:16px; font-weight:800; color:%4; }
        QLabel#chatOnline  { font-size:11px; color:%5; }
        QLabel#chatOffline { font-size:11px; color:%6; }
        QListWidget#chatMsgList { background:%1; border:none; }
        QLineEdit {
            background:%2; border:1px solid %3;
            border-radius:14px; padding:8px 14px; color:%4;
        }
        QPushButton#btnSend {
            background:%7; color:%8;
            border-radius:22px; font-size:16px;
            border:none; font-weight:bold;
        }
    )").arg(t.bgPrimary, t.bgSecondary, t.border, t.textPrimary,
            t.green, t.textMuted, t.accent, t.bgDeep));
}

// ─────────────────────────────────────────────────
//  FriendScreen
// ─────────────────────────────────────────────────
FriendScreen::FriendScreen(QWidget* p) : QWidget(p) {
    buildUi();
    applyStyle();
}

void FriendScreen::buildUi() {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0,0,0,0); root->setSpacing(0);

    // Status bar
    auto* sb  = new QWidget(this); sb->setObjectName("statusBar"); sb->setFixedHeight(44);
    auto* sbL = new QHBoxLayout(sb); sbL->setContentsMargins(20,0,20,0);
    auto* sbTime = new QLabel(QTime::currentTime().toString("HH:mm"), sb);
    sbTime->setObjectName("statusTime");
    auto* sbMid = new QLabel("Komunitas", sb); sbMid->setObjectName("labelMuted");
    sbL->addWidget(sbTime); sbL->addStretch();
    sbL->addWidget(sbMid); sbL->addStretch();
    sbL->addWidget(new QLabel("📶 🔋", sb));
    root->addWidget(sb);

    // Page header
    auto* ph  = new QWidget(this); ph->setObjectName("pageHeader");
    auto* phL = new QHBoxLayout(ph); phL->setContentsMargins(20,12,20,12);
    auto* phTitle = new QLabel("👥 Teman Belajar", ph); phTitle->setObjectName("pageTitle");
    phL->addWidget(phTitle, 1);
    root->addWidget(ph);

    // Search
    auto* searchW  = new QWidget(this);
    auto* searchLay = new QHBoxLayout(searchW);
    searchLay->setContentsMargins(16,8,16,8);
    edtSearch = new QLineEdit(searchW);
    edtSearch->setPlaceholderText("Cari teman...");
    edtSearch->setFixedHeight(44);
    connect(edtSearch, &QLineEdit::textChanged, this, [this](const QString& s){
        renderFriends(s);
    });
    searchLay->addWidget(edtSearch);
    root->addWidget(searchW);

    // Scrollable list
    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto* inner = new QWidget();
    auto* cl    = new QVBoxLayout(inner);
    cl->setContentsMargins(16,8,16,24); cl->setSpacing(0);
    scroll->setWidget(inner);
    root->addWidget(scroll, 1);

    // Online section
    auto* onlineTitle = new QLabel("🟢 Online Sekarang", inner);
    onlineTitle->setObjectName("sectionLabel");
    cl->addWidget(onlineTitle);

    onlineContainer = new QWidget(inner);
    onlineLay = new QVBoxLayout(onlineContainer);
    onlineLay->setContentsMargins(0,8,0,16); onlineLay->setSpacing(8);
    cl->addWidget(onlineContainer);

    // Offline section
    auto* offlineTitle = new QLabel("⚫ Offline", inner);
    offlineTitle->setObjectName("sectionLabel");
    cl->addWidget(offlineTitle);

    offlineContainer = new QWidget(inner);
    offlineLay = new QVBoxLayout(offlineContainer);
    offlineLay->setContentsMargins(0,8,0,0); offlineLay->setSpacing(8);
    cl->addWidget(offlineContainer);

    cl->addStretch();

    renderFriends();
}

void FriendScreen::renderFriends(const QString& search) {
    // Clear both containers
    auto clearLayout = [](QVBoxLayout* lay) {
        QLayoutItem* ch;
        while ((ch = lay->takeAt(0)) != nullptr) {
            if (ch->widget()) ch->widget()->deleteLater();
            delete ch;
        }
    };
    clearLayout(onlineLay);
    clearLayout(offlineLay);

    auto& app = AppState::instance();
    auto& t   = app.theme;
    QString lc = search.toLower();

    // Build one friend card
    auto makeCard = [&](const Friend& f, QVBoxLayout* targetLay) {
        auto* card  = new QFrame();
        card->setObjectName("friendCard");
        card->setStyleSheet(QString(
            "QFrame#friendCard { background:%1; border:1px solid %2; border-radius:14px; }")
            .arg(t.bgSecondary, t.border));
        card->setCursor(Qt::PointingHandCursor);

        auto* cl2 = new QHBoxLayout(card);
        cl2->setContentsMargins(14,12,14,12); cl2->setSpacing(12);

        // Avatar label
        auto* av = new QLabel(f.avatar, card);
        av->setFixedSize(42, 42);
        av->setAlignment(Qt::AlignCenter);
        av->setStyleSheet(QString(
            "background:%1; border:2px solid %2; border-radius:21px; font-size:22px;")
            .arg(t.bgCard, t.border));

        // Info
        auto* info  = new QWidget(card);
        auto* infoL = new QVBoxLayout(info);
        infoL->setContentsMargins(0,0,0,0); infoL->setSpacing(2);
        auto* lbName = new QLabel(f.name,   info); lbName->setObjectName("friendName");
        auto* lbSt   = new QLabel(f.status, info); lbSt->setObjectName("friendStatus");
        infoL->addWidget(lbName); infoL->addWidget(lbSt);

        // Study time
        int  m  = f.studySeconds / 60;
        QString stStr = m > 59 ? QString("%1j").arg(m/60) : QString("%1m").arg(m);
        auto* meta  = new QWidget(card);
        auto* metaL = new QVBoxLayout(meta);
        metaL->setContentsMargins(0,0,0,0); metaL->setSpacing(2);
        auto* lbTime = new QLabel(stStr, meta);
        lbTime->setObjectName("friendTime"); lbTime->setAlignment(Qt::AlignRight);
        auto* lbFok = new QLabel("fokus", meta);
        lbFok->setObjectName("labelMutedSmall"); lbFok->setAlignment(Qt::AlignRight);
        metaL->addWidget(lbTime); metaL->addWidget(lbFok);

        // Online indicator on avatar
        if (f.isOnline) {
            lbName->setStyleSheet(QString("color:%1;").arg(t.textPrimary));
        }

        cl2->addWidget(av);
        cl2->addWidget(info, 1);
        cl2->addWidget(meta);

        // Clickable via transparent button overlay
        auto* clickOverlay = new QPushButton(card);
        clickOverlay->setFlat(true);
        clickOverlay->setStyleSheet("background:transparent; border:none;");
        // Resize overlay to fill card on show
        QString nm = f.name;
        connect(clickOverlay, &QPushButton::clicked, this, [this, nm](){
            openChat(nm);
        });
        // Position overlay after card layout
        QTimer::singleShot(0, clickOverlay, [clickOverlay, card](){
            clickOverlay->setGeometry(card->rect());
        });

        targetLay->addWidget(card);
    };

    bool onlineShown = false, offlineShown = false;
    for (const auto& f : app.friends) {
        if (!lc.isEmpty() &&
            !f.name.toLower().contains(lc) &&
            !f.status.toLower().contains(lc)) continue;

        if (f.isOnline) { makeCard(f, onlineLay);  onlineShown  = true; }
        else            { makeCard(f, offlineLay); offlineShown = true; }
    }

    if (!onlineShown) {
        auto* lbl = new QLabel("Tidak ada teman online", onlineContainer);
        lbl->setObjectName("labelMuted");
        lbl->setStyleSheet("padding:12px 4px;");
        onlineLay->addWidget(lbl);
    }
    if (!offlineShown) {
        auto* lbl = new QLabel("Semua teman online!", offlineContainer);
        lbl->setObjectName("labelMuted");
        lbl->setStyleSheet("padding:12px 4px;");
        offlineLay->addWidget(lbl);
    }
}

void FriendScreen::openChat(const QString& name) {
    for (const auto& f : AppState::instance().friends) {
        if (f.name == name) {
            ChatDialog dlg(f, this);
            dlg.exec();
            return;
        }
    }
}

void FriendScreen::refresh() {
    renderFriends(edtSearch ? edtSearch->text() : QString());
}

void FriendScreen::applyStyle() {
    auto& t = AppState::instance().theme;
    setStyleSheet(QString(R"(
        FriendScreen      { background:%1; }
        QWidget#statusBar { background:%1; border-bottom:1px solid %2; }
        QLabel#statusTime { font-family:'Courier New'; font-size:13px; font-weight:700; color:%3; }
        QWidget#pageHeader { background:%4; border-bottom:1px solid %2; }
        QLabel#pageTitle   { font-size:20px; font-weight:900; color:%3; }
        QLabel#sectionLabel { font-size:12px; font-weight:800; color:%7; padding:8px 4px; }
        QLabel#friendName   { font-size:14px; font-weight:700; color:%3; }
        QLabel#friendStatus { font-size:12px; color:%7; }
        QLabel#friendTime   { font-family:'Courier New'; font-size:14px; font-weight:700; color:%5; }
        QLabel#labelMutedSmall { font-size:10px; color:%7; }
        QLabel#labelMuted   { font-size:12px; color:%7; }
    )").arg(t.bgPrimary, t.border, t.textPrimary, t.bgSecondary,
            t.accent, t.green, t.textMuted));
}
