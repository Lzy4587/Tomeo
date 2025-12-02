//
// ShareDialog - 分享/转发对话框实现
//

#include "share_dialog.h"
#include "design_system.h"
#include <QDebug>
#include <QMessageBox>

ShareDialog::ShareDialog(const VideoPost& postData, QWidget* parent)
    : QDialog(parent),
    post(postData),
    selectedPlatform("") {

    setWindowTitle(tr("Share Video"));
    setModal(true);
    setMinimumWidth(500);
    setMinimumHeight(600);

    setupUI();
    connectSignals();
    applyStyles();
    updatePostPreview();
}

void ShareDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(DesignSystem::Dimensions::SpacingMedium);
    mainLayout->setContentsMargins(
        DesignSystem::Dimensions::SpacingLarge,
        DesignSystem::Dimensions::SpacingLarge,
        DesignSystem::Dimensions::SpacingLarge,
        DesignSystem::Dimensions::SpacingLarge
        );

    // === 标题 ===
    QLabel* titleLabel = new QLabel(tr("Share to"), this);
    titleLabel->setFont(DesignSystem::Typography::getTitle());
    mainLayout->addWidget(titleLabel);

    // === 帖子预览区域 ===
    QFrame* previewFrame = new QFrame(this);
    previewFrame->setFrameShape(QFrame::Box);
    QVBoxLayout* previewLayout = new QVBoxLayout(previewFrame);
    previewLayout->setSpacing(8);
    previewLayout->setContentsMargins(12, 12, 12, 12);

    // 作者信息
    authorLabel = new QLabel(previewFrame);
    authorLabel->setFont(DesignSystem::Typography::getSubtitle());
    previewLayout->addWidget(authorLabel);

    // 帖子内容预览
    postPreviewLabel = new QLabel(previewFrame);
    postPreviewLabel->setFixedHeight(200);
    postPreviewLabel->setScaledContents(true);
    postPreviewLabel->setAlignment(Qt::AlignCenter);
    previewLayout->addWidget(postPreviewLabel);

    // 标题
    captionLabel = new QLabel(previewFrame);
    captionLabel->setFont(DesignSystem::Typography::getBody());
    captionLabel->setWordWrap(true);
    captionLabel->setMaximumHeight(60);
    previewLayout->addWidget(captionLabel);

    mainLayout->addWidget(previewFrame);

    // === 分享平台选择 ===
    QLabel* platformLabel = new QLabel(tr("Select platform:"), this);
    platformLabel->setFont(DesignSystem::Typography::getSubtitle());
    mainLayout->addWidget(platformLabel);

    // 平台按钮网格
    QWidget* platformWidget = new QWidget(this);
    QGridLayout* platformLayout = new QGridLayout(platformWidget);
    platformLayout->setSpacing(12);

    platformGroup = new QButtonGroup(this);
    platformGroup->setExclusive(true);

    // Facebook
    facebookBtn = new QPushButton("📘 Facebook", platformWidget);
    facebookBtn->setCheckable(true);
    facebookBtn->setMinimumHeight(50);
    facebookBtn->setCursor(Qt::PointingHandCursor);
    platformGroup->addButton(facebookBtn, 0);
    platformLayout->addWidget(facebookBtn, 0, 0);

    // Twitter
    twitterBtn = new QPushButton("🐦 Twitter", platformWidget);
    twitterBtn->setCheckable(true);
    twitterBtn->setMinimumHeight(50);
    twitterBtn->setCursor(Qt::PointingHandCursor);
    platformGroup->addButton(twitterBtn, 1);
    platformLayout->addWidget(twitterBtn, 0, 1);

    // Instagram
    instagramBtn = new QPushButton("📷 Instagram", platformWidget);
    instagramBtn->setCheckable(true);
    instagramBtn->setMinimumHeight(50);
    instagramBtn->setCursor(Qt::PointingHandCursor);
    platformGroup->addButton(instagramBtn, 2);
    platformLayout->addWidget(instagramBtn, 1, 0);

    // WeChat
    wechatBtn = new QPushButton("💬 WeChat", platformWidget);
    wechatBtn->setCheckable(true);
    wechatBtn->setMinimumHeight(50);
    wechatBtn->setCursor(Qt::PointingHandCursor);
    platformGroup->addButton(wechatBtn, 3);
    platformLayout->addWidget(wechatBtn, 1, 1);

    // WhatsApp
    whatsappBtn = new QPushButton("📱 WhatsApp", platformWidget);
    whatsappBtn->setCheckable(true);
    whatsappBtn->setMinimumHeight(50);
    whatsappBtn->setCursor(Qt::PointingHandCursor);
    platformGroup->addButton(whatsappBtn, 4);
    platformLayout->addWidget(whatsappBtn, 2, 0);

    mainLayout->addWidget(platformWidget);

    // === 评论输入 ===
    QLabel* commentLabel = new QLabel(tr("Add a comment (optional):"), this);
    commentLabel->setFont(DesignSystem::Typography::getBody());
    mainLayout->addWidget(commentLabel);

    commentEdit = new QTextEdit(this);
    commentEdit->setPlaceholderText(tr("Write something about this video..."));
    commentEdit->setMaximumHeight(100);
    commentEdit->setFont(DesignSystem::Typography::getBody());
    mainLayout->addWidget(commentEdit);

    mainLayout->addStretch();

    // === 底部按钮 ===
    QWidget* buttonWidget = new QWidget(this);
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(DesignSystem::Dimensions::SpacingSmall);

    buttonLayout->addStretch();

    cancelButton = new QPushButton(tr("Cancel"), buttonWidget);
    cancelButton->setMinimumWidth(DesignSystem::Dimensions::ButtonMinWidth);
    cancelButton->setMinimumHeight(DesignSystem::Dimensions::ButtonHeight);
    cancelButton->setFont(DesignSystem::Typography::getButton());
    cancelButton->setCursor(Qt::PointingHandCursor);
    buttonLayout->addWidget(cancelButton);

    shareButton = new QPushButton(tr("Share"), buttonWidget);
    shareButton->setMinimumWidth(DesignSystem::Dimensions::ButtonMinWidth);
    shareButton->setMinimumHeight(DesignSystem::Dimensions::ButtonHeight);
    shareButton->setFont(DesignSystem::Typography::getButton());
    shareButton->setDefault(true);
    shareButton->setCursor(Qt::PointingHandCursor);
    shareButton->setEnabled(false); // 默认禁用，直到选择平台
    buttonLayout->addWidget(shareButton);

    mainLayout->addWidget(buttonWidget);
}

