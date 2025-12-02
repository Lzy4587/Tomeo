#include "bottom_navigation_bar.h"
#include "design_system.h"
#include <QGraphicsDropShadowEffect>
#include <QDebug>

BottomNavigationBar::BottomNavigationBar(QWidget* parent)
    : QWidget(parent),
    themeManager(ThemeManager::getInstance()),
    currentPage(HomePage),
    unreadCount(0) {

    // [修改 1] 增加高度，防止遮挡
    setFixedHeight(80);
    setupUI();
    connectSignals();
    applyStyles();
    updateButtonStates();
}

void BottomNavigationBar::setupUI() {
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    // [修改 2] 调整内边距，顶部留更多空间
    mainLayout->setContentsMargins(8, 6, 8, 4);
    mainLayout->setSpacing(0);

    // 1. 视频 (Videos)
    homeBtn = new QPushButton(this);
    homeLabel = new QLabel(tr("Videos"), this);
    mainLayout->addWidget(createNavigationItem(homeBtn, homeLabel, "🎥", tr("Videos")), 1);

    // 2. 社交 (Social)
    exploreBtn = new QPushButton(this);
    exploreLabel = new QLabel(tr("Social"), this);
    mainLayout->addWidget(createNavigationItem(exploreBtn, exploreLabel, "🔥", tr("Social")), 1);

    // 3. 创建按钮 (Create)
    createBtn = new QPushButton("+", this);
    createBtn->setFixedSize(54, 54);
    // 字体稍微改小一点点，防止溢出
    createBtn->setFont(QFont("Arial", 26, QFont::Bold));
    createBtn->setCursor(Qt::PointingHandCursor);

    // 阴影
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0,0,0,60));
    shadow->setOffset(0, 4);
    createBtn->setGraphicsEffect(shadow);

    QWidget* createWrapper = new QWidget(this);
    QVBoxLayout* cl = new QVBoxLayout(createWrapper);
    cl->setContentsMargins(0,0,0,10); // 底部留点空隙
    cl->setAlignment(Qt::AlignCenter);
    cl->addWidget(createBtn);
    mainLayout->addWidget(createWrapper, 1);

    // 4. 消息 (Messages)
    messagesBtn = new QPushButton(this);
    messagesLabel = new QLabel(tr("Messages"), this);
    QWidget* msgWidget = createNavigationItem(messagesBtn, messagesLabel, "💬", tr("Messages"));

    // 角标
    badgeLabel = new QLabel(messagesBtn);
    badgeLabel->setFixedSize(18, 18);
    badgeLabel->setAlignment(Qt::AlignCenter);
    badgeLabel->setStyleSheet("background-color: #FF4444; color: white; border-radius: 9px; font-size: 10px; font-weight: bold;");
    badgeLabel->hide();

    mainLayout->addWidget(msgWidget, 1);

    // 5. 我 (Profile)
    profileBtn = new QPushButton(this);
    profileLabel = new QLabel(tr("Me"), this);
    mainLayout->addWidget(createNavigationItem(profileBtn, profileLabel, "👤", tr("Me")), 1);
}

QWidget* BottomNavigationBar::createNavigationItem(QPushButton* btn, QLabel* lbl, const QString& icon, const QString& text) {
    QWidget* w = new QWidget(this);
    QVBoxLayout* l = new QVBoxLayout(w);
    // [修改 3] 调整垂直布局的间距
    l->setContentsMargins(0, 4, 0, 4);
    l->setSpacing(4); // 图标和文字之间的间距
    l->setAlignment(Qt::AlignCenter);

    btn->setText(icon);
    btn->setFixedSize(40,40); // 保持图标尺寸
    btn->setFont(QFont("Segoe UI Emoji", 20));
    btn->setCursor(Qt::PointingHandCursor);

    lbl->setText(text);
    lbl->setAlignment(Qt::AlignCenter);
    lbl->setFont(DesignSystem::Typography::getCaption());

    l->addWidget(btn);
    l->addWidget(lbl);
    return w;
}

