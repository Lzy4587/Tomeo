//
// SettingsDialog - 设置对话框实现
//

#include "settings_dialog.h"
#include "design_system.h"
#include <QDebug>

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent),
    languageManager(LanguageManager::getInstance()),
    themeManager(ThemeManager::getInstance()) {

    setWindowTitle(tr("Settings"));
    setModal(true);
    setMinimumWidth(400);
    setMinimumHeight(300);

    setupUI();
    connectSignals();
    loadCurrentSettings();
    applyStyles();
}

void SettingsDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(DesignSystem::Dimensions::SpacingMedium);
    mainLayout->setContentsMargins(
        DesignSystem::Dimensions::SpacingLarge,
        DesignSystem::Dimensions::SpacingLarge,
        DesignSystem::Dimensions::SpacingLarge,
        DesignSystem::Dimensions::SpacingLarge
        );

    // === 语言设置 ===
    QGroupBox* languageGroup = new QGroupBox(tr("Language / 语言 / Idioma"), this);
    QVBoxLayout* languageLayout = new QVBoxLayout(languageGroup);

    QLabel* languageLabel = new QLabel(tr("Select your preferred language:"), languageGroup);
    languageLabel->setFont(DesignSystem::Typography::getBody());
    languageLayout->addWidget(languageLabel);

    languageComboBox = new QComboBox(languageGroup);
    languageComboBox->addItem("English", LanguageManager::English);
    languageComboBox->addItem("中文 (Chinese)", LanguageManager::Chinese);
    languageComboBox->addItem("Español (Spanish)", LanguageManager::Spanish);
    languageComboBox->setFont(DesignSystem::Typography::getBody());
    languageComboBox->setMinimumHeight(DesignSystem::Dimensions::ButtonHeight);
    languageLayout->addWidget(languageComboBox);

    QLabel* restartHint = new QLabel(
        tr("Note: Some changes may require restart to take full effect."),
        languageGroup
        );
    restartHint->setFont(DesignSystem::Typography::getCaption());
    restartHint->setWordWrap(true);
    languageLayout->addWidget(restartHint);

    mainLayout->addWidget(languageGroup);

    // === 外观设置 ===
    QGroupBox* appearanceGroup = new QGroupBox(tr("Appearance"), this);
    QVBoxLayout* appearanceLayout = new QVBoxLayout(appearanceGroup);

    darkModeCheckBox = new QCheckBox(tr("Dark Mode"), appearanceGroup);
    darkModeCheckBox->setFont(DesignSystem::Typography::getBody());
    appearanceLayout->addWidget(darkModeCheckBox);

    mainLayout->addWidget(appearanceGroup);

    // === 播放设置 ===
    QGroupBox* playbackGroup = new QGroupBox(tr("Playback"), this);
    QVBoxLayout* playbackLayout = new QVBoxLayout(playbackGroup);

    autoPlayCheckBox = new QCheckBox(tr("Auto-play next video"), playbackGroup);
    autoPlayCheckBox->setFont(DesignSystem::Typography::getBody());
    autoPlayCheckBox->setChecked(true);
    playbackLayout->addWidget(autoPlayCheckBox);

    mainLayout->addWidget(playbackGroup);

    mainLayout->addStretch();

    // === 底部按钮 ===
    QWidget* buttonWidget = new QWidget(this);
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(DesignSystem::Dimensions::SpacingSmall);

    buttonLayout->addStretch();

    applyButton = new QPushButton(tr("Apply"), buttonWidget);
    applyButton->setMinimumWidth(DesignSystem::Dimensions::ButtonMinWidth);
    applyButton->setMinimumHeight(DesignSystem::Dimensions::ButtonHeight);
    applyButton->setFont(DesignSystem::Typography::getButton());
    buttonLayout->addWidget(applyButton);

    cancelButton = new QPushButton(tr("Cancel"), buttonWidget);
    cancelButton->setMinimumWidth(DesignSystem::Dimensions::ButtonMinWidth);
    cancelButton->setMinimumHeight(DesignSystem::Dimensions::ButtonHeight);
    cancelButton->setFont(DesignSystem::Typography::getButton());
    buttonLayout->addWidget(cancelButton);

    okButton = new QPushButton(tr("OK"), buttonWidget);
    okButton->setMinimumWidth(DesignSystem::Dimensions::ButtonMinWidth);
    okButton->setMinimumHeight(DesignSystem::Dimensions::ButtonHeight);
    okButton->setFont(DesignSystem::Typography::getButton());
    okButton->setDefault(true);
    buttonLayout->addWidget(okButton);

    mainLayout->addWidget(buttonWidget);
}

void SettingsDialog::connectSignals() {
    connect(languageComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::onLanguageChanged);
    connect(darkModeCheckBox, &QCheckBox::toggled,
            this, &SettingsDialog::onDarkModeToggled);
    connect(okButton, &QPushButton::clicked,
            this, &SettingsDialog::onOkClicked);
    connect(cancelButton, &QPushButton::clicked,
            this, &SettingsDialog::onCancelClicked);
    connect(applyButton, &QPushButton::clicked,
            this, &SettingsDialog::onApplyClicked);
}

