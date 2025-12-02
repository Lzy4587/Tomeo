#include "main_container.h"
#include "design_system.h"
#include "settings_dialog.h" // [修复] 引入设置对话框
#include "comment_dialog.h"  // [修复] 引入评论对话框

#include <QDebug>
#include <QLabel>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>

MainContainer::MainContainer(std::vector<TheButtonInfo>& videos, QWidget* parent)
    : QWidget(parent), recordDialog(nullptr) {

    setupUI();

    // 初始化页面
    createVideosPage();   // Index 0: 本地视频
    createSocialPage();   // Index 1: 社交Feed
    createMessagesPage(); // Index 2: 消息
    createProfilePage();  // Index 3: 个人中心

    // 填充视频网格 (本地文件)
    if (gridLayout) {
        for (size_t i = 0; i < videos.size(); i++) {
            TheButton* button = new TheButton(gridWidget);
            button->init(&videos.at(i));
            connect(button, &TheButton::jumpTo, this, &MainContainer::onVideoSelected);
            allButtons.push_back(button);
            gridLayout->addWidget(button);
        }
    }

    // 初始化播放器内容
    if (player) {
        player->setContent(&allButtons, &videos);
    }

    // 默认显示 '视频' 页面
    contentStack->setCurrentIndex(0);
}

MainContainer::~MainContainer() {
    if (player) delete player;
}

void MainContainer::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 1. 顶部工具栏
    topToolbar = new TopToolbar(this);
    // [修复] 连接设置按钮点击信号
    connect(topToolbar, &TopToolbar::settingsClicked, this, &MainContainer::onSettingsClicked);

    mainLayout->addWidget(topToolbar);

    // 2. 内容堆栈
    contentStack = new QStackedWidget(this);
    mainLayout->addWidget(contentStack, 1);

    // 3. 底部导航栏
    bottomNav = new BottomNavigationBar(this);
    connect(bottomNav, &BottomNavigationBar::pageChanged,
            this, &MainContainer::onNavigationPageChanged);
    connect(bottomNav, &BottomNavigationBar::createRequested,
            this, &MainContainer::onCreateRequested);

    mainLayout->addWidget(bottomNav);
}

// === PAGE 1: 视频 ===
void MainContainer::createVideosPage() {
    videosPage = new QWidget(contentStack);
    QVBoxLayout* layout = new QVBoxLayout(videosPage);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    // A. 播放器
    playerContainer = new QWidget(videosPage);
    QVBoxLayout* playerLayout = new QVBoxLayout(playerContainer);
    playerLayout->setContentsMargins(0, 0, 0, 0);
    playerLayout->setSpacing(0);

    videoWidget = new QVideoWidget(playerContainer);
    videoWidget->setMinimumHeight(320);
    videoWidget->setStyleSheet("background-color: black;");
    playerLayout->addWidget(videoWidget);

    controls = new PlaybackControls(playerContainer);
    playerLayout->addWidget(controls);

    layout->addWidget(playerContainer);

    // B. 网格
    gridScrollArea = new QScrollArea(videosPage);
    gridScrollArea->setWidgetResizable(true);
    gridScrollArea->setFrameShape(QFrame::NoFrame);
    gridScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    gridWidget = new QWidget();
    gridLayout = new QGridLayout(gridWidget);
    gridLayout->setContentsMargins(10, 10, 10, 10);
    gridLayout->setSpacing(10);

    gridScrollArea->setWidget(gridWidget);
    layout->addWidget(gridScrollArea, 1);

    player = new ThePlayer();
    player->setVideoOutput(videoWidget);
    player->setControls(controls);

    videosPage->setStyleSheet(QString("background-color: %1;")
                                  .arg(DesignSystem::Colors::getBackground().name()));
    contentStack->addWidget(videosPage);
}

// === PAGE 2: 社交 ===
void MainContainer::createSocialPage() {
    SocialFeedWidget* feedWidget = new SocialFeedWidget(contentStack);

    // 连接播放信号
    connect(feedWidget, &SocialFeedWidget::videoPlayRequested,
            this, &MainContainer::onSocialPlayRequested);

    // [修复] 连接评论弹窗信号
    connect(feedWidget, &SocialFeedWidget::commentDialogRequested,
            this, &MainContainer::onCommentDialogRequested);

    socialPage = feedWidget;
    contentStack->addWidget(socialPage);
}

// === PAGE 3: 消息 ===
void MainContainer::createMessagesPage() {
    messagesPage = new QWidget(contentStack);
    QVBoxLayout* layout = new QVBoxLayout(messagesPage);
    layout->setAlignment(Qt::AlignCenter);

    QLabel* icon = new QLabel("💬", messagesPage);
    icon->setFont(QFont("Segoe UI Emoji", 64));
    layout->addWidget(icon, 0, Qt::AlignCenter);

    QLabel* text = new QLabel(tr("No New Messages"), messagesPage);
    text->setFont(DesignSystem::Typography::getTitle());
    layout->addWidget(text, 0, Qt::AlignCenter);

    messagesPage->setStyleSheet(QString("background-color: %1;")
                                    .arg(DesignSystem::Colors::getBackground().name()));
    contentStack->addWidget(messagesPage);
}

