//
// ShareDialog - 分享/转发对话框
// 模拟社交分享功能
//

#ifndef SHARE_DIALOG_H
#define SHARE_DIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QButtonGroup>
#include <QFrame>
#include "social_types.h"

class ShareDialog : public QDialog {
    Q_OBJECT

private:
    VideoPost post;

    // UI组件
    QLabel* postPreviewLabel;
    QLabel* authorLabel;
    QLabel* captionLabel;

    // 分享平台按钮
    QButtonGroup* platformGroup;
    QPushButton* facebookBtn;
    QPushButton* twitterBtn;
    QPushButton* instagramBtn;
    QPushButton* wechatBtn;
    QPushButton* whatsappBtn;

    // 评论输入
    QTextEdit* commentEdit;

    // 底部按钮
    QPushButton* shareButton;
    QPushButton* cancelButton;

    QString selectedPlatform;

    void setupUI();
    void connectSignals();
    void applyStyles();
    void updatePostPreview();

public:
    explicit ShareDialog(const VideoPost& post, QWidget* parent = nullptr);

    QString getSelectedPlatform() const { return selectedPlatform; }
    QString getComment() const { return commentEdit->toPlainText(); }

private slots:
    void onPlatformSelected(int id);
    void onShareClicked();
    void onCancelClicked();

signals:
    void shareConfirmed(const QString& postId, const QString& platform, const QString& comment);
};

#endif // SHARE_DIALOG_H
