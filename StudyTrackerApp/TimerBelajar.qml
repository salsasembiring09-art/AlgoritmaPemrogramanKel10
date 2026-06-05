import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia

Rectangle {
    id: timerPage
    anchors.fill: parent
    color: window.bgPrimary
  
    SoundEffect { id: soundBeep; source: "qrc:/sounds/notif.wav" }

    function updateClosestTask() {
        if (typeof globalTaskModel === "undefined" || globalTaskModel.count === 0)
            return lang.belumAdaTugas

        var closest = null
        for (var i = 0; i < globalTaskModel.count; i++) {
            var task = globalTaskModel.get(i)
            if (!task.isDone) {
                if (closest === null || task.deadlineTimestamp < closest.deadlineTimestamp) {
                    closest = task
                }
            }
        }

        if (closest === null) return lang.semuaTugasSelesai
        return closest.title + "\n📅 " + closest.deadline
    }

    function handleStart() {
        if (!window.globalTimerRunning && window.globalCurrentTimerValue === 0) {
            let h = parseInt(hIn.text) || 0
            let m = parseInt(mIn.text) || 0
            let s = parseInt(sIn.text) || 0
            let total = (h * 3600) + (m * 60) + s
            window.globalCurrentTimerValue = total
        }
        if (window.globalCurrentTimerValue > 0) window.globalTimerRunning = true
    }

    function handleStop() {
        window.globalTimerRunning = false
    }

    function handleSkip() {
        window.globalTimerRunning = false
        window.globalCurrentTimerValue = 0
        window.globalSessionsCompleted++
    }

    // --- MAIN LAYOUT ---
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 30
        spacing: 10

        // 1. HEADER
        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            spacing: 16

            // Tombol Exit ke Main Menu
            Rectangle {
                id: backBtn
                width: backBtnRow.implicitWidth + 24
                height: 36; radius: 18
                color: backArea.pressed
                       ? Qt.rgba(window.accentColor.r, window.accentColor.g, window.accentColor.b, 0.22)
                       : backArea.containsMouse
                         ? Qt.rgba(window.accentColor.r, window.accentColor.g, window.accentColor.b, 0.10)
                         : window.bgSecondary
                border.color: backArea.containsMouse ? window.accentColor : window.borderColor
                border.width: backArea.containsMouse ? 1.5 : 1

                Behavior on color       { ColorAnimation { duration: 160 } }
                Behavior on border.color { ColorAnimation { duration: 160 } }

                Row {
                    id: backBtnRow
                    anchors.centerIn: parent
                    spacing: 7

                    Text {
                        text: "←"
                        color: backArea.containsMouse ? window.accentColor : window.textMuted
                        font.pixelSize: 15; font.bold: true
                        anchors.verticalCenter: parent.verticalCenter
                        Behavior on color { ColorAnimation { duration: 160 } }
                    }
                    Text {
                        text: lang.kembaliKeMenu
                        color: backArea.containsMouse ? window.accentColor : window.textMuted
                        font.pixelSize: 13; font.bold: true
                        anchors.verticalCenter: parent.verticalCenter
                        Behavior on color { ColorAnimation { duration: 160 } }
                    }
                }

                MouseArea {
                    id: backArea
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true
                    onClicked: pageLoader.sourceComponent = mainComponent
                }
            }

            // Divider tipis
            Rectangle {
                width: 1; height: 24
                color: window.borderColor
                opacity: 0.6
            }

            Text {
                text: lang.timer
                color: window.textPrimary
                font.pixelSize: 28; font.bold: true
            }
            Item { Layout.fillWidth: true }
        }

        // 2. CONTENT AREA
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 40

            // ═══════════════════════════════════════════
            // SISI KIRI: TIMER & TUGAS — REDESIGNED
            // ═══════════════════════════════════════════
            ColumnLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
                spacing: 24

                // ── Lingkaran Timer Cantik ──────────────────────────────────
                Item {
                    width: 320; height: 320
                    Layout.preferredWidth: 320; Layout.preferredHeight: 320
                    Layout.alignment: Qt.AlignHCenter

                    // Lapisan 1 — Background glow gelap
                    Rectangle {
                        anchors.centerIn: parent
                        width: 320; height: 320
                        radius: 160
                        color: window.bgCard
                        border.color: window.borderColor
                        border.width: 1
                    }

                    // Lapisan 2 — Ring dekoratif luar (tipis, warna border)
                    Rectangle {
                        anchors.centerIn: parent
                        width: 310; height: 310
                        radius: 155
                        color: "transparent"
                        border.color: Qt.rgba(
                            window.accentColor.r,
                            window.accentColor.g,
                            window.accentColor.b,
                            0.08
                        )
                        border.width: 18
                    }

                    // Lapisan 3 — Ring aksen utama (berdenyut saat jalan)
                    Rectangle {
                        id: mainRing
                        anchors.centerIn: parent
                        width: 280; height: 280
                        radius: 140
                        color: "transparent"
                        border.color: window.accentColor
                        border.width: 4
                        opacity: window.globalTimerRunning ? 1.0 : 0.45

                        Behavior on opacity { NumberAnimation { duration: 400 } }

                        SequentialAnimation on scale {
                            running: window.globalTimerRunning
                            loops: Animation.Infinite
                            NumberAnimation { from: 1.0; to: 1.025; duration: 1100; easing.type: Easing.InOutSine }
                            NumberAnimation { from: 1.025; to: 1.0;  duration: 1100; easing.type: Easing.InOutSine }
                        }
                    }

                    // Lapisan 4 — Ring dalam (warna bgSecondary, separasi visual)
                    Rectangle {
                        anchors.centerIn: parent
                        width: 262; height: 262
                        radius: 131
                        color: window.bgSecondary
                        border.color: window.borderColor
                        border.width: 1
                    }

                    // Lapisan 5 — Dot-dot orbit mengelilingi (dekoratif)
                    Repeater {
                        model: 8
                        delegate: Rectangle {
                            property real angle: (index / 8) * Math.PI * 2
                            width:  index % 2 === 0 ? 7 : 4
                            height: index % 2 === 0 ? 7 : 4
                            radius: width / 2
                            color:  index % 2 === 0
                                    ? window.accentColor
                                    : Qt.rgba(window.accentColor.r, window.accentColor.g, window.accentColor.b, 0.35)
                            opacity: window.globalTimerRunning ? 1.0 : 0.25
                            Behavior on opacity { NumberAnimation { duration: 400 } }

                            x: 160 + Math.cos(angle) * 147 - width / 2
                            y: 160 + Math.sin(angle) * 147 - height / 2

                            SequentialAnimation on opacity {
                                running: window.globalTimerRunning
                                loops: Animation.Infinite
                                PauseAnimation { duration: index * 180 }
                                NumberAnimation { from: 0.2; to: 1.0; duration: 600 }
                                NumberAnimation { from: 1.0; to: 0.2; duration: 600 }
                            }
                        }
                    }

                    // ── Konten tengah lingkaran ──
                    Column {
                        anchors.centerIn: parent
                        spacing: 6

                        // Status label kecil atas
                        Rectangle {
                            anchors.horizontalCenter: parent.horizontalCenter
                            width: statusLabel.implicitWidth + 20
                            height: 20; radius: 10
                            color: window.globalTimerRunning
                                   ? Qt.rgba(window.accentColor.r, window.accentColor.g, window.accentColor.b, 0.18)
                                   : Qt.rgba(window.borderColor.r, window.borderColor.g, window.borderColor.b, 0.5)
                            border.color: window.globalTimerRunning ? window.accentColor : window.borderColor
                            border.width: 1

                            Text {
                                id: statusLabel
                                anchors.centerIn: parent
                                text: window.globalTimerRunning ? "● " + lang.fokusAktif : "○ " + lang.mulaiSesi
                                color: window.globalTimerRunning ? window.accentColor : window.textMuted
                                font.pixelSize: 9; font.bold: true; font.letterSpacing: 0.8
                            }
                        }

                        // Waktu besar
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: window.formatTime(window.globalCurrentTimerValue)
                            color: window.textPrimary
                            font.pixelSize: 54; font.bold: true
                            font.letterSpacing: 2

                            // Efek fade jika tidak jalan
                            opacity: window.globalTimerRunning ? 1.0 : 0.75
                            Behavior on opacity { NumberAnimation { duration: 400 } }
                        }

                        // Sub-label "sesi ke-N"
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: lang.targetSesiHarian + ": " + window.globalSessionsCompleted + " / " + window.globalTargetSessions
                            color: window.textMuted
                            font.pixelSize: 11
                        }
                    }
                }

                // ── Input Waktu — Stylish Pill Inputs ─────────────────────
                Row {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 8

                    // Helper komponen time-input
                    Repeater {
                        model: [
                            { ph: lang.jam[0].toUpperCase(),   ref: "h" },
                            { ph: lang.menit[0].toUpperCase(), ref: "m" },
                            { ph: lang.detik[0].toUpperCase(), ref: "s" },
                        ]
                        delegate: Rectangle {
                            width: 58; height: 52; radius: 12
                            color: window.bgSecondary
                            border.color: inputField.activeFocus ? window.accentColor : window.borderColor
                            border.width: inputField.activeFocus ? 2 : 1

                            Behavior on border.color { ColorAnimation { duration: 180 } }
                            Behavior on border.width  { NumberAnimation { duration: 180 } }

                            Column {
                                anchors.centerIn: parent
                                spacing: 1

                                TextField {
                                    id: inputField
                                    // Bind ke field yang sesuai via objectName trick
                                    objectName: modelData.ref + "Field"
                                    width: 54
                                    placeholderText: modelData.ph
                                    horizontalAlignment: TextInput.AlignHCenter
                                    color: window.textPrimary
                                    placeholderTextColor: window.textMuted
                                    font.pixelSize: 18; font.bold: true
                                    background: Rectangle { color: "transparent" }
                                    padding: 0; topPadding: 6
                                    validator: IntValidator { bottom: 0; top: modelData.ref === "h" ? 23 : 59 }

                                    // Assign ke id yang digunakan handleStart
                                    Component.onCompleted: {
                                        if (modelData.ref === "h") hIn = inputField
                                        else if (modelData.ref === "m") mIn = inputField
                                        else sIn = inputField
                                    }
                                }
                                Text {
                                    text: modelData.ph
                                    color: window.textMuted
                                    font.pixelSize: 9; font.letterSpacing: 0.5
                                    anchors.horizontalCenter: parent.horizontalCenter
                                }
                            }
                        }
                    }
                }

                // ── Tombol Kontrol — Elegan ────────────────────────────────
                Row {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 12

                    // Tombol MULAI / LANJUT
                    Rectangle {
                        width: 130; height: 44; radius: 22
                        color: startArea.pressed
                               ? Qt.darker(window.accentColor, 1.15)
                               : window.accentColor
                        Behavior on color { ColorAnimation { duration: 120 } }

                        // Glow effect saat timer jalan
                        layer.enabled: window.globalTimerRunning
                        layer.effect: null   // placeholder; pakai shadow manual

                        // Shadow/glow border luar
                        Rectangle {
                            anchors.centerIn: parent
                            width: parent.width + 4; height: parent.height + 4
                            radius: parent.radius + 2
                            color: "transparent"
                            border.color: Qt.rgba(window.accentColor.r, window.accentColor.g, window.accentColor.b, 0.45)
                            border.width: 2
                            visible: window.globalTimerRunning
                        }

                        Row {
                            anchors.centerIn: parent; spacing: 6
                            Text { text: "▶"; color: window.bgDeep; font.pixelSize: 12; font.bold: true; anchors.verticalCenter: parent.verticalCenter }
                            Text { text: lang.mulaiSesi; color: window.bgDeep; font.pixelSize: 13; font.bold: true; anchors.verticalCenter: parent.verticalCenter }
                        }
                        MouseArea { id: startArea; anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: handleStart() }
                    }

                    // Tombol STOP
                    Rectangle {
                        width: 90; height: 44; radius: 22
                        color: stopArea.pressed ? window.borderColor : window.bgSecondary
                        border.color: window.borderColor; border.width: 1.5
                        Behavior on color { ColorAnimation { duration: 120 } }

                        Row {
                            anchors.centerIn: parent; spacing: 6
                            Text { text: "■"; color: window.textMuted; font.pixelSize: 10; anchors.verticalCenter: parent.verticalCenter }
                            Text { text: lang.stopTimer; color: window.textPrimary; font.pixelSize: 13; font.bold: true; anchors.verticalCenter: parent.verticalCenter }
                        }
                        MouseArea { id: stopArea; anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: handleStop() }
                    }

                    // Tombol SKIP
                    Rectangle {
                        width: 90; height: 44; radius: 22
                        color: skipArea.pressed ? window.borderColor : window.bgSecondary
                        border.color: window.borderColor; border.width: 1.5
                        Behavior on color { ColorAnimation { duration: 120 } }

                        Row {
                            anchors.centerIn: parent; spacing: 6
                            Text { text: "⏭"; color: window.textMuted; font.pixelSize: 12; anchors.verticalCenter: parent.verticalCenter }
                            Text { text: lang.skipTimer; color: window.textPrimary; font.pixelSize: 13; font.bold: true; anchors.verticalCenter: parent.verticalCenter }
                        }
                        MouseArea { id: skipArea; anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: handleSkip() }
                    }
                }

                // ── Kartu Tugas Terdekat — Cantik ─────────────────────────
                Rectangle {
                    Layout.fillWidth: true
                    height: 70; radius: 16
                    color: window.bgSecondary
                    border.color: taskCardArea.containsMouse ? window.accentColor : window.borderColor
                    border.width: taskCardArea.containsMouse ? 1.5 : 1

                    Behavior on border.color { ColorAnimation { duration: 200 } }

                    // Stripe aksen kiri
                    Rectangle {
                        width: 4; height: parent.height - 20; radius: 2
                        anchors.left: parent.left; anchors.leftMargin: 0
                        anchors.verticalCenter: parent.verticalCenter
                        color: window.accentColor
                        opacity: 0.85
                    }

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 16; anchors.rightMargin: 14
                        anchors.topMargin: 10; anchors.bottomMargin: 10
                        spacing: 12

                        Column {
                            spacing: 3; Layout.fillWidth: true

                            Text {
                                text: lang.tugasTerdekat
                                color: window.accentColor
                                font.pixelSize: 10; font.bold: true; font.letterSpacing: 0.8
                            }
                            Text {
                                id: taskBtn
                                width: parent.width
                                text: (globalTaskModel.count >= 0) ? updateClosestTask() : ""
                                color: window.textPrimary
                                font.pixelSize: 13; font.bold: true
                                elide: Text.ElideRight
                                maximumLineCount: 1
                            }
                        }

                        // Panah / ikon klik
                        Rectangle {
                            width: 30; height: 30; radius: 15
                            color: Qt.rgba(window.accentColor.r, window.accentColor.g, window.accentColor.b, 0.12)
                            border.color: Qt.rgba(window.accentColor.r, window.accentColor.g, window.accentColor.b, 0.3)
                            border.width: 1
                            Text { anchors.centerIn: parent; text: "→"; color: window.accentColor; font.pixelSize: 14; font.bold: true }
                        }
                    }

                    MouseArea {
                        id: taskCardArea
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onClicked: pageLoader.source = "InputTugas.qml"
                    }
                }
            }

            // SISI KANAN: STATISTIK (tidak diubah)
            ColumnLayout {
                Layout.preferredWidth: 320
                spacing: 15

                // ── Kartu Pengingat Istirahat ──────────────────────────────
                Rectangle {
                    Layout.fillWidth: true; Layout.preferredHeight: 90; radius: 12
                    color: window.bgSecondary; border.color: window.accentColor; border.width: 1

                    SequentialAnimation on border.width {
                        running: window.globalTimerRunning
                        loops: Animation.Infinite
                        NumberAnimation { from: 1; to: 2.5; duration: 900 }
                        NumberAnimation { from: 2.5; to: 1; duration: 900 }
                    }

                    RowLayout {
                        anchors.fill: parent; anchors.margins: 14; spacing: 10

                        Text {
                            text: "🍅"
                            font.pixelSize: 30
                            RotationAnimation on rotation {
                                running: window.globalTimerRunning
                                loops: Animation.Infinite
                                from: -8; to: 8; duration: 1200
                                easing.type: Easing.InOutSine
                            }
                            onVisibleChanged: if (!window.globalTimerRunning) rotation = 0
                        }

                        Column {
                            spacing: 4; Layout.fillWidth: true
                            Text { text: lang.waktuIstirahat;  color: window.textMuted; font.pixelSize: 11 }
                            Text { text: window.pomodoroIstirahat + " " + lang.menit; color: window.accentColor; font.pixelSize: 24; font.bold: true }
                            Text { text: window.pomodoroFokus + " " + lang.menit + " · " + window.pomodoroIstirahat + " " + lang.menit; color: Qt.lighter(window.borderColor, 1.5); font.pixelSize: 10 }
                        }
                    }

                    Rectangle {
                        anchors.top: parent.top; anchors.right: parent.right
                        anchors.margins: 6
                        width: settingsBadge.implicitWidth + 10; height: 16; radius: 8
                        color: Qt.rgba(255/255,204/255,0/255,0.15)
                        border.color: window.accentColor; border.width: 1
                        Text {
                            id: settingsBadge
                            anchors.centerIn: parent
                            text: lang.dariSettings
                            color: window.accentColor; font.pixelSize: 8; font.bold: true
                        }
                        MouseArea {
                            anchors.fill: parent; cursorShape: Qt.PointingHandCursor
                            onClicked: pageLoader.sourceComponent = settingsComponent
                        }
                    }
                }

                // Kotak-kotak info
                Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 90; radius: 12; color: window.textPrimary
                    Column { anchors.centerIn: parent;
                        Text { text: lang.targetSesiHarian; font.pixelSize: 12; color: "#666"; anchors.horizontalCenter: parent.horizontalCenter }
                        Text { text: window.globalSessionsCompleted + " / " + window.globalTargetSessions; font.pixelSize: 28; font.bold: true; color: "black" }
                    }
                }

                TextField {
                    id: targetInput
                    text: window.globalTargetSessions.toString()
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    color: window.textPrimary
                    validator: IntValidator { bottom: 1; top: 99 }
                    background: Rectangle { color: window.bgSecondary; radius: 8 }
                    onTextChanged: {
                        let val = parseInt(text)
                        if (!isNaN(val)) window.globalTargetSessions = val
                    }
                }

                Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 90; radius: 12; color: window.textPrimary
                    Column { anchors.centerIn: parent;
                        Text { text: lang.totalFokus; font.pixelSize: 12; color: "#666"; anchors.horizontalCenter: parent.horizontalCenter }
                        Text { text: Math.floor(window.globalSecondsFocused/3600)+lang.jam[0] + " "+Math.floor((window.globalSecondsFocused%3600)/60)+"m"; font.pixelSize: 24; font.bold: true; color: "black" }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true; Layout.preferredHeight: 100; radius: 12; color: window.textPrimary
                    RowLayout {
                        anchors.centerIn: parent; spacing: 10

                        Text {
                            id: fireEmoji
                            text: "🔥"
                            font.pixelSize: 32
                            scale: window.globalTimerRunning ? 1.0 : 0.8
                            opacity: window.globalTimerRunning ? 1.0 : 0.3
                            Behavior on scale { NumberAnimation { duration: 500; easing.type: Easing.InOutQuad } }
                            SequentialAnimation on scale {
                                running: window.globalTimerRunning
                                loops: Animation.Infinite
                                NumberAnimation { from: 1.0; to: 1.2; duration: 800; easing.type: Easing.InOutBack }
                                NumberAnimation { from: 1.2; to: 1.0; duration: 800; easing.type: Easing.InOutBack }
                            }
                        }

                        Column {
                            Text {
                                text: window.globalTimerRunning ? lang.fokusAktif : lang.mulaiSesi
                                font.bold: true
                                color: window.globalTimerRunning ? "#e67e22" : "black"
                            }
                            Text {
                                text: window.globalTimerRunning ? lang.fokusSubAktif : lang.fokusSubIdle
                                color: window.globalTimerRunning ? "green" : "#95a5a6"
                                font.pixelSize: 11
                            }
                        }
                    }
                }
            }
        }

        // 3. FOOTER
        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            Button {
                anchors.centerIn: parent
                onClicked: pageLoader.sourceComponent = mainComponent
                contentItem: Text { text: lang.kembaliKeMenu; color: "black"; font.bold: true; horizontalAlignment: Text.AlignHCenter }
                background: Rectangle { implicitWidth: 200; implicitHeight: 40; color: window.accentColor; radius: 20 }
            }
        }
    }

    // ── Property alias untuk TextField input waktu ─────────────────────────
    // (diisi oleh Repeater via Component.onCompleted)
    property var hIn: null
    property var mIn: null
    property var sIn: null

    Component.onCompleted: {
        taskBtn.text = updateClosestTask()
        globalTaskModel.dataChanged.connect(function() { taskBtn.text = updateClosestTask() })
        globalTaskModel.rowsInserted.connect(function() { taskBtn.text = updateClosestTask() })
        globalTaskModel.rowsRemoved.connect(function()  { taskBtn.text = updateClosestTask() })
    }
}
