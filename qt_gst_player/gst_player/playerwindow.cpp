#include <unistd.h>
#include "playerwindow.h"

PlayerWindow::PlayerWindow(ST_USER_HANDLE **pstUserHandle)
{
    playBt = new QPushButton("Play");
    pauseBt = new QPushButton("Pause");
    stopBt = new QPushButton("Stop");
    videoWindow = new QWidget();
    slider = new QSlider(Qt::Horizontal);
    timer = new QTimer();
    volumeSlider = new QSlider(Qt::Horizontal);

    connect(playBt, SIGNAL(clicked()), this, SLOT(onPlayClicked()));
    connect(pauseBt, SIGNAL(clicked()), this, SLOT(onPauseClicked()));
    connect(stopBt, SIGNAL(clicked()), this, SLOT(onStopClicked()));

    buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(playBt);
    buttonLayout->addWidget(pauseBt);
    buttonLayout->addWidget(stopBt);
    buttonLayout->addWidget(slider);
    buttonLayout->addWidget(volumeSlider);


    playerLayout = new QVBoxLayout;
    playerLayout->addWidget(videoWindow);
    playerLayout->addLayout(buttonLayout);

    this->setLayout(playerLayout);

    //init param
    pstHandle = *pstUserHandle;
    hanldeId = pstHandle->handleId;

}

WId PlayerWindow::getVideoWId(){
    return videoWindow->winId();
}

void PlayerWindow::onPlayClicked() {
    bool ret = false;
    MM_PLAYER_CMD_TYPE type = CMD_PLAY;

    if (NULL == pstHandle)
    {
        LOG_ERROR("pst is NULL\n");
        goto end;
    }
    ret = createCmdAndPush(pstHandle, type, NULL);
    if (!ret)
    {
        LOG_ERROR("start play error\n");
    }
 end:
    LOG_OUT();
}

void PlayerWindow::onPauseClicked() {
    bool ret = false;
    MM_PLAYER_CMD_TYPE type = CMD_PAUSE;

    if (NULL == pstHandle)
    {
        LOG_ERROR("pst is NULL\n");
        goto end;
    }
    ret = createCmdAndPush(pstHandle, type, NULL);
    if (!ret)
    {
        LOG_ERROR("start play error\n");
    }
 end:
    LOG_OUT();
}

void PlayerWindow::onStopClicked()
{

}

void PlayerWindow::closeEvent(QCloseEvent *event)
{
    bool ret = false;
    MM_PLAYER_CMD_TYPE type = CMD_STOP;

    if (NULL == pstHandle)
    {
        LOG_ERROR("pst is NULL\n");
        goto end;
    }
    ret = createCmdAndPush(pstHandle, type, NULL);
    if (!ret)
    {
        LOG_ERROR("start play error\n");
    }
    g_mutex_lock(&pstHandle->quitMutex);
    g_cond_wait(&pstHandle->quitCond, &pstHandle->quitMutex);
    g_mutex_unlock(&pstHandle->quitMutex);

    g_mutex_clear(&pstHandle->quitMutex);
    g_cond_clear(&pstHandle->quitCond);

    if (pstHandle)
    {
        LOG_INFO("free MEM(%p)\n", pstHandle);
        g_free(pstHandle);
        pstHandle = NULL;
    }

 end:
    LOG_OUT();
}


