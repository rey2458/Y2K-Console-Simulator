#include "musicview.h"
#include <QTime>

MusicView::MusicView(QWidget *parent) : QWidget(parent) {
    this->setFixedSize(1150, 750);
    this->setStyleSheet("background-color: #07070a; border: 2px solid #1a1525;");

    QString cyanSynthStyle = "color: #00ffff; font-family: 'Consolas', 'Courier New'; font-weight: bold; background: transparent; border: none;";

    QString listStyle = "QListWidget { background-color: #0c0c12; color: #00ffff; border: 1px solid #23193a; font-family: 'Consolas'; font-size: 14px; outline: none; padding: 4px; } QListWidget::item:selected { background-color: #23193a; color: white; }";

    QString btnStyle = "QPushButton { border: 1px solid #23193a; color: #00ffff; font-family: 'Consolas'; font-size: 14px; border-radius: 3px; padding: 5px; background-color: #0c0c12; } QPushButton:hover { background-color: #23193a; } QPushButton:pressed { background-color: #b829ea; color: white; }";

    leftStack = new QStackedWidget(this);

    QWidget *libraryWidget = new QWidget();
    QVBoxLayout *libLayout = new QVBoxLayout(libraryWidget);
    QLabel *libLabel = new QLabel("YOUR LIBRARY", this);
    libLabel->setStyleSheet(cyanSynthStyle + "font-size: 16px; border-bottom: 1px solid #23193a;");
    libraryList = new QListWidget(this);
    libraryList->setStyleSheet(listStyle);
    libraryList->setWordWrap(true);
    libraryList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    libraryList->setFocus();
    libLayout->addWidget(libLabel);
    libLayout->addWidget(libraryList);
    libLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *trackListWidget = new QWidget();
    QVBoxLayout *trackLayout = new QVBoxLayout(trackListWidget);

    QHBoxLayout *topNavLayout = new QHBoxLayout();
    btnBack = new QPushButton("< BACK", this);
    btnBack->setStyleSheet(btnStyle);
    btnBack->setFixedWidth(70);
    QLabel *playlistNameLabel = new QLabel("CURRENT TRACKS", this);
    playlistNameLabel->setStyleSheet(cyanSynthStyle + "font-size: 16px;");
    topNavLayout->addWidget(btnBack);
    topNavLayout->addWidget(playlistNameLabel);
    topNavLayout->addStretch();

    trackList = new QListWidget(this);
    trackList->setWordWrap(true);
    trackList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    trackList->setStyleSheet(listStyle);

    virtualDrive["Alice in Chains - Alice in Chains"] = {
        {"Again", "Alice in Chains", "D:/QT/XMB/MusicPlayer/covers/aic_self.jpg", "D:/QT/XMB/MusicPlayer/music/again.mp3"},
        {"Head Creeps", "Alice in Chains", "D:/QT/XMB/MusicPlayer/covers/aic_self.jpg", "D:/QT/XMB/MusicPlayer/music/head_creeps.mp3"}
    };
    virtualDrive["Mindless Self Indulgence - Frankenstein Girls Will Seem Strangely Sexy"] = {
        {"I Hate Jimmy Page", "Mindless Self Indulgence", "D:/QT/XMB/MusicPlayer/covers/msi_fran.jpg", "D:/QT/XMB/MusicPlayer/music/ihjp.mp3"},
        {"Clarissa", "Mindless Self Indulgence", "D:/QT/XMB/MusicPlayer/covers/msi_fran.jpg", "D:/QT/XMB/MusicPlayer/music/clarissa.mp3"},
        {"Planet Of Apes", "Mindless Self Indulgence", "D:/QT/XMB/MusicPlayer/covers/msi_fran.jpg", "D:/QT/XMB/MusicPlayer/music/poa.mp3"}
    };
    virtualDrive["Type O Negative - Life Is Killing Me"] = {
        {"(We Were) Electrocute", "Type O Negative", "D:/QT/XMB/MusicPlayer/covers/ton_likm.jpg", "D:/QT/XMB/MusicPlayer/music/electrocute.mp3"},
        {"I Don't Wanna Be Me", "Type O Negative", "D:/QT/XMB/MusicPlayer/covers/ton_likm.jpg", "D:/QT/XMB/MusicPlayer/music/idwbm.mp3"}
    };
    virtualDrive["Nine Inch Nails - The Downward Spiral"] = {
        {"March Of The Pigs", "Nine Inch Nails", "D:/QT/XMB/MusicPlayer/covers/nin_tds.jpg", "D:/QT/XMB/MusicPlayer/music/motp.mp3"},
        {"Ruiner", "Nine Inch Nails", "D:/QT/XMB/MusicPlayer/covers/nin_tds.jpg", "D:/QT/XMB/MusicPlayer/music/ruiner.mp3"},
        {"Hurt", "Nine Inch Nails", "D:/QT/XMB/MusicPlayer/covers/nin_tds.jpg", "D:/QT/XMB/MusicPlayer/music/hurt.mp3"}
    };

    libraryList->addItem("[+] Create New Playlist");
    for (const QString &albumName : virtualDrive.keys()) {
        libraryList->addItem(albumName);
    }

    trackLayout->addLayout(topNavLayout);
    trackLayout->addWidget(trackList);
    trackLayout->setContentsMargins(0, 0, 0, 0);

    leftStack->addWidget(libraryWidget);
    leftStack->addWidget(trackListWidget);

    connect(libraryList, &QListWidget::itemActivated, this, &MusicView::openPlaylist);
    connect(btnBack, &QPushButton::clicked, this, &MusicView::backToLibrary);
    connect(trackList, &QListWidget::itemActivated, this, &MusicView::playSelectedTrack);

    QFrame *centerTopFrame = new QFrame(this);
    centerTopFrame->setStyleSheet("QFrame { background-color: #0c0c12; border: 1px solid #23193a; }");

    QVBoxLayout *centerTopLayout = new QVBoxLayout(centerTopFrame);
    centerTopLayout->setContentsMargins(15, 15, 15, 15);

    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setStyleSheet("QSlider::groove:horizontal { background: #0c0c12; height: 8px; border: 1px solid #23193a;} QSlider::sub-page:horizontal { background: #a084dc; }");

    coverLabel = new QLabel(this);
    coverLabel->setFixedSize(300, 300);
    coverLabel->setScaledContents(true);
    coverLabel->setStyleSheet("background-color: #000000; border: 1px solid #23193a;");

    titleLabel = new QLabel("TRACK TITLE", this);
    titleLabel->setStyleSheet(cyanSynthStyle + "font-size: 24px; text-transform: uppercase;");
    titleLabel->setAlignment(Qt::AlignCenter);

    artistLabel = new QLabel("ARTIST", this);
    artistLabel->setStyleSheet(cyanSynthStyle + "font-size: 16px; color: #a084dc;");
    artistLabel->setAlignment(Qt::AlignCenter);

    progressBar = new QSlider(Qt::Horizontal, this);
    progressBar->setStyleSheet("QSlider::groove:horizontal { background: #0c0c12; height: 12px; border: 1px solid #23193a; } QSlider::sub-page:horizontal { background: #b829ea; } QSlider::handle:horizontal { background: none; width: 0px; }");

    timeLabel = new QLabel("0:00 / 0:00", this);
    timeLabel->setStyleSheet(cyanSynthStyle + "font-size: 12px;");
    QHBoxLayout *progressLayout = new QHBoxLayout();
    progressLayout->addWidget(progressBar);
    progressLayout->addWidget(timeLabel);

    centerTopLayout->addWidget(new QLabel("VOL", this), 0, Qt::AlignCenter);
    centerTopLayout->addWidget(volumeSlider);
    centerTopLayout->addStretch();
    centerTopLayout->addWidget(coverLabel, 0, Qt::AlignCenter);
    centerTopLayout->addWidget(titleLabel);
    centerTopLayout->addWidget(artistLabel);
    centerTopLayout->addLayout(progressLayout);

    QHBoxLayout *playbackControlsLayout = new QHBoxLayout();
    btnPrev = new QPushButton("|<", this);
    btnPlay = new QPushButton("||", this);
    btnNext = new QPushButton(">|", this);

    btnPrev->setStyleSheet(btnStyle);
    btnPlay->setStyleSheet(btnStyle);
    btnNext->setStyleSheet(btnStyle);

    connect(btnPrev, &QPushButton::clicked, this, &MusicView::playPrev);
    connect(btnNext, &QPushButton::clicked, this, &MusicView::playNext);
    connect(btnPlay, &QPushButton::clicked, this, [this](){ emit requestTogglePlay(); });

    playbackControlsLayout->addStretch();
    playbackControlsLayout->addWidget(btnPrev);
    playbackControlsLayout->addWidget(btnPlay);
    playbackControlsLayout->addWidget(btnNext);
    playbackControlsLayout->addStretch();

    centerTopLayout->addLayout(playbackControlsLayout);

    centerTopLayout->addStretch();

    QVBoxLayout *rightTopLayout = new QVBoxLayout();
    lyricsLabel = new QLabel("TRACK LYRICS", this);
    lyricsLabel->setStyleSheet(cyanSynthStyle + "font-size: 16px; border-bottom: 1px solid #23193a;");
    lyricsLabel->setAlignment(Qt::AlignCenter);

    lyricsBox = new QTextBrowser(this);
    lyricsBox->setStyleSheet("QTextBrowser { background-color: #0c0c12; color: #a084dc; border: 1px solid #23193a; font-family: 'Consolas'; font-size: 14px; padding: 10px; }");
    lyricsBox->setAlignment(Qt::AlignCenter);
    lyricsBox->setText("\n\n(Lyrics will load here...)\n\n");

    rightTopLayout->addWidget(lyricsLabel);
    rightTopLayout->addWidget(lyricsBox);

    QVBoxLayout *centerBottomLayout = new QVBoxLayout();
    searchBar = new QLineEdit(this);
    searchBar->setPlaceholderText("Search Library...");
    searchBar->setStyleSheet("QLineEdit { background-color: #0c0c12; color: #00ffff; border: 1px solid #23193a; font-family: 'Consolas'; font-size: 14px; padding: 5px; }");

    searchHistory = new QListWidget(this);
    searchHistory->setStyleSheet(listStyle);
    searchHistory->setFixedHeight(100);
    searchHistory->addItem("> recent searches...");

    centerBottomLayout->addWidget(searchBar);
    centerBottomLayout->addWidget(searchHistory);

    QVBoxLayout *rightBottomLayout = new QVBoxLayout();
    QLabel *resultsLabel = new QLabel("RESULTS", this);
    resultsLabel->setStyleSheet(cyanSynthStyle + "font-size: 16px;");

    searchResults = new QListWidget(this);
    searchResults->setStyleSheet(listStyle);
    searchResults->setFixedHeight(125);
    searchResults->addItem("No results yet.");

    rightBottomLayout->addWidget(resultsLabel);
    rightBottomLayout->addWidget(searchResults);

    QGridLayout *mainGrid = new QGridLayout(this);
    mainGrid->setContentsMargins(20, 20, 20, 20);
    mainGrid->setSpacing(20);

    mainGrid->addWidget(leftStack, 0, 0, 2, 1);
    mainGrid->addWidget(centerTopFrame, 0, 1);
    mainGrid->addLayout(centerBottomLayout, 1, 1);
    mainGrid->addLayout(rightTopLayout, 0, 2);
    mainGrid->addLayout(rightBottomLayout, 1, 2);

    mainGrid->setColumnStretch(0, 10);
    mainGrid->setColumnStretch(1, 15);
    mainGrid->setColumnStretch(2, 10);
}
void MusicView::openPlaylist() {
    QListWidgetItem *selected = libraryList->currentItem();
    if (!selected || selected->text() == "[+] Create New Playlist") return;

    QString albumName = selected->text();

    trackList->clear();
    QList<TrackData> tracks = virtualDrive[albumName];

    for(int i = 0; i < tracks.size(); ++i){
        QListWidgetItem *item = new QListWidgetItem(QString::number(i + 1) + ". " + tracks[i].title);
        item->setData(Qt::UserRole, tracks[i].audioPath);
        item->setData(Qt::UserRole + 1, tracks[i].title);
        item->setData(Qt::UserRole + 2, tracks[i].artist);
        item->setData(Qt::UserRole + 3, tracks[i].coverPath);
        trackList->addItem(item);
    }

    leftStack->setCurrentIndex(1);

    if(trackList->count() > 0) {
        trackList->setCurrentRow(0);
    }
    trackList->setFocus();
}
void MusicView::backToLibrary() {
    leftStack->setCurrentIndex(0);
}
void MusicView::setTrackInfo(const QString &title, const QString &artist, const QString &coverPath){
    titleLabel->setText(title);
    artistLabel->setText(artist);
    QPixmap cover(coverPath);
    if(!cover.isNull()){
        coverLabel->setPixmap(cover);
    } else {
        coverLabel->clear();
        coverLabel->setText("COVER NOT FOUND");
    }
}
void MusicView::updateDuration(qint64 duration) {
    currentDuration = duration;
    progressBar->setMaximum(duration);
}
void MusicView::updatePosition(qint64 position){
    progressBar->setValue(position);
    QTime posTime(0, (position / 60000) % 60, (position / 1000) % 60);
    QTime durTime(0, (currentDuration / 60000) % 60, (currentDuration / 1000) % 60);
    timeLabel->setText(posTime.toString("m:ss") + " / " + durTime.toString("m:ss"));
}
void MusicView::playSelectedTrack(QListWidgetItem *item){
    if (!item) return;

    QString audioPath = item->data(Qt::UserRole).toString();
    QString title = item->data(Qt::UserRole + 1).toString();
    QString artist = item->data(Qt::UserRole + 2).toString();
    QString cover = item->data(Qt::UserRole + 3).toString();

    setTrackInfo(title, artist, cover);

    emit requestPlay(audioPath);
}
void MusicView::initView() {
    leftStack->setCurrentIndex(0);
    if (libraryList->count() > 0) {
        libraryList->setCurrentRow(0);
    }
    libraryList->setFocus();
}
bool MusicView::handleEscape() {
    if(leftStack ->currentIndex() == 1) {
        backToLibrary();
        return true;
    }
    return false;
}

void MusicView::playNext() {
    int currentRow = trackList->currentRow();
    if (currentRow < trackList->count() - 1) {
        trackList->setCurrentRow(currentRow + 1);
        playSelectedTrack(trackList->currentItem());
    }
}

void MusicView::playPrev() {
    int currentRow = trackList->currentRow();
    if(currentRow > 0){
        trackList->setCurrentRow(currentRow - 1);
        playSelectedTrack(trackList->currentItem());
    }
}
void MusicView::updatePlaybackState(bool isPlaying){
    if(isPlaying) {
        btnPlay->setText("||");
    } else {
        btnPlay->setText(">");
    }
}