void ShareDialog::connectSignals() {
    // 平台选择
    connect(platformGroup,
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
            &QButtonGroup::idClicked,
#else
            static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
#endif
            this, &ShareDialog::onPlatformSelected);

    // 底部按钮
    connect(shareButton, &QPushButton::clicked,
            this, &ShareDialog::onShareClicked);
    connect(cancelButton, &QPushButton::clicked,
            this, &ShareDialog::onCancelClicked);
}

void ShareDialog::applyStyles() {
    // 对话框背景
    setStyleSheet(QString(R"(
        QDialog {
            background-color: %1;
        }
        QLabel {
            color: %2;
        }
        QFrame {
            background-color: %3;
            border: 1px solid %4;
            border-radius: %5px;
        }
        QTextEdit {
            background-color: %3;
            border: 1px solid %4;
            border-radius: %6px;
            padding: 8px;
            color: %2;
        }
        QTextEdit:focus {
            border: 2px solid %7;
        }
    )").arg(DesignSystem::Colors::getBackground().name())
                      .arg(DesignSystem::Colors::getTextPrimary().name())
                      .arg(DesignSystem::Colors::getSurface().name())
                      .arg(DesignSystem::Colors::getBorder().name())
                      .arg(DesignSystem::Dimensions::BorderRadius)
                      .arg(DesignSystem::Dimensions::BorderRadiusSmall)
                      .arg(DesignSystem::Colors::getPrimary().name()));

    // 平台按钮样式
    QString platformButtonStyle = QString(R"(
        QPushButton {
            background-color: %1;
            border: 2px solid %2;
            border-radius: %3px;
            color: %4;
            font-weight: bold;
            padding: 8px;
        }
        QPushButton:checked {
            background-color: %5;
            border-color: %5;
            color: white;
        }
        QPushButton:hover {
            background-color: %6;
        }
    )").arg(DesignSystem::Colors::getSurface().name())
                                      .arg(DesignSystem::Colors::getBorder().name())
                                      .arg(DesignSystem::Dimensions::BorderRadius)
                                      .arg(DesignSystem::Colors::getTextPrimary().name())
                                      .arg(DesignSystem::Colors::getPrimary().name())
                                      .arg(DesignSystem::Colors::getHoverOverlay().name());

    facebookBtn->setStyleSheet(platformButtonStyle);
    twitterBtn->setStyleSheet(platformButtonStyle);
    instagramBtn->setStyleSheet(platformButtonStyle);
    wechatBtn->setStyleSheet(platformButtonStyle);
    whatsappBtn->setStyleSheet(platformButtonStyle);

    // 分享按钮样式
    QString shareButtonStyle = QString(R"(
        QPushButton {
            background-color: %1;
            border: none;
            border-radius: %2px;
            color: white;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: %3;
        }
        QPushButton:pressed {
            background-color: %4;
        }
        QPushButton:disabled {
            background-color: %5;
            color: %6;
        }
    )").arg(DesignSystem::Colors::getPrimary().name())
                                   .arg(DesignSystem::Dimensions::BorderRadiusSmall)
                                   .arg(DesignSystem::Colors::getPrimaryDark().name())
                                   .arg(DesignSystem::Colors::getPrimaryDark().name())
                                   .arg(DesignSystem::Colors::getDivider().name())
                                   .arg(DesignSystem::Colors::getTextSecondary().name());

    shareButton->setStyleSheet(shareButtonStyle);

    // 取消按钮样式
    QString cancelButtonStyle = QString(R"(
        QPushButton {
            background-color: transparent;
            border: 2px solid %1;
            border-radius: %2px;
            color: %3;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: %4;
        }
    )").arg(DesignSystem::Colors::getBorder().name())
                                    .arg(DesignSystem::Dimensions::BorderRadiusSmall)
                                    .arg(DesignSystem::Colors::getTextPrimary().name())
                                    .arg(DesignSystem::Colors::getHoverOverlay().name());

    cancelButton->setStyleSheet(cancelButtonStyle);
}

