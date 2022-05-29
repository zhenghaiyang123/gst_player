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

    PALY_ERROR = 20,      
} CALL_BACK_EVENT_TYPE;

/*call back function*/ 
typedef void (*CALL_BACK_FUNCTION)(CALL_BACK_EVENT_TYPE eventType, void *param);

typedef int HANDLE_ID;

typedef struct _ST_INIT_PARAM {
    gint handleId;
    char *path;
    CALL_BACK_FUNCTION callBackFunction;
} ST_INIT_PARAM;

/*function*/
int MMPlayerInit(ST_INIT_PARAM *pstInitParam);
int MMPlayerPlay(HANDLE_ID hanldeId);
int MMPlayerStop(HANDLE_ID hanldeId);
int MMPlayerPause(HANDLE_ID hanldeId);
int MMPlayerResume(HANDLE_ID hanldeId);
int MMPlayerGetPostion(HANDLE_ID hanldeId);

#endif