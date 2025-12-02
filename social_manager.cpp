//
// SocialManager - 实现（添加真实缩略图支持）
//

#include "social_manager.h"
#include <QDebug>
#include <QRandomGenerator>
#include <QDir>
#include <QFileInfo>
#include <QImageReader>
#include <algorithm>

SocialManager* SocialManager::instance = nullptr;

SocialManager::SocialManager(QObject* parent)
    : QObject(parent),
    settings(nullptr) {

    settings = new QSettings("BeRealVideo", "Social", this);

    // 设置当前用户（模拟）
    currentUser.userId = "user001";
    currentUser.username = "@YourName";
    currentUser.displayName = "Your Name";
    currentUser.bio = "📸 Recording life moments";
    currentUser.followersCount = 245;
    currentUser.followingCount = 189;

    // 生成模拟数据
    generateMockData();

    qDebug() << "SocialManager initialized";
}

SocialManager* SocialManager::getInstance() {
    if (instance == nullptr) {
        instance = new SocialManager();
    }
    return instance;
}

// 🔥 新增：加载真实的视频缩略图
void SocialManager::loadRealThumbnails(const QString& videoDir) {
    qDebug() << "Loading real thumbnails from:" << videoDir;

    QDir dir(videoDir);
    if (!dir.exists()) {
        qDebug() << "Video directory does not exist:" << videoDir;
        return;
    }

    // 获取所有视频文件及其缩略图
    QStringList videoFilters;
    videoFilters << "*.mp4" << "*.mov" << "*.wmv" << "*.avi";
    QFileInfoList videoFiles = dir.entryInfoList(videoFilters, QDir::Files);

    if (videoFiles.isEmpty()) {
        qDebug() << "No video files found in:" << videoDir;
        return;
    }

    qDebug() << "Found" << videoFiles.size() << "video files";

    // 为每个帖子尝试加载对应的缩略图
    int postIndex = 0;
    for (const QFileInfo& videoFile : videoFiles) {
        if (postIndex >= allPosts.size()) {
            break;  // 已经有足够的帖子了
        }

        QString thumbnailPath = videoFile.absolutePath() + "/" +
                                videoFile.completeBaseName() + ".png";

        if (QFile::exists(thumbnailPath)) {
            QImageReader imageReader(thumbnailPath);
            QImage image = imageReader.read();

            if (!image.isNull()) {
                allPosts[postIndex].thumbnail = QPixmap::fromImage(image);
                allPosts[postIndex].videoUrl = QUrl::fromLocalFile(videoFile.absoluteFilePath());
                qDebug() << "Loaded thumbnail for post" << postIndex << ":" << thumbnailPath;
            } else {
                qDebug() << "Failed to load thumbnail:" << thumbnailPath;
            }
        } else {
            qDebug() << "Thumbnail not found:" << thumbnailPath;
        }

        postIndex++;
    }

    qDebug() << "Loaded thumbnails for" << postIndex << "posts";
}

void SocialManager::generateMockData() {
    // 生成模拟好友
    UserInfo friend1;
    friend1.userId = "user002";
    friend1.username = "@HandName";
    friend1.displayName = "HandName";
    friend1.bio = "🎬 Video creator";
    friend1.followersCount = 1024;
    friend1.followingCount = 432;
    friend1.isFriend = true;
    friends.append(friend1);

    UserInfo friend2;
    friend2.userId = "user003";
    friend2.username = "@anotherfriend";
    friend2.displayName = "Another Friend";
    friend2.bio = "✨ Life is beautiful";
    friend2.followersCount = 567;
    friend2.followingCount = 234;
    friend2.isFriend = true;
    friends.append(friend2);

    // 生成模拟帖子
    for (int i = 0; i < 10; i++) {
        VideoPost post;
        post.postId = QString("post_%1").arg(i);

        // 随机选择作者
        if (i % 3 == 0) {
            post.author = currentUser;
        } else if (i % 3 == 1) {
            post.author = friend1;
        } else {
            post.author = friend2;
        }

        post.caption = QString("This is video post #%1").arg(i + 1);
        post.timestamp = QDateTime::currentDateTime().addSecs(-i * 3600);
        post.likesCount = QRandomGenerator::global()->bounded(10, 100);
        post.commentsCount = QRandomGenerator::global()->bounded(0, 20);
        post.viewsCount = QRandomGenerator::global()->bounded(50, 500);
        post.isLiked = (i % 4 == 0);
        post.isBeRealMoment = (i % 5 == 0);

        // 添加一些评论
        if (i % 2 == 0) {
            Comment comment;
            comment.commentId = QString("comment_%1_1").arg(i);
            comment.author = friend1;
            comment.content = "Amazing! 👍";
            comment.timestamp = post.timestamp.addSecs(600);
            comment.likesCount = 5;
            post.comments.append(comment);
        }

        allPosts.append(post);
    }

    qDebug() << "Generated" << allPosts.size() << "mock posts";
}

void SocialManager::setCurrentUser(const UserInfo& user) {
    currentUser = user;
    qDebug() << "Current user set to:" << user.username;
}

