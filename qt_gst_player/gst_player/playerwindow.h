#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSlider>
#include <QTimer>
#include <QCloseEvent>

#include <gst/gst.h>
#include <glib.h>

#include "common.h"

extern "C"
{
#include <mediaPlayer.h>
}

class PlayerWindow : public QWidget
{
    Q_OBJECT
public:
    PlayerWindow(ST_USER_HANDLE **pstUserHandle);
    WId getVideoWId();

signals:

public slots:
    void onPlayClicked();
    void onPauseClicked();
    void onStopClicked();

private:
  ST_USER_HANDLE *pstHandle;
  GstElement *pipeline;
  QPushButton *playBt;
  QPushButton *pauseBt;
  QPushButton *stopBt;
  QWidget *videoWindow;
  QSlider *slider;
  QHBoxLayout *buttonLayout;
  QVBoxLayout *playerLayout;
  QTimer *timer;
  QSlider *volumeSlider;

  GstState state;
  gint64 totalDuration;
  HANDLE_ID hanldeId;

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // PLAYERWINDOW_H
