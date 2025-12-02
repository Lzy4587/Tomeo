//
// LanguageManager - 语言管理器
//

#ifndef LANGUAGE_MANAGER_H
#define LANGUAGE_MANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTranslator>
#include <QSettings>

class LanguageManager : public QObject {
    Q_OBJECT

public:
    enum Language {
        English = 0,
        Chinese = 1,
        Spanish = 2
    };

private:
    Language currentLanguage;
    QTranslator* translator;
    QSettings* settings;
    static LanguageManager* instance;

    explicit LanguageManager(QObject* parent = nullptr);
    bool loadLanguage(Language lang);

public:
    static LanguageManager* getInstance();

    void setLanguage(Language lang);
    Language getLanguage() const { return currentLanguage; }
    QString getLanguageName() const;
    QString getLanguageCode() const;

    static QStringList getAvailableLanguages();
    static QString getLanguageDisplayName(Language lang);
    static QString getLanguageCode(Language lang);

signals:
    void languageChanged(Language lang);
};

#endif // LANGUAGE_MANAGER_H
