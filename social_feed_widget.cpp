//
// SocialFeedWidget - 实现
//

#include "social_feed_widget.h"
#include "social_manager.h"
#include "design_system.h"
#include "share_dialog.h"
#include <QDebug>

SocialFeedWidget::SocialFeedWidget(QWidget* parent)
    : QWidget(parent),
    currentFilter(AllPosts) {

    setupUI();
    connectSignals();
    applyStyles();
    loadPosts();
}

void SocialFeedWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // === 顶部：过滤标签栏 ===
    QWidget* filterWidget = new QWidget(this);
    filterWidget->setFixedHeight(60);
    QHBoxLayout* filterLayout = new QHBoxLayout(filterWidget);
    filterLayout->setContentsMargins(16, 8, 16, 8);
    filterLayout->setSpacing(12);

    filterButtonGroup = new QButtonGroup(this);
    filterButtonGroup->setExclusive(true);

    // 全部按钮
    allBtn = new QPushButton(tr("All"), filterWidget);
    allBtn->setCheckable(true);
    allBtn->setChecked(true);
    allBtn->setFixedHeight(40);
    allBtn->setMinimumWidth(100);
    allBtn->setCursor(Qt::PointingHandCursor);
    filterButtonGroup->addButton(allBtn, 0);
    filterLayout->addWidget(allBtn);

    // 热门按钮
    hotBtn = new QPushButton(tr("Hot"), filterWidget);
    hotBtn->setCheckable(true);
    hotBtn->setFixedHeight(40);
    hotBtn->setMinimumWidth(100);
    hotBtn->setCursor(Qt::PointingHandCursor);
    filterButtonGroup->addButton(hotBtn, 1);
    filterLayout->addWidget(hotBtn);

    // 好友按钮
    friendsBtn = new QPushButton(tr("Friends"), filterWidget);
    friendsBtn->setCheckable(true);
    friendsBtn->setFixedHeight(40);
    friendsBtn->setMinimumWidth(100);
    friendsBtn->setCursor(Qt::PointingHandCursor);
    filterButtonGroup->addButton(friendsBtn, 2);
    filterLayout->addWidget(friendsBtn);

    filterLayout->addStretch();

    mainLayout->addWidget(filterWidget);

    // 添加分隔线
    QFrame* separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFixedHeight(1);
    mainLayout->addWidget(separator);

    // === 中间：滚动区域 ===
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    contentWidget = new QWidget();
    contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(16, 16, 16, 16);
    contentLayout->setSpacing(16);
    contentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    // 空状态标签
    emptyStateLabel = new QLabel(tr("No content available"), contentWidget);
    emptyStateLabel->setFont(DesignSystem::Typography::getTitle());
    emptyStateLabel->setAlignment(Qt::AlignCenter);
    emptyStateLabel->hide();
    contentLayout->addWidget(emptyStateLabel);

    scrollArea->setWidget(contentWidget);
    mainLayout->addWidget(scrollArea);
}

void SocialFeedWidget::connectSignals() {
    // 修复：使用 idClicked 信号而不是 buttonClicked
    connect(filterButtonGroup,
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
            &QButtonGroup::idClicked,
#else
            static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
#endif
            this, &SocialFeedWidget::onFilterChanged);
}

