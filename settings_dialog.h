//
// SettingsDialog - 设置对话框
// 包含语言选择、主题切换等设置
//

#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QCheckBox>
#include "language_manager.h"
#include "theme_manager.h"

class SettingsDialog : public QDialog {
    Q_OBJECT

private:
    // UI组件
    QComboBox* languageComboBox;
    QCheckBox* darkModeCheckBox;
    QCheckBox* autoPlayCheckBox;
    QPushButton* okButton;
    QPushButton* cancelButton;
    QPushButton* applyButton;

    // 管理器
    LanguageManager* languageManager;
    ThemeManager* themeManager;

    // 临时设置
    LanguageManager::Language selectedLanguage;
    bool darkModeEnabled;

    void setupUI();
    void connectSignals();
    void applyStyles();
    void loadCurrentSettings();

public:
    explicit SettingsDialog(QWidget* parent = nullptr);

private slots:
    void onLanguageChanged(int index);
    void onDarkModeToggled(bool checked);
    void onOkClicked();
    void onCancelClicked();
    void onApplyClicked();

signals:
    void settingsApplied();
};

#endif // SETTINGS_DIALOG_H
