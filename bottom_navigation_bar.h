//
// BottomNavigationBar - 底部导航栏
// 类似抖音/小红书的底部导航设计
//

#ifndef BOTTOM_NAVIGATION_BAR_H
#define BOTTOM_NAVIGATION_BAR_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include "theme_manager.h"

class BottomNavigationBar : public QWidget {
    Q_OBJECT

public:
    enum NavigationPage {
        HomePage = 0,      // 首页/视频流
        ExplorePage = 1,   // 探索/热门
        CreatePage = 2,    // 创建（实际不切换页面，触发录制）
        MessagesPage = 3,  // 消息
        ProfilePage = 4    // 我的
    };

private:
    ThemeManager* themeManager;

    // 导航按钮
    QPushButton* homeBtn;
    QPushButton* exploreBtn;
    QPushButton* createBtn;      // 中间大按钮
    QPushButton* messagesBtn;
    QPushButton* profileBtn;

    // 标签
    QLabel* homeLabel;
    QLabel* exploreLabel;
    QLabel* messagesLabel;
    QLabel* profileLabel;

    // 当前选中的页面
    NavigationPage currentPage;

    // 消息未读数
    int unreadCount;
    QLabel* badgeLabel;

    void setupUI();
    void connectSignals();
    void applyStyles();
    void updateButtonStates();
    void updateBadge();

    // 创建单个导航项
    QWidget* createNavigationItem(QPushButton* button, QLabel* label,
                                  const QString& icon, const QString& text);

protected:
    void resizeEvent(QResizeEvent* event) override;

public:
    explicit BottomNavigationBar(QWidget* parent = nullptr);

    void setCurrentPage(NavigationPage page);
    NavigationPage getCurrentPage() const { return currentPage; }

    void setUnreadCount(int count);
    int getUnreadCount() const { return unreadCount; }

    void updateTheme();

private slots:
    void onHomeClicked();
    void onExploreClicked();
    void onCreateClicked();
    void onMessagesClicked();
    void onProfileClicked();
    void onThemeChanged(DesignSystem::Theme theme);

signals:
    void pageChanged(NavigationPage page);
    void createRequested();  // 录制按钮点击
};

#endif // BOTTOM_NAVIGATION_BAR_H
