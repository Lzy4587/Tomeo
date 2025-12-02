//
// ThemeManager - 实现
//

#include "theme_manager.h"
#include <QDebug>

ThemeManager* ThemeManager::instance = nullptr;

ThemeManager::ThemeManager(QObject* parent)
    : QObject(parent) {

    settings = new QSettings("Tomeo", "ThemeSettings", this);
    loadSettings();

    qDebug() << "ThemeManager initialized";
}

ThemeManager* ThemeManager::getInstance() {
    if (instance == nullptr) {
        instance = new ThemeManager();
    }
    return instance;
}

void ThemeManager::setTheme(DesignSystem::Theme theme) {
    qDebug() << "setTheme called with:" << (theme == DesignSystem::DarkTheme ? "Dark" : "Light");

    // 1. 先更新 DesignSystem 的主题
    DesignSystem::setTheme(theme);

    qDebug() << "DesignSystem theme now:" << (DesignSystem::isDarkTheme() ? "Dark" : "Light");

    // 2. 保存设置
    saveSettings();

    // 3. 最后发射信号
    emit themeChanged(theme);
}

DesignSystem::Theme ThemeManager::getCurrentTheme() const {
    return DesignSystem::getTheme();
}

bool ThemeManager::isDarkTheme() const {
    return DesignSystem::isDarkTheme();
}

void ThemeManager::toggleTheme() {
    qDebug() << "Toggle theme called, current:" << (DesignSystem::isDarkTheme() ? "Dark" : "Light");

    DesignSystem::Theme newTheme = DesignSystem::isDarkTheme()
                                       ? DesignSystem::LightTheme
                                       : DesignSystem::DarkTheme;

    setTheme(newTheme);

    qDebug() << "Theme changed to:" << (DesignSystem::isDarkTheme() ? "Dark" : "Light");
}

QString ThemeManager::getApplicationStyleSheet() const {
    return QString(R"(
        * {
            font-family: "Segoe UI", Arial, sans-serif;
        }
        QWidget {
            background-color: %1;
            color: %2;
        }
        QToolTip {
            background-color: %3;
            color: %4;
            border: 1px solid %5;
            padding: 4px;
            border-radius: 4px;
        }
    )").arg(DesignSystem::Colors::getBackground().name())
        .arg(DesignSystem::Colors::getTextPrimary().name())
        .arg(DesignSystem::Colors::getSurface().name())
        .arg(DesignSystem::Colors::getTextPrimary().name())
        .arg(DesignSystem::Colors::getDivider().name());
}

void ThemeManager::saveSettings() {
    settings->setValue("theme", static_cast<int>(getCurrentTheme()));
    qDebug() << "Theme settings saved";
}

void ThemeManager::loadSettings() {
    int themeValue = settings->value("theme", DesignSystem::LightTheme).toInt();
    DesignSystem::setTheme(static_cast<DesignSystem::Theme>(themeValue));

    qDebug() << "Theme settings loaded:" << (isDarkTheme() ? "Dark" : "Light");
}
