#include <gst/gst.h>
#include <glib.h>

#include <QApplication>
#include <QTime>
#include <gst/video/videooverlay.h>

#include "playerwindow.h"

extern "C"
{
#include <mediaPlayer.h>
}

typedef struct _ST_USER_HANDLE {
    gint handleId;
    GstElement *pipeline;  /* Our one and only element */
    GstElement  *videoSink;
} ST_USER_HANDLE;

void sleep(unsigned int msec);
void hanleCallBackEvent(CALL_BACK_EVENT_TYPE eventType, void *param);

ST_USER_HANDLE userHandle;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    int ret = 0;
    ST_INIT_PARAM initParam;
    WId xwinid;
    int maxWaitCount =10;

    initParam.path = "https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_cropped_multilingual.webm";
    initParam.callBackFunction = hanleCallBackEvent;
    ret = MMPlayerInit(&initParam);
    if (ret != 0)
    {
        g_printerr ("play file %s error.\n", initParam.path);
        return - 1;
    }

    //play mm in QT window
    PlayerWindow *window = new PlayerWindow(userHandle.pipeline);
    window->setWindowTitle("Qt&&GStreamer Player demo");
    window->resize(900, 600);
    xwinid = window->getVideoWId();

    //wait playsink added
    while (NULL == userHandle.videoSink)
    {
        userHandle.videoSink = gst_bin_get_by_name (GST_BIN (userHandle.pipeline), "playsink");
        maxWaitCount++;
        if (maxWaitCount > 20)
        {
            g_printerr ("can not find video sink.\n", initParam.path);
            goto stop;
        }
        sleep(100);
    }

    gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY(userHandle.videoSink), xwinid);

    if (userHandle.videoSink)
    {
         gst_object_unref (userHandle.videoSink);
    }

    MMPlayerPlay(userHandle.handleId);
    sleep(1000);
    window->show();

    //play 50s
    sleep(20 * 1000);

stop:
    ret = MMPlayerStop(userHandle.handleId);
    if (ret != 0)
    {
        g_printerr ("stop play file %s error.\n", initParam.path);
        ret = - 1;
    }
    return ret;
    //return app.exec();
}

void sleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void hanleCallBackEvent(CALL_BACK_EVENT_TYPE eventType, void *param)
{
    g_print("recive event type (%d)\n", eventType);
    switch (eventType)
    {
        case PLAYER_INIT_OK:
        {
            g_print("player init OK! \n");
            ST_HANDLE_INFO *retHanleInfo = (ST_HANDLE_INFO *)param;
            userHandle.handleId = retHanleInfo->handleId;
            userHandle.pipeline = retHanleInfo->pipeline;
        }
            break;

        default:
            break;
    }
}
