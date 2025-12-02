QT += core gui widgets multimedia multimediawidgets

CONFIG += c++11
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

TARGET = tomeo
TEMPLATE = app

SOURCES += \
    tomeo.cpp \
    the_player.cpp \
    the_button.cpp \
    playback_controls.cpp \
    theme_manager.cpp \
    language_manager.cpp \
    top_toolbar.cpp \
    social_manager.cpp \
    video_post_card.cpp \
    social_feed_widget.cpp \
    comment_dialog.cpp \
    daily_reminder_dialog.cpp \
    settings_dialog.cpp \
    share_dialog.cpp \
    bottom_navigation_bar.cpp \
    record_dialog.cpp \
    main_container.cpp

HEADERS += \
    the_player.h \
    the_button.h \
    playback_controls.h \
    design_system.h \
    theme_manager.h \
    language_manager.h \
    top_toolbar.h \
    social_types.h \
    social_manager.h \
    social_icons.h \
    video_post_card.h \
    social_feed_widget.h \
    comment_dialog.h \
    daily_reminder_dialog.h \
    settings_dialog.h \
    share_dialog.h \
    bottom_navigation_bar.h \
    record_dialog.h \
    main_container.h

INCLUDEPATH += .

TRANSLATIONS += \
    translations/tomeo_zh_CN.ts \
    translations/tomeo_es_ES.ts
