#ifndef AUDIO_H
#define AUDIO_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>

class Audio : public QObject {
    Q_OBJECT
public:
    explicit Audio(QObject *parent = nullptr);
    QMediaPlayer* getPlayer() const { return player; }
public slots:
    void playTrack(const QString &filePath);
    void pause();
    void stop();
    void setVolume(float volume);
private:
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
};

#endif