// === PAGE 4: 我 ===
void MainContainer::createProfilePage() {
    profilePage = new QWidget(contentStack);
    QVBoxLayout* layout = new QVBoxLayout(profilePage);
    layout->setAlignment(Qt::AlignTop);
    layout->setContentsMargins(20, 40, 20, 20);
    layout->setSpacing(20);

    UserInfo user = SocialManager::getInstance()->getCurrentUser();

    QLabel* avatar = new QLabel("👤", profilePage);
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setFont(QFont("Segoe UI Emoji", 80));
    layout->addWidget(avatar);

    QLabel* name = new QLabel(user.username, profilePage);
    name->setAlignment(Qt::AlignCenter);
    name->setFont(DesignSystem::Typography::getHeadline());
    layout->addWidget(name);

    QWidget* statsWidget = new QWidget(profilePage);
    QHBoxLayout* statsLayout = new QHBoxLayout(statsWidget);
    statsLayout->setAlignment(Qt::AlignCenter);
    statsLayout->setSpacing(40);

    auto addStat = [&](const QString& label, int count) {
        QWidget* s = new QWidget();
        QVBoxLayout* sl = new QVBoxLayout(s);
        QLabel* num = new QLabel(QString::number(count));
        num->setFont(DesignSystem::Typography::getTitle());
        num->setAlignment(Qt::AlignCenter);
        QLabel* txt = new QLabel(label);
        txt->setFont(DesignSystem::Typography::getCaption());
        txt->setAlignment(Qt::AlignCenter);
        sl->addWidget(num);
        sl->addWidget(txt);
        statsLayout->addWidget(s);
    };

    addStat(tr("Following"), user.followingCount);
    addStat(tr("Followers"), user.followersCount);
    addStat(tr("Likes"), 1024);

    layout->addWidget(statsWidget);
    layout->addStretch();

    profilePage->setStyleSheet(QString("background-color: %1;")
                                   .arg(DesignSystem::Colors::getBackground().name()));
    contentStack->addWidget(profilePage);
}

// === 逻辑处理 ===

void MainContainer::onNavigationPageChanged(BottomNavigationBar::NavigationPage page) {
    switch (page) {
    case BottomNavigationBar::HomePage:
        contentStack->setCurrentIndex(0);
        break;
    case BottomNavigationBar::ExplorePage:
        contentStack->setCurrentIndex(1);
        player->pause();
        break;
    case BottomNavigationBar::MessagesPage:
        contentStack->setCurrentIndex(2);
        player->pause();
        break;
    case BottomNavigationBar::ProfilePage:
        contentStack->setCurrentIndex(3);
        player->pause();
        break;
    default: break;
    }
}

void MainContainer::onCreateRequested() {
    if (!recordDialog) {
        recordDialog = new RecordDialog(this);
        connect(recordDialog, &RecordDialog::videoRecorded, this, &MainContainer::onVideoRecorded);
    }
    recordDialog->exec();
}

void MainContainer::onVideoRecorded(const QString& videoPath, bool isFrontCamera) {
    QMessageBox::information(this, tr("Upload Complete"),
                             tr("Video recorded to: %1").arg(videoPath));
}

void MainContainer::onVideoSelected(TheButtonInfo* info) {
    player->jumpTo(info);
    playerContainer->show();
    bottomNav->setCurrentPage(BottomNavigationBar::HomePage);
    player->play();
}

void MainContainer::onSocialPlayRequested(const VideoPost& post) {
    if (!post.videoUrl.isEmpty()) {
        bottomNav->setCurrentPage(BottomNavigationBar::HomePage);
        contentStack->setCurrentIndex(0);
        playerContainer->show();

        player->setMedia(post.videoUrl);
        player->play();
    } else {
        QMessageBox::information(this, tr("Error"), tr("Video file not found for this post."));
    }
}

// [修复] 实现设置按钮点击槽函数
void MainContainer::onSettingsClicked() {
    SettingsDialog* settingsDialog = new SettingsDialog(this);
    // 设置更改后刷新UI
    connect(settingsDialog, &SettingsDialog::settingsApplied,
            this, &MainContainer::updateTheme);

    settingsDialog->exec();
    settingsDialog->deleteLater();
}

// [修复] 实现评论对话框槽函数
void MainContainer::onCommentDialogRequested(const QString& postId) {
    CommentDialog* dialog = new CommentDialog(postId, this);
    dialog->exec();
    dialog->deleteLater();
}

void MainContainer::updateResponsiveLayout(int windowWidth) {
    if (allButtons.empty() || !gridLayout) return;

    int columns = DesignSystem::Dimensions::getGridColumns(windowWidth);

    QLayoutItem* item;
    while ((item = gridLayout->takeAt(0)) != nullptr) {
        delete item;
    }

    for (size_t i = 0; i < allButtons.size(); i++) {
        int row = i / columns;
        int col = i % columns;
        allButtons[i]->setResponsiveSize(windowWidth);
        gridLayout->addWidget(allButtons[i], row, col);
    }
}

void MainContainer::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    updateResponsiveLayout(event->size().width());
}

void MainContainer::updateTheme() {
    topToolbar->applyStyles();
    bottomNav->updateTheme();

    QString bg = DesignSystem::Colors::getBackground().name();
    videosPage->setStyleSheet("background-color: " + bg);

    if (socialPage) socialPage->setStyleSheet("background-color: " + bg);
    if (messagesPage) messagesPage->setStyleSheet("background-color: " + bg);
    if (profilePage) profilePage->setStyleSheet("background-color: " + bg);

    for (TheButton* btn : allButtons) {
        btn->updateTheme();
    }

    if (SocialFeedWidget* feed = qobject_cast<SocialFeedWidget*>(socialPage)) {
        feed->updateTheme();
    }
}
