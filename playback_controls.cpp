//
// PlaybackControls - 播放控制栏实现（响应式）
// 手机端简化UI，平板/PC端完整功能
//

#include "playback_controls.h"
#include <QDebug>

PlaybackControls::PlaybackControls(QWidget* parent)
    : QWidget(parent),
    isPlaying(false),
    isMuted(false),
    lastVolume(70),
    currentDevice(DesignSystem::Desktop) {

    setupUI();
    connectSignals();
    applyStyles();
}

void PlaybackControls::setupUI() {
    // 创建主布局
    mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(
        DesignSystem::Dimensions::SpacingSmall,
        DesignSystem::Dimensions::SpacingSmall,
        DesignSystem::Dimensions::SpacingSmall,
        DesignSystem::Dimensions::SpacingSmall
        );
    mainLayout->setSpacing(DesignSystem::Dimensions::SpacingSmall);

    // 创建三个区域的容器
    setupLeftControls();
    setupCenterControls();
    setupRightControls();

    // 添加到主布局
    mainLayout->addWidget(leftControlsWidget);
    mainLayout->addWidget(centerControlsWidget, 1); // 拉伸因子为1
    mainLayout->addWidget(rightControlsWidget);
}

void PlaybackControls::setupLeftControls() {
    leftControlsWidget = new QWidget(this);
    QHBoxLayout* leftLayout = new QHBoxLayout(leftControlsWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(DesignSystem::Dimensions::SpacingSmall);

    int btnSize = DesignSystem::Dimensions::getButtonHeight(DesignSystem::Desktop);

    // 上一个按钮
    prevBtn = new QPushButton(leftControlsWidget);
    prevBtn->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    prevBtn->setFixedSize(btnSize - 8, btnSize - 8);
    prevBtn->setToolTip(tr("Previous video (Shift+Left)"));
    prevBtn->setCursor(Qt::PointingHandCursor);

    // 播放/暂停按钮
    playPauseBtn = new QPushButton(leftControlsWidget);
    playPauseBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    playPauseBtn->setFixedSize(btnSize, btnSize);
    playPauseBtn->setToolTip(tr("Play (Space)"));
    playPauseBtn->setCursor(Qt::PointingHandCursor);

    // 停止按钮
    stopBtn = new QPushButton(leftControlsWidget);
    stopBtn->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    stopBtn->setFixedSize(btnSize - 8, btnSize - 8);
    stopBtn->setToolTip(tr("Stop"));
    stopBtn->setCursor(Qt::PointingHandCursor);

    // 下一个按钮
    nextBtn = new QPushButton(leftControlsWidget);
    nextBtn->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    nextBtn->setFixedSize(btnSize - 8, btnSize - 8);
    nextBtn->setToolTip(tr("Next video (Shift+Right)"));
    nextBtn->setCursor(Qt::PointingHandCursor);

    leftLayout->addWidget(prevBtn);
    leftLayout->addWidget(playPauseBtn);
    leftLayout->addWidget(stopBtn);
    leftLayout->addWidget(nextBtn);
    leftLayout->addStretch();
}

void PlaybackControls::setupCenterControls() {
    centerControlsWidget = new QWidget(this);
    QHBoxLayout* centerLayout = new QHBoxLayout(centerControlsWidget);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerLayout->setSpacing(DesignSystem::Dimensions::SpacingSmall);

    // 当前时间
    currentTimeLabel = new QLabel("00:00", centerControlsWidget);
    currentTimeLabel->setFont(DesignSystem::Typography::getCaption());
    currentTimeLabel->setMinimumWidth(50);
    currentTimeLabel->setAlignment(Qt::AlignCenter);

    // 进度条
    progressSlider = new QSlider(Qt::Horizontal, centerControlsWidget);
    progressSlider->setMinimum(0);
    progressSlider->setMaximum(100);
    progressSlider->setValue(0);
    progressSlider->setCursor(Qt::PointingHandCursor);

    // 总时长
    totalTimeLabel = new QLabel("00:00", centerControlsWidget);
    totalTimeLabel->setFont(DesignSystem::Typography::getCaption());
    totalTimeLabel->setMinimumWidth(50);
    totalTimeLabel->setAlignment(Qt::AlignCenter);

    centerLayout->addWidget(currentTimeLabel);
    centerLayout->addWidget(progressSlider, 1);
    centerLayout->addWidget(totalTimeLabel);
}

void PlaybackControls::setupRightControls() {
    rightControlsWidget = new QWidget(this);
    QHBoxLayout* rightLayout = new QHBoxLayout(rightControlsWidget);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(DesignSystem::Dimensions::SpacingSmall);

    int btnSize = DesignSystem::Dimensions::getButtonHeight(DesignSystem::Desktop);

    // 音量按钮
    volumeBtn = new QPushButton(rightControlsWidget);
    volumeBtn->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    volumeBtn->setFixedSize(btnSize - 8, btnSize - 8);
    volumeBtn->setToolTip(tr("Mute/Unmute (M)"));
    volumeBtn->setCursor(Qt::PointingHandCursor);

    // 音量滑块
    volumeSlider = new QSlider(Qt::Horizontal, rightControlsWidget);
    volumeSlider->setMinimum(0);
    volumeSlider->setMaximum(100);
    volumeSlider->setValue(70);
    volumeSlider->setFixedWidth(100);
    volumeSlider->setCursor(Qt::PointingHandCursor);

    // 播放速度（仅桌面端）
    speedCombo = new QComboBox(rightControlsWidget);
    speedCombo->addItems({"0.25x", "0.5x", "0.75x", "1x", "1.25x", "1.5x", "1.75x", "2x"});
    speedCombo->setCurrentIndex(3); // 默认1x
    speedCombo->setMinimumWidth(80);
    speedCombo->setCursor(Qt::PointingHandCursor);

    // 全屏按钮（加大尺寸，更显眼）
    fullscreenBtn = new QPushButton(rightControlsWidget);
    fullscreenBtn->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));
    fullscreenBtn->setFixedSize(btnSize, btnSize);  // 和播放按钮一样大
    fullscreenBtn->setToolTip(tr("Fullscreen (F or Double-click video)"));
    fullscreenBtn->setCursor(Qt::PointingHandCursor);
    fullscreenBtn->setStyleSheet(QString(R"(
        QPushButton {
            background-color: %1;
            border: 2px solid %2;
            border-radius: %3px;
            padding: 4px;
        }
        QPushButton:hover {
            background-color: %2;
            border-color: %4;
        }
        QPushButton:pressed {
            background-color: %4;
        }
    )").arg(DesignSystem::Colors::getPrimary().name())
                                     .arg(DesignSystem::Colors::getPrimaryDark().name())
                                     .arg(DesignSystem::Dimensions::BorderRadiusLarge)
                                     .arg(DesignSystem::Colors::getPrimaryLight().name()));

    rightLayout->addStretch();
    rightLayout->addWidget(volumeBtn);
    rightLayout->addWidget(volumeSlider);
    rightLayout->addWidget(speedCombo);
    rightLayout->addWidget(fullscreenBtn);
}

