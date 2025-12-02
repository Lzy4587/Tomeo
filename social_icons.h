//
// SocialIcons - 社交功能图标工具类
// 提供点赞、评论、分享等图标的绘制
//

#ifndef SOCIAL_ICONS_H
#define SOCIAL_ICONS_H

#include <QPixmap>
#include <QPainter>
#include <QColor>
#include <QPainterPath>

class SocialIcons {
public:
    // 图标尺寸枚举
    enum IconSize {
        Small = 16,
        Medium = 20,
        Large = 24,
        XLarge = 32
    };

    // 创建点赞图标（心形）
    static QPixmap createLikeIcon(IconSize size, const QColor& color, bool filled = false) {
        int s = static_cast<int>(size);
        QPixmap pixmap(s, s);
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);

        QPainterPath path;
        qreal w = s * 0.85;
        qreal h = s * 0.85;
        qreal offsetX = (s - w) / 2;
        qreal offsetY = (s - h) / 2 + h * 0.1;

        // 绘制心形路径
        path.moveTo(w/2 + offsetX, h * 0.3 + offsetY);

        // 左半心
        path.cubicTo(
            w/2 + offsetX, h * 0.15 + offsetY,
            0 + offsetX, h * 0.15 + offsetY,
            0 + offsetX, h * 0.45 + offsetY
            );
        path.cubicTo(
            0 + offsetX, h * 0.7 + offsetY,
            w/2 + offsetX, h + offsetY,
            w/2 + offsetX, h + offsetY
            );

        // 右半心
        path.cubicTo(
            w/2 + offsetX, h + offsetY,
            w + offsetX, h * 0.7 + offsetY,
            w + offsetX, h * 0.45 + offsetY
            );
        path.cubicTo(
            w + offsetX, h * 0.15 + offsetY,
            w/2 + offsetX, h * 0.15 + offsetY,
            w/2 + offsetX, h * 0.3 + offsetY
            );

        if (filled) {
            painter.fillPath(path, color);
        } else {
            painter.setPen(QPen(color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter.setBrush(Qt::NoBrush);
            painter.drawPath(path);
        }

        return pixmap;
    }

    // 创建评论图标（气泡）
    static QPixmap createCommentIcon(IconSize size, const QColor& color) {
        int s = static_cast<int>(size);
        QPixmap pixmap(s, s);
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.setBrush(Qt::NoBrush);

        qreal margin = s * 0.15;
        qreal bubbleWidth = s - margin * 2;
        qreal bubbleHeight = s * 0.6;

        // 绘制对话气泡
        QPainterPath path;
        path.addRoundedRect(
            margin, margin,
            bubbleWidth, bubbleHeight,
            s * 0.15, s * 0.15
            );

        // 添加小尾巴
        path.moveTo(margin + bubbleWidth * 0.3, margin + bubbleHeight);
        path.lineTo(margin + bubbleWidth * 0.25, s - margin);
        path.lineTo(margin + bubbleWidth * 0.45, margin + bubbleHeight);

        painter.drawPath(path);

        return pixmap;
    }

    // 创建分享图标（箭头）
    static QPixmap createShareIcon(IconSize size, const QColor& color) {
        int s = static_cast<int>(size);
        QPixmap pixmap(s, s);
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.setBrush(Qt::NoBrush);

        qreal margin = s * 0.15;
        qreal centerX = s / 2;
        qreal centerY = s / 2;
        qreal arrowSize = s - margin * 2;

        // 绘制向上的箭头
        QPainterPath path;

        // 箭头头部
        path.moveTo(centerX, margin);
        path.lineTo(centerX + arrowSize * 0.3, margin + arrowSize * 0.25);
        path.moveTo(centerX, margin);
        path.lineTo(centerX - arrowSize * 0.3, margin + arrowSize * 0.25);

        // 箭头杆
        path.moveTo(centerX, margin);
        path.lineTo(centerX, s - margin);

        // 底部圆弧
        path.moveTo(margin + arrowSize * 0.2, s - margin);
        path.arcTo(
            margin, s - margin - arrowSize * 0.4,
            arrowSize * 0.4, arrowSize * 0.4,
            180, 90
            );

        painter.drawPath(path);

        return pixmap;
    }

    // 创建播放图标
    static QPixmap createPlayIcon(IconSize size, const QColor& color) {
        int s = static_cast<int>(size);
        QPixmap pixmap(s, s);
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);

        qreal margin = s * 0.2;
        qreal triangleHeight = s - margin * 2;
        qreal triangleWidth = triangleHeight * 0.85;

        // 绘制三角形
        QPainterPath path;
        path.moveTo(margin + s * 0.1, margin);
        path.lineTo(s - margin, s / 2);
        path.lineTo(margin + s * 0.1, s - margin);
        path.closeSubpath();

        painter.drawPath(path);

        return pixmap;
    }

    // 创建眼睛图标（观看数）
    static QPixmap createViewIcon(IconSize size, const QColor& color) {
        int s = static_cast<int>(size);
        QPixmap pixmap(s, s);
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(color, 1.5, Qt::SolidLine));
        painter.setBrush(Qt::NoBrush);

        qreal margin = s * 0.15;
        qreal eyeWidth = s - margin * 2;
        qreal eyeHeight = s * 0.5;
        qreal centerX = s / 2;
        qreal centerY = s / 2;

        // 绘制眼睛轮廓
        QPainterPath eye;
        eye.moveTo(margin, centerY);
        eye.quadTo(centerX, margin + eyeHeight * 0.3, s - margin, centerY);
        eye.quadTo(centerX, s - margin - eyeHeight * 0.3, margin, centerY);

        painter.drawPath(eye);

        // 绘制瞳孔
        painter.setBrush(color);
        painter.drawEllipse(
            QPointF(centerX, centerY),
            s * 0.15, s * 0.15
            );

        return pixmap;
    }

    // 创建更多选项图标（三个点）
    static QPixmap createMoreIcon(IconSize size, const QColor& color) {
        int s = static_cast<int>(size);
        QPixmap pixmap(s, s);
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);

        qreal dotSize = s * 0.15;
        qreal spacing = s * 0.25;
        qreal startY = s / 2 - spacing;

        // 绘制三个点（垂直排列）
        for (int i = 0; i < 3; i++) {
            painter.drawEllipse(
                QPointF(s / 2, startY + i * spacing),
                dotSize, dotSize
                );
        }

        return pixmap;
    }
};

#endif // SOCIAL_ICONS_H
