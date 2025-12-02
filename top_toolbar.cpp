#include "top_toolbar.h"
#include "design_system.h"
#include <QDebug>

TopToolbar::TopToolbar(QWidget* parent)
    : QWidget(parent),
    themeManager(ThemeManager::getInstance()) {

    setupUI();
    connectSignals();
    applyStyles();

    // 初始化状态
    updateThemeButton();
}

void TopToolbar::setupUI() {
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(
        DesignSystem::Dimensions::SpacingMedium,
        DesignSystem::Dimensions::SpacingSmall,
        DesignSystem::Dimensions::SpacingMedium,
        DesignSystem::Dimensions::SpacingSmall
        );
    mainLayout->setSpacing(DesignSystem::Dimensions::SpacingMedium);

    // === 左侧：应用标题 ===
    titleLabel = new QLabel(tr("Tomeo"), this);
    titleLabel->setFont(DesignSystem::Typography::getTitle());
    mainLayout->addWidget(titleLabel);

    mainLayout->addStretch(1);

    // === 中间：搜索框 ===
    searchBox = new QLineEdit(this);
    searchBox->setPlaceholderText(tr("Search videos..."));
    searchBox->setMinimumWidth(200);
    searchBox->setMaximumWidth(400);
    searchBox->setClearButtonEnabled(true);
    searchBox->setAccessibleName(tr("Search box"));
    mainLayout->addWidget(searchBox);

    mainLayout->addStretch(1);

    // === 右侧：主题切换按钮 ===
    themeToggleBtn = new QPushButton(this);
    themeToggleBtn->setFixedSize(
        DesignSystem::Dimensions::ButtonHeight,
        DesignSystem::Dimensions::ButtonHeight
        );
    themeToggleBtn->setToolTip(tr("Toggle theme"));
    themeToggleBtn->setAccessibleName(tr("Theme toggle button"));
    themeToggleBtn->setCursor(Qt::PointingHandCursor);
    mainLayout->addWidget(themeToggleBtn);

    // === 设置按钮 ===
    settingsBtn = new QPushButton(this);
    settingsBtn->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    settingsBtn->setFixedSize(
        DesignSystem::Dimensions::ButtonHeight,
        DesignSystem::Dimensions::ButtonHeight
        );
    settingsBtn->setToolTip(tr("Settings"));
    settingsBtn->setAccessibleName(tr("Settings button"));
    settingsBtn->setCursor(Qt::PointingHandCursor);
    mainLayout->addWidget(settingsBtn);
}

void TopToolbar::connectSignals() {
    // 主题切换
    connect(themeToggleBtn, &QPushButton::clicked,
            this, &TopToolbar::onThemeToggled);
    connect(themeManager, &ThemeManager::themeChanged,
            this, &TopToolbar::onThemeChanged);

    // 搜索
    connect(searchBox, &QLineEdit::textChanged,
            this, &TopToolbar::searchTextChanged);

    // 设置 - 发射信号让主窗口处理
    connect(settingsBtn, &QPushButton::clicked,
            this, &TopToolbar::settingsClicked);
}

void TopToolbar::applyStyles() {
    // 工具栏背景
    setStyleSheet(QString(R"(
        TopToolbar {
            background-color: %1;
            border-bottom: 1px solid %2;
        }
    )").arg(DesignSystem::Colors::getSurface().name())
                      .arg(DesignSystem::Colors::getDivider().name()));

    // 标题样式
    titleLabel->setStyleSheet(QString(R"(
        QLabel {
            color: %1;
            font-weight: bold;
        }
    )").arg(DesignSystem::Colors::getPrimary().name()));

    // 搜索框样式
    searchBox->setStyleSheet(QString(R"(
        QLineEdit {
            background-color: %1;
            border: 1px solid %2;
            border-radius: %3px;
            padding: 8px 12px;
            color: %4;
        }
        QLineEdit:focus {
            border: 2px solid %5;
        }
        QLineEdit::placeholder {
            color: %6;
        }
    )").arg(DesignSystem::Colors::getBackground().name())
                                 .arg(DesignSystem::Colors::getBorder().name())
                                 .arg(DesignSystem::Dimensions::BorderRadius)
                                 .arg(DesignSystem::Colors::getTextPrimary().name())
                                 .arg(DesignSystem::Colors::getPrimary().name())
                                 .arg(DesignSystem::Colors::getTextSecondary().name()));

    // 按钮样式
    QString buttonStyle = QString(R"(
        QPushButton {
            background-color: transparent;
            border: none;
            border-radius: %1px;
        }
        QPushButton:hover {
            background-color: %2;
        }
        QPushButton:pressed {
            background-color: %3;
        }
    )").arg(DesignSystem::Dimensions::BorderRadiusLarge)
                              .arg(DesignSystem::Colors::getHoverOverlay().name())
                              .arg(DesignSystem::Colors::getHoverOverlay().name());

    themeToggleBtn->setStyleSheet(buttonStyle);
    settingsBtn->setStyleSheet(buttonStyle);

    qDebug() << "Toolbar styles applied, theme:" << (themeManager->isDarkTheme() ? "Dark" : "Light");
}

void TopToolbar::updateThemeButton() {
    if (themeManager->isDarkTheme()) {
        themeToggleBtn->setText("☀️");
        themeToggleBtn->setFont(QFont("Segoe UI Emoji", 16));
        themeToggleBtn->setToolTip(tr("Switch to light theme"));
        qDebug() << "Theme button: Dark mode (sun icon)";
    } else {
        themeToggleBtn->setText("🌙");
        themeToggleBtn->setFont(QFont("Segoe UI Emoji", 16));
        themeToggleBtn->setToolTip(tr("Switch to dark theme"));
        qDebug() << "Theme button: Light mode (moon icon)";
    }
}

void TopToolbar::onThemeToggled() {
    qDebug() << "=== Theme Toggle Clicked ===";
    themeManager->toggleTheme();
}

void TopToolbar::onThemeChanged(DesignSystem::Theme theme) {
    Q_UNUSED(theme);
    qDebug() << "TopToolbar received theme change signal";
    updateThemeButton();
    applyStyles();
}

QString TopToolbar::getSearchText() const {
    return searchBox->text();
}

void TopToolbar::clearSearch() {
    searchBox->clear();
}