void PlaybackControls::connectSignals() {
    // 播放控制
    connect(playPauseBtn, &QPushButton::clicked,
            this, &PlaybackControls::onPlayPauseClicked);
    connect(stopBtn, &QPushButton::clicked,
            this, &PlaybackControls::stopClicked);
    connect(prevBtn, &QPushButton::clicked,
            this, &PlaybackControls::previousClicked);
    connect(nextBtn, &QPushButton::clicked,
            this, &PlaybackControls::nextClicked);

    // 进度控制
    connect(progressSlider, &QSlider::sliderMoved,
            this, &PlaybackControls::onProgressSliderMoved);

    // 音量控制
    connect(volumeBtn, &QPushButton::clicked,
            this, &PlaybackControls::onVolumeButtonClicked);
    connect(volumeSlider, &QSlider::valueChanged,
            this, &PlaybackControls::onVolumeChanged);

    // 播放速度
    connect(speedCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PlaybackControls::onSpeedChanged);

    // 全屏控制
    connect(fullscreenBtn, &QPushButton::clicked,
            this, &PlaybackControls::fullscreenToggled);
}

void PlaybackControls::applyStyles() {
    // 控制栏背景
    setStyleSheet(QString(R"(
        PlaybackControls {
            background-color: %1;
            border-top: 1px solid %2;
        }
    )").arg(DesignSystem::Colors::getSurface().name())
                      .arg(DesignSystem::Colors::getDivider().name()));

    // 按钮样式
    QString buttonStyle = QString(R"(
        QPushButton {
            background-color: transparent;
            border: none;
            border-radius: %1px;
            padding: 4px;
        }
        QPushButton:hover {
            background-color: rgba(33, 150, 243, 0.1);
        }
        QPushButton:pressed {
            background-color: rgba(33, 150, 243, 0.2);
        }
    )").arg(DesignSystem::Dimensions::BorderRadiusLarge);

    playPauseBtn->setStyleSheet(buttonStyle);
    stopBtn->setStyleSheet(buttonStyle);
    prevBtn->setStyleSheet(buttonStyle);
    nextBtn->setStyleSheet(buttonStyle);
    volumeBtn->setStyleSheet(buttonStyle);
    fullscreenBtn->setStyleSheet(buttonStyle);

    // 进度条样式
    progressSlider->setStyleSheet(QString(R"(
        QSlider::groove:horizontal {
            height: 4px;
            background: %1;
            border-radius: 2px;
        }
        QSlider::handle:horizontal {
            background: %2;
            width: 16px;
            height: 16px;
            margin: -6px 0;
            border-radius: 8px;
        }
        QSlider::handle:horizontal:hover {
            background: %3;
            width: 18px;
            height: 18px;
            margin: -7px 0;
            border-radius: 9px;
        }
        QSlider::sub-page:horizontal {
            background: %2;
            border-radius: 2px;
        }
    )").arg(DesignSystem::Colors::getDivider().name())
                                      .arg(DesignSystem::Colors::getPrimary().name())
                                      .arg(DesignSystem::Colors::getPrimaryDark().name()));

    // 音量滑块样式
    volumeSlider->setStyleSheet(QString(R"(
        QSlider::groove:horizontal {
            height: 4px;
            background: %1;
            border-radius: 2px;
        }
        QSlider::handle:horizontal {
            background: %2;
            width: 12px;
            height: 12px;
            margin: -4px 0;
            border-radius: 6px;
        }
        QSlider::handle:horizontal:hover {
            background: %3;
        }
        QSlider::sub-page:horizontal {
            background: %2;
            border-radius: 2px;
        }
    )").arg(DesignSystem::Colors::getDivider().name())
                                    .arg(DesignSystem::Colors::getPrimary().name())
                                    .arg(DesignSystem::Colors::getPrimaryDark().name()));

    // 时间标签样式
    QString labelStyle = QString(R"(
        QLabel {
            color: %1;
            background: transparent;
        }
    )").arg(DesignSystem::Colors::getTextSecondary().name());

    currentTimeLabel->setStyleSheet(labelStyle);
    totalTimeLabel->setStyleSheet(labelStyle);

    // 速度下拉框样式
    speedCombo->setStyleSheet(QString(R"(
        QComboBox {
            background-color: transparent;
            border: 1px solid %1;
            border-radius: 4px;
            padding: 4px 8px;
            color: %2;
        }
        QComboBox:hover {
            border-color: %3;
        }
        QComboBox::drop-down {
            border: none;
        }
    )").arg(DesignSystem::Colors::getBorder().name())
                                  .arg(DesignSystem::Colors::getTextPrimary().name())
                                  .arg(DesignSystem::Colors::getPrimary().name()));
}

