//
// ThePlayer - 实现
//

#include "the_player.h"
#include "playback_controls.h"
#include <QDebug>

ThePlayer::ThePlayer(QWidget* parent)
    : QMediaPlayer(parent),
    infos(nullptr),
    buttons(nullptr),
    controls(nullptr),
    currentVideoIndex(0),
    autoRepeat(true),
    shuffleEnabled(false),
    updateCount(0) {

    // 设置默认音量
    setVolume(70);

    // 连接内部信号
    connect(this, &QMediaPlayer::stateChanged,
            this, &ThePlayer::playStateChanged);
    connect(this, &QMediaPlayer::positionChanged,
            this, &ThePlayer::onPositionChanged);
    connect(this, &QMediaPlayer::durationChanged,
            this, &ThePlayer::onDurationChanged);
    connect(this, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
            this, &ThePlayer::onError);

    // 创建shuffle计时器（默认禁用）
    shuffleTimer = new QTimer(this);
    shuffleTimer->setInterval(5000); // 5秒更新一次按钮
    connect(shuffleTimer, &QTimer::timeout, this, &ThePlayer::shuffle);
}

void ThePlayer::setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i) {
    buttons = b;
    infos = i;

    if (!buttons->empty() && !infos->empty()) {
        // 播放第一个视频
        jumpToIndex(0);
    }
}

void ThePlayer::setControls(PlaybackControls* ctrl) {
    controls = ctrl;

    if (controls) {
        // 连接控制栏信号
        connect(controls, &PlaybackControls::playPauseClicked,
                this, &ThePlayer::togglePlayPause);
        connect(controls, &PlaybackControls::stopClicked,
                this, &QMediaPlayer::stop);
        connect(controls, &PlaybackControls::previousClicked,
                this, &ThePlayer::playPrevious);
        connect(controls, &PlaybackControls::nextClicked,
                this, &ThePlayer::playNext);
        connect(controls, &PlaybackControls::seekRequested,
                this, &ThePlayer::seekToPosition);
        connect(controls, &PlaybackControls::volumeChanged,
                this, &ThePlayer::changeVolume);

        // 初始化控制栏状态
        controls->setVolume(volume());
        controls->setPlaying(state() == QMediaPlayer::PlayingState);
    }
}

void ThePlayer::setShuffleEnabled(bool enable) {
    shuffleEnabled = enable;

    if (shuffleEnabled) {
        shuffleTimer->start();
    } else {
        shuffleTimer->stop();
    }
}

void ThePlayer::shuffle() {
    if (!infos || infos->empty() || !buttons || buttons->empty()) {
        return;
    }

    // 随机选择一个视频信息
    TheButtonInfo* randomInfo = &infos->at(rand() % infos->size());

    // 随机选择一个按钮并更新其图标
    TheButton* button = buttons->at(updateCount++ % buttons->size());
    button->init(randomInfo);

    qDebug() << "Shuffled button" << (updateCount - 1) % buttons->size();
}

void ThePlayer::playStateChanged(QMediaPlayer::State ms) {
    switch (ms) {
    case QMediaPlayer::PlayingState:
        qDebug() << "State: Playing";
        if (controls) {
            controls->setPlaying(true);
        }
        emit playbackStateChanged(true);
        break;

    case QMediaPlayer::PausedState:
        qDebug() << "State: Paused";
        if (controls) {
            controls->setPlaying(false);
        }
        emit playbackStateChanged(false);
        break;

    case QMediaPlayer::StoppedState:
        qDebug() << "State: Stopped";
        if (controls) {
            controls->setPlaying(false);
        }
        emit playbackStateChanged(false);

        // 如果启用了自动重复,重新播放
        if (autoRepeat) {
            setPosition(0);
            play();
        }
        break;
    }
}

void ThePlayer::onPositionChanged(qint64 position) {
    if (controls) {
        controls->updateProgress(position, duration());
    }
}

void ThePlayer::onDurationChanged(qint64 duration) {
    if (controls) {
        controls->setTotalDuration(duration);
    }
    qDebug() << "Video duration:" << duration << "ms";
}

void ThePlayer::onError(QMediaPlayer::Error error) {
    qDebug() << "Player error:" << errorString();
    qDebug() << "Error code:" << error;
}

void ThePlayer::jumpTo(TheButtonInfo* button) {
    if (!button || !button->url) {
        qDebug() << "Invalid button info";
        return;
    }

    qDebug() << "Jumping to video:" << button->url->toString();

    // 查找这个视频在列表中的索引
    for (size_t i = 0; i < infos->size(); ++i) {
        if (infos->at(i).url == button->url) {
            currentVideoIndex = i;
            break;
        }
    }

    // 设置媒体并播放
    setMedia(*button->url);
    play();

    emit videoChanged(currentVideoIndex);
}

void ThePlayer::jumpToIndex(int index) {
    if (!infos || index < 0 || index >= static_cast<int>(infos->size())) {
        qDebug() << "Invalid video index:" << index;
        return;
    }

    currentVideoIndex = index;
    TheButtonInfo* info = &infos->at(index);

    qDebug() << "Jumping to index:" << index;

    setMedia(*info->url);
    play();

    emit videoChanged(currentVideoIndex);
}

void ThePlayer::togglePlayPause() {
    if (state() == QMediaPlayer::PlayingState) {
        pause();
        qDebug() << "Paused";
    } else {
        play();
        qDebug() << "Playing";
    }
}

void ThePlayer::playNext() {
    if (!infos || infos->empty()) {
        return;
    }

    int nextIndex = (currentVideoIndex + 1) % infos->size();
    jumpToIndex(nextIndex);

    qDebug() << "Playing next video:" << nextIndex;
}

void ThePlayer::playPrevious() {
    if (!infos || infos->empty()) {
        return;
    }

    int prevIndex = (currentVideoIndex - 1 + infos->size()) % infos->size();
    jumpToIndex(prevIndex);

    qDebug() << "Playing previous video:" << prevIndex;
}

void ThePlayer::seekToPosition(int percentage) {
    qint64 newPosition = (duration() * percentage) / 100;
    setPosition(newPosition);

    qDebug() << "Seeking to" << percentage << "% (" << newPosition << "ms)";
}

void ThePlayer::seekRelative(qint64 milliseconds) {
    qint64 newPosition = position() + milliseconds;

    // 确保不超出范围
    if (newPosition < 0) {
        newPosition = 0;
    } else if (newPosition > duration()) {
        newPosition = duration();
    }

    setPosition(newPosition);

    qDebug() << "Seeking by" << milliseconds << "ms to" << newPosition << "ms";
}

void ThePlayer::changeVolume(int volume) {
    setVolume(volume);
    qDebug() << "Volume changed to:" << volume;
}

void ThePlayer::toggleMute() {
    setMuted(!isMuted());

    if (controls) {
        controls->setMuted(isMuted());
    }

    qDebug() << "Mute toggled:" << (isMuted() ? "ON" : "OFF");
}

void ThePlayer::setPlaybackRate(qreal rate) {
    QMediaPlayer::setPlaybackRate(rate);
    qDebug() << "Playback rate set to:" << rate;
}
