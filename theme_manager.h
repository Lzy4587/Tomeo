//
// ThemeManager - 主题管理器
// 管理应用程序的主题切换（浅色/深色模式）
//

#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include <QObject>
#include <QString>
#include <QSettings>
#include "design_system.h"

class ThemeManager : public QObject {
    Q_OBJECT

private:
    static ThemeManager* instance;
    QSettings* settings;

    explicit ThemeManager(QObject* parent = nullptr);

public:
    // 单例模式
    static ThemeManager* getInstance();

    // 主题管理
    void setTheme(DesignSystem::Theme theme);
    DesignSystem::Theme getCurrentTheme() const;
    bool isDarkTheme() const;
    void toggleTheme();

    // 获取应用样式表
    QString getApplicationStyleSheet() const;

    // 保存和加载设置
    void saveSettings();
    void loadSettings();

signals:
    void themeChanged(DesignSystem::Theme theme);
};

#endif // THEME_MANAGER_H
