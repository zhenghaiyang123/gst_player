#include "playerwindow.h"

PlayerWindow::PlayerWindow(GstElement *pipeline)
{
    playBt = new QPushButton("Play");
    pauseBt = new QPushButton("Pause");
    stopBt = new QPushButton("Stop");
    videoWindow = new QWidget();
    slider = new QSlider(Qt::Horizontal);
    timer = new QTimer();
    volumeSlider = new QSlider(Qt::Horizontal);

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
}

WId PlayerWindow::getVideoWId(){
    return videoWindow->winId();
}
