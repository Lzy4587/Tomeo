//
// DailyReminderDialog - 实现
//

#include "daily_reminder_dialog.h"
#include "design_system.h"
#include <QHBoxLayout>
#include <QTime>
#include <QTimer>
#include <QDebug>

DailyReminderDialog::DailyReminderDialog(QWidget* parent)
    : QDialog(parent) {

    setupUI();
    connectSignals();
    applyStyles();
    updateTimerDisplay();

    setFixedSize(500, 600);
    setWindowTitle(tr("⏰ Time to Record!"));
}

void DailyReminderDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(20);

    // === 顶部：提示信息 ===
    titleLabel = new QLabel(tr("⏰ Time to BeReal"), this);
    titleLabel->setFont(DesignSystem::Typography::getHeadline());
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    messageLabel = new QLabel(
        tr("It's time to record a video!\nCapture this moment"),
        this
        );
    messageLabel->setFont(DesignSystem::Typography::getBody());
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setWordWrap(true);
    mainLayout->addWidget(messageLabel);

    // === 倒计时显示 ===
    timerLabel = new QLabel(tr("Time remaining: 2:00"), this);
    timerLabel->setFont(DesignSystem::Typography::getTitle());
    timerLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(timerLabel);

    mainLayout->addSpacing(20);

    // === 操作按钮 ===
    recordNowBtn = new QPushButton(tr("📸 Record Now"), this);
    recordNowBtn->setFont(DesignSystem::Typography::getButton());
    recordNowBtn->setFixedHeight(56);
    recordNowBtn->setCursor(Qt::PointingHandCursor);
    mainLayout->addWidget(recordNowBtn);

    remindLaterBtn = new QPushButton(tr("🕐 Remind in 10 minutes"), this);
    remindLaterBtn->setFont(DesignSystem::Typography::getButton());
    remindLaterBtn->setFixedHeight(48);
    remindLaterBtn->setCursor(Qt::PointingHandCursor);
    mainLayout->addWidget(remindLaterBtn);

    cancelBtn = new QPushButton(tr("Cancel"), this);
    cancelBtn->setFont(DesignSystem::Typography::getButton());
    cancelBtn->setFixedHeight(40);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    mainLayout->addWidget(cancelBtn);

    mainLayout->addSpacing(20);

    // === 设置区域 ===
    settingsWidget = new QWidget(this);
    QVBoxLayout* settingsLayout = new QVBoxLayout(settingsWidget);
    settingsLayout->setContentsMargins(16, 16, 16, 16);
    settingsLayout->setSpacing(12);

    QLabel* settingsLabel = new QLabel(tr("Reminder Settings"), settingsWidget);
    settingsLabel->setFont(DesignSystem::Typography::getSubtitle());
    settingsLayout->addWidget(settingsLabel);

    // 启用提醒
    enableReminderCheck = new QCheckBox(tr("Enable daily reminders"), settingsWidget);
    enableReminderCheck->setChecked(settings.enabled);
    settingsLayout->addWidget(enableReminderCheck);

    // 时间选择
    QWidget* timeWidget = new QWidget(settingsWidget);
    QHBoxLayout* timeLayout = new QHBoxLayout(timeWidget);
    timeLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* timeLabel = new QLabel(tr("Reminder time:"), timeWidget);
    timeLayout->addWidget(timeLabel);

    timeEdit = new QTimeEdit(settings.reminderTime, timeWidget);
    timeEdit->setDisplayFormat("HH:mm");
    timeLayout->addWidget(timeEdit);
    timeLayout->addStretch();

    settingsLayout->addWidget(timeWidget);

    // 随机时间
    randomTimeCheck = new QCheckBox(tr("Random time (±2 hours)"), settingsWidget);
    randomTimeCheck->setChecked(settings.randomTime);
    settingsLayout->addWidget(randomTimeCheck);

    // 声音和震动
    soundCheck = new QCheckBox(tr("Enable sound"), settingsWidget);
    soundCheck->setChecked(settings.soundEnabled);
    settingsLayout->addWidget(soundCheck);

    vibrationCheck = new QCheckBox(tr("Enable vibration"), settingsWidget);
    vibrationCheck->setChecked(settings.vibrationEnabled);
    settingsLayout->addWidget(vibrationCheck);

    mainLayout->addWidget(settingsWidget);
    mainLayout->addStretch();
}

void DailyReminderDialog::connectSignals() {
    connect(recordNowBtn, &QPushButton::clicked,
            this, &DailyReminderDialog::onRecordNowClicked);
    connect(remindLaterBtn, &QPushButton::clicked,
            this, &DailyReminderDialog::onRemindLaterClicked);
    connect(cancelBtn, &QPushButton::clicked,
            this, &DailyReminderDialog::onCancelClicked);

    connect(enableReminderCheck, &QCheckBox::stateChanged,
            this, &DailyReminderDialog::onSettingsChanged);
    connect(randomTimeCheck, &QCheckBox::stateChanged,
            this, &DailyReminderDialog::onSettingsChanged);
    connect(soundCheck, &QCheckBox::stateChanged,
            this, &DailyReminderDialog::onSettingsChanged);
    connect(vibrationCheck, &QCheckBox::stateChanged,
            this, &DailyReminderDialog::onSettingsChanged);
    connect(timeEdit, &QTimeEdit::timeChanged,
            this, &DailyReminderDialog::onTimeChanged);
}

