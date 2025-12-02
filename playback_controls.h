//
// PlaybackControls - 播放控制栏组件（响应式版本）
// Iteration 2+: 手机/平板/PC端差异化UI
//

#ifndef PLAYBACK_CONTROLS_H
#define PLAYBACK_CONTROLS_H

#include <QWidget>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStyle>
#include <QTime>
#include <QComboBox>
#include <QResizeEvent>
#include "design_system.h"

class PlaybackControls : public QWidget {
    Q_OBJECT

private:
    // 控制按钮
    QPushButton* playPauseBtn;
    QPushButton* stopBtn;
    QPushButton* prevBtn;
    QPushButton* nextBtn;

    // 进度控制
    QSlider* progressSlider;
    QLabel* currentTimeLabel;
    QLabel* totalTimeLabel;

    // 音量控制
    QPushButton* volumeBtn;
    QSlider* volumeSlider;

    // 播放速度
    QComboBox* speedCombo;

    // 其他控制
    QPushButton* fullscreenBtn;

    // 布局管理
    QHBoxLayout* mainLayout;
    QWidget* leftControlsWidget;   // 左侧按钮组
    QWidget* centerControlsWidget; // 中间进度条
    QWidget* rightControlsWidget;  // 右侧音量/全屏

    // 状态
    bool isPlaying;
    bool isMuted;
    int lastVolume;
    DesignSystem::DeviceType currentDevice;

    void setupUI();
    void setupLeftControls();
    void setupCenterControls();
    void setupRightControls();
    void connectSignals();
    void applyStyles();
    void updateLayoutForDevice(DesignSystem::DeviceType device);
    QString formatTime(qint64 milliseconds);

protected:
    void resizeEvent(QResizeEvent* event) override;

public:
    explicit PlaybackControls(QWidget* parent = nullptr);

    // 公开方法
    void setPlaying(bool playing);
    void setVolume(int volume);
    void setMuted(bool muted);
    void updateTheme();

public slots:
    void updateProgress(qint64 position, qint64 duration);
    void setTotalDuration(qint64 duration);
    void onPlayPauseClicked();
    void onVolumeButtonClicked();
    void onVolumeChanged(int value);
    void onProgressSliderMoved(int position);
    void onSpeedChanged(int index);

signals:
    // 播放控制信号
    void playPauseClicked();
    void stopClicked();
    void previousClicked();
    void nextClicked();

    // 进度控制信号
    void seekRequested(qint64 position);

    // 音量控制信号
    void volumeChanged(int volume);
    void muteToggled(bool muted);

    // 播放速度信号
    void playbackSpeedChanged(qreal speed);

    // 其他信号
    void fullscreenToggled();
};

#endif // PLAYBACK_CONTROLS_H
