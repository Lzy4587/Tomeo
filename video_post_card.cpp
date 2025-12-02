//
// VideoPostCard - 实现（添加缩略图显示）
//

#include "video_post_card.h"
#include "design_system.h"
#include "social_manager.h"
#include <QDebug>

VideoPostCard::VideoPostCard(const VideoPost& postData, QWidget* parent)
    : QFrame(parent),
    post(postData) {

    setupUI();
    connectSignals();
    applyStyles();
    updateUI();
}

void VideoPostCard::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    // === 顶部：用户信息栏 ===
    QWidget* headerWidget = new QWidget(this);
    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(12);

    // 头像
    avatarLabel = new QLabel(headerWidget);
    avatarLabel->setFixedSize(40, 40);
    avatarLabel->setScaledContents(true);
    avatarLabel->setStyleSheet("border-radius: 20px; background-color: #BDBDBD;");
    headerLayout->addWidget(avatarLabel);

    // 用户名和时间
    QWidget* userInfoWidget = new QWidget(headerWidget);
    QVBoxLayout* userInfoLayout = new QVBoxLayout(userInfoWidget);
    userInfoLayout->setContentsMargins(0, 0, 0, 0);
    userInfoLayout->setSpacing(2);

    usernameLabel = new QLabel(headerWidget);
    usernameLabel->setFont(DesignSystem::Typography::getSubtitle());
    userInfoLayout->addWidget(usernameLabel);

    timeLabel = new QLabel(headerWidget);
    timeLabel->setFont(DesignSystem::Typography::getCaption());
    userInfoLayout->addWidget(timeLabel);

    headerLayout->addWidget(userInfoWidget);
    headerLayout->addStretch();

    // 更多按钮
    moreBtn = new QPushButton("⋮", headerWidget);
    moreBtn->setFixedSize(32, 32);
    moreBtn->setFont(QFont("Arial", 16, QFont::Bold));
    moreBtn->setCursor(Qt::PointingHandCursor);
    headerLayout->addWidget(moreBtn);

    mainLayout->addWidget(headerWidget);

    // === 中间：视频缩略图 ===
    QWidget* videoWidget = new QWidget(this);
    videoWidget->setFixedHeight(300);
    QVBoxLayout* videoLayout = new QVBoxLayout(videoWidget);
    videoLayout->setContentsMargins(0, 0, 0, 0);

    videoThumbnail = new QLabel(videoWidget);
    videoThumbnail->setFixedHeight(300);
    videoThumbnail->setScaledContents(true);
    videoThumbnail->setAlignment(Qt::AlignCenter);
    videoThumbnail->setStyleSheet("background-color: #424242; border-radius: 8px;");

    // 播放按钮覆盖层
    playBtn = new QPushButton("▶", videoThumbnail);
    playBtn->setFixedSize(60, 60);
    playBtn->setFont(QFont("Arial", 24));
    playBtn->setCursor(Qt::PointingHandCursor);
    playBtn->move(videoThumbnail->width()/2 - 30, videoThumbnail->height()/2 - 30);

    // BeReal标记
    beRealBadge = new QLabel("⏰ BeReal", videoThumbnail);
    beRealBadge->setFont(DesignSystem::Typography::getCaption());
    beRealBadge->setStyleSheet(R"(
        background-color: rgba(33, 150, 243, 0.9);
        color: white;
        padding: 4px 8px;
        border-radius: 4px;
    )");
    beRealBadge->adjustSize();
    beRealBadge->move(12, 12);
    beRealBadge->hide();

    videoLayout->addWidget(videoThumbnail);
    mainLayout->addWidget(videoWidget);

    // === 互动按钮栏 ===
    QWidget* actionsWidget = new QWidget(this);
    QHBoxLayout* actionsLayout = new QHBoxLayout(actionsWidget);
    actionsLayout->setContentsMargins(0, 0, 0, 0);
    actionsLayout->setSpacing(16);

    // 点赞按钮
    likeBtn = new QPushButton(actionsWidget);
    likeBtn->setFixedSize(40, 40);
    likeBtn->setFont(QFont("Arial", 18));
    likeBtn->setCursor(Qt::PointingHandCursor);
    actionsLayout->addWidget(likeBtn);

    likesLabel = new QLabel("0", actionsWidget);
    likesLabel->setFont(DesignSystem::Typography::getBody());
    actionsLayout->addWidget(likesLabel);

    actionsLayout->addSpacing(8);

    // 评论按钮
    commentBtn = new QPushButton("💬", actionsWidget);
    commentBtn->setFixedSize(40, 40);
    commentBtn->setFont(QFont("Arial", 18));
    commentBtn->setCursor(Qt::PointingHandCursor);
    actionsLayout->addWidget(commentBtn);

    commentsLabel = new QLabel("0", actionsWidget);
    commentsLabel->setFont(DesignSystem::Typography::getBody());
    actionsLayout->addWidget(commentsLabel);

    actionsLayout->addSpacing(8);

    // 分享按钮
    shareBtn = new QPushButton("🔗", actionsWidget);
    shareBtn->setFixedSize(40, 40);
    shareBtn->setFont(QFont("Arial", 18));
    shareBtn->setCursor(Qt::PointingHandCursor);
    actionsLayout->addWidget(shareBtn);

    actionsLayout->addStretch();

    // 观看数
    viewsLabel = new QLabel("👁 0", actionsWidget);
    viewsLabel->setFont(DesignSystem::Typography::getCaption());
    actionsLayout->addWidget(viewsLabel);

    mainLayout->addWidget(actionsWidget);

    // === 底部：标题描述 ===
    captionLabel = new QLabel(this);
    captionLabel->setFont(DesignSystem::Typography::getBody());
    captionLabel->setWordWrap(true);
    mainLayout->addWidget(captionLabel);

    // 设置frame样式
    setFrameShape(QFrame::Box);
    setFixedWidth(400);
}

