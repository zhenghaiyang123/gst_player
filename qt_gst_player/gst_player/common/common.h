#ifndef COMMON_H
#define COMMON_H



#include <gst/gst.h>
#include <glib.h>

extern "C"
{
#include <mediaPlayer.h>
}

#define MAX_QUEUE_ESIZE 4
typedef enum {
    CMD_PLAY,
    CMD_PAUSE,
    CMD_RESUME,
    CMD_STOP,
    CMD_SEEK,
    CMD_ALL,
} MM_PLAYER_CMD_TYPE;

typedef struct _ST_PLAYER_CMD
{
    MM_PLAYER_CMD_TYPE type;
    int seekTime;
    int volumeValue;
} ST_PLAYER_CMD;

typedef struct _ST_PLAYER_CMD_QUEUE
{
    ST_PLAYER_CMD *cmd[MAX_QUEUE_ESIZE];
    int front;
    int rear;
    int length;
} ST_PLAYER_CMD_QUEUE;

typedef struct _ST_USER_HANDLE {
    gint handleId;
    HANDLE_STATUS handleStatus;
    GThread *controlThread;
    GstElement *pipeline;  /* Our one and only element */
    GstElement  *videoSink;
    ST_PLAYER_CMD_QUEUE *cmdQueue;
    GMutex queueMutex;
    GCond queueCond;
    GMutex quitMutex;
    GCond quitCond;
} ST_USER_HANDLE;

bool cmdQueueInit(ST_PLAYER_CMD_QUEUE **cmdQueue);
bool cmdQueueIsEmpty(ST_PLAYER_CMD_QUEUE *cmdQueue);
bool cmdQueueIsFull(ST_PLAYER_CMD_QUEUE *cmdQueue);
bool cmdQueuePush(ST_PLAYER_CMD_QUEUE *cmdQueue);
ST_PLAYER_CMD *cmdQueuePop(ST_PLAYER_CMD_QUEUE *cmdQueue);
bool cmdQueueDeInit(ST_PLAYER_CMD_QUEUE *cmdQueue);
bool createCmdAndPush(ST_USER_HANDLE *userHandle, MM_PLAYER_CMD_TYPE type, void *data);

static LOG_LEVEL user_log_level = LOG_LEVEL_TRACE;
#define LIB_NAME "userplayer"

#define LOG_ENTER()	\
do {							\
    if (user_log_level >= LOG_LEVEL_TRACE) {	\
        g_print("[%s][IN][%s][%d]\n", LIB_NAME, __FUNCTION__, __LINE__);	\
    }	\
} while(0)

#define LOG_OUT()	\
do {							\
    if (user_log_level >= LOG_LEVEL_TRACE) {	\
        g_print("[%s][OUT][%s][%d]\n", LIB_NAME, __FUNCTION__, __LINE__);	\
    }	\
} while(0)

#define LOG_DEBUG(format,...)	\
do {							\
    if (user_log_level >= LOG_LEVEL_DEBUG) {	\
        g_print("[%s][DEBUG][%s][%d] " format " ", LIB_NAME , __FUNCTION__ , __LINE__ , ##__VA_ARGS__ );	\
    }	\
} while(0)

#define LOG_INFO(format,...)	\
do {							\
    if (user_log_level >= LOG_LEVEL_INFO) {	\
        g_print("[%s][INFO][%s][%d]" format "", LIB_NAME , __FUNCTION__ , __LINE__ , ##__VA_ARGS__ );	\
    }	\
} while(0)

#define LOG_WARN(format,...)	\
do {							\
    if (user_log_level >= LOG_LEVEL_WARN) {	\
        g_print("[%s][WARN][%s][%d]" format "", LIB_NAME, __FUNCTION__, __LINE__, ##__VA_ARGS__);	\
    }	\
} while(0)

#define LOG_ERROR(format,...)	\
do {							\
    if (user_log_level >= LOG_LEVEL_ERROR) {	\
        g_print("[%s][ERROR][%s][%d]" format "", LIB_NAME, __FUNCTION__, __LINE__, ##__VA_ARGS__);	\
    }	\
} while(0)
#endif // COMMON_H
