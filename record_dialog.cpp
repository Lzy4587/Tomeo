#include "record_dialog.h"
#include "design_system.h"
#include <QDebug>
#include <QMessageBox>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>

RecordDialog::RecordDialog(QWidget* parent)
    : QDialog(parent),
    isRecording(false),
    isFrontCamera(true),
    recordingSeconds(0),
    maxRecordingSeconds(60),
    camera(nullptr),
    mediaRecorder(nullptr) {

    setWindowTitle(tr("Record Video"));
    setModal(true);
    resize(600, 800);

    setupUI();
    connectSignals();
    applyStyles();

    recordTimer = new QTimer(this);
    connect(recordTimer, &QTimer::timeout, this, &RecordDialog::onRecordTimerTimeout);
}

RecordDialog::~RecordDialog() {
    if (camera) {
        camera->stop();
        delete camera;
    }
    if (mediaRecorder) {
        delete mediaRecorder;
    }
}

void RecordDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // === 顶部控制栏 ===
    QWidget* topBar = new QWidget(this);
    topBar->setFixedHeight(60);
    QHBoxLayout* topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(16, 8, 16, 8);

    closeBtn = new QPushButton("✕", topBar);
    closeBtn->setFixedSize(40, 40);
    closeBtn->setFont(QFont("Arial", 20));
    closeBtn->setCursor(Qt::PointingHandCursor);
    topLayout->addWidget(closeBtn);

    topLayout->addStretch();

    flashBtn = new QPushButton("⚡", topBar);
    flashBtn->setFixedSize(40, 40);
    flashBtn->setFont(QFont("Segoe UI Emoji", 18));
    flashBtn->setCursor(Qt::PointingHandCursor);
    topLayout->addWidget(flashBtn);

    switchCameraBtn = new QPushButton("🔄", topBar);
    switchCameraBtn->setFixedSize(40, 40);
    switchCameraBtn->setFont(QFont("Segoe UI Emoji", 18));
    switchCameraBtn->setCursor(Qt::PointingHandCursor);
    topLayout->addWidget(switchCameraBtn);

    mainLayout->addWidget(topBar);

    // === 取景器区域 ===
    viewfinderLabel = new QLabel(this);
    viewfinderLabel->setAlignment(Qt::AlignCenter);
    viewfinderLabel->setStyleSheet(R"(
        QLabel {
            background-color: #000000;
            color: #FFFFFF;
            font-size: 48px;
        }
    )");
    viewfinderLabel->setText("📹");

    // [关键修改] 让取景器占据所有剩余空间，并把它作为中间层
    // 这样计时器和底部栏就会自然排列在它下面
    mainLayout->addWidget(viewfinderLabel, 1);

    // === 录制信息栏 (计时器) ===
    QWidget* infoBar = new QWidget(this);
    // [修改] 增加固定高度，确保数字不会被底部遮挡
    infoBar->setFixedHeight(50);
    QHBoxLayout* infoLayout = new QHBoxLayout(infoBar);
    infoLayout->setAlignment(Qt::AlignCenter);
    infoLayout->setContentsMargins(0, 10, 0, 10); // 上下留白

    timerLabel = new QLabel("00:00", infoBar);
    timerLabel->setFont(QFont("Arial", 20, QFont::Bold));
    timerLabel->setStyleSheet("color: #FF4444;");
    infoLayout->addWidget(timerLabel);

    // 将计时器放在底部栏之上
    mainLayout->addWidget(infoBar);

    // === 进度条 ===
    progressBar = new QProgressBar(this);
    progressBar->setFixedHeight(6);
    progressBar->setRange(0, maxRecordingSeconds);
    progressBar->setValue(0);
    progressBar->setTextVisible(false);
    mainLayout->addWidget(progressBar);

    // === 底部控制栏 ===
    QWidget* bottomBar = new QWidget(this);
    bottomBar->setFixedHeight(120);
    QVBoxLayout* bottomLayout = new QVBoxLayout(bottomBar);
    bottomLayout->setContentsMargins(16, 16, 16, 16);
    bottomLayout->setSpacing(12);

    // 录制按钮容器
    QWidget* recordContainer = new QWidget(bottomBar);
    QHBoxLayout* recordLayout = new QHBoxLayout(recordContainer);
    recordLayout->setAlignment(Qt::AlignCenter);

    recordBtn = new QPushButton(this);
    recordBtn->setFixedSize(80, 80);
    recordBtn->setCursor(Qt::PointingHandCursor);
    recordLayout->addWidget(recordBtn);

    bottomLayout->addWidget(recordContainer);

    // 预览控制按钮（初始隐藏）
    QWidget* previewContainer = new QWidget(bottomBar);
    QHBoxLayout* previewLayout = new QHBoxLayout(previewContainer);
    previewLayout->setSpacing(20);
    previewLayout->setAlignment(Qt::AlignCenter);

    retakeBtn = new QPushButton(tr("Retake"), previewContainer);
    retakeBtn->setFixedSize(120, 45);
    retakeBtn->setCursor(Qt::PointingHandCursor);
    previewLayout->addWidget(retakeBtn);

    useVideoBtn = new QPushButton(tr("Use Video"), previewContainer);
    useVideoBtn->setFixedSize(120, 45);
    useVideoBtn->setCursor(Qt::PointingHandCursor);
    previewLayout->addWidget(useVideoBtn);

    bottomLayout->addWidget(previewContainer);
    previewContainer->hide();

    mainLayout->addWidget(bottomBar);

    updateRecordButton();
}

