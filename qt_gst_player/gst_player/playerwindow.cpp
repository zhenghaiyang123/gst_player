#include <unistd.h>
#include <QCoreApplication>
#include <QFileInfoList>
#include <QDir>
#include "playerwindow.h"

PlayerWindow::PlayerWindow(ST_USER_HANDLE **pstUserHandle)
{
    videoLayout();

    timer = new QTimer();
    /* 设置按钮的属性 */
    pushButton[0]->setCheckable(true);
    pushButton[5]->setCheckable(true);

    connect(pushButton[0], SIGNAL(clicked()),
            this, SLOT(onControlBtClicked()));
    connect(timer, SIGNAL(timeout()),
            this, SLOT(refreshSlider()));
    connect(durationSlider, SIGNAL(sliderReleased()),
            this, SLOT(durationSliderReleased()));
    connect(pushButton[5], SIGNAL(clicked()),
            this, SLOT(btn_fullscreen_clicked()));

    //init param
    pstHandle = *pstUserHandle;
    hanldeId = pstHandle->handleId;
    totalDuration = 0;

}

PlayerWindow::~PlayerWindow()
{

}

WId PlayerWindow::getVideoWId(){
    return videoWidget->winId();
}

void PlayerWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    vWidget[1]->setGeometry(0, this->height() - 80, this->width(), 80);
}

