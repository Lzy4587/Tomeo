//
// LanguageManager - 语言管理器实现
//

#include "language_manager.h"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QTranslator>
#include <QSettings>

LanguageManager* LanguageManager::instance = nullptr;

LanguageManager::LanguageManager(QObject* parent)
    : QObject(parent),
    currentLanguage(English),
    translator(new QTranslator(this)),
    settings(new QSettings("Tomeo", "LanguageSettings", this)) {

    // 加载保存的语言设置
    int savedLang = settings->value("language", English).toInt();
    currentLanguage = static_cast<Language>(savedLang);

    loadLanguage(currentLanguage);

    qDebug() << "LanguageManager initialized with language:" << getLanguageName();
}

LanguageManager* LanguageManager::getInstance() {
    if (instance == nullptr) {
        instance = new LanguageManager();
    }
    return instance;
}

void LanguageManager::setLanguage(Language lang) {
    if (currentLanguage == lang) {
        return;
    }

    qDebug() << "Changing language from" << getLanguageName() << "to" << getLanguageDisplayName(lang);

    if (loadLanguage(lang)) {
        currentLanguage = lang;

        // 保存设置
        settings->setValue("language", static_cast<int>(lang));
        settings->sync();

        emit languageChanged(lang);
        qDebug() << "Language successfully changed to:" << getLanguageName();
    }
}

bool LanguageManager::loadLanguage(Language lang) {
    // 移除旧的翻译
    qApp->removeTranslator(translator);

    // 如果是英语，不需要加载翻译文件
    if (lang == English) {
        qDebug() << "Using default English";
        return true;
    }

    // 构建翻译文件名
    QString langCode = getLanguageCode(lang);
    QString translationFile = QString("tomeo_%1").arg(langCode);

    // 尝试多个可能的路径
    QStringList searchPaths;
    searchPaths << QApplication::applicationDirPath() + "/translations"
                << QApplication::applicationDirPath() + "/../translations"
                << "./translations"
                << "../translations";

    bool loaded = false;
    for (const QString& path : searchPaths) {
        QString fullPath = path + "/" + translationFile;
        qDebug() << "Trying to load translation from:" << fullPath;

        if (translator->load(translationFile, path)) {
            qDebug() << "✓ Successfully loaded translation:" << fullPath;
            loaded = true;
            break;
        }
    }

    if (!loaded) {
        qDebug() << "✗ Warning: Could not load translation file for" << getLanguageDisplayName(lang);
        return false;
    }

    // 安装翻译器
    qApp->installTranslator(translator);
    qDebug() << "Translation installed for:" << getLanguageDisplayName(lang);

    return true;
}

QString LanguageManager::getLanguageName() const {
    return getLanguageDisplayName(currentLanguage);
}

QString LanguageManager::getLanguageCode() const {
    return getLanguageCode(currentLanguage);
}

QString LanguageManager::getLanguageCode(Language lang) {
    switch (lang) {
    case Chinese:
        return "zh_CN";
    case Spanish:
        return "es_ES";
    case English:
    default:
        return "en";
    }
}

QStringList LanguageManager::getAvailableLanguages() {
    return QStringList() << "English" << "中文" << "Español";
}

QString LanguageManager::getLanguageDisplayName(Language lang) {
    switch (lang) {
    case Chinese:
        return "中文";
    case Spanish:
        return "Español";
    case English:
    default:
        return "English";
    }
}
