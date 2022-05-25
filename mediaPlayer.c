
#include <gst/gst.h>
#include <glib.h>

/* playbin flags */
typedef enum {
    GST_PLAY_FLAG_VIDEO         = (1 << 0), /* We want video output */
    GST_PLAY_FLAG_AUDIO         = (1 << 1), /* We want audio output */
    GST_PLAY_FLAG_TEXT          = (1 << 2)  /* We want subtitle output */
} GstPlayFlags;

typedef struct _ST_MEDIA_HANDLE {
    char *filePath;
    GThread *playThread;
    GstElement *pipeline;  /* Our one and only element */
    GMainLoop *main_loop;  /* GLib's Main Loop */
    GstBus *bus;
    gint flags;
} ST_MEDIA_HANDLE;

ST_MEDIA_HANDLE *mediaHandle;

void _paly_thread(void);
gboolean handle_message (GstBus *bus, GstMessage *msg, ST_MEDIA_HANDLE *mediaHandle);

void _paly_thread(void)
{
    GstStateChangeReturn ret;

    mediaHandle->pipeline = gst_element_factory_make ("playbin", "media_playbin");
    if (!mediaHandle->pipeline) 
    {
        g_printerr ("Not all elements could be created.\n");
        goto exit;
    }

    /* Set the URI to play */
    g_object_set (mediaHandle->pipeline, "uri", mediaHandle->filePath, NULL);

    /* Set flags to show Audio and Video but ignore Subtitles */
    g_object_get (mediaHandle->pipeline, "flags", &mediaHandle->flags, NULL);
    mediaHandle->flags |= GST_PLAY_FLAG_VIDEO | GST_PLAY_FLAG_AUDIO;
    mediaHandle->flags &= ~GST_PLAY_FLAG_TEXT;
    g_object_set (mediaHandle->pipeline, "flags", mediaHandle->flags, NULL);

        /* Add a bus watch, so we get notified when a message arrives */
    mediaHandle->bus = gst_element_get_bus (mediaHandle->pipeline);
    gst_bus_add_watch (mediaHandle->bus, (GstBusFunc)handle_message, mediaHandle);

    gst_object_unref (mediaHandle->bus);
    mediaHandle->bus = NULL;

    /* Start playing */
    ret = gst_element_set_state (mediaHandle->pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        goto exit;
    }

    /* Create a GLib Main Loop and set it to run */
    mediaHandle->main_loop = g_main_loop_new (NULL, FALSE);
    g_main_loop_run (mediaHandle->main_loop);
    g_print("main loop run stop. \n");

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

    if (mediaHandle)
    {
        free(mediaHandle);
    }
    
    return NULL;
    
}
int Start_Play(char *path)
{
    int ret = 0;

    g_print("start play IN. \n");

    if (path == NULL)
    {
        g_printerr ("file path is NULL.\n");
        ret = -1;
        goto end;
    }
    
    mediaHandle = (ST_MEDIA_HANDLE *)malloc(sizeof(ST_MEDIA_HANDLE));
    if (!mediaHandle)
    {
        g_printerr ("malloc mediaHandle fail.\n");
        ret = -1;
        goto end;
    }

    mediaHandle->filePath = path;

    gst_init(NULL, NULL);

    mediaHandle->playThread = g_thread_new("paly_thread", _paly_thread, NULL);
    if (!mediaHandle->playThread)
    {
        g_printerr ("create paly thread fail.\n");
        ret = -1;
    }
    g_print("start play OUT. \n");
end:
    return ret;
}

int Stop_Play(void)
{ 
    GstStateChangeReturn stateRet;
    int ret = 0;

    g_print("stop play IN. \n");

    if (!mediaHandle || !mediaHandle->pipeline)
    {
        g_printerr ("handle or pipeline is NULL.\n");
        ret = -1;
        goto end;
    }

    stateRet = gst_element_set_state (mediaHandle->pipeline, GST_STATE_NULL);
    if (stateRet == GST_STATE_CHANGE_FAILURE) 
    {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        ret = -1;
        goto end;
    }

    g_main_loop_quit (mediaHandle->main_loop);
    g_print("stop play OK. \n");
end:
    return ret;
}

gboolean handle_message (GstBus *bus, GstMessage *msg, ST_MEDIA_HANDLE *mediaHandle) {
  GError *err;
  gchar *debug_info;
  GstState old_state, new_state, pending_state;
  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_ERROR:
        gst_message_parse_error (msg, &err, &debug_info);
        g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
        g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
        g_clear_error (&err);
        g_free (debug_info);
        g_main_loop_quit (mediaHandle->main_loop);
        break;
    case GST_MESSAGE_EOS:
        g_print ("End-Of-Stream reached.\n");
        g_main_loop_quit (mediaHandle->main_loop);
        break;
    case GST_MESSAGE_STATE_CHANGED: 
        gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
        if (GST_MESSAGE_SRC (msg) == GST_OBJECT (mediaHandle->pipeline)) 
        {
            if (new_state == GST_STATE_PLAYING) 
            {
                g_print("starting play media. \n");
            }
        }
        break;
    }

}

int main()
{
    int ret = 0;
    char *filePath = "https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_cropped_multilingual.webm";
    
    ret = Start_Play(filePath);
    if (ret != 0)
    {
        g_printerr ("play file %s error.\n", filePath);
        return - 1;
    }

    //play 50s
    sleep(50);

    ret = Stop_Play();
    if (ret != 0)
    {
        g_printerr ("stop play file %s error.\n", filePath);
        return - 1;
    }

    return 0;
}