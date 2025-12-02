//
// TheButton - 改进的视频缩略图按钮
// Iteration 2: 增加响应式支持、深色模式、视频信息显示
//

#ifndef THE_BUTTON_H
#define THE_BUTTON_H

#include <QPushButton>
#include <QUrl>
#include <QIcon>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QLabel>
#include <QVBoxLayout>
#include <QResizeEvent>

class TheButtonInfo {
public:
    QUrl* url;          // 视频文件URL
    QIcon* icon;        // 缩略图图标
    QString title;      // 视频标题
    qint64 duration;    // 视频时长（毫秒）
    qint64 fileSize;    // 文件大小（字节）

    // 构造函数
    TheButtonInfo(QUrl* url, QIcon* icon, QString title = "",
                  qint64 duration = 0, qint64 fileSize = 0)
        : url(url), icon(icon), title(title),
        duration(duration), fileSize(fileSize) {}
};

class TheButton : public QPushButton {
    Q_OBJECT
    Q_PROPERTY(qreal hoverOpacity READ hoverOpacity WRITE setHoverOpacity)

private:
    TheButtonInfo* info;
    qreal m_hoverOpacity;
    QPropertyAnimation* hoverAnimation;
    bool isHovered;

    // 信息标签（悬停时显示）
    QLabel* infoOverlay;
    QLabel* titleLabel;
    QLabel* durationLabel;

    void setupAnimation();
    void setupInfoOverlay();
    void applyStyles();
    void updateSize(int width, int height);
    QString formatDuration(qint64 milliseconds) const;
    QString formatFileSize(qint64 bytes) const;

protected:
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

public:
    explicit TheButton(QWidget* parent = nullptr);

    void init(TheButtonInfo* i);
    TheButtonInfo* getInfo() const { return info; }

    // 响应式尺寸调整
    void setResponsiveSize(int windowWidth);

    // 主题更新
    void updateTheme();

    // 属性访问器
    qreal hoverOpacity() const { return m_hoverOpacity; }
    void setHoverOpacity(qreal opacity);

private slots:
    void clicked();

signals:
    void jumpTo(TheButtonInfo*);
};

#endif // THE_BUTTON_H
