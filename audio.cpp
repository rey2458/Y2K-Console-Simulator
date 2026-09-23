#include "audio.h"
#include <QDebug>

Audio::Audio(QObject *parent) : QObject(parent) {
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.5);
}
void Audio::playTrack(const QString &filePath){
    player->setSource(QUrl::fromLocalFile(filePath));
    player->play();
    qDebug() << "Now playing: "<< filePath;
}

void Audio::pause(){
    if (player->playbackState() == QMediaPlayer::PlayingState){
        player->pause();
    } else {
        player->play();
    }
}
void Audio::stop(){
    player->stop();
}
void Audio::setVolume(float volume){
    audioOutput->setVolume(volume);
}