void ShareDialog::updatePostPreview() {
    // 更新作者信息
    authorLabel->setText(QString("@%1").arg(post.author.username));

    // 更新缩略图
    if (!post.thumbnail.isNull()) {
        postPreviewLabel->setPixmap(post.thumbnail.scaled(
            480, 200,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            ));
    } else {
        postPreviewLabel->setText("📹");
        postPreviewLabel->setStyleSheet(QString(R"(
            background-color: %1;
            font-size: 48px;
            color: %2;
        )").arg(DesignSystem::Colors::getCardBackground().name())
                                            .arg(DesignSystem::Colors::getTextSecondary().name()));
    }

    // 更新标题
    QString caption = post.caption;
    if (caption.length() > 100) {
        caption = caption.left(97) + "...";
    }
    captionLabel->setText(caption);
}

void ShareDialog::onPlatformSelected(int id) {
    // 根据ID设置平台名称
    switch (id) {
    case 0:
        selectedPlatform = "Facebook";
        break;
    case 1:
        selectedPlatform = "Twitter";
        break;
    case 2:
        selectedPlatform = "Instagram";
        break;
    case 3:
        selectedPlatform = "WeChat";
        break;
    case 4:
        selectedPlatform = "WhatsApp";
        break;
    default:
        selectedPlatform = "";
    }

    // 启用分享按钮
    shareButton->setEnabled(!selectedPlatform.isEmpty());

    qDebug() << "Platform selected:" << selectedPlatform;
}

void ShareDialog::onShareClicked() {
    if (selectedPlatform.isEmpty()) {
        QMessageBox::warning(this, tr("No Platform Selected"),
                             tr("Please select a platform to share to."));
        return;
    }

    QString comment = commentEdit->toPlainText();

    qDebug() << "Sharing post" << post.postId
             << "to" << selectedPlatform
             << "with comment:" << comment;

    // 发射信号
    emit shareConfirmed(post.postId, selectedPlatform, comment);

    // 显示成功消息
    QMessageBox::information(this, tr("Share Successful"),
                             tr("Video shared to %1 successfully!").arg(selectedPlatform));

    accept();
}

void ShareDialog::onCancelClicked() {
    qDebug() << "Share cancelled";
    reject();
}
