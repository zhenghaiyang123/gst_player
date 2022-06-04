#include "common.h"

bool cmdQueueInit(ST_PLAYER_CMD_QUEUE **cmdQueue)
{
    bool ret = false;
    ST_PLAYER_CMD_QUEUE *retQueue = NULL;

    if (NULL == cmdQueue)
    {
        LOG_ERROR("cmd queue is NULL\n");
        goto end;
    }

    retQueue = (ST_PLAYER_CMD_QUEUE *)g_malloc(sizeof(ST_PLAYER_CMD_QUEUE));
    if (retQueue == NULL)
    {
        LOG_ERROR("malloc cmd queue error\n");
        goto end;
    }
    memset(retQueue->cmd, 0, sizeof(retQueue->cmd));

    retQueue->rear = retQueue->front = 0;
    retQueue->length = 0;
    *cmdQueue = retQueue;
    ret = true;
end:
    return ret;
}

bool cmdQueueIsEmpty(ST_PLAYER_CMD_QUEUE *cmdQueue)
{
    bool ret = false;

    if (NULL == cmdQueue)
    {
        LOG_ERROR("cmd queue is NULL\n");
        goto end;
    }

    ret = (cmdQueue->rear == cmdQueue->front ? true : false);

end:
    return ret;
}

bool cmdQueueIsFull(ST_PLAYER_CMD_QUEUE *cmdQueue)
{
    bool ret = false;

    if (NULL == cmdQueue)
    {
        LOG_ERROR("cmd queue is NULL\n");
        goto end;
    }

    ret = ((cmdQueue->rear + 1) % MAX_QUEUE_ESIZE == cmdQueue->front ? true : false);

end:
    return ret;
}

bool cmdQueuePush(ST_PLAYER_CMD_QUEUE *cmdQueue, ST_PLAYER_CMD *cmd)
{
    bool ret = false;

    if (NULL == cmdQueue || NULL == cmd)
    {
        LOG_ERROR("cmd queue is NULL\n");
        goto end;
    }

    if (cmdQueueIsFull(cmdQueue))
    {
        LOG_ERROR("cmd queue is full, throw cmd (%d)\n", cmd->type);
        goto end;
    }

    cmdQueue->cmd[cmdQueue->rear] = cmd;
    cmdQueue->rear = (cmdQueue->rear + 1) % MAX_QUEUE_ESIZE;
    ret = true;
    LOG_INFO ("push cmd(%d) to queue success\n", cmd->type);

end:
    return ret;
}

ST_PLAYER_CMD *cmdQueuePop(ST_PLAYER_CMD_QUEUE *cmdQueue)
{
    ST_PLAYER_CMD *ret = NULL;
    if (NULL == cmdQueue)
    {
        LOG_ERROR("cmd queue is NULL\n");
        goto end;
    }
    if (cmdQueue->rear == cmdQueue->front)
    {
        LOG_ERROR("cmd queue is empty\n");
        goto end;
    }

    ret = cmdQueue->cmd[cmdQueue->front];
    cmdQueue->cmd[cmdQueue->front] = NULL;
    cmdQueue->front = (cmdQueue->front + 1) % MAX_QUEUE_ESIZE;

end:
    return ret;
}

bool cmdQueueDeInit(ST_PLAYER_CMD_QUEUE *cmdQueue)
{
    bool ret = false;
    int i = 0;

    if (NULL == cmdQueue)
    {
        LOG_ERROR("cmd queue is NULL\n");
        goto end;
    }

    LOG_INFO("start free cmd\n");

    for (i = 0; i < MAX_QUEUE_ESIZE; i++)
    {
        if (cmdQueue->cmd[i])
        {
            LOG_INFO ("free cmd(%p)\n", cmdQueue->cmd[i]);
            g_free(cmdQueue->cmd[i]);
            cmdQueue->cmd[i] = NULL;
        }
    }

    cmdQueue->rear = cmdQueue->front = 0;
    cmdQueue->length = 0;
    ret = true;
end:
    return ret;
}

bool createCmdAndPush(ST_USER_HANDLE *userHandle, MM_PLAYER_CMD_TYPE type, void *data)
{
    bool ret = false;
    ST_PLAYER_CMD *cmd = NULL;

    if (NULL == userHandle || type > CMD_ALL)
    {
        LOG_ERROR("invalid param\n");
        goto end;
    }

    cmd = (ST_PLAYER_CMD *)g_malloc(sizeof(ST_PLAYER_CMD));
    if (cmd == NULL)
    {
        LOG_ERROR("malloc cmd error\n");
        goto end;
    }
    cmd->type = type;
    switch (type)
    {
        case CMD_PLAY:
        {
            break;
        }
        case CMD_PAUSE:
        {
            break;
        }
        case CMD_RESUME:
        {
            break;
        }
        case CMD_STOP:
        {
            break;
        }
        case CMD_SEEK:
        {
            break;
        }
        default:
        {
            break;
        }
    }

    g_mutex_lock(&userHandle->queueMutex);
    ret = cmdQueuePush(userHandle->cmdQueue, cmd);
    g_cond_signal(&userHandle->queueCond);
    g_mutex_unlock(&userHandle->queueMutex);

end:

    if (ret == false && cmd != NULL)
    {
        //need free cmd when error
        g_free(cmd);
        cmd = NULL;
        ret = false;
    }

    return ret;
}
