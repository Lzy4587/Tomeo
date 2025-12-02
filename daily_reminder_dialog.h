//
// DailyReminderDialog - 每日提醒对话框
// Iteration 3: BeReal风格的每日录制提醒
//

#ifndef DAILY_REMINDER_DIALOG_H
#define DAILY_REMINDER_DIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTimeEdit>
#include <QCheckBox>
#include <QVBoxLayout>
#include "social_types.h"

class DailyReminderDialog : public QDialog {
    Q_OBJECT

private:
    DailyReminderSettings settings;

    // UI组件
    QLabel* titleLabel;
    QLabel* messageLabel;
    QLabel* timerLabel;

    // 时间选择
    QTimeEdit* timeEdit;
    QCheckBox* randomTimeCheck;

    // 按钮
    QPushButton* recordNowBtn;
    QPushButton* remindLaterBtn;
    QPushButton* cancelBtn;

    // 设置区域
    QWidget* settingsWidget;
    QCheckBox* enableReminderCheck;
    QCheckBox* soundCheck;
    QCheckBox* vibrationCheck;

    void setupUI();
    void connectSignals();
    void applyStyles();
    void updateTimerDisplay();
    void startCountdown();

public:
    explicit DailyReminderDialog(QWidget* parent = nullptr);

    DailyReminderSettings getSettings() const { return settings; }
    void setSettings(const DailyReminderSettings& s);

private slots:
    void onRecordNowClicked();
    void onRemindLaterClicked();
    void onCancelClicked();
    void onSettingsChanged();
    void onTimeChanged();

signals:
    void recordNowRequested();
    void remindLaterRequested(int minutes);
    void settingsChanged(const DailyReminderSettings& settings);
};

#endif // DAILY_REMINDER_DIALOG_H