void PlayerWindow::onControlBtClicked() {
    bool ret = false;
    MM_PLAYER_CMD_TYPE type;

    if (NULL == pstHandle)
    {
        LOG_ERROR("pst is NULL\n");
        goto end;
    }

    LOG_INFO("hanle  status(%d)\n", pstHandle->handleStatus);
    if (pstHandle->handleStatus == READY_STATUS || pstHandle->handleStatus == PAUSE_STATUS)
    {
        type = CMD_PLAY;
        pushButton[0]->setChecked(true);
        timer->start(1000);
    }
    else if (pstHandle->handleStatus == PLAYING_STATUS)
    {
        type = CMD_PAUSE;
        pushButton[0]->setChecked(false);
        timer->stop();
    }
    else
    {
        LOG_ERROR("hanle in error status\n");
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

void PlayerWindow::refreshSlider() {
    gint64 current = GST_CLOCK_TIME_NONE;
    QString mediaDuration;
    int second = 0;
    int minute = 0;
    QString mediaPosition;


    LOG_INFO("IN \n");
    if (pstHandle->handleStatus == PLAYING_STATUS)
    {
        if (!GST_CLOCK_TIME_IS_VALID(totalDuration) || !totalDuration) {
            if (MMPlayerGetDuration(pstHandle->handleId,&totalDuration) == 0) {
                durationSlider->setRange(0, totalDuration/GST_SECOND);
                mediaDuration.clear();
                second  = totalDuration / GST_SECOND;
                minute = second / 60;

                if (minute >= 10)
                    mediaDuration = QString::number(minute, 10);
                else
                    mediaDuration = "0" + QString::number(minute, 10);

                if (second >= 10)
                    mediaDuration = mediaDuration
                            + ":" + QString::number(second, 10);
                else
                    mediaDuration = mediaDuration
                            + ":0" + QString::number(second, 10);

                label[1]->setText("/" + mediaDuration);
            }
        }

        if (MMPlayerGetPostion (pstHandle->handleId, &current) == 0) {
            LOG_INFO("%ld / %ld\n", current/GST_SECOND, totalDuration/GST_SECOND);
            durationSlider->setValue(current/GST_SECOND);
            LOG_INFO("get current %d\n", current/GST_SECOND);
            /* 显示现在播放的时间 */
            mediaPosition.clear();
            second  = current / GST_SECOND;
            minute = second / 60;

            if (minute >= 10)
                mediaPosition = QString::number(minute, 10);
            else
                mediaPosition = "0" + QString::number(minute, 10);

            if (second >= 10)
                mediaPosition = mediaPosition
                        + ":" + QString::number(second, 10);
            else
                mediaPosition = mediaPosition
                        + ":0" + QString::number(second, 10);
            label[0]->setText(mediaPosition);
        }
    }
}
void PlayerWindow::doSeek() {
    gint64 current = GST_CLOCK_TIME_NONE;

    timer->stop();
    gint64 pos = durationSlider->sliderPosition();
    LOG_INFO("seek to: %ld\n", pos);
    gst_element_seek_simple (pstHandle->pipeline, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH ,
                  pos * GST_SECOND);
    timer->start(1000);
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

void PlayerWindow::videoLayout()
{
    /* 设置位置与大小,这里固定为800, 480 */
    //this->setGeometry(0, 0, 800, 480);
    //this->setMinimumSize(800, 480);
    //this->setMaximumSize(800, 480);
    QPalette pal;
    pal.setColor(QPalette::WindowText, Qt::white);

    for (int i = 0; i < 3; i++) {
        /* 水平容器 */
        hWidget[i] = new QWidget();
        hWidget[i]->setAutoFillBackground(true);
        /* 水平布局 */
        hBoxLayout[i] = new QHBoxLayout();
    }

    for (int i = 0; i < 2; i++) {
        /* 垂直容器 */
        vWidget[i] = new QWidget();
       // vWidget[i]->setAutoFillBackground(true);
        /* 垂直布局 */
        //vBoxLayout[i] = new QVBoxLayout();
    }
    vBoxLayout[0] = new QVBoxLayout();
    for (int i = 0; i < 2; i++) {
        label[i] = new QLabel();
    }

    for (int i = 0; i < 6; i++) {
        pushButton[i] = new QPushButton();
        pushButton[i]->setMaximumSize(44, 44);
        pushButton[i]->setMinimumSize(44, 44);
    }
    QWidget *mainWidget = new QWidget();
    /* 设置 */
    vWidget[0]->setObjectName("vWidget0");
    vWidget[1]->setObjectName("vWidget1");
    hWidget[1]->setObjectName("hWidget1");
    hWidget[2]->setObjectName("hWidget2");
    pushButton[0]->setObjectName("btn_play");
    pushButton[1]->setObjectName("btn_previous");
    pushButton[2]->setObjectName("btn_next");
    pushButton[3]->setObjectName("btn_volumedown");
    pushButton[4]->setObjectName("btn_volumeup");
    pushButton[5]->setObjectName("btn_screen");


    QFont font;

    font.setPixelSize(18);
    label[0]->setFont(font);
    label[1]->setFont(font);

    pal.setColor(QPalette::WindowText, Qt::white);
    label[0]->setPalette(pal);
    label[1]->setPalette(pal);

    label[0]->setText("00:00");
    label[1]->setText("/00:00");

    durationSlider = new QSlider(Qt::Horizontal);
    durationSlider->setMaximumHeight(15);
    durationSlider->setObjectName("durationSlider");

    volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    volumeSlider->setMaximumWidth(80);
    volumeSlider->setObjectName("volumeSlider");
    volumeSlider->setValue(50);

    listWidget = new QListWidget();
    listWidget->setObjectName("listWidget");
    listWidget->setVerticalScrollBarPolicy(
                Qt::ScrollBarAlwaysOff);
    listWidget->setHorizontalScrollBarPolicy(
                Qt::ScrollBarAlwaysOff);
    listWidget->setFocusPolicy(Qt::NoFocus);
    listWidget->setMinimumSize(200, 480);
    listWidget->setMaximumWidth(200);

    videoWidget = new QWidget();

    videoWidget->setStyleSheet("border-image: none;"
                               "background: transparent;"
                               "border:none");

    /* H0布局 */
    vWidget[0]->setMinimumSize(200, 480);
    vWidget[0]->setMaximumWidth(200);
    videoWidget->setMinimumSize(200, 480);
    videoWidget->setObjectName("videoLayer");


    hBoxLayout[0]->addWidget(videoWidget);
    hBoxLayout[0]->addWidget(listWidget);

    //hWidget[0]->setLayout(hBoxLayout[0]);

    setCentralWidget(mainWidget);

    vBoxLayout[0] -> addLayout(hBoxLayout[0]);

    /* V0布局 */


    /* V1布局 */
    /* 底板部件布局 */
    //hWidget[1]->setMaximumHeight(15);
    //hWidget[2]->setMinimumHeight(65);

    /* H1布局 进度条栏*/
    hBoxLayout[1]->addWidget(durationSlider);
    hBoxLayout[1]->setContentsMargins(0, 0, 0, 0);
    hWidget[1]->setFixedHeight(20);
    hWidget[1]->setLayout(hBoxLayout[1]);
    //vBoxLayout[0] -> addLayout(hBoxLayout[1]);

    /* H2布局 底部按钮栏*/
    QSpacerItem *hSpacer0 = new
            QSpacerItem(300, 80,
                        QSizePolicy::Expanding,
                        QSizePolicy::Maximum);

    hBoxLayout[2]->addSpacing(20);
    hBoxLayout[2]->addWidget(pushButton[0]);
    hBoxLayout[2]->addSpacing(10);
    hBoxLayout[2]->addWidget(pushButton[1]);
    hBoxLayout[2]->addWidget(pushButton[2]);
    hBoxLayout[2]->addSpacing(10);
    hBoxLayout[2]->addWidget(pushButton[3]);
    hBoxLayout[2]->addWidget(volumeSlider);
    hBoxLayout[2]->addWidget(pushButton[4]);
    hBoxLayout[2]->addWidget(label[0]);
    hBoxLayout[2]->addWidget(label[1]);
    hBoxLayout[2]->addSpacerItem(hSpacer0);
    hBoxLayout[2]->addWidget(pushButton[5]);
    hBoxLayout[2]->addSpacing(20);
    hBoxLayout[2]->setContentsMargins(0, 0, 0, 0);
    hBoxLayout[2]->setAlignment(Qt::AlignLeft | Qt::AlignTop);



    hWidget[2]->setLayout(hBoxLayout[2]);
    hWidget[2]->setFixedHeight(50);

    vBoxLayout[0]->addWidget(hWidget[1]);
    vBoxLayout[0]->addWidget(hWidget[2]);
    //vBoxLayout[0]->addLayout(hBoxLayout[2]);

    mainWidget->setLayout(vBoxLayout[0]);

}

/* 播放按钮点击 */
void PlayerWindow::btn_play_clicked()
{

}

/* 下一个视频按钮点击 */
void PlayerWindow::btn_next_clicked()
{

}

/* 前一个视频按钮点击 */
void PlayerWindow::btn_pre_clicked()
{

}

/* 音量加 */
void PlayerWindow::btn_volmeup_clicked()
{

}

/* 音量减 */
void PlayerWindow::btn_volmedown_clicked()
{

}

/* 全屏 */
void PlayerWindow::btn_fullscreen_clicked()
{
    /* 全屏/非全屏操作 */
    listWidget->setVisible(!pushButton[5]->isChecked());
}

/* 列表单击 */
void PlayerWindow::listWidgetCliked(QListWidgetItem*)
{

}

/* 媒体列表项改变 */
void PlayerWindow::mediaPlaylistCurrentIndexChanged(int)
{

}

/* 媒体总长度改变 */
void PlayerWindow::musicPlayerDurationChanged(qint64)
{

}

/* 媒体播放位置改变 */
void PlayerWindow::mediaPlayerPositionChanged(qint64)
{

}

/* 播放进度条松开 */
void PlayerWindow::durationSliderReleased()
{
    gint64 current = GST_CLOCK_TIME_NONE;

    timer->stop();
    gint64 pos = durationSlider->sliderPosition();
    LOG_INFO("seek to: %ld\n", pos);
    MMPlayerDoSeek (pstHandle->handleId, pos * GST_SECOND);
    timer->start(1000);
}

/* 音量条松开 */
void PlayerWindow::volumeSliderReleased()
{

}



