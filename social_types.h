//
// SocialTypes - 社交功能数据结构
// Iteration 3: BeReal 社交功能整合
//

#ifndef SOCIAL_TYPES_H
#define SOCIAL_TYPES_H

#include <QString>
#include <QDateTime>
#include <QPixmap>
#include <QVector>
#include <QUrl>

// 用户信息
struct UserInfo {
    QString userId;           // 用户ID
    QString username;         // 用户名
    QString displayName;      // 显示名称
    QPixmap avatar;          // 头像
    QString bio;             // 个人简介
    int followersCount;      // 粉丝数
    int followingCount;      // 关注数
    bool isFriend;           // 是否是好友

    UserInfo()
        : userId(""), username(""), displayName(""),
        bio(""), followersCount(0), followingCount(0),
        isFriend(false) {}

    // 添加比较操作符
    bool operator==(const UserInfo& other) const {
        return userId == other.userId;
    }

    bool operator!=(const UserInfo& other) const {
        return !(*this == other);
    }
};

// 评论
struct Comment {
    QString commentId;       // 评论ID
    UserInfo author;         // 评论作者
    QString content;         // 评论内容
    QDateTime timestamp;     // 评论时间
    int likesCount;          // 点赞数
    bool isLiked;           // 当前用户是否点赞

    Comment()
        : commentId(""), content(""),
        likesCount(0), isLiked(false) {}
};

// 视频帖子
struct VideoPost {
    QString postId;              // 帖子ID
    UserInfo author;             // 作者信息
    QUrl videoUrl;              // 视频URL
    QPixmap thumbnail;          // 缩略图
    QString caption;            // 标题/描述
    QDateTime timestamp;        // 发布时间
    int likesCount;             // 点赞数
    int commentsCount;          // 评论数
    int viewsCount;             // 观看数
    bool isLiked;              // 当前用户是否点赞
    QVector<Comment> comments;  // 评论列表
    QVector<QString> tags;      // 标签
    bool isFrontCamera;         // 是否前置摄像头
    bool isBeRealMoment;       // 是否是BeReal时刻

    VideoPost()
        : postId(""), caption(""),
        likesCount(0), commentsCount(0), viewsCount(0),
        isLiked(false), isFrontCamera(false), isBeRealMoment(false) {}
};

// 社交过滤类型
enum SocialFilter {
    AllPosts,        // 全部
    HotPosts,        // 热门
    FriendsPosts     // 好友
};

// 快捷评论预设
namespace QuickComments {
const QStringList REACTIONS = {
    "❤️ Amazing!",
    "🔥 So cool!",
    "😂 Haha",
    "😍 Love it",
    "👏 Great!",
    "💯 Perfect",
    "⭐ Nice",
    "🎉 Awesome"
};
}

// 每日提醒设置
struct DailyReminderSettings {
    bool enabled;            // 是否启用
    QTime reminderTime;      // 提醒时间
    bool randomTime;         // 是否随机时间
    int timeWindow;          // 时间窗口（分钟）
    bool soundEnabled;       // 是否启用声音
    bool vibrationEnabled;   // 是否启用震动

    DailyReminderSettings()
        : enabled(true),
        reminderTime(QTime(12, 0)),
        randomTime(true),
        timeWindow(120),
        soundEnabled(true),
        vibrationEnabled(true) {}
};

#endif // SOCIAL_TYPES_H
