//
// SocialManager - 社交功能管理器（添加缩略图加载功能）
// Iteration 3: 管理用户、帖子、评论等社交数据
//

#ifndef SOCIAL_MANAGER_H
#define SOCIAL_MANAGER_H

#include <QObject>
#include <QVector>
#include <QSettings>
#include "social_types.h"

class SocialManager : public QObject {
    Q_OBJECT

private:
    static SocialManager* instance;

    UserInfo currentUser;                    // 当前用户
    QVector<VideoPost> allPosts;            // 所有帖子
    QVector<UserInfo> friends;              // 好友列表
    DailyReminderSettings reminderSettings; // 提醒设置
    QSettings* settings;

    explicit SocialManager(QObject* parent = nullptr);
    void generateMockData();  // 生成模拟数据

public:
    // 单例模式
    static SocialManager* getInstance();

    // 🔥 新增：加载真实视频缩略图
    void loadRealThumbnails(const QString& videoDir);

    // 用户管理
    UserInfo getCurrentUser() const { return currentUser; }
    void setCurrentUser(const UserInfo& user);
    QVector<UserInfo> getFriends() const { return friends; }
    void addFriend(const UserInfo& user);
    void removeFriend(const QString& userId);

    // 帖子管理
    QVector<VideoPost> getAllPosts() const;
    QVector<VideoPost> getHotPosts() const;
    QVector<VideoPost> getFriendsPosts() const;
    VideoPost getPost(const QString& postId) const;
    void addPost(const VideoPost& post);
    void deletePost(const QString& postId);

    // 互动功能
    void likePost(const QString& postId);
    void unlikePost(const QString& postId);
    void addComment(const QString& postId, const Comment& comment);
    void deleteComment(const QString& postId, const QString& commentId);
    void likeComment(const QString& postId, const QString& commentId);

    // 提醒设置
    DailyReminderSettings getReminderSettings() const { return reminderSettings; }
    void setReminderSettings(const DailyReminderSettings& settings);

    // 数据持久化
    void saveData();
    void loadData();

signals:
    void postAdded(const VideoPost& post);
    void postDeleted(const QString& postId);
    void postLiked(const QString& postId, bool isLiked);
    void commentAdded(const QString& postId, const Comment& comment);
    void friendAdded(const UserInfo& user);
    void friendRemoved(const QString& userId);
};

#endif // SOCIAL_MANAGER_H
