#ifndef TOP_TOOLBAR_H
#define TOP_TOOLBAR_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include "theme_manager.h"

class TopToolbar : public QWidget {
    Q_OBJECT

private:
    QLabel* titleLabel;
    QLineEdit* searchBox;
    QPushButton* themeToggleBtn;
    QPushButton* settingsBtn;
    ThemeManager* themeManager;

    void setupUI();
    void connectSignals();

public:
    explicit TopToolbar(QWidget* parent = nullptr);

    QString getSearchText() const;
    void clearSearch();
    void updateThemeButton();
    void applyStyles();

private slots:
    void onThemeToggled();
    void onThemeChanged(DesignSystem::Theme theme);

signals:
    void searchTextChanged(const QString& text);
    void settingsClicked();
};

#endif // TOP_TOOLBAR_H
