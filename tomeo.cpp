#include <QApplication>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QImageReader>
#include <QMessageBox>
#include <vector>

#include "main_container.h"
#include "the_button.h"
#include "theme_manager.h"
#include "language_manager.h"
#include "social_manager.h"

// 辅助函数: 扫描目录获取视频
std::vector<TheButtonInfo> getInfoIn(std::string loc) {
    std::vector<TheButtonInfo> out;
    QDir dir(QString::fromStdString(loc));

    if (!dir.exists()) return out;

    QDirIterator it(dir);
    while (it.hasNext()) {
        QString f = it.next();
        QFileInfo fileInfo(f);

        if (fileInfo.isDir() || fileInfo.fileName().startsWith(".")) continue;

        QString lowerF = f.toLower();
        if (lowerF.endsWith(".mp4") || lowerF.endsWith(".mov") ||
            lowerF.endsWith(".wmv") || lowerF.endsWith(".avi")) {

            QString thumb = f.left(f.length() - 4) + ".png";
            if (QFile(thumb).exists()) {
                QImageReader* imageReader = new QImageReader(thumb);
                QImage sprite = imageReader->read();
                if (!sprite.isNull()) {
                    QIcon* ico = new QIcon(QPixmap::fromImage(sprite));
                    QUrl* url = new QUrl(QUrl::fromLocalFile(f));
                    out.push_back(TheButtonInfo(url, ico, fileInfo.baseName(), 0, fileInfo.size()));
                }
            }
        }
    }
    return out;
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Tomeo");

    // 1. 初始化管理器
    ThemeManager::getInstance();
    SocialManager::getInstance();

    // 2. 加载视频
    QString videoPath = (argc == 2) ? QString::fromLocal8Bit(argv[1]) : QDir::currentPath() + "/videos";
    if (videoPath.startsWith('"')) videoPath = videoPath.mid(1, videoPath.length() - 2);

    std::vector<TheButtonInfo> videos = getInfoIn(videoPath.toStdString());

    // 加载真实缩略图
    SocialManager::getInstance()->loadRealThumbnails(videoPath);

    // 3. 创建主窗口
    MainContainer window(videos);
    window.setWindowTitle("Tomeo - Social Video Platform");
    window.setMinimumSize(375, 667);
    window.resize(450, 800);

    // 应用主题
    window.setStyleSheet(ThemeManager::getInstance()->getApplicationStyleSheet());

    // 连接主题变更信号
    QObject::connect(ThemeManager::getInstance(), &ThemeManager::themeChanged,
                     &window, &MainContainer::updateTheme);

    window.show();

    return app.exec();
}