void BottomNavigationBar::connectSignals() {
    connect(homeBtn, &QPushButton::clicked, this, &BottomNavigationBar::onHomeClicked);
    connect(exploreBtn, &QPushButton::clicked, this, &BottomNavigationBar::onExploreClicked);
    connect(createBtn, &QPushButton::clicked, this, &BottomNavigationBar::onCreateClicked);
    connect(messagesBtn, &QPushButton::clicked, this, &BottomNavigationBar::onMessagesClicked);
    connect(profileBtn, &QPushButton::clicked, this, &BottomNavigationBar::onProfileClicked);

    connect(themeManager, &ThemeManager::themeChanged, this, &BottomNavigationBar::onThemeChanged);
}

void BottomNavigationBar::applyStyles() {
    setStyleSheet(QString("BottomNavigationBar { background-color: %1; border-top: 1px solid %2; }")
                      .arg(DesignSystem::Colors::getSurface().name())
                      .arg(DesignSystem::Colors::getDivider().name()));

    QString btnStyle = "QPushButton { border: none; background: transparent; border-radius: 20px; }";
    homeBtn->setStyleSheet(btnStyle);
    exploreBtn->setStyleSheet(btnStyle);
    messagesBtn->setStyleSheet(btnStyle);
    profileBtn->setStyleSheet(btnStyle);

    createBtn->setStyleSheet(QString(
                                 "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 %1, stop:1 %2); "
                                 "border-radius: 27px; color: white; border: none; }"
                                 "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 %2, stop:1 %1); }")
                                 .arg(DesignSystem::Colors::getPrimary().name())
                                 .arg(DesignSystem::Colors::getPrimaryDark().name()));

    QString labelStyle = QString("color: %1;").arg(DesignSystem::Colors::getTextSecondary().name());
    homeLabel->setStyleSheet(labelStyle);
    exploreLabel->setStyleSheet(labelStyle);
    messagesLabel->setStyleSheet(labelStyle);
    profileLabel->setStyleSheet(labelStyle);
}

void BottomNavigationBar::updateButtonStates() {
    QString normalColor = DesignSystem::Colors::getTextSecondary().name();
    QString activeColor = DesignSystem::Colors::getPrimary().name();
    QString hoverColor = DesignSystem::Colors::getHoverOverlay().name();

    auto updateItem = [&](QPushButton* b, QLabel* l, bool active) {
        if (active) {
            b->setStyleSheet(QString("background-color: %1; border: none; border-radius: 20px;").arg(hoverColor));
            l->setStyleSheet(QString("color: %1; font-weight: bold;").arg(activeColor));
        } else {
            b->setStyleSheet("background-color: transparent; border: none; border-radius: 20px;");
            l->setStyleSheet(QString("color: %1; font-weight: normal;").arg(normalColor));
        }
    };

    updateItem(homeBtn, homeLabel, currentPage == HomePage);
    updateItem(exploreBtn, exploreLabel, currentPage == ExplorePage);
    updateItem(messagesBtn, messagesLabel, currentPage == MessagesPage);
    updateItem(profileBtn, profileLabel, currentPage == ProfilePage);
}

void BottomNavigationBar::updateTheme() {
    applyStyles();
    updateButtonStates();
    update();
}

void BottomNavigationBar::setCurrentPage(NavigationPage page) {
    if (currentPage != page) {
        currentPage = page;
        updateButtonStates();
        emit pageChanged(page);
    }
}

void BottomNavigationBar::setUnreadCount(int count) {
    unreadCount = count;
    if (badgeLabel) {
        if (unreadCount > 0) {
            badgeLabel->setText(unreadCount > 99 ? "99+" : QString::number(unreadCount));
            badgeLabel->show();
        } else {
            badgeLabel->hide();
        }
    }
}

void BottomNavigationBar::onThemeChanged(DesignSystem::Theme theme) {
    Q_UNUSED(theme);
    updateTheme();
}

void BottomNavigationBar::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (badgeLabel && messagesBtn) {
        QPoint pos = messagesBtn->pos();
        badgeLabel->move(pos.x() + messagesBtn->width() - 10, pos.y() + 2);
    }
}

void BottomNavigationBar::onHomeClicked() { setCurrentPage(HomePage); }
void BottomNavigationBar::onExploreClicked() { setCurrentPage(ExplorePage); }
void BottomNavigationBar::onCreateClicked() { emit createRequested(); }
void BottomNavigationBar::onMessagesClicked() { setCurrentPage(MessagesPage); }
void BottomNavigationBar::onProfileClicked() { setCurrentPage(ProfilePage); }
