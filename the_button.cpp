//
// TheButton - 改进的视频缩略图按钮实现
// Iteration 2: 响应式 + 深色模式 + 视频信息
//

#include "the_button.h"
#include "design_system.h"
#include <QDebug>
#include <QVBoxLayout>

TheButton::TheButton(QWidget* parent)
    : QPushButton(parent),
    info(nullptr),
    m_hoverOpacity(1.0),
    hoverAnimation(nullptr),
    isHovered(false),
    infoOverlay(nullptr),
    titleLabel(nullptr),
    durationLabel(nullptr) {

    // 设置图标大小（默认值）
    setIconSize(QSize(200, 110));

    // 设置固定大小（默认值）
    setFixedSize(216, 126);

    // 设置焦点策略
    setFocusPolicy(Qt::StrongFocus);

    // 设置光标
    setCursor(Qt::PointingHandCursor);

    // 连接点击信号
    connect(this, &QPushButton::released, this, &TheButton::clicked);

    // 设置动画
    setupAnimation();

    // 设置信息覆盖层
    setupInfoOverlay();

    // 应用样式
    applyStyles();
}

void TheButton::init(TheButtonInfo* i) {
    info = i;

    if (info && info->icon) {
        setIcon(*info->icon);

        // 设置工具提示
        QString tooltip = info->title.isEmpty() ?
                              info->url->fileName() : info->title;

        if (info->duration > 0) {
            tooltip += "\n" + tr("Duration: ") + formatDuration(info->duration);
        }
        if (info->fileSize > 0) {
            tooltip += "\n" + tr("Size: ") + formatFileSize(info->fileSize);
        }

        setToolTip(tooltip);
        setAccessibleName(info->title.isEmpty() ?
                              info->url->fileName() : info->title);

        // 更新标签
        if (titleLabel) {
            titleLabel->setText(info->title.isEmpty() ?
                                    info->url->fileName() : info->title);
        }
        if (durationLabel && info->duration > 0) {
            durationLabel->setText(formatDuration(info->duration));
            durationLabel->setVisible(true);
        }
    }
}

void TheButton::setupAnimation() {
    hoverAnimation = new QPropertyAnimation(this, "hoverOpacity");
    hoverAnimation->setDuration(DesignSystem::Animation::DurationFast);
    hoverAnimation->setEasingCurve(QEasingCurve::InOutQuad);
}

void TheButton::setupInfoOverlay() {
    // 创建覆盖层容器
    infoOverlay = new QLabel(this);
    infoOverlay->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
    infoOverlay->hide();

    // 创建布局
    QVBoxLayout* overlayLayout = new QVBoxLayout(infoOverlay);
    overlayLayout->setContentsMargins(8, 8, 8, 8);
    overlayLayout->setSpacing(4);

    overlayLayout->addStretch();

    // 标题标签
    titleLabel = new QLabel(infoOverlay);
    titleLabel->setFont(DesignSystem::Typography::getCaption());
    titleLabel->setWordWrap(true);
    titleLabel->setMaximumHeight(40);
    overlayLayout->addWidget(titleLabel);

    // 时长标签
    durationLabel = new QLabel(infoOverlay);
    durationLabel->setFont(DesignSystem::Typography::getCaption());
    durationLabel->setAlignment(Qt::AlignRight);
    durationLabel->hide();
    overlayLayout->addWidget(durationLabel);
}

