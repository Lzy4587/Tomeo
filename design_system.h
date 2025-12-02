//
// Tomeo - Design System
// Iteration 2+: 响应式设计优化 + 社交功能准备
//

#ifndef DESIGN_SYSTEM_H
#define DESIGN_SYSTEM_H

#include <QColor>
#include <QFont>
#include <QString>
#include <QDebug>

namespace DesignSystem {

// 主题枚举
enum Theme {
    LightTheme,
    DarkTheme
};

// 当前主题（默认浅色）
inline Theme currentTheme = LightTheme;

// 主题管理函数
inline void setTheme(Theme theme) {
    currentTheme = theme;
    qDebug() << "DesignSystem::setTheme() called, new theme:" << (theme == DarkTheme ? "Dark" : "Light");
    qDebug() << "currentTheme is now:" << (currentTheme == DarkTheme ? "Dark" : "Light");
}

inline Theme getTheme() {
    return currentTheme;
}

inline bool isDarkTheme() {
    return currentTheme == DarkTheme;
}

// ===== 响应式断点（优化） =====
namespace Breakpoints {
const int Mobile = 768;         // 手机/小平板分界线
const int Tablet = 1024;        // 平板/桌面分界线
const int Desktop = 1440;       // 桌面/大屏分界线
const int LargeDesktop = 1920;  // 超大屏
}

// 设备类型判断
enum DeviceType {
    Mobile,   // < 768px
    Tablet,   // 768-1024px
    Desktop   // > 1024px
};

inline DeviceType getDeviceType(int windowWidth) {
    if (windowWidth < Breakpoints::Mobile) {
        return Mobile;
    } else if (windowWidth < Breakpoints::Tablet) {
        return Tablet;
    } else {
        return Desktop;
    }
}

// 颜色系统 - 支持深色模式
namespace Colors {
// 根据当前主题返回颜色
inline QColor getPrimary() {
    return QColor("#2196F3");  // 蓝色（两个主题通用）
}

inline QColor getPrimaryDark() {
    return QColor("#1976D2");
}

inline QColor getPrimaryLight() {
    return QColor("#BBDEFB");
}

inline QColor getAccent() {
    return currentTheme == LightTheme ?
               QColor("#FF4081") : QColor("#FF80AB");
}

// 背景色（根据主题变化）
inline QColor getBackground() {
    return currentTheme == LightTheme ?
               QColor("#FAFAFA") : QColor("#121212");
}

inline QColor getSurface() {
    return currentTheme == LightTheme ?
               QColor("#FFFFFF") : QColor("#1E1E1E");
}

inline QColor getCardBackground() {
    return currentTheme == LightTheme ?
               QColor("#FFFFFF") : QColor("#2C2C2C");
}

// 文本色（根据主题变化）
inline QColor getTextPrimary() {
    return currentTheme == LightTheme ?
               QColor("#212121") : QColor("#FFFFFF");
}

inline QColor getTextSecondary() {
    return currentTheme == LightTheme ?
               QColor("#757575") : QColor("#B0B0B0");
}

inline QColor getTextDisabled() {
    return currentTheme == LightTheme ?
               QColor("#BDBDBD") : QColor("#666666");
}

inline QColor getTextOnPrimary() {
    return QColor("#FFFFFF");
}

// 状态色
inline QColor getSuccess() {
    return QColor("#4CAF50");
}

inline QColor getWarning() {
    return QColor("#FF9800");
}

inline QColor getError() {
    return QColor("#F44336");
}

inline QColor getInfo() {
    return QColor("#2196F3");
}

// 边框和分隔线
inline QColor getDivider() {
    return currentTheme == LightTheme ?
               QColor("#E0E0E0") : QColor("#3A3A3A");
}

inline QColor getBorder() {
    return currentTheme == LightTheme ?
               QColor("#BDBDBD") : QColor("#4A4A4A");
}

// 悬停效果
inline QColor getHoverOverlay() {
    return currentTheme == LightTheme ?
               QColor(33, 150, 243, 26) :   // rgba(33, 150, 243, 0.1)
               QColor(33, 150, 243, 38);    // rgba(33, 150, 243, 0.15)
}

// 兼容旧代码的静态颜色
const QColor Primary = QColor("#2196F3");
const QColor PrimaryDark = QColor("#1976D2");
const QColor PrimaryLight = QColor("#BBDEFB");
const QColor Accent = QColor("#FF4081");
const QColor AccentLight = QColor("#FF80AB");
const QColor Background = QColor("#FAFAFA");
const QColor Surface = QColor("#FFFFFF");
const QColor CardBackground = QColor("#FFFFFF");
const QColor TextPrimary = QColor("#212121");
const QColor TextSecondary = QColor("#757575");
const QColor TextDisabled = QColor("#BDBDBD");
const QColor TextOnPrimary = QColor("#FFFFFF");
const QColor Success = QColor("#4CAF50");
const QColor Warning = QColor("#FF9800");
const QColor Error = QColor("#F44336");
const QColor Info = QColor("#2196F3");
const QColor Divider = QColor("#E0E0E0");
const QColor Border = QColor("#BDBDBD");
}

// 响应式尺寸系统
namespace Dimensions {
// 按钮尺寸（根据设备类型调整）
inline int getButtonHeight(DeviceType device) {
    switch (device) {
    case Mobile: return 44;  // 更大的触摸目标
    case Tablet: return 48;
    case Desktop: return 48;
    default: return 48;
    }
}

const int ButtonHeightSmall = 32;
const int ButtonHeight = 48;
const int ButtonHeightLarge = 56;
const int ButtonMinWidth = 88;

// 图标尺寸
const int IconSizeSmall = 16;
const int IconSize = 24;
const int IconSizeLarge = 32;
const int IconSizeXLarge = 48;

// 间距系统（8px基准）
inline int getSpacing(DeviceType device) {
    switch (device) {
    case Mobile: return 8;
    case Tablet: return 12;
    case Desktop: return 16;
    default: return 16;
    }
}

const int SpacingXSmall = 4;
const int SpacingSmall = 8;
const int SpacingMedium = 16;
const int SpacingLarge = 24;
const int SpacingXLarge = 32;
const int SpacingXXLarge = 48;

// 圆角
const int BorderRadiusSmall = 2;
const int BorderRadius = 4;
const int BorderRadiusLarge = 8;
const int BorderRadiusXLarge = 16;

// 缩略图尺寸（响应式）
inline int getThumbnailWidth(int windowWidth) {
    if (windowWidth < Breakpoints::Mobile) {
        return 160;  // 手机
    } else if (windowWidth < Breakpoints::Tablet) {
        return 180;  // 平板
    } else if (windowWidth < Breakpoints::Desktop) {
        return 200;  // 小桌面
    } else {
        return 220;  // 大桌面
    }
}

inline int getThumbnailHeight(int windowWidth) {
    return getThumbnailWidth(windowWidth) * 9 / 16;  // 16:9比例
}

// 网格列数（响应式）
inline int getGridColumns(int windowWidth) {
    if (windowWidth < Breakpoints::Mobile) {
        return 2;      // 手机：2列
    } else if (windowWidth < Breakpoints::Tablet) {
        return 3;      // 平板：3列
    } else if (windowWidth < Breakpoints::Desktop) {
        return 4;      // 小桌面：4列
    } else if (windowWidth < Breakpoints::LargeDesktop) {
        return 5;      // 桌面：5列
    } else {
        return 6;      // 大屏：6列
    }
}

// 默认值（向后兼容）
const int ThumbnailWidth = 200;
const int ThumbnailHeight = 110;
}

// 字体系统
namespace Typography {
inline QFont getHeadline() {
    QFont font;
    font.setFamily("Segoe UI, Arial, sans-serif");
    font.setPointSize(24);
    font.setWeight(QFont::Bold);
    return font;
}

inline QFont getTitle() {
    QFont font;
    font.setFamily("Segoe UI, Arial, sans-serif");
    font.setPointSize(20);
    font.setWeight(QFont::DemiBold);
    return font;
}

inline QFont getSubtitle() {
    QFont font;
    font.setFamily("Segoe UI, Arial, sans-serif");
    font.setPointSize(16);
    font.setWeight(QFont::DemiBold);
    return font;
}

inline QFont getBody() {
    QFont font;
    font.setFamily("Segoe UI, Arial, sans-serif");
    font.setPointSize(14);
    font.setWeight(QFont::Normal);
    return font;
}

inline QFont getCaption() {
    QFont font;
    font.setFamily("Segoe UI, Arial, sans-serif");
    font.setPointSize(12);
    font.setWeight(QFont::Normal);
    return font;
}

inline QFont getButton() {
    QFont font;
    font.setFamily("Segoe UI, Arial, sans-serif");
    font.setPointSize(14);
    font.setWeight(QFont::Medium);
    return font;
}
}

// 动画时长
namespace Animation {
const int DurationFast = 150;       // ms
const int DurationNormal = 300;
const int DurationSlow = 500;
const int DurationVerySlow = 800;
}

// 阴影效果
namespace Shadow {
inline QString getElevation1() {
    return currentTheme == LightTheme ?
               "0 1px 3px rgba(0,0,0,0.12), 0 1px 2px rgba(0,0,0,0.24)" :
               "0 1px 3px rgba(0,0,0,0.4), 0 1px 2px rgba(0,0,0,0.6)";
}

inline QString getElevation2() {
    return currentTheme == LightTheme ?
               "0 3px 6px rgba(0,0,0,0.16), 0 3px 6px rgba(0,0,0,0.23)" :
               "0 3px 6px rgba(0,0,0,0.5), 0 3px 6px rgba(0,0,0,0.7)";
}

inline QString getElevation3() {
    return currentTheme == LightTheme ?
               "0 10px 20px rgba(0,0,0,0.19), 0 6px 6px rgba(0,0,0,0.23)" :
               "0 10px 20px rgba(0,0,0,0.6), 0 6px 6px rgba(0,0,0,0.8)";
}
}

} // namespace DesignSystem

#endif // DESIGN_SYSTEM_H