void PlaybackControls::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);

    int width = event->size().width();
    DesignSystem::DeviceType device = DesignSystem::getDeviceType(width);

    if (device != currentDevice) {
        currentDevice = device;
        updateLayoutForDevice(device);
        qDebug() << "Device type changed to:" << device;
    }
}

void PlaybackControls::updateLayoutForDevice(DesignSystem::DeviceType device) {
    switch (device) {
    case DesignSystem::Mobile:
        // 手机端：隐藏停止和上一个/下一个按钮
        stopBtn->hide();
        prevBtn->hide();
        nextBtn->hide();

        // 隐藏音量滑块（保留按钮）
        volumeSlider->hide();

        // 隐藏速度选择器
        speedCombo->hide();

        // 时间标签更小
        currentTimeLabel->setMinimumWidth(40);
        totalTimeLabel->setMinimumWidth(40);

        qDebug() << "Switched to MOBILE layout";
        break;

    case DesignSystem::Tablet:
        // 平板端：显示所有主要控制
        stopBtn->show();
        prevBtn->show();
        nextBtn->show();
        volumeSlider->show();
        speedCombo->hide(); // 平板端可选择隐藏速度

        currentTimeLabel->setMinimumWidth(50);
        totalTimeLabel->setMinimumWidth(50);

        qDebug() << "Switched to TABLET layout";
        break;

    case DesignSystem::Desktop:
        // 桌面端：显示所有功能
        stopBtn->show();
        prevBtn->show();
        nextBtn->show();
        volumeSlider->show();
        speedCombo->show();

        currentTimeLabel->setMinimumWidth(50);
        totalTimeLabel->setMinimumWidth(50);

        qDebug() << "Switched to DESKTOP layout";
        break;
    }
}

