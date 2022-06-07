#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSlider>
#include <QTimer>
#include <QCloseEvent>
#include <QLabel>
#include <QListWidget>
#include <QSpacerItem>

#include <QMainWindow>

#include <gst/gst.h>
#include <glib.h>

#include "common.h"

extern "C"
{
#include <mediaPlayer.h>
}

class PlayerWindow : public QMainWindow
{
    Q_OBJECT
public:
    PlayerWindow(ST_USER_HANDLE **pstUserHandle);
    ~PlayerWindow();
    WId getVideoWId();

signals:

public slots:
    void onControlBtClicked();
    void refreshSlider();
    void doSeek();
    /*copy*/
    /* 播放按钮点击 */
    void btn_play_clicked();

    /* 下一个视频按钮点击 */
    void btn_next_clicked();

    /* 前一个视频按钮点击 */
    void btn_pre_clicked();

    /* 音量加 */
    void btn_volmeup_clicked();

    /* 音量减 */
    void btn_volmedown_clicked();

    /* 全屏 */
    void btn_fullscreen_clicked();

    /* 列表单击 */
    void listWidgetCliked(QListWidgetItem*);

    /* 媒体列表项改变 */
    void mediaPlaylistCurrentIndexChanged(int);

    /* 媒体总长度改变 */
    void musicPlayerDurationChanged(qint64);

    /* 媒体播放位置改变 */
    void mediaPlayerPositionChanged(qint64);

    /* 播放进度条松开 */
    void durationSliderReleased();

    /* 音量条松开 */
    void volumeSliderReleased();

private:
  ST_USER_HANDLE *pstHandle;

  QTimer *timer;

  GstState state;
  gint64 totalDuration;
  HANDLE_ID hanldeId;

  /*视频播放窗口*/
  QWidget *videoWidget;
  /*copy from zdyz*/
  /* 视频列表 */
  QListWidget *listWidget;

  /* 播放进度条 */
  QSlider *durationSlider;

  /* 音量条 */
  QSlider *volumeSlider;

  /* 视频播放器按钮 */
  QPushButton *pushButton[6];

  /* 水平布局 */
  QHBoxLayout *hBoxLayout[3];

  /* 水平容器 */
  QWidget *hWidget[3];

  /* 标签文本 */
  QLabel *label[2];

  /* 垂直容器 */
  QWidget *vWidget[2];

  /* 垂直界面 */
  QVBoxLayout *vBoxLayout[2];

  /*function*/
  /* 视频布局函数 */
  void videoLayout();

  /* 主窗体大小重设大小函数重写 */
  void resizeEvent(QResizeEvent *event);

  /* 扫描本地视频文件 */
  void scanVideoFiles();

  /* 媒体初始化 */
  void mediaPlayerInit();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // PLAYERWINDOW_H