void RecordDialog::connectSignals() {
    connect(recordBtn, &QPushButton::clicked, this, &RecordDialog::onRecordClicked);
    connect(switchCameraBtn, &QPushButton::clicked, this, &RecordDialog::onSwitchCameraClicked);
    connect(flashBtn, &QPushButton::clicked, this, &RecordDialog::onFlashClicked);
    connect(closeBtn, &QPushButton::clicked, this, &RecordDialog::onCloseClicked);
    connect(useVideoBtn, &QPushButton::clicked, this, &RecordDialog::onUseVideoClicked);
    connect(retakeBtn, &QPushButton::clicked, this, &RecordDialog::onRetakeClicked);
}

void RecordDialog::applyStyles() {
    setStyleSheet(QString(R"(
        QDialog {
            background-color: %1;
        }
    )").arg(DesignSystem::Colors::getBackground().name()));

    QString iconButtonStyle = QString(R"(
        QPushButton {
            background-color: rgba(255, 255, 255, 0.2);
            border: none;
            border-radius: 20px;
            color: white;
        }
        QPushButton:hover {
            background-color: rgba(255, 255, 255, 0.3);
        }
        QPushButton:pressed {
            background-color: rgba(255, 255, 255, 0.4);
        }
    )");

    closeBtn->setStyleSheet(iconButtonStyle);
    flashBtn->setStyleSheet(iconButtonStyle);
    switchCameraBtn->setStyleSheet(iconButtonStyle);

    updateRecordButton();

    retakeBtn->setStyleSheet(QString(R"(
        QPushButton {
            background-color: %1;
            border: 2px solid white;
            border-radius: 22px;
            color: white;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: %2;
        }
    )").arg(DesignSystem::Colors::getTextSecondary().name())
                                 .arg(DesignSystem::Colors::getTextPrimary().name()));

    useVideoBtn->setStyleSheet(QString(R"(
        QPushButton {
            background-color: %1;
            border: none;
            border-radius: 22px;
            color: white;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: %2;
        }
    )").arg(DesignSystem::Colors::getPrimary().name())
                                   .arg(DesignSystem::Colors::getPrimaryDark().name()));

    progressBar->setStyleSheet(QString(R"(
        QProgressBar {
            background-color: rgba(255, 255, 255, 0.2);
            border: none;
        }
        QProgressBar::chunk {
            background-color: #FF4444;
        }
    )"));
}

void RecordDialog::updateRecordButton() {
    if (isRecording) {
        recordBtn->setStyleSheet(R"(
            QPushButton {
                background-color: #FF4444;
                border: 6px solid white;
                border-radius: 40px;
            }
            QPushButton:hover {
                background-color: #FF6666;
            }
        )");
    } else {
        recordBtn->setStyleSheet(R"(
            QPushButton {
                background-color: white;
                border: 4px solid rgba(0,0,0,0.1);
                border-radius: 40px;
            }
            QPushButton:hover {
                background-color: #EEEEEE;
            }
        )");
    }
}

void RecordDialog::updateTimer() {
    int minutes = recordingSeconds / 60;
    int seconds = recordingSeconds % 60;
    timerLabel->setText(QString("%1:%2")
                            .arg(minutes, 2, 10, QChar('0'))
                            .arg(seconds, 2, 10, QChar('0')));
    progressBar->setValue(recordingSeconds);
}

void RecordDialog::showPreviewControls(bool show) {
    recordBtn->setVisible(!show);
    useVideoBtn->parentWidget()->setVisible(show);
}

void RecordDialog::simulateRecording() {
    QString videosPath = "E:/Qt project/cw3/2811_cw3-master-release-lowres/test_videos";
    QDir dir(videosPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    recordedVideoPath = QString("%1/tomeo_video_%2.mp4")
                            .arg(videosPath)
                            .arg(timestamp);

    qDebug() << "Simulated recording saved to:" << recordedVideoPath;
}

void RecordDialog::showEvent(QShowEvent* event) {
    QDialog::showEvent(event);
    qDebug() << "RecordDialog shown - attempting to initialize camera";
}

void RecordDialog::closeEvent(QCloseEvent* event) {
    if (isRecording) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            tr("Stop Recording"),
            tr("Recording is in progress. Do you want to stop and discard?"),
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply == QMessageBox::No) {
            event->ignore();
            return;
        }

        isRecording = false;
        recordTimer->stop();
    }

    emit recordingCancelled();
    QDialog::closeEvent(event);
}

void RecordDialog::onRecordClicked() {
    if (!isRecording) {
        qDebug() << "=== Starting recording ===";
        isRecording = true;
        recordingSeconds = 0;

        updateRecordButton();
        updateTimer();

        recordTimer->start(1000);

        viewfinderLabel->setText("🔴 REC");

    } else {
        qDebug() << "=== Stopping recording ===";
        onRecordingFinished();
    }
}

void RecordDialog::onSwitchCameraClicked() {
    isFrontCamera = !isFrontCamera;
    qDebug() << "Camera switched to:" << (isFrontCamera ? "Front" : "Back");
    if (isFrontCamera) {
        viewfinderLabel->setText("📹\nFront Camera");
    } else {
        viewfinderLabel->setText("📹\nBack Camera");
    }
}

void RecordDialog::onFlashClicked() {
    qDebug() << "Flash toggled";
    static bool flashOn = false;
    flashOn = !flashOn;
    flashBtn->setText(flashOn ? "⚡" : "🔦");
}

void RecordDialog::onCloseClicked() {
    close();
}

void RecordDialog::onUseVideoClicked() {
    qDebug() << "=== Use video clicked ===";
    emit videoRecorded(recordedVideoPath, isFrontCamera);
    accept();
}

void RecordDialog::onRetakeClicked() {
    qDebug() << "=== Retake clicked ===";
    recordingSeconds = 0;
    recordedVideoPath.clear();
    updateTimer();
    showPreviewControls(false);
    viewfinderLabel->setText("📹");
}

void RecordDialog::onRecordTimerTimeout() {
    recordingSeconds++;
    updateTimer();

    if (recordingSeconds >= maxRecordingSeconds) {
        qDebug() << "Max recording time reached";
        onRecordingFinished();
    }
}

void RecordDialog::onRecordingFinished() {
    qDebug() << "=== Recording finished ===";
    qDebug() << "Duration:" << recordingSeconds << "seconds";

    isRecording = false;
    recordTimer->stop();

    updateRecordButton();
    simulateRecording();

    viewfinderLabel->setText(QString("✓\nRecorded\n%1s").arg(recordingSeconds));
    showPreviewControls(true);
}
