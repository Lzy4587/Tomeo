//
// RecordDialog - 视频录制对话框
// 模拟相机录制功能
//

#ifndef RECORD_DIALOG_H
#define RECORD_DIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QMediaRecorder>
#include <QProgressBar>
#include <QCloseEvent>
#include <QShowEvent>
#include "social_types.h"

class RecordDialog : public QDialog {
    Q_OBJECT

private:
    // UI组件
    QLabel* viewfinderLabel;        // 取景器预览
    QPushButton* recordBtn;          // 录制按钮
    QPushButton* switchCameraBtn;    // 切换摄像头
    QPushButton* closeBtn;           // 关闭按钮
    QPushButton* flashBtn;           // 闪光灯
    QLabel* timerLabel;              // 录制时长
    QProgressBar* progressBar;       // 录制进度

    // 底部操作按钮
    QPushButton* useVideoBtn;        // 使用视频
    QPushButton* retakeBtn;          // 重新录制

    // 录制状态
    bool isRecording;
    bool isFrontCamera;
    int recordingSeconds;
    int maxRecordingSeconds;

    QTimer* recordTimer;

    // 相机相关（可选，如果系统支持）
    QCamera* camera;
    QMediaRecorder* mediaRecorder;

    QString recordedVideoPath;

    void setupUI();
    void connectSignals();
    void applyStyles();
    void updateRecordButton();
    void updateTimer();
    void showPreviewControls(bool show);

    // 模拟录制（如果没有真实相机）
    void simulateRecording();

protected:
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

public:
    explicit RecordDialog(QWidget* parent = nullptr);
    ~RecordDialog();

    QString getRecordedVideoPath() const { return recordedVideoPath; }
    bool isFrontCameraUsed() const { return isFrontCamera; }

private slots:
    void onRecordClicked();
    void onSwitchCameraClicked();
    void onFlashClicked();
    void onCloseClicked();
    void onUseVideoClicked();
    void onRetakeClicked();
    void onRecordTimerTimeout();
    void onRecordingFinished();

signals:
    void videoRecorded(const QString& videoPath, bool isFrontCamera);
    void recordingCancelled();
};

#endif // RECORD_DIALOG_H
