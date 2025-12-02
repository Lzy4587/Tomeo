//
// CommentDialog - 实现
//

#include "comment_dialog.h"
#include "social_manager.h"
#include "design_system.h"
#include <QHBoxLayout>
#include <QDateTime>
#include <QDebug>

CommentDialog::CommentDialog(const QString& postId, QWidget* parent)
    : QDialog(parent),
    postId(postId) {

    // 获取帖子数据
    post = SocialManager::getInstance()->getPost(postId);

    setupUI();
    setupQuickComments();
    connectSignals();
    applyStyles();
    loadComments();

    setMinimumSize(500, 600);
    setWindowTitle(tr("Comments"));
}

void CommentDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(16);

    // === 标题 ===
    titleLabel = new QLabel(tr("Comments (%1)").arg(post.commentsCount), this);
    titleLabel->setFont(DesignSystem::Typography::getTitle());
    mainLayout->addWidget(titleLabel);

    // === 快捷评论 ===
    quickCommentsWidget = new QWidget(this);
    QVBoxLayout* quickLayout = new QVBoxLayout(quickCommentsWidget);
    quickLayout->setContentsMargins(0, 0, 0, 0);
    quickLayout->setSpacing(8);

    QLabel* quickLabel = new QLabel(tr("Quick Comments:"), quickCommentsWidget);
    quickLabel->setFont(DesignSystem::Typography::getCaption());
    quickLayout->addWidget(quickLabel);

    // 快捷评论按钮容器
    QWidget* quickButtonsWidget = new QWidget(quickCommentsWidget);
    QHBoxLayout* quickButtonsLayout = new QHBoxLayout(quickButtonsWidget);
    quickButtonsLayout->setContentsMargins(0, 0, 0, 0);
    quickButtonsLayout->setSpacing(8);
    quickLayout->addWidget(quickButtonsWidget);

    mainLayout->addWidget(quickCommentsWidget);

    // === 评论列表 ===
    commentsScrollArea = new QScrollArea(this);
    commentsScrollArea->setWidgetResizable(true);
    commentsScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    commentsWidget = new QWidget();
    commentsLayout = new QVBoxLayout(commentsWidget);
    commentsLayout->setContentsMargins(0, 0, 0, 0);
    commentsLayout->setSpacing(12);
    commentsLayout->setAlignment(Qt::AlignTop);

    commentsScrollArea->setWidget(commentsWidget);
    mainLayout->addWidget(commentsScrollArea, 1);

    // === 输入区域 ===
    QWidget* inputWidget = new QWidget(this);
    QHBoxLayout* inputLayout = new QHBoxLayout(inputWidget);
    inputLayout->setContentsMargins(0, 0, 0, 0);
    inputLayout->setSpacing(8);

    commentInput = new QLineEdit(inputWidget);
    commentInput->setPlaceholderText(tr("Add a comment..."));
    commentInput->setMinimumHeight(40);
    inputLayout->addWidget(commentInput, 1);

    sendBtn = new QPushButton(tr("Send"), inputWidget);
    sendBtn->setFixedSize(80, 40);
    sendBtn->setCursor(Qt::PointingHandCursor);
    inputLayout->addWidget(sendBtn);

    mainLayout->addWidget(inputWidget);
}

void CommentDialog::setupQuickComments() {
    // 获取快捷评论容器
    QWidget* quickButtonsWidget = quickCommentsWidget->findChild<QWidget*>();
    if (!quickButtonsWidget) return;

    QHBoxLayout* quickButtonsLayout = qobject_cast<QHBoxLayout*>(
        quickButtonsWidget->layout());
    if (!quickButtonsLayout) return;

    // 创建快捷评论按钮
    for (int i = 0; i < 4 && i < QuickComments::REACTIONS.size(); i++) {
        QPushButton* btn = new QPushButton(QuickComments::REACTIONS[i],
                                           quickButtonsWidget);
        btn->setFixedHeight(36);
        btn->setCursor(Qt::PointingHandCursor);

        connect(btn, &QPushButton::clicked,
                this, &CommentDialog::onQuickCommentClicked);

        quickButtonsLayout->addWidget(btn);
        quickCommentButtons.append(btn);
    }

    quickButtonsLayout->addStretch();
}

void CommentDialog::connectSignals() {
    connect(sendBtn, &QPushButton::clicked,
            this, &CommentDialog::onSendClicked);
    connect(commentInput, &QLineEdit::returnPressed,
            this, &CommentDialog::onSendClicked);
}