void TheButton::applyStyles() {
    setStyleSheet(QString(R"(
        TheButton {
            background-color: %1;
            border: 2px solid %2;
            border-radius: %3px;
            padding: 4px;
        }
        TheButton:hover {
            border-color: %4;
            background-color: %5;
        }
        TheButton:pressed {
            background-color: %6;
        }
        TheButton:focus {
            outline: none;
            border: 3px solid %4;
        }
    )").arg(DesignSystem::Colors::getCardBackground().name())
                      .arg(DesignSystem::Colors::getDivider().name())
                      .arg(DesignSystem::Dimensions::BorderRadius)
                      .arg(DesignSystem::Colors::getPrimary().name())
                      .arg(DesignSystem::Colors::getHoverOverlay().name())
                      .arg(DesignSystem::Colors::getDivider().name()));

    // 覆盖层样式
    if (infoOverlay) {
        infoOverlay->setStyleSheet(QString(R"(
            QLabel {
                background-color: rgba(0, 0, 0, 0.7);
                color: white;
                border-radius: 4px;
            }
        )"));
    }

    // 标题标签样式
    if (titleLabel) {
        titleLabel->setStyleSheet(QString(R"(
            QLabel {
                color: white;
                background-color: transparent;
            }
        )"));
    }

    // 时长标签样式
    if (durationLabel) {
        durationLabel->setStyleSheet(QString(R"(
            QLabel {
                color: white;
                background-color: rgba(0, 0, 0, 0.5);
                padding: 2px 6px;
                border-radius: 3px;
            }
        )"));
    }
}

void TheButton::updateSize(int width, int height) {
    setIconSize(QSize(width - 8, height - 8));
    setFixedSize(width, height);

    if (infoOverlay) {
        infoOverlay->setGeometry(0, 0, width, height);
    }
}

void TheButton::setResponsiveSize(int windowWidth) {
    int width = DesignSystem::Dimensions::getThumbnailWidth(windowWidth);
    int height = DesignSystem::Dimensions::getThumbnailHeight(windowWidth);
    updateSize(width + 16, height + 16);
}

void TheButton::updateTheme() {
    applyStyles();
    update();
}

void TheButton::enterEvent(QEvent* event) {
    QPushButton::enterEvent(event);

    isHovered = true;

    if (hoverAnimation) {
        hoverAnimation->stop();
        hoverAnimation->setStartValue(m_hoverOpacity);
        hoverAnimation->setEndValue(1.1);
        hoverAnimation->start();
    }

    // 显示信息覆盖层
    if (infoOverlay && info && !info->title.isEmpty()) {
        infoOverlay->show();
    }
}

void TheButton::leaveEvent(QEvent* event) {
    QPushButton::leaveEvent(event);

    isHovered = false;

    if (hoverAnimation) {
        hoverAnimation->stop();
        hoverAnimation->setStartValue(m_hoverOpacity);
        hoverAnimation->setEndValue(1.0);
        hoverAnimation->start();
    }

    // 隐藏信息覆盖层
    if (infoOverlay) {
        infoOverlay->hide();
    }
}

void TheButton::focusInEvent(QFocusEvent* event) {
    QPushButton::focusInEvent(event);
    update();
}

void TheButton::focusOutEvent(QFocusEvent* event) {
    QPushButton::focusOutEvent(event);
    update();
}

void TheButton::resizeEvent(QResizeEvent* event) {
    QPushButton::resizeEvent(event);

    if (infoOverlay) {
        infoOverlay->setGeometry(0, 0, width(), height());
    }
}

void TheButton::setHoverOpacity(qreal opacity) {
    m_hoverOpacity = opacity;
    update();
}

void TheButton::clicked() {
    if (info) {
        qDebug() << "Button clicked:" << (info->title.isEmpty() ?
                                              info->url->fileName() : info->title);
        emit jumpTo(info);
    }
}

QString TheButton::formatDuration(qint64 milliseconds) const {
    int seconds = milliseconds / 1000;
    int minutes = seconds / 60;
    seconds = seconds % 60;

    if (minutes >= 60) {
        int hours = minutes / 60;
        minutes = minutes % 60;
        return QString("%1:%2:%3")
            .arg(hours)
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));
    }

    return QString("%1:%2")
        .arg(minutes)
        .arg(seconds, 2, 10, QChar('0'));
}

QString TheButton::formatFileSize(qint64 bytes) const {
    const qint64 KB = 1024;
    const qint64 MB = KB * 1024;
    const qint64 GB = MB * 1024;

    if (bytes >= GB) {
        return QString::number(bytes / (double)GB, 'f', 2) + " GB";
    } else if (bytes >= MB) {
        return QString::number(bytes / (double)MB, 'f', 2) + " MB";
    } else if (bytes >= KB) {
        return QString::number(bytes / (double)KB, 'f', 2) + " KB";
    } else {
        return QString::number(bytes) + " B";
    }
}