void DailyReminderDialog::applyStyles() {
    // 对话框背景
    setStyleSheet(QString(R"(
        QDialog {
            background-color: %1;
            border-radius: 16px;
        }
    )").arg(DesignSystem::Colors::getBackground().name()));

    // 标题
    titleLabel->setStyleSheet(QString("color: %1; font-weight: bold;")
                                  .arg(DesignSystem::Colors::getPrimary().name()));

    // 消息
    messageLabel->setStyleSheet(QString("color: %1;")
                                    .arg(DesignSystem::Colors::getTextPrimary().name()));

    // 倒计时
    timerLabel->setStyleSheet(QString(R"(
        color: %1;
        font-weight: bold;
        background-color: %2;
        padding: 16px;
        border-radius: 12px;
    )").arg(DesignSystem::Colors::getTextOnPrimary().name())
                                  .arg(DesignSystem::Colors::getPrimary().name()));

    // 立即录制按钮（主按钮）
    recordNowBtn->setStyleSheet(QString(R"(
        QPushButton {
            background-color: %1;
            border: none;
            border-radius: 28px;
            color: white;
            font-weight: bold;
            font-size: 16px;
        }
        QPushButton:hover {
            background-color: %2;
        }
        QPushButton:pressed {
            background-color: %3;
        }
    )").arg(DesignSystem::Colors::getPrimary().name())
                                    .arg(DesignSystem::Colors::getPrimaryDark().name())
                                    .arg(DesignSystem::Colors::getPrimaryDark().name()));

    // 稍后提醒按钮（次要按钮）
    remindLaterBtn->setStyleSheet(QString(R"(
        QPushButton {
            background-color: transparent;
            border: 2px solid %1;
            border-radius: 24px;
            color: %2;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: %3;
        }
    )").arg(DesignSystem::Colors::getBorder().name())
                                      .arg(DesignSystem::Colors::getTextPrimary().name())
                                      .arg(DesignSystem::Colors::getHoverOverlay().name()));

    // 取消按钮
    cancelBtn->setStyleSheet(QString(R"(
        QPushButton {
            background-color: transparent;
            border: none;
            color: %1;
        }
        QPushButton:hover {
            background-color: %2;
            border-radius: 20px;
        }
    )").arg(DesignSystem::Colors::getTextSecondary().name())
                                 .arg(DesignSystem::Colors::getHoverOverlay().name()));

    // 设置区域
    settingsWidget->setStyleSheet(QString(R"(
        QWidget {
            background-color: %1;
            border: 1px solid %2;
            border-radius: 12px;
        }
        QLabel {
            color: %3;
            background: transparent;
        }
        QCheckBox {
            color: %3;
        }
    )").arg(DesignSystem::Colors::getCardBackground().name())
                                      .arg(DesignSystem::Colors::getDivider().name())
                                      .arg(DesignSystem::Colors::getTextPrimary().name()));

    // 时间选择器
    timeEdit->setStyleSheet(QString(R"(
        QTimeEdit {
            background-color: %1;
            border: 1px solid %2;
            border-radius: 8px;
            padding: 6px 12px;
            color: %3;
        }
    )").arg(DesignSystem::Colors::getBackground().name())
                                .arg(DesignSystem::Colors::getBorder().name())
                                .arg(DesignSystem::Colors::getTextPrimary().name()));
}

void DailyReminderDialog::updateTimerDisplay() {
    // TODO: 实现真实的倒计时
    // 这里只是示例
    timerLabel->setText(tr("Time remaining: 2:00"));
}

void DailyReminderDialog::startCountdown() {
    // TODO: 实现倒计时定时器
    qDebug() << "Countdown started";
}

void DailyReminderDialog::setSettings(const DailyReminderSettings& s) {
    settings = s;

    enableReminderCheck->setChecked(settings.enabled);
    timeEdit->setTime(settings.reminderTime);
    randomTimeCheck->setChecked(settings.randomTime);
    soundCheck->setChecked(settings.soundEnabled);
    vibrationCheck->setChecked(settings.vibrationEnabled);
}

void DailyReminderDialog::onRecordNowClicked() {
    qDebug() << "Record now clicked";
    emit recordNowRequested();
    accept();
}

void DailyReminderDialog::onRemindLaterClicked() {
    qDebug() << "Remind later clicked";
    emit remindLaterRequested(10); // 10分钟后
    accept();
}

void DailyReminderDialog::onCancelClicked() {
    qDebug() << "Cancel clicked";
    reject();
}

void DailyReminderDialog::onSettingsChanged() {
    settings.enabled = enableReminderCheck->isChecked();
    settings.randomTime = randomTimeCheck->isChecked();
    settings.soundEnabled = soundCheck->isChecked();
    settings.vibrationEnabled = vibrationCheck->isChecked();

    qDebug() << "Settings changed";
    emit settingsChanged(settings);
}

void DailyReminderDialog::onTimeChanged() {
    settings.reminderTime = timeEdit->time();
    qDebug() << "Time changed to:" << settings.reminderTime.toString();
    emit settingsChanged(settings);
}
