#ifndef MUSICVIEW_H
#define MUSICVIEW_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QListWidget>
#include <QTextBrowser>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QMap>

struct TrackData {
    QString title;
    QString artist;
    QString coverPath;
    QString audioPath;
};

class MusicView : public QWidget{
    Q_OBJECT
public:
    explicit MusicView(QWidget *parent = nullptr);
    void setTrackInfo(const QString &title, const QString &artist, const QString &coverPath);
    void initView();
    bool handleEscape();
public slots:
    void updateDuration(qint64 duration);
    void updatePosition(qint64 position);

    void updatePlaybackState(bool isPlaying);
    void playNext();
    void playPrev();
signals:
    void requestPlay(const QString &audioPath);
    void requestTogglePlay();
private slots:
    void openPlaylist();
    void backToLibrary();
    void playSelectedTrack(QListWidgetItem *item);
private:
    qint64 currentDuration = 0;

    QMap<QString, QList<TrackData>> virtualDrive;

    QStackedWidget *leftStack;
    QListWidget *libraryList;
    QListWidget *trackList;
    QPushButton *btnBack;

    QSlider *volumeSlider;
    QLabel *coverLabel;
    QLabel *titleLabel;
    QLabel *artistLabel;
    QSlider *progressBar;
    QLabel *timeLabel;
    QPushButton *btnPrev;
    QPushButton *btnPlay;
    QPushButton *btnNext;

    QLineEdit *searchBar;
    QListWidget *searchHistory;
    QListWidget *searchResults;

    QLabel *lyricsLabel;
    QTextBrowser *lyricsBox;
};

#endif