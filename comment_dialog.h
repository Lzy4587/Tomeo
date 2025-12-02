//
// CommentDialog - 评论对话框
// Iteration 3: 显示和添加评论的对话框
//

#ifndef COMMENT_DIALOG_H
#define COMMENT_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLabel>
#include "social_types.h"

class CommentDialog : public QDialog {
    Q_OBJECT

private:
    QString postId;
    VideoPost post;

    // UI组件
    QLabel* titleLabel;
    QScrollArea* commentsScrollArea;
    QWidget* commentsWidget;
    QVBoxLayout* commentsLayout;

    // 快捷评论
    QWidget* quickCommentsWidget;
    QVector<QPushButton*> quickCommentButtons;

    // 输入区域
    QLineEdit* commentInput;
    QPushButton* sendBtn;

    void setupUI();
    void connectSignals();
    void applyStyles();
    void loadComments();
    void addCommentToUI(const Comment& comment);
    void setupQuickComments();

public:
    explicit CommentDialog(const QString& postId, QWidget* parent = nullptr);

private slots:
    void onSendClicked();
    void onQuickCommentClicked();
    void onCommentLikeClicked();

signals:
    void commentAdded(const QString& postId, const Comment& comment);
};

#endif // COMMENT_DIALOG_H