void SocialManager::addFriend(const UserInfo& user) {
    // 修复：使用 userId 比较而不是直接比较 UserInfo
    bool alreadyExists = false;
    for (const UserInfo& existingFriend : friends) {
        if (existingFriend.userId == user.userId) {
            alreadyExists = true;
            break;
        }
    }

    if (!alreadyExists) {
        friends.append(user);
        emit friendAdded(user);
        qDebug() << "Friend added:" << user.username;
    }
}

void SocialManager::removeFriend(const QString& userId) {
    for (int i = 0; i < friends.size(); i++) {
        if (friends[i].userId == userId) {
            friends.removeAt(i);
            emit friendRemoved(userId);
            qDebug() << "Friend removed:" << userId;
            break;
        }
    }
}

QVector<VideoPost> SocialManager::getAllPosts() const {
    return allPosts;
}

QVector<VideoPost> SocialManager::getHotPosts() const {
    // 按点赞数排序返回热门帖子
    QVector<VideoPost> hotPosts = allPosts;
    std::sort(hotPosts.begin(), hotPosts.end(),
              [](const VideoPost& a, const VideoPost& b) {
                  return a.likesCount > b.likesCount;
              });
    return hotPosts;
}

QVector<VideoPost> SocialManager::getFriendsPosts() const {
    // 只返回好友的帖子
    QVector<VideoPost> friendsPosts;
    for (const VideoPost& post : allPosts) {
        for (const UserInfo& friendUser : friends) {
            if (post.author.userId == friendUser.userId) {
                friendsPosts.append(post);
                break;
            }
        }
    }
    return friendsPosts;
}

VideoPost SocialManager::getPost(const QString& postId) const {
    for (const VideoPost& post : allPosts) {
        if (post.postId == postId) {
            return post;
        }
    }
    return VideoPost();
}

void SocialManager::addPost(const VideoPost& post) {
    allPosts.prepend(post);  // 添加到开头
    emit postAdded(post);
    qDebug() << "Post added:" << post.postId;
}

void SocialManager::deletePost(const QString& postId) {
    for (int i = 0; i < allPosts.size(); i++) {
        if (allPosts[i].postId == postId) {
            allPosts.removeAt(i);
            emit postDeleted(postId);
            qDebug() << "Post deleted:" << postId;
            break;
        }
    }
}

void SocialManager::likePost(const QString& postId) {
    for (int i = 0; i < allPosts.size(); i++) {
        if (allPosts[i].postId == postId) {
            if (!allPosts[i].isLiked) {
                allPosts[i].isLiked = true;
                allPosts[i].likesCount++;
                emit postLiked(postId, true);
                qDebug() << "Post liked:" << postId;
            }
            break;
        }
    }
}

void SocialManager::unlikePost(const QString& postId) {
    for (int i = 0; i < allPosts.size(); i++) {
        if (allPosts[i].postId == postId) {
            if (allPosts[i].isLiked) {
                allPosts[i].isLiked = false;
                allPosts[i].likesCount--;
                emit postLiked(postId, false);
                qDebug() << "Post unliked:" << postId;
            }
            break;
        }
    }
}

void SocialManager::addComment(const QString& postId, const Comment& comment) {
    for (int i = 0; i < allPosts.size(); i++) {
        if (allPosts[i].postId == postId) {
            allPosts[i].comments.append(comment);
            allPosts[i].commentsCount++;
            emit commentAdded(postId, comment);
            qDebug() << "Comment added to post:" << postId;
            break;
        }
    }
}

void SocialManager::deleteComment(const QString& postId, const QString& commentId) {
    for (int i = 0; i < allPosts.size(); i++) {
        if (allPosts[i].postId == postId) {
            for (int j = 0; j < allPosts[i].comments.size(); j++) {
                if (allPosts[i].comments[j].commentId == commentId) {
                    allPosts[i].comments.removeAt(j);
                    allPosts[i].commentsCount--;
                    qDebug() << "Comment deleted:" << commentId;
                    break;
                }
            }
            break;
        }
    }
}

void SocialManager::likeComment(const QString& postId, const QString& commentId) {
    for (int i = 0; i < allPosts.size(); i++) {
        if (allPosts[i].postId == postId) {
            for (int j = 0; j < allPosts[i].comments.size(); j++) {
                if (allPosts[i].comments[j].commentId == commentId) {
                    if (!allPosts[i].comments[j].isLiked) {
                        allPosts[i].comments[j].isLiked = true;
                        allPosts[i].comments[j].likesCount++;
                        qDebug() << "Comment liked:" << commentId;
                    } else {
                        allPosts[i].comments[j].isLiked = false;
                        allPosts[i].comments[j].likesCount--;
                        qDebug() << "Comment unliked:" << commentId;
                    }
                    break;
                }
            }
            break;
        }
    }
}

void SocialManager::setReminderSettings(const DailyReminderSettings& settings) {
    reminderSettings = settings;
    qDebug() << "Reminder settings updated";
}

void SocialManager::saveData() {
    // TODO: 实现数据持久化
    qDebug() << "Saving social data...";
}

void SocialManager::loadData() {
    // TODO: 实现数据加载
    qDebug() << "Loading social data...";
}
