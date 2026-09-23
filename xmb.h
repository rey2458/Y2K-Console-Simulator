#pragma once
#include "musicview.h"
#include "audio.h"
#include <QWidget>
#include <QKeyEvent>
#include <QString>
#include <QList>
#include <QPixmap>
#include <QTimer>
#include <QSettings>
#include <QRandomGenerator>

class XMB : public QWidget {
    Q_OBJECT

public:
    XMB(QWidget *parent = nullptr);
    ~XMB();

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    MusicView *musicWindow;
    Audio *audioPlayer;
    int uiTheme;
    int sysVolume;
    QString editName;
    int avatarGridIndex;
    int currentState;
    float menuTransition;
    QStringList systemSubCategories;
    int currentSubCategory;
    float animatedSubCategory;
    QString userName;
    int userAvatar;
    QList<QPixmap> anomalyFrames;
    int currentFrameIndex;
    QPixmap bgImage;
    QString customFontFamily;
    QList<QString> categories;
    int currentCategory;
    float animatedCategory;
    QTimer* animationTimer;
};