void SettingsDialog::applyStyles() {
    // 对话框背景
    setStyleSheet(QString(R"(
        QDialog {
            background-color: %1;
        }
        QGroupBox {
            background-color: %2;
            border: 1px solid %3;
            border-radius: %4px;
            margin-top: 12px;
            padding-top: 12px;
            font-weight: bold;
            color: %5;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 5px;
        }
        QLabel {
            color: %6;
        }
        QComboBox {
            background-color: %2;
            border: 1px solid %3;
            border-radius: %7px;
            padding: 8px;
            color: %5;
        }
        QComboBox:hover {
            border-color: %8;
        }
        QComboBox::drop-down {
            border: none;
        }
        QComboBox::down-arrow {
            width: 12px;
            height: 12px;
        }
        QCheckBox {
            color: %5;
            spacing: 8px;
        }
        QCheckBox::indicator {
            width: 20px;
            height: 20px;
            border: 2px solid %3;
            border-radius: 4px;
            background-color: %2;
        }
        QCheckBox::indicator:checked {
            background-color: %8;
            border-color: %8;
        }
    )").arg(DesignSystem::Colors::getBackground().name())
                      .arg(DesignSystem::Colors::getSurface().name())
                      .arg(DesignSystem::Colors::getBorder().name())
                      .arg(DesignSystem::Dimensions::BorderRadius)
                      .arg(DesignSystem::Colors::getTextPrimary().name())
                      .arg(DesignSystem::Colors::getTextSecondary().name())
                      .arg(DesignSystem::Dimensions::BorderRadiusSmall)
                      .arg(DesignSystem::Colors::getPrimary().name()));

    // 按钮样式
    QString buttonStyle = QString(R"(
        QPushButton {
            background-color: %1;
            border: none;
            border-radius: %2px;
            color: white;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: %3;
        }
        QPushButton:pressed {
            background-color: %4;
        }
    )").arg(DesignSystem::Colors::getPrimary().name())
                              .arg(DesignSystem::Dimensions::BorderRadiusSmall)
                              .arg(DesignSystem::Colors::getPrimaryDark().name())
                              .arg(DesignSystem::Colors::getPrimaryDark().name());

    okButton->setStyleSheet(buttonStyle);
    applyButton->setStyleSheet(buttonStyle);

    QString cancelButtonStyle = QString(R"(
        QPushButton {
            background-color: transparent;
            border: 2px solid %1;
            border-radius: %2px;
            color: %3;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: %4;
        }
    )").arg(DesignSystem::Colors::getBorder().name())
                                    .arg(DesignSystem::Dimensions::BorderRadiusSmall)
                                    .arg(DesignSystem::Colors::getTextPrimary().name())
                                    .arg(DesignSystem::Colors::getHoverOverlay().name());

    cancelButton->setStyleSheet(cancelButtonStyle);
}

void SettingsDialog::loadCurrentSettings() {
    // 加载当前语言
    LanguageManager::Language currentLang = languageManager->getLanguage();
    for (int i = 0; i < languageComboBox->count(); i++) {
        if (languageComboBox->itemData(i).toInt() == currentLang) {
            languageComboBox->setCurrentIndex(i);
            break;
        }
    }
    selectedLanguage = currentLang;

    // 加载当前主题
    darkModeEnabled = themeManager->isDarkTheme();
    darkModeCheckBox->setChecked(darkModeEnabled);

    qDebug() << "Settings loaded - Language:" << languageManager->getLanguageName()
             << "Dark Mode:" << darkModeEnabled;
}

void SettingsDialog::onLanguageChanged(int index) {
    selectedLanguage = static_cast<LanguageManager::Language>(
        languageComboBox->itemData(index).toInt()
        );
    qDebug() << "Language selection changed to:"
             << LanguageManager::getLanguageDisplayName(selectedLanguage);
}

void SettingsDialog::onDarkModeToggled(bool checked) {
    darkModeEnabled = checked;
    qDebug() << "Dark mode toggled:" << checked;
}

void SettingsDialog::onOkClicked() {
    onApplyClicked();
    accept();
}

void SettingsDialog::onCancelClicked() {
    qDebug() << "Settings cancelled";
    reject();
}

void SettingsDialog::onApplyClicked() {
    qDebug() << "Applying settings...";

    // 应用语言设置
    if (selectedLanguage != languageManager->getLanguage()) {
        languageManager->setLanguage(selectedLanguage);
        qDebug() << "Language changed to:" << languageManager->getLanguageName();
    }

    // 应用主题设置
    bool currentDarkMode = themeManager->isDarkTheme();
    if (darkModeEnabled != currentDarkMode) {
        themeManager->toggleTheme();
        qDebug() << "Theme changed to:" << (darkModeEnabled ? "Dark" : "Light");
    }

    emit settingsApplied();

    qDebug() << "Settings applied successfully";
}
