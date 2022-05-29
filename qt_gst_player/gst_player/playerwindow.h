#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSlider>
#include <QTimer>

#include <gst/gst.h>
#include <glib.h>

class PlayerWindow : public QWidget
{
    Q_OBJECT
public:
    PlayerWindow(GstElement *pipeline);
    WId getVideoWId();

signals:

public slots:

private:
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

};

#endif // PLAYERWINDOW_H
