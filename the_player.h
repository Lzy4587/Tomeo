//
// ThePlayer - 改进的媒体播放器
// Iteration 2: 增加播放速度控制
//

#ifndef THE_PLAYER_H
#define THE_PLAYER_H

#include <QMediaPlayer>
#include <QTimer>
#include <vector>
#include "the_button.h"

class PlaybackControls;

class ThePlayer : public QMediaPlayer {
    Q_OBJECT

private:
    std::vector<TheButtonInfo>* infos;
    std::vector<TheButton*>* buttons;
    QTimer* shuffleTimer;
    PlaybackControls* controls;

    int currentVideoIndex;
    bool autoRepeat;
    bool shuffleEnabled;
    long updateCount;

public:
    explicit ThePlayer(QWidget* parent = nullptr);

    // 内容管理
    void setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i);
    void setControls(PlaybackControls* ctrl);

    // 访问器
    int getCurrentIndex() const { return currentVideoIndex; }
    bool isAutoRepeat() const { return autoRepeat; }
    bool isShuffleEnabled() const { return shuffleEnabled; }

    // 配置
    void setAutoRepeat(bool enable) { autoRepeat = enable; }
    void setShuffleEnabled(bool enable);

private slots:
    void shuffle();
    void playStateChanged(QMediaPlayer::State ms);
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);
    void onError(QMediaPlayer::Error error);

public slots:
    // 播放控制
    void jumpTo(TheButtonInfo* button);
    void jumpToIndex(int index);
    void togglePlayPause();
    void playNext();
    void playPrevious();

    // 进度控制
    void seekToPosition(int percentage);
    void seekRelative(qint64 milliseconds);

    // 音量控制
    void changeVolume(int volume);
    void toggleMute();

    // 播放速度控制
    void setPlaybackRate(qreal rate);

signals:
    void videoChanged(int index);
    void playbackStateChanged(bool playing);
};

#endif // THE_PLAYER_H