void VideoPostCard::connectSignals() {
    connect(playBtn, &QPushButton::clicked, this, &VideoPostCard::onPlayClicked);
    connect(likeBtn, &QPushButton::clicked, this, &VideoPostCard::onLikeClicked);
    connect(commentBtn, &QPushButton::clicked, this, &VideoPostCard::onCommentClicked);
    connect(shareBtn, &QPushButton::clicked, this, &VideoPostCard::onShareClicked);
    connect(moreBtn, &QPushButton::clicked, this, &VideoPostCard::onMoreClicked);
}

void VideoPostCard::applyStyles() {
    // 卡片背景
    setStyleSheet(QString(R"(
        VideoPostCard {
            background-color: %1;
            border: 1px solid %2;
            border-radius: 12px;
        }
    )").arg(DesignSystem::Colors::getCardBackground().name())
                      .arg(DesignSystem::Colors::getDivider().name()));

    // 用户名
    usernameLabel->setStyleSheet(QString("color: %1; font-weight: bold;")
                                     .arg(DesignSystem::Colors::getTextPrimary().name()));

    // 时间
    timeLabel->setStyleSheet(QString("color: %1;")
                                 .arg(DesignSystem::Colors::getTextSecondary().name()));

    // 按钮样式
    QString buttonStyle = QString(R"(
        QPushButton {
            background-color: transparent;
            border: none;
            border-radius: 20px;
        }
        QPushButton:hover {
            background-color: %1;
        }
    )").arg(DesignSystem::Colors::getHoverOverlay().name());

    playBtn->setStyleSheet(buttonStyle + QString(R"(
        QPushButton {
            background-color: rgba(0, 0, 0, 0.5);
            color: white;
            border-radius: 30px;
        }
        QPushButton:hover {
            background-color: rgba(0, 0, 0, 0.7);
        }
    )"));

    likeBtn->setStyleSheet(buttonStyle);
    commentBtn->setStyleSheet(buttonStyle);
    shareBtn->setStyleSheet(buttonStyle);
    moreBtn->setStyleSheet(buttonStyle);

    // 标签
    likesLabel->setStyleSheet(QString("color: %1;")
                                  .arg(DesignSystem::Colors::getTextPrimary().name()));
    commentsLabel->setStyleSheet(QString("color: %1;")
                                     .arg(DesignSystem::Colors::getTextPrimary().name()));
    viewsLabel->setStyleSheet(QString("color: %1;")
                                  .arg(DesignSystem::Colors::getTextSecondary().name()));
    captionLabel->setStyleSheet(QString("color: %1;")
                                    .arg(DesignSystem::Colors::getTextPrimary().name()));
}

void VideoPostCard::updateUI() {
    // 更新用户信息
    usernameLabel->setText(post.author.username);

    // 计算时间差
    QDateTime now = QDateTime::currentDateTime();
    qint64 seconds = post.timestamp.secsTo(now);
    QString timeText;
    if (seconds < 60) {
        timeText = tr("%1s ago").arg(seconds);
    } else if (seconds < 3600) {
        timeText = tr("%1m ago").arg(seconds / 60);
    } else if (seconds < 86400) {
        timeText = tr("%1h ago").arg(seconds / 3600);
    } else {
        timeText = tr("%1d ago").arg(seconds / 86400);
    }
    timeLabel->setText(timeText);

    // 更新标题
    captionLabel->setText(post.caption);

    // 更新统计数据
    likesLabel->setText(QString::number(post.likesCount));
    commentsLabel->setText(QString::number(post.commentsCount));
    viewsLabel->setText(QString("👁 %1").arg(post.viewsCount));

    // 更新点赞按钮
    updateLikeButton();

    // 🔥 显示视频缩略图
    if (!post.thumbnail.isNull()) {
        videoThumbnail->setPixmap(post.thumbnail);
        qDebug() << "Set thumbnail for post:" << post.postId;
    } else {
        // 如果没有缩略图，显示默认图标
        videoThumbnail->setText("📹");
        videoThumbnail->setAlignment(Qt::AlignCenter);
        videoThumbnail->setStyleSheet(QString(R"(
            background-color: %1;
            border-radius: 8px;
            font-size: 48px;
            color: %2;
        )").arg(DesignSystem::Colors::getCardBackground().name())
                                          .arg(DesignSystem::Colors::getTextSecondary().name()));
        qDebug() << "No thumbnail for post:" << post.postId;
    }

    // 显示BeReal标记
    if (post.isBeRealMoment) {
        beRealBadge->show();
    } else {
        beRealBadge->hide();
    }
}

void VideoPostCard::updateLikeButton() {
    if (post.isLiked) {
        likeBtn->setText("❤️");
    } else {
        likeBtn->setText("🤍");
    }
}

void VideoPostCard::setPost(const VideoPost& postData) {
    post = postData;
    updateUI();
}

void VideoPostCard::updateTheme() {
    applyStyles();
    update();
}

void VideoPostCard::enterEvent(QEvent* event) {
    QFrame::enterEvent(event);
    setStyleSheet(styleSheet() + QString(R"(
        VideoPostCard {
            border: 2px solid %1;
        }
    )").arg(DesignSystem::Colors::getPrimary().name()));
}

void VideoPostCard::leaveEvent(QEvent* event) {
    QFrame::leaveEvent(event);
    applyStyles();
}

void VideoPostCard::onPlayClicked() {
    qDebug() << "Play clicked for post:" << post.postId;
    emit playRequested(post);
}

void VideoPostCard::onLikeClicked() {
    post.isLiked = !post.isLiked;

    if (post.isLiked) {
        post.likesCount++;
        SocialManager::getInstance()->likePost(post.postId);
    } else {
        post.likesCount--;
        SocialManager::getInstance()->unlikePost(post.postId);
    }

    updateLikeButton();
    likesLabel->setText(QString::number(post.likesCount));

    qDebug() << "Like toggled for post:" << post.postId << "isLiked:" << post.isLiked;
    emit likeToggled(post.postId, post.isLiked);
}

void VideoPostCard::onCommentClicked() {
    qDebug() << "Comment clicked for post:" << post.postId;
    emit commentRequested(post.postId);
}

void VideoPostCard::onShareClicked() {
    qDebug() << "Share clicked for post:" << post.postId;
    emit shareRequested(post.postId);
}

void VideoPostCard::onMoreClicked() {
    qDebug() << "More clicked for post:" << post.postId;
    // TODO: 显示更多选项菜单
}