void PlaybackControls::onPlayPauseClicked() {
    emit playPauseClicked();
}

void PlaybackControls::setPlaying(bool playing) {
    isPlaying = playing;

    if (isPlaying) {
        playPauseBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        playPauseBtn->setToolTip(tr("Pause (Space)"));
    } else {
        playPauseBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        playPauseBtn->setToolTip(tr("Play (Space)"));
    }
}

void PlaybackControls::updateProgress(qint64 position, qint64 duration) {
    if (!progressSlider->isSliderDown() && duration > 0) {
        progressSlider->setValue(static_cast<int>((position * 100) / duration));
    }

    currentTimeLabel->setText(formatTime(position));
}

void PlaybackControls::setTotalDuration(qint64 duration) {
    totalTimeLabel->setText(formatTime(duration));
}

void PlaybackControls::onProgressSliderMoved(int position) {
    emit seekRequested(position);
}

void PlaybackControls::onVolumeButtonClicked() {
    if (isMuted) {
        setMuted(false);
        volumeSlider->setValue(lastVolume);
        emit volumeChanged(lastVolume);
        emit muteToggled(false);
    } else {
        lastVolume = volumeSlider->value();
        setMuted(true);
        emit volumeChanged(0);
        emit muteToggled(true);
    }
}

void PlaybackControls::onVolumeChanged(int value) {
    if (!isMuted) {
        lastVolume = value;
        emit volumeChanged(value);
    }

    if (value == 0 && !isMuted) {
        setMuted(true);
    } else if (value > 0 && isMuted) {
        setMuted(false);
    }
}

void PlaybackControls::setVolume(int volume) {
    volumeSlider->setValue(volume);
    lastVolume = volume;

    if (volume == 0) {
        setMuted(true);
    } else {
        setMuted(false);
    }
}

void PlaybackControls::setMuted(bool muted) {
    isMuted = muted;

    if (isMuted) {
        volumeBtn->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
        volumeBtn->setToolTip(tr("Unmute (M)"));
    } else {
        volumeBtn->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
        volumeBtn->setToolTip(tr("Mute (M)"));
    }
}

void PlaybackControls::updateTheme() {
    applyStyles();
    update();
}

void PlaybackControls::onSpeedChanged(int index) {
    QStringList speeds = {"0.25", "0.5", "0.75", "1.0", "1.25", "1.5", "1.75", "2.0"};
    qreal speed = speeds[index].toDouble();

    qDebug() << "Playback speed changed to:" << speed;
    emit playbackSpeedChanged(speed);
}

QString PlaybackControls::formatTime(qint64 milliseconds) {
    int seconds = milliseconds / 1000;
    int minutes = seconds / 60;
    seconds = seconds % 60;

    return QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}
