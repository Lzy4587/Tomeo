#ifndef MAIN_CONTAINER_H
#define MAIN_CONTAINER_H

#include <QWidget>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QScrollArea>
#include <QVideoWidget>
#include <QGridLayout>
#include <vector>

// 引入自定义头文件
#include "top_toolbar.h"
#include "bottom_navigation_bar.h"
#include "record_dialog.h"
#include "the_player.h"
#include "the_button.h"
#include "playback_controls.h"
#include "social_manager.h"
#include "video_post_card.h"
#include "social_feed_widget.h"
// 注意：SettingsDialog 和 CommentDialog 在 cpp 中引入即可，这里不需要

class MainContainer : public QWidget {
    Q_OBJECT

private:
    // UI组件
    TopToolbar* topToolbar;
    QStackedWidget* contentStack;
    BottomNavigationBar* bottomNav;

    // 页面
    QWidget* videosPage;
    QWidget* socialPage;
    QWidget* messagesPage;
    QWidget* profilePage;

    // 视频页组件
    QWidget* playerContainer;
    QVideoWidget* videoWidget;
    PlaybackControls* controls;
    QScrollArea* gridScrollArea;
    QWidget* gridWidget;
    QGridLayout* gridLayout;

    // 逻辑组件
    ThePlayer* player;
    RecordDialog* recordDialog;
    std::vector<TheButton*> allButtons;

    // 函数
    void setupUI();
    void createVideosPage();
    void createSocialPage();
    void createMessagesPage();
    void createProfilePage();
    void updateResponsiveLayout(int windowWidth);

protected:
    void resizeEvent(QResizeEvent* event) override;

public:
    explicit MainContainer(std::vector<TheButtonInfo>& videos, QWidget* parent = nullptr);
    ~MainContainer();

    ThePlayer* getPlayer() const { return player; }
    void updateTheme();

private slots:
    void onNavigationPageChanged(BottomNavigationBar::NavigationPage page);
    void onCreateRequested();
    void onVideoRecorded(const QString& videoPath, bool isFrontCamera);
    void onVideoSelected(TheButtonInfo* info);
    void onSocialPlayRequested(const VideoPost& post);

    // [修复] 新增：处理设置和评论的槽函数
    void onSettingsClicked();
    void onCommentDialogRequested(const QString& postId);
};

#endif // MAIN_CONTAINER_H
