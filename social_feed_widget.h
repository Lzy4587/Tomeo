//
// SocialFeedWidget - 社交Feed流组件
// Iteration 3: 显示所有视频帖子的Feed流
//

#ifndef SOCIAL_FEED_WIDGET_H
#define SOCIAL_FEED_WIDGET_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPushButton>
#include <QButtonGroup>
#include <QLabel>
#include "social_types.h"
#include "video_post_card.h"

class ShareDialog;

class SocialFeedWidget : public QWidget {
    Q_OBJECT

private:
    // 过滤标签页
    QButtonGroup* filterButtonGroup;
    QPushButton* allBtn;
    QPushButton* hotBtn;
    QPushButton* friendsBtn;

    // 滚动区域
    QScrollArea* scrollArea;
    QWidget* contentWidget;
    QVBoxLayout* contentLayout;

    // 当前过滤类型
    SocialFilter currentFilter;

    // 帖子卡片列表
    QVector<VideoPostCard*> postCards;

    // 空状态
    QLabel* emptyStateLabel;

    void setupUI();
    void connectSignals();
    void applyStyles();
    void loadPosts();
    void clearPosts();
    void showEmptyState(const QString& message);

public:
    explicit SocialFeedWidget(QWidget* parent = nullptr);

    void setFilter(SocialFilter filter);
    void refreshFeed();
    void updateTheme();

private slots:
    void onFilterChanged(int id);
    void onPostPlayRequested(const VideoPost& post);
    void onPostLikeToggled(const QString& postId, bool isLiked);
    void onPostCommentRequested(const QString& postId);
    void onPostShareRequested(const QString& postId);
    void onShareConfirmed(const QString& postId, const QString& platform, const QString& comment);

signals:
    void videoPlayRequested(const VideoPost& post);
    void commentDialogRequested(const QString& postId);
};

#endif // SOCIAL_FEED_WIDGET_H