void CommentDialog::applyStyles() {
    // 对话框背景
    setStyleSheet(QString(R"(
        QDialog {
            background-color: %1;
        }
    )").arg(DesignSystem::Colors::getBackground().name()));

    // 标题
    titleLabel->setStyleSheet(QString("color: %1; font-weight: bold;")
                                  .arg(DesignSystem::Colors::getTextPrimary().name()));

    // 快捷评论标签
    QLabel* quickLabel = quickCommentsWidget->findChild<QLabel*>();
    if (quickLabel) {
        quickLabel->setStyleSheet(QString("color: %1;")
                                      .arg(DesignSystem::Colors::getTextSecondary().name()));
    }

    // 快捷评论按钮
    QString quickButtonStyle = QString(R"(
        QPushButton {
            background-color: %1;
            border: 1px solid %2;
            border-radius: 18px;
            padding: 6px 12px;
            color: %3;
        }
        QPushButton:hover {
            background-color: %4;
            border-color: %5;
        }
    )").arg(DesignSystem::Colors::getCardBackground().name())
                                   .arg(DesignSystem::Colors::getBorder().name())
                                   .arg(DesignSystem::Colors::getTextPrimary().name())
                                   .arg(DesignSystem::Colors::getHoverOverlay().name())
                                   .arg(DesignSystem::Colors::getPrimary().name());

    for (QPushButton* btn : quickCommentButtons) {
        btn->setStyleSheet(quickButtonStyle);
    }

    // 滚动区域
    commentsScrollArea->setStyleSheet(QString(R"(
        QScrollArea {
            background-color: %1;
            border: 1px solid %2;
            border-radius: 8px;
        }
    )").arg(DesignSystem::Colors::getBackground().name())
                                          .arg(DesignSystem::Colors::getDivider().name()));

    commentsWidget->setStyleSheet(QString("background-color: %1;")
                                      .arg(DesignSystem::Colors::getBackground().name()));

    // 输入框
    commentInput->setStyleSheet(QString(R"(
        QLineEdit {
            background-color: %1;
            border: 2px solid %2;
            border-radius: 20px;
            padding: 8px 16px;
            color: %3;
        }
        QLineEdit:focus {
            border-color: %4;
        }
    )").arg(DesignSystem::Colors::getCardBackground().name())
                                    .arg(DesignSystem::Colors::getBorder().name())
                                    .arg(DesignSystem::Colors::getTextPrimary().name())
                                    .arg(DesignSystem::Colors::getPrimary().name()));

    // 发送按钮
    sendBtn->setStyleSheet(QString(R"(
        QPushButton {
            background-color: %1;
            border: none;
            border-radius: 20px;
            color: white;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: %2;
        }
        QPushButton:disabled {
            background-color: %3;
            color: %4;
        }
    )").arg(DesignSystem::Colors::getPrimary().name())
                               .arg(DesignSystem::Colors::getPrimaryDark().name())
                               .arg(DesignSystem::Colors::getDivider().name())
                               .arg(DesignSystem::Colors::getTextDisabled().name()));
}

void CommentDialog::loadComments() {
    // 清空现有评论
    QLayoutItem* item;
    while ((item = commentsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // 如果没有评论，显示提示
    if (post.comments.isEmpty()) {
        QLabel* emptyLabel = new QLabel(tr("No comments yet.\nBe the first to comment!"),
                                        commentsWidget);
        emptyLabel->setFont(DesignSystem::Typography::getBody());
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet(QString("color: %1; padding: 40px;")
                                      .arg(DesignSystem::Colors::getTextSecondary().name()));
        commentsLayout->addWidget(emptyLabel);
        return;
    }

    // 添加评论
    for (const Comment& comment : post.comments) {
        addCommentToUI(comment);
    }

    commentsLayout->addStretch();
}

void CommentDialog::addCommentToUI(const Comment& comment) {
    QWidget* commentWidget = new QWidget(commentsWidget);
    QVBoxLayout* layout = new QVBoxLayout(commentWidget);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(6);

    // 顶部：用户名和时间
    QWidget* headerWidget = new QWidget(commentWidget);
    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(8);

    QLabel* usernameLabel = new QLabel(comment.author.username, headerWidget);
    usernameLabel->setFont(DesignSystem::Typography::getSubtitle());
    usernameLabel->setStyleSheet(QString("color: %1; font-weight: bold;")
                                     .arg(DesignSystem::Colors::getTextPrimary().name()));
    headerLayout->addWidget(usernameLabel);

    // 时间
    qint64 seconds = comment.timestamp.secsTo(QDateTime::currentDateTime());
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

    QLabel* timeLabel = new QLabel(timeText, headerWidget);
    timeLabel->setFont(DesignSystem::Typography::getCaption());
    timeLabel->setStyleSheet(QString("color: %1;")
                                 .arg(DesignSystem::Colors::getTextSecondary().name()));
    headerLayout->addWidget(timeLabel);
    headerLayout->addStretch();

    layout->addWidget(headerWidget);

    // 评论内容
    QLabel* contentLabel = new QLabel(comment.content, commentWidget);
    contentLabel->setFont(DesignSystem::Typography::getBody());
    contentLabel->setWordWrap(true);
    contentLabel->setStyleSheet(QString("color: %1;")
                                    .arg(DesignSystem::Colors::getTextPrimary().name()));
    layout->addWidget(contentLabel);

    // 底部：点赞按钮
    QWidget* actionsWidget = new QWidget(commentWidget);
    QHBoxLayout* actionsLayout = new QHBoxLayout(actionsWidget);
    actionsLayout->setContentsMargins(0, 0, 0, 0);
    actionsLayout->setSpacing(4);

    QPushButton* likeBtn = new QPushButton(actionsWidget);
    likeBtn->setText(comment.isLiked ? "❤️" : "🤍");
    likeBtn->setFixedSize(32, 32);
    likeBtn->setCursor(Qt::PointingHandCursor);
    likeBtn->setProperty("commentId", comment.commentId);
    connect(likeBtn, &QPushButton::clicked,
            this, &CommentDialog::onCommentLikeClicked);
    actionsLayout->addWidget(likeBtn);

    QLabel* likesLabel = new QLabel(QString::number(comment.likesCount),
                                    actionsWidget);
    likesLabel->setFont(DesignSystem::Typography::getCaption());
    likesLabel->setStyleSheet(QString("color: %1;")
                                  .arg(DesignSystem::Colors::getTextSecondary().name()));
    actionsLayout->addWidget(likesLabel);
    actionsLayout->addStretch();

    layout->addWidget(actionsWidget);

    // 设置评论卡片样式
    commentWidget->setStyleSheet(QString(R"(
        QWidget {
            background-color: %1;
            border-radius: 8px;
        }
    )").arg(DesignSystem::Colors::getCardBackground().name()));

    commentsLayout->addWidget(commentWidget);
}

void CommentDialog::onSendClicked() {
    QString text = commentInput->text().trimmed();

    if (text.isEmpty()) {
        return;
    }

    // 创建新评论
    Comment comment;
    comment.commentId = QString("comment_%1_%2")
                            .arg(postId)
                            .arg(QDateTime::currentMSecsSinceEpoch());
    comment.author = SocialManager::getInstance()->getCurrentUser();
    comment.content = text;
    comment.timestamp = QDateTime::currentDateTime();
    comment.likesCount = 0;
    comment.isLiked = false;

    // 添加到帖子
    SocialManager::getInstance()->addComment(postId, comment);
    post.comments.append(comment);
    post.commentsCount++;

    // 更新UI
    addCommentToUI(comment);
    titleLabel->setText(tr("Comments (%1)").arg(post.commentsCount));

    // 清空输入框
    commentInput->clear();

    qDebug() << "Comment added:" << comment.content;
    emit commentAdded(postId, comment);
}

void CommentDialog::onQuickCommentClicked() {
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (btn) {
        commentInput->setText(btn->text());
        onSendClicked();
    }
}

void CommentDialog::onCommentLikeClicked() {
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    QString commentId = btn->property("commentId").toString();

    // 更新点赞状态
    SocialManager::getInstance()->likeComment(postId, commentId);

    // 查找对应的评论并更新UI
    for (Comment& comment : post.comments) {
        if (comment.commentId == commentId) {
            comment.isLiked = !comment.isLiked;
            btn->setText(comment.isLiked ? "❤️" : "🤍");

            // 更新点赞数标签
            QWidget* actionsWidget = btn->parentWidget();
            QLabel* likesLabel = actionsWidget->findChild<QLabel*>();
            if (likesLabel) {
                likesLabel->setText(QString::number(comment.likesCount));
            }
            break;
        }
    }

    qDebug() << "Comment like toggled:" << commentId;
}
