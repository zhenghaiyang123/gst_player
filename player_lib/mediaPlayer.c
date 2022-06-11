
#include <gst/gst.h>
#include <glib.h>

#include "mediaPlayer.h"

#define LIB_NAME "mmplayer"

static LOG_LEVEL log_level;

#define LOG_ENTER()	\
do {							\
	if (log_level >= LOG_LEVEL_TRACE) {	\
		g_print("[%s][IN][%s][%d]\n", LIB_NAME, __FUNCTION__, __LINE__);	\
	}	\
} while(0)

#define LOG_OUT()	\
do {							\
	if (log_level >= LOG_LEVEL_TRACE) {	\
		g_print("[%s][OUT][%s][%d]\n", LIB_NAME, __FUNCTION__, __LINE__);	\
	}	\
} while(0)

#define LOG_DEBUG(format,...)	\
do {							\
	if (log_level >= LOG_LEVEL_DEBUG) {	\
		g_print("[%s][DEBUG][%s][%d]"format"", LIB_NAME, __FUNCTION__, __LINE__, ##__VA_ARGS__);	\
	}	\
} while(0)
 
#define LOG_INFO(format,...)	\
do {							\
	if (log_level >= LOG_LEVEL_INFO) {	\
		g_print("[%s][INFO][%s][%d]"format"", LIB_NAME, __FUNCTION__, __LINE__, ##__VA_ARGS__);	\
	}	\
} while(0)
 
#define LOG_WARN(format,...)	\
do {							\
	if (log_level >= LOG_LEVEL_WARN) {	\
		g_print("[%s][WARN][%s][%d]"format"", LIB_NAME, __FUNCTION__, __LINE__, ##__VA_ARGS__);	\
	}	\
} while(0)
 
#define LOG_ERROR(format,...)	\
do {							\
	if (log_level >= LOG_LEVEL_ERROR) {	\
		g_print("[%s][ERROR][%s][%d]"format"", LIB_NAME, __FUNCTION__, __LINE__, ##__VA_ARGS__);	\
	}	\
} while(0)

/* playbin flags */
typedef enum {
    GST_PLAY_FLAG_VIDEO         = (1 << 0), /* We want video output */
    GST_PLAY_FLAG_AUDIO         = (1 << 1), /* We want audio output */
    GST_PLAY_FLAG_TEXT          = (1 << 2)  /* We want subtitle output */
} GstPlayFlags;

typedef struct _ST_MEDIA_HANDLE {
    HANDLE_ID handleId;
    char *filePath;
    GThread *playThread;
    GstElement *pipeline;  /* Our one and only element */
    GMainLoop *main_loop;  /* GLib's Main Loop */
    GstBus *bus;
    gint flags;
    CALL_BACK_FUNCTION hanlecallBackFn;
    ST_HANDLE_INFO handleInfo;
} ST_MEDIA_HANDLE;

ST_MEDIA_HANDLE *mediaHandle;

void _paly_thread(void);
gboolean handle_message (GstBus *bus, GstMessage *msg, ST_MEDIA_HANDLE *mediaHandle);
void handle_element_added(GstBin *bin, GstElement *element, ST_MEDIA_HANDLE *mediaHandle);

int MMPlayerInit(ST_INIT_PARAM *pstInitParam)
{
    int ret = 0;

    LOG_ENTER();

    if (pstInitParam->path == NULL)
    {
        LOG_ERROR ("file path is NULL.\n");
        ret = -1;
        goto end;
    }
    
    if (pstInitParam->path == NULL)
    {
        LOG_ERROR ("callback funtion is NULL.\n");
        ret = -1;
        goto end;
    }

    mediaHandle = (ST_MEDIA_HANDLE *)malloc(sizeof(ST_MEDIA_HANDLE));
    if (!mediaHandle)
    {
        LOG_ERROR ("malloc mediaHandle fail.\n");
        ret = -1;
        goto end;
    }

    mediaHandle->filePath = pstInitParam->path;
    mediaHandle->hanlecallBackFn = pstInitParam->callBackFunction;
    log_level = pstInitParam->logLevel;

    gst_init(NULL, NULL);

    mediaHandle->playThread = g_thread_new("paly_thread", _paly_thread, NULL);
    if (!mediaHandle->playThread)
    {
        LOG_ERROR ("create paly thread fail.\n");
        ret = -1;
    }

    LOG_OUT();
end:
    return ret;
}

int MMPlayerPlay(HANDLE_ID hanldeId)
{
    GstStateChangeReturn stateRet;
    int ret = 0;

    LOG_ENTER();

    if (!mediaHandle || !mediaHandle->pipeline)
    {
        LOG_ERROR ("handle or pipeline is NULL.\n");
        ret = -1;
        goto end;
    }

    stateRet = gst_element_set_state (mediaHandle->pipeline, GST_STATE_PLAYING);
    if (stateRet == GST_STATE_CHANGE_FAILURE) 
    {
        LOG_ERROR ("Unable to set the pipeline to the playing state.\n");
        ret = -1;
        goto end;
    }

   LOG_OUT();
end:
    if (ret != 0)
    {
        g_main_loop_quit (mediaHandle->main_loop);
    }
    return ret;
}

int MMPlayerPause(HANDLE_ID hanldeId)
{
    GstStateChangeReturn stateRet;
    int ret = 0;

    LOG_ENTER();

    if (!mediaHandle || !mediaHandle->pipeline)
    {
        LOG_ERROR ("handle or pipeline is NULL.\n");
        ret = -1;
        goto end;
    }

    stateRet = gst_element_set_state (mediaHandle->pipeline, GST_STATE_PAUSED);
    if (stateRet == GST_STATE_CHANGE_FAILURE) 
    {
        LOG_ERROR ("Unable to set the pipeline to the pause state.\n");
        ret = -1;
        goto end;
    }

   LOG_OUT();
end:
    if (ret != 0)
    {
        g_main_loop_quit (mediaHandle->main_loop);
    }
    return ret;
}

int MMPlayerDoSeek(HANDLE_ID hanldeId, gint64 seekPos)
{
    LOG_ENTER();
    int ret = 0;

    //find handle
    if (hanldeId != mediaHandle->handleId)
    {
        LOG_ERROR ("user hanlde id (%d) do not match media handle id(%d).\n", hanldeId, mediaHandle->handleId);
        ret = -1;
        goto end;
    }

    ret = gst_element_seek_simple (mediaHandle->pipeline, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH ,
                  seekPos);
end:
    LOG_OUT();
    return ret;

}

int MMPlayerGetDuration(HANDLE_ID hanldeId, gint64 *duration)
{
    LOG_ENTER();
    int ret = 0;

    //find handle
    if (hanldeId != mediaHandle->handleId)
    {
        LOG_ERROR ("user hanlde id (%d) do not match media handle id(%d).\n", hanldeId, mediaHandle->handleId);
        ret = -1;
        goto end;
    }

    if (!gst_element_query_duration (mediaHandle->pipeline, GST_FORMAT_TIME, duration))
    {
        LOG_ERROR ("get media duration fail.\n");
        ret = -1;
    }

    LOG_INFO ("get media duration = %d.\n", *duration);
    

end:
    LOG_OUT();
    return ret;
}

int MMPlayerGetPostion(HANDLE_ID hanldeId, gint64 *postion)
{
    //LOG_ENTER();
    int ret = 0;
    
    //find handle
    if (hanldeId != mediaHandle->handleId)
    {
        LOG_ERROR ("user hanlde id (%d) do not match media handle id(%d).\n", hanldeId, mediaHandle->handleId);
        ret = -1;
        goto end;
    }

    if (!gst_element_query_position (mediaHandle->pipeline, GST_FORMAT_TIME, postion))
    {
        LOG_ERROR ("get media posion fail.\n");
        ret = -1;
    }

     LOG_INFO ("get media postion = %d.\n", postion);
    

end:
    //LOG_OUT();
    return ret;
}

int MMPlayerStop(HANDLE_ID hanldeId)
{ 
    GstStateChangeReturn stateRet;
    int ret = 0;

    LOG_ENTER();

    if (!mediaHandle || !mediaHandle->pipeline)
    {
        LOG_ERROR ("handle or pipeline is NULL.\n");
        ret = -1;
        goto end;
    }

    stateRet = gst_element_set_state (mediaHandle->pipeline, GST_STATE_NULL);
    if (stateRet == GST_STATE_CHANGE_FAILURE) 
    {
        LOG_ERROR ("Unable to set the pipeline to the NULL state.\n");
        ret = -1;
        goto end;
    }

    g_main_loop_quit (mediaHandle->main_loop);


    LOG_OUT();
end:
    return ret;
}

void _paly_thread(void)
{
    GstStateChangeReturn ret;
    GstElement *videosink;
    mediaHandle->pipeline = gst_element_factory_make ("playbin3", "media_playbin");
    if (!mediaHandle->pipeline) 
    {
        LOG_ERROR ("Not all elements could be created.\n");
        goto exit;
    }

    /* Set the URI to play */
    g_object_set (mediaHandle->pipeline, "uri", mediaHandle->filePath, NULL);

    /* Set flags to show Audio and Video but ignore Subtitles */
    g_object_get (mediaHandle->pipeline, "flags", &mediaHandle->flags, NULL);
    mediaHandle->flags |= GST_PLAY_FLAG_VIDEO | GST_PLAY_FLAG_AUDIO;
    mediaHandle->flags &= ~GST_PLAY_FLAG_TEXT;
    g_object_set (mediaHandle->pipeline, "flags", mediaHandle->flags, NULL);

    g_signal_connect(G_OBJECT(mediaHandle->pipeline), "element-added",  G_CALLBACK(handle_element_added), mediaHandle);

    /* Add a bus watch, so we get notified when a message arrives */
    mediaHandle->bus = gst_element_get_bus (mediaHandle->pipeline);
    gst_bus_add_watch (mediaHandle->bus, (GstBusFunc)handle_message, mediaHandle);

    gst_object_unref (mediaHandle->bus);
    mediaHandle->bus = NULL;


    /*set pipeline to pause*/
    ret = gst_element_set_state (mediaHandle->pipeline, GST_STATE_PAUSED);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        LOG_ERROR ("Unable to set the pipeline to the pause state.\n");
        goto exit;
    }

    mediaHandle->handleId = mediaHandle->handleInfo.handleId = (int)mediaHandle;
    mediaHandle->handleInfo.handleStatus = READY_STATUS;
    mediaHandle->handleInfo.pipeline = mediaHandle->pipeline;
    if (mediaHandle->hanlecallBackFn)
    {
       (mediaHandle->hanlecallBackFn)(PLAYER_INIT_OK ,(void *)&mediaHandle->handleInfo);
    }

    /* Create a GLib Main Loop and set it to run */
    mediaHandle->main_loop = g_main_loop_new (NULL, FALSE);
    g_main_loop_run (mediaHandle->main_loop);
    LOG_INFO("main loop run stop. \n");

exit:
    if (mediaHandle->pipeline)
    {
        gst_object_unref (mediaHandle->pipeline);

    }

    g_thread_unref(mediaHandle->playThread);
    mediaHandle->playThread = NULL;

    if (mediaHandle->main_loop)
    {
        g_main_loop_unref (mediaHandle->main_loop);
    }
    
    mediaHandle->handleInfo.handleStatus = STOP_STATUS;
    if (mediaHandle->hanlecallBackFn)
    {
       (mediaHandle->hanlecallBackFn)(PLAYER_STOP_OK ,(void *)&mediaHandle->handleInfo);
    }
    
    if (mediaHandle)
    {
        free(mediaHandle);
    }


    return NULL;
    
}

gboolean handle_message (GstBus *bus, GstMessage *msg, ST_MEDIA_HANDLE *mediaHandle) {
  GError *err;
  gchar *debug_info;
  GstState old_state, new_state, pending_state;
  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_ERROR:
        gst_message_parse_error (msg, &err, &debug_info);
        LOG_ERROR ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
        LOG_ERROR ("Debugging information: %s\n", debug_info ? debug_info : "none");
        g_clear_error (&err);
        g_free (debug_info);
        g_main_loop_quit (mediaHandle->main_loop);
        break;
    case GST_MESSAGE_EOS:
        LOG_INFO ("End-Of-Stream reached.\n");
        g_main_loop_quit (mediaHandle->main_loop);
        break;
    case GST_MESSAGE_STATE_CHANGED: 
        gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
        if (GST_MESSAGE_SRC (msg) == GST_OBJECT (mediaHandle->pipeline)) 
        {
            if (new_state == GST_STATE_PLAYING) 
            {
                LOG_INFO("starting play media. \n");
            }
        }
        break;
    }

}

void handle_element_added(GstBin *bin, GstElement *element, ST_MEDIA_HANDLE *mediaHandle)
{
    //Q_UNUSED(bin);

    gchar *elementName = gst_element_get_name(element);
    LOG_INFO("elementName %s. \n", elementName);

    if (g_str_has_prefix(elementName, "video-sink")) {
        LOG_INFO("find video-sink in element_add function. \n");
    } 
    else if (g_str_has_prefix(elementName, "ximagesink")) {
        LOG_INFO("find ximagesink in element_add function. \n");
    } 
    else if (g_str_has_prefix(elementName, "uridecodebin3") 
    || g_str_has_prefix(elementName, "decodebin2") 
    || g_str_has_prefix(elementName, "urisourcebin0")
    ) {
        g_signal_connect(element, "element-added",
                         G_CALLBACK(handle_element_added), mediaHandle);
    }

    g_free(elementName);
}