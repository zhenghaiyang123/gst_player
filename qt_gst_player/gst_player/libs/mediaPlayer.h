#ifndef __MEDIAPLAYER_H__ 
#define __MEDIAPLAYER_H__ 
#include <gst/gst.h>
#include <glib.h>


/* handle status */
typedef enum {
    INIT_STATUS = 0,         
    READY_STATUS,
    PLAYING_STATUS,
    PAUSE_STATUS,
    STOP_STATUS,
    ERROR_STATUS,        
} HANDLE_STATUS;

typedef struct _ST_HANDLE_INFO {
    gint handleId;
    HANDLE_STATUS handleStatus;
    GstElement *pipeline;
} ST_HANDLE_INFO;



typedef enum {
    PLAYER_INIT_OK = 0,         
    CHANGE_TO_PALYING_OK,
    CHANGE_TO_PUASED_OK, 
    PLAYER_STOP_OK,
    PALY_ERROR = 20,      
} CALL_BACK_EVENT_TYPE;

/*call back function*/ 
typedef void (*CALL_BACK_FUNCTION)(CALL_BACK_EVENT_TYPE eventType, void *param);

typedef int HANDLE_ID;

/*log level*/
typedef enum {
    LOG_LEVEL_OFF = -3,
    LOG_LEVEL_ERROR = -2,
    LOG_LEVEL_WARN = -1,
    LOG_LEVEL_INFO = 0,
    LOG_LEVEL_DEBUG = 1,
    LOG_LEVEL_TRACE = 2,
    LOG_LEVEL_ALL = 3,
} LOG_LEVEL;

typedef struct _ST_INIT_PARAM {
    gint handleId;
    char *path;
    CALL_BACK_FUNCTION callBackFunction;
    LOG_LEVEL logLevel;
} ST_INIT_PARAM;


/*function*/
int MMPlayerInit(ST_INIT_PARAM *pstInitParam);
int MMPlayerPlay(HANDLE_ID hanldeId);
int MMPlayerStop(HANDLE_ID hanldeId);
int MMPlayerPause(HANDLE_ID hanldeId);
int MMPlayerResume(HANDLE_ID hanldeId);
int MMPlayerGetPostion(HANDLE_ID hanldeId);

#endif