void SocialFeedWidget::applyStyles() {
    // 过滤按钮样式
    QString filterButtonStyle = QString(R"(
        QPushButton {
            background-color: transparent;
            border: 2px solid %1;
            border-radius: 20px;
            padding: 8px 16px;
            color: %2;
            font-weight: bold;
        }
        QPushButton:checked {
            background-color: %3;
            border-color: %3;
            color: white;
        }
        QPushButton:hover {
            background-color: %4;
        }
    )").arg(DesignSystem::Colors::getBorder().name())
                                    .arg(DesignSystem::Colors::getTextPrimary().name())
                                    .arg(DesignSystem::Colors::getPrimary().name())
                                    .arg(DesignSystem::Colors::getHoverOverlay().name());

    allBtn->setStyleSheet(filterButtonStyle);
    hotBtn->setStyleSheet(filterButtonStyle);
    friendsBtn->setStyleSheet(filterButtonStyle);

    // 滚动区域样式
    scrollArea->setStyleSheet(QString(R"(
        QScrollArea {
            background-color: %1;
            border: none;
        }
    )").arg(DesignSystem::Colors::getBackground().name()));

    contentWidget->setStyleSheet(QString("background-color: %1;")
                                     .arg(DesignSystem::Colors::getBackground().name()));

    // 空状态标签
    emptyStateLabel->setStyleSheet(QString("color: %1;")
                                       .arg(DesignSystem::Colors::getTextSecondary().name()));
}

void SocialFeedWidget::loadPosts() {
    clearPosts();

    SocialManager* socialManager = SocialManager::getInstance();
    QVector<VideoPost> posts;

    // 根据过滤类型加载帖子
    switch (currentFilter) {
    case AllPosts:
        posts = socialManager->getAllPosts();
        break;
    case HotPosts:
        posts = socialManager->getHotPosts();
        break;
    case FriendsPosts:
        posts = socialManager->getFriendsPosts();
        break;
    }

    if (posts.isEmpty()) {
        QString message;
        if (currentFilter == FriendsPosts) {
            message = tr("Your friends haven't posted any videos yet.\nAdd more friends!");
        } else {
            message = tr("No content available");
        }
        showEmptyState(message);
        return;
    }

    emptyStateLabel->hide();

    // 创建帖子卡片
    for (const VideoPost& post : posts) {
        VideoPostCard* card = new VideoPostCard(post, contentWidget);

        // 连接信号
        connect(card, &VideoPostCard::playRequested,
                this, &SocialFeedWidget::onPostPlayRequested);
        connect(card, &VideoPostCard::likeToggled,
                this, &SocialFeedWidget::onPostLikeToggled);
        connect(card, &VideoPostCard::commentRequested,
                this, &SocialFeedWidget::onPostCommentRequested);
        connect(card, &VideoPostCard::shareRequested,
                this, &SocialFeedWidget::onPostShareRequested);

        contentLayout->addWidget(card, 0, Qt::AlignHCenter);
        postCards.append(card);
    }

    contentLayout->addStretch();

    qDebug() << "Loaded" << posts.size() << "posts for filter:" << currentFilter;
}

void SocialFeedWidget::clearPosts() {
    // 删除所有帖子卡片
    for (VideoPostCard* card : postCards) {
        contentLayout->removeWidget(card);
        card->deleteLater();
    }
    postCards.clear();
}

void SocialFeedWidget::showEmptyState(const QString& message) {
    emptyStateLabel->setText(message);
    emptyStateLabel->show();
}

void SocialFeedWidget::setFilter(SocialFilter filter) {
    if (currentFilter != filter) {
        currentFilter = filter;
        loadPosts();
    }
}

void SocialFeedWidget::refreshFeed() {
    loadPosts();
    qDebug() << "Feed refreshed";
}

void SocialFeedWidget::updateTheme() {
    applyStyles();

    // 更新所有帖子卡片
    for (VideoPostCard* card : postCards) {
        card->updateTheme();
    }

    update();
}

void SocialFeedWidget::onFilterChanged(int id) {
    qDebug() << "Filter changed to:" << id;

    switch (id) {
    case 0:
        setFilter(AllPosts);
        break;
    case 1:
        setFilter(HotPosts);
        break;
    case 2:
        setFilter(FriendsPosts);
        break;
    }
}

void SocialFeedWidget::onPostPlayRequested(const VideoPost& post) {
    qDebug() << "Play requested for post:" << post.postId;
    emit videoPlayRequested(post);
}

void SocialFeedWidget::onPostLikeToggled(const QString& postId, bool isLiked) {
    qDebug() << "Like toggled for post:" << postId << "isLiked:" << isLiked;
}

void SocialFeedWidget::onPostCommentRequested(const QString& postId) {
    qDebug() << "Comment requested for post:" << postId;
    emit commentDialogRequested(postId);
}

void SocialFeedWidget::onPostShareRequested(const QString& postId) {
    qDebug() << "Share requested for post:" << postId;

    // 查找对应的帖子
    SocialManager* socialManager = SocialManager::getInstance();
    VideoPost post = socialManager->getPost(postId);

    if (post.postId.isEmpty()) {
        qDebug() << "Post not found:" << postId;
        return;
    }

    // 创建并显示分享对话框
    ShareDialog* shareDialog = new ShareDialog(post, this);

    // 连接分享确认信号
    connect(shareDialog, &ShareDialog::shareConfirmed,
            this, &SocialFeedWidget::onShareConfirmed);

    shareDialog->exec();
    shareDialog->deleteLater();
}

void SocialFeedWidget::onShareConfirmed(const QString& postId, const QString& platform, const QString& comment) {
    qDebug() << "Share confirmed - Post:" << postId
             << "Platform:" << platform
             << "Comment:" << comment;

    // 这里可以添加实际的分享逻辑
    // 例如：记录分享次数，发送到服务器等
}
