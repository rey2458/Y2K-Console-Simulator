#include "XMB.h"
#include <QPainter>
#include <QApplication>
#include <QPainterPath>
#include <QFontDatabase>
#include <QDateTime>
#include <QCoreApplication>
#include <QRandomGenerator>
#include <QSettings>

XMB::XMB(QWidget *parent) : QWidget(parent) {
    audioPlayer = new Audio(this);
    musicWindow = new MusicView(this);
    connect(audioPlayer->getPlayer(), &QMediaPlayer::durationChanged, musicWindow, &MusicView::updateDuration);
    connect(audioPlayer->getPlayer(), &QMediaPlayer::positionChanged, musicWindow, &MusicView::updatePosition);
    connect(musicWindow, &MusicView::requestPlay, audioPlayer, &Audio::playTrack);

    connect(musicWindow, &MusicView::requestTogglePlay, this, [=](){
        if (audioPlayer->getPlayer()->playbackState() == QMediaPlayer::PlayingState) {
            audioPlayer->getPlayer()->pause();
        } else {
            audioPlayer->getPlayer()->play();
        }
    });

    connect(audioPlayer->getPlayer(), &QMediaPlayer::playbackStateChanged, musicWindow, [=](QMediaPlayer::PlaybackState state) {
        musicWindow->updatePlaybackState(state == QMediaPlayer::PlayingState);
    });

    musicWindow->hide();
    avatarGridIndex = 0;
    QSettings settings("Y2K_Corp", "XMB_Interface");

    if(!settings.contains("userName")){
        int randomNum = QRandomGenerator::global()->bounded(1000, 9999);
        settings.setValue("userName", "User_" + QString::number(randomNum));
        settings.setValue("userAvatar", QRandomGenerator::global()->bounded(1, 12));
        settings.setValue("uiTheme", 0);
        settings.setValue("sysVolume", 8);
    }
    userName = settings.value("userName").toString();
    userAvatar = settings.value("userAvatar").toInt();
    uiTheme = settings.value("uiTheme").toInt();
    sysVolume = settings.value("sysVolume").toInt();

    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setStyleSheet("background-color: rgb(10, 10, 12);");
    categories = {"SYSTEM", "GAMES", "MUSIC", "VIDEO", "NETWORK"};
    currentCategory = 2;

    currentState = 0;
    menuTransition = 0.0f;

    systemSubCategories = {"USER PROFILE", "DISPLAY", "AUDIO", "EXIT SYSTEM"};
    currentSubCategory = 0;
    animatedSubCategory = 0.0f;

    QString appPath = "D:/QT/XMB";

    int fontId = QFontDatabase::addApplicationFont(appPath + "/LEDLIGHT.otf");
    if (fontId != -1){
        customFontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    } else {
        customFontFamily = "Arial";
    }
    bgImage = QPixmap(appPath + "/mtlhrt2.png");

    animatedCategory = currentCategory;

    currentFrameIndex = 0;
    for(int i = 1; i < 500; i++){
        QString filename = QString("%1/XMB_Anim/%2.png").arg(appPath).arg(i, 4, 10, QChar('0'));
        QPixmap frame(filename);
        if(!frame.isNull()){
            anomalyFrames.append(frame);
        }
    }

    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, [=](){
        float diff = currentCategory - animatedCategory;
        if(qAbs(diff) > 0.001) animatedCategory += diff * 0.15f;
        else animatedCategory = currentCategory;

        float subDiff = currentSubCategory - animatedSubCategory;
        if(qAbs(subDiff) > 0.001) animatedSubCategory += subDiff * 0.15f;
        else animatedSubCategory = currentSubCategory;

        float targetTransition = (currentState == 1 || currentState == 2 || currentState == 3) ? 1.0f : 0.0f;
        float transDiff = targetTransition - menuTransition;
        if(qAbs(transDiff) > 0.001) menuTransition += transDiff * 0.15f;
        else menuTransition = targetTransition;

        if (!anomalyFrames.isEmpty()){
            currentFrameIndex = (currentFrameIndex + 1) % anomalyFrames.size();
        }
        update();
    });
    animationTimer->start(16);
}

XMB::~XMB() {}

void XMB::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setRenderHint(QPainter::TextAntialiasing, false);

    int screenW = width();
    int screenH = height();
    QString appPath = "D:/QT/XMB";

    QColor themeColor;
    if(uiTheme == 1) themeColor = QColor("#C0FDFB");
    else if(uiTheme == 2) themeColor = QColor("#A09ABC");
    else if(uiTheme == 3) themeColor = QColor("#020887");
    else themeColor = QColor(220, 240, 255);

    if(!bgImage.isNull()){
        painter.drawPixmap(rect(), bgImage);
    } else {
        painter.fillRect(rect(), QColor(8, 10, 14));
    }
    QRadialGradient vignette(screenW / 2, screenH / 2, screenW / 1.5);
    vignette.setColorAt(0.3, QColor(0, 0, 0, 0));
    vignette.setColorAt(1.0, QColor(0, 0, 0, 240));
    painter.fillRect(rect(), vignette);

    if(!anomalyFrames.isEmpty()){
        QPixmap currentFrame = anomalyFrames[currentFrameIndex];
        int drawX = (screenW / 2) - (currentFrame.width() / 2);
        int drawY = (screenH / 2) - (currentFrame.height() / 2);
        painter.setOpacity(1.0 - menuTransition);
        painter.drawPixmap(drawX, drawY, currentFrame);
        painter.setOpacity(1.0);
    }

    QFont font(customFontFamily, 42);
    painter.setFont(font);

    int spacing = 110;
    int textWidth = 500;
    int textHeight = 90;
    int rightMargin = 70;

    for (int i = 0; i < categories.size(); i++){
        float yOffset = (i - animatedCategory) * spacing;
        painter.save();
        painter.translate(screenW - rightMargin, (screenH / 2) + yOffset);
        float scale = 1.0f - (menuTransition * 0.35f);
        painter.scale(scale, scale);
        float curveX = qAbs(yOffset) * 0.15f * menuTransition;
        painter.translate(curveX, 0);
        int alpha = 255 - (menuTransition * 200);
        QRect textRect(-textWidth, -(textHeight / 2), textWidth, textHeight);

        if (i == currentCategory && currentState == 0) {
            painter.setPen(QColor(0, 0, 0, alpha));
            painter.drawText(textRect.translated(4, 4), Qt::AlignRight | Qt::AlignVCenter, categories[i]);
            painter.setPen(QColor(themeColor.red(), themeColor.green(), themeColor.blue(), alpha));
            painter.drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, categories[i]);
        } else {
            painter.setPen(QColor(40, 60, 80, 120 * (alpha / 255.0f)));
            painter.drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, categories[i]);
        }
        painter.restore();
    }

    if (menuTransition > 0.01f){
        float slideX = -300 + (menuTransition * 450);
        for (int j = 0; j < systemSubCategories.size(); j++){
            float subYOffset = (j - animatedSubCategory) * spacing;
            painter.save();
            painter.translate(slideX, (screenH / 2) + subYOffset);
            int subAlpha = menuTransition * 255;
            QRect subRect(0, -(textHeight / 2), textWidth, textHeight);

            if(j == currentSubCategory) {
                painter.setPen(QColor(0, 0, 0, subAlpha));
                painter.drawText(subRect.translated(4, 4), Qt::AlignLeft | Qt::AlignVCenter, systemSubCategories[j]);
                painter.setPen(QColor(themeColor.red(), themeColor.green(), themeColor.blue(), subAlpha));
                painter.drawText(subRect, Qt::AlignLeft | Qt::AlignVCenter, systemSubCategories[j]);
            } else {
                painter.setPen(QColor(40, 60, 80, 120 * (subAlpha / 255.0f)));
                painter.drawText(subRect, Qt::AlignLeft | Qt::AlignVCenter, systemSubCategories[j]);
            }
            painter.restore();
        }
    }

    if (menuTransition > 0.01f) {
        int cardAlpha = menuTransition * 255;
        int cardW = 600;
        int cardH = 260;
        int cardX = (screenW / 2) - (cardW / 2) - 50;
        int cardY = (screenH / 2) - (cardH / 2);

        painter.save();
        painter.fillRect(cardX, cardY, cardW, cardH, QColor(15, 18, 22, cardAlpha * 0.85));
        painter.setPen(QPen(QColor(themeColor.red(), themeColor.green(), themeColor.blue(), cardAlpha), 2));
        painter.drawRect(cardX, cardY, cardW, cardH);
        painter.drawLine(cardX, cardY + 20, cardX - 15, cardY + 20);
        painter.drawLine(cardX + cardW, cardY + cardH - 20, cardX + cardW + 15, cardY + cardH - 20);

        if(currentSubCategory == 0) {
            if(currentState == 1 || currentState == 3){
                QPixmap bigAvatar(QString("%1/Ava/%2.jpg").arg(appPath).arg(userAvatar));
                if(!bigAvatar.isNull()){
                    painter.drawPixmap(cardX + 30, cardY + 30, 200, 200, bigAvatar);
                    painter.setPen(QPen(QColor(themeColor.red(), themeColor.green(), themeColor.blue(), cardAlpha), 1));
                    painter.drawRect(cardX + 30, cardY + 30, 200, 200);
                }

                painter.setPen(QColor(themeColor.red(), themeColor.green(), themeColor.blue(), cardAlpha));
                QFont numF("Consolas", 42, QFont::Bold);

                QString textToDraw = (currentState == 3) ? editName : userName;
                if (currentState == 3 && QDateTime::currentMSecsSinceEpoch() % 1000 < 500){
                    textToDraw += "_";
                }

                int totalTextWidth = 0;
                for(int k = 0; k < textToDraw.length(); k++){
                    QChar c = textToDraw[k];
                    if (c.isDigit()) totalTextWidth += QFontMetrics(numF).horizontalAdvance(c);
                    else totalTextWidth += QFontMetrics(font).horizontalAdvance(c);
                }

                float textScale = 1.0f;
                int maxAllowedWidth = 310;
                if (totalTextWidth > maxAllowedWidth) {
                    textScale = (float)maxAllowedWidth / (float)totalTextWidth;
                }

                painter.save();
                painter.translate(cardX + 260, cardY + 100);
                painter.scale(textScale, textScale);

                int cursorX = 0;
                for(int k = 0; k < textToDraw.length(); k++){
                    QChar c = textToDraw[k];
                    if(c.isDigit()) painter.setFont(numF);
                    else painter.setFont(font);

                    painter.drawText(cursorX, 0, QString(c));
                    cursorX += QFontMetrics(painter.font()).horizontalAdvance(c);
                }
                painter.restore();

                QFont smallFont("Consolas", 12, QFont::Bold);
                painter.setFont(smallFont);
                painter.setPen(QColor(100, 120, 140, cardAlpha));
                if (currentState == 3) {
                    painter.drawText(cardX + 260, cardY + 170, "Type your new name...");
                    painter.drawText(cardX + 260, cardY + 200, "Press [Enter] to Save");
                } else {
                    painter.drawText(cardX + 260, cardY + 170, "[Q] Avatar | [R] Rename");
                    painter.drawText(cardX + 260, cardY + 200, "[E] Random Name");
                }
            }
            else if (currentState == 2){
                int cols = 6;
                int iconSize = 64;
                int spacingX = 15;
                int spacingY = 45;

                int gridWidth = (cols * iconSize) + ((cols - 1) * spacingX);
                int startX = cardX + (cardW / 2) - (gridWidth / 2);
                int startY = cardY + 40;

                for(int i = 0; i < 11; i++){
                    int row = i / cols;
                    int col = i % cols;

                    int x = startX + col * (iconSize + spacingX);
                    int y = startY + row * (iconSize + spacingY);

                    QPixmap ava(QString("%1/Ava/%2.jpg").arg(appPath).arg(i + 1));
                    if (ava.isNull()) continue;
                    if(i != avatarGridIndex) {
                        painter.setOpacity(0.4);
                    } else {
                        painter.setOpacity(1.0);
                        painter.setPen(QPen(QColor(255, 255, 255, 200), 2));
                        painter.drawRect(x - 2, y - 2, iconSize + 4, iconSize + 4);
                    }
                    painter.drawPixmap(x, y, iconSize, iconSize, ava);

                    int refHeight = iconSize / 2;
                    QLinearGradient fade(x, y + iconSize, x, y + iconSize + refHeight);
                    fade.setColorAt(0, QColor(15, 18, 22, 120));
                    fade.setColorAt(1, QColor(15, 18, 22, 255));
                    painter.fillRect(x, y + iconSize, iconSize, refHeight, fade);
                }
                painter.setOpacity(1.0);

                QFont smallFont("Consolas", 12, QFont::Bold);
                painter.setFont(smallFont);
                painter.setPen(QColor(100, 120, 140, cardAlpha));
                painter.drawText(cardX, cardY + cardH - 30, cardW, 20, Qt::AlignCenter, "Select with Arrows, Press [Enter] To Save");
            }
        }
        else if (currentSubCategory == 1 && currentState == 1) {
            painter.setFont(font);
            painter.setPen(QColor(themeColor.red(), themeColor.green(), themeColor.blue(), cardAlpha));
            painter.drawText(cardX, cardY + 80, cardW, 50, Qt::AlignCenter, "INTERFACE THEME");

            QString themeName = "ICE BLUE";
            if(uiTheme == 1) themeName = "NEON CYAN";
            else if(uiTheme == 2) themeName = "FADED LAVENDER";
            else if(uiTheme == 3) themeName = "DARK ABYSS";

            QFont numF("Consolas", 32, QFont::Bold);
            painter.setFont(numF);
            painter.drawText(cardX, cardY + 150, cardW, 50, Qt::AlignCenter, "<  " + themeName + "  >");

            QFont smallFont("Consolas", 12, QFont::Bold);
            painter.setFont(smallFont);
            painter.setPen(QColor(100, 120, 140, cardAlpha));
            painter.drawText(cardX, cardY + 220, cardW, 20, Qt::AlignCenter, "Use [Left] / [Right] to change theme");
        }
        else if (currentSubCategory == 2 && currentState == 1) {
            painter.setFont(font);
            painter.setPen(QColor(themeColor.red(), themeColor.green(), themeColor.blue(), cardAlpha));
            painter.drawText(cardX, cardY + 80, cardW, 50, Qt::AlignCenter, "SYSTEM VOLUME");

            int bars = 10;
            int barW = 25;
            int barH = 40;
            int gap = 12;
            int totalW = (bars * barW) + ((bars - 1) * gap);
            int startX = cardX + (cardW / 2) - (totalW / 2);

            for(int i = 0; i < bars; i++) {
                if(i < sysVolume) {
                    painter.fillRect(startX + i * (barW + gap), cardY + 120, barW, barH, QColor(themeColor.red(), themeColor.green(), themeColor.blue(), cardAlpha));
                } else {
                    painter.fillRect(startX + i * (barW + gap), cardY + 120, barW, barH, QColor(40, 50, 60, cardAlpha));
                }
            }

            QFont smallFont("Consolas", 12, QFont::Bold);
            painter.setFont(smallFont);
            painter.setPen(QColor(100, 120, 140, cardAlpha));
            painter.drawText(cardX, cardY + 220, cardW, 20, Qt::AlignCenter, "Use [Left] / [Right] to adjust volume");
        }
        painter.restore();
    }

    QFont sysFont("Consolas", 14);
    sysFont.setBold(true);
    painter.setFont(sysFont);
    painter.setPen(QColor(180, 190, 200, 180));

    QDateTime now = QDateTime::currentDateTime();
    QString currentDate = now.toString("dd/MM/yyyy");
    QString currentTime = now.toString("HH:mm:ss");
    painter.drawText(40, 50, currentDate);
    painter.drawText(40, 70, currentTime);
    painter.drawText(40, screenH - 40, "W Up        S Down        X Enter        0 Back        ^ Options");

    int avatarX = screenW - 90;
    int avatarY = 40;

    QPixmap avatar(QString("%1/Ava/%2.jpg").arg(appPath).arg(userAvatar));
    if(!avatar.isNull()){
        painter.drawPixmap(avatarX, avatarY, 50, 50, avatar);
        painter.setPen(QPen(QColor(themeColor.red(), themeColor.green(), themeColor.blue(), 150), 2));
        painter.drawRect(avatarX, avatarY, 50, 50);
    }

    painter.setPen(QColor(themeColor.red(), themeColor.green(), themeColor.blue()));
    QFont numFHUD("Consolas", 42, QFont::Bold);

    int totalW = 0;
    for (int k = 0; k < userName.length(); k++){
        QChar c = userName[k];
        if(c.isDigit()) totalW += QFontMetrics(numFHUD).horizontalAdvance(c);
        else totalW += QFontMetrics(font).horizontalAdvance(c);
    }

    int textStartX = avatarX - 20 - totalW;

    for(int k = 0; k < userName.length(); k++){
        QChar c = userName[k];
        if (c.isDigit()) painter.setFont(numFHUD);
        else painter.setFont(font);

        painter.drawText(QRect(textStartX, avatarY, 500, 50), Qt::AlignLeft | Qt::AlignVCenter, QString(c));
        textStartX += QFontMetrics(painter.font()).horizontalAdvance(c);
    }

    painter.fillRect(rect(), QBrush(QColor(0, 0, 0, 100), Qt::Dense6Pattern));
    painter.setPen(QPen(QColor(0, 0, 0, 180), 1));
    for (int y = 0; y < screenH; y += 3){
        painter.drawLine(0, y, screenW, y);
    }
}

void XMB::keyPressEvent(QKeyEvent* event) {
    if(currentState == 3){
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter){
            if(!editName.trimmed().isEmpty()){
                userName = editName.trimmed();
                QSettings settings("Y2K_Corp", "XMB_Interface");
                settings.setValue("userName", userName);
            }
            currentState = 1;
        }
        else if (event->key() == Qt::Key_Escape){
            currentState = 1;
        }
        else if(event->key() == Qt::Key_Backspace){
            editName.chop(1);
        }
        else {
            if(editName.length() < 13) {
                QString text = event->text();
                if(!text.isEmpty() && (text[0].isLetterOrNumber() || text[0] == '_' || text[0] == ' ')){
                    editName += text.toUpper();
                }
            }
        }
        update();
        return;
    }
    if (event->key() == Qt::Key_Escape) {
        if(currentState == 4){
            if (!musicWindow->handleEscape()) {
                musicWindow->hide();
                musicWindow->clearFocus();
                this->setFocus();
                currentState = 0;
            }
        } else if (currentState != 0) {
            currentState = 0;
        } else {
            QApplication::quit();
        }
    }
    else if (event->key() == Qt::Key_0 || event->key() == Qt::Key_Backspace){
        if(currentState == 4){
            if (!musicWindow->handleEscape()) {
                musicWindow->hide();
                musicWindow->clearFocus();
                this->setFocus();
                currentState = 0;
            }
        }
        else if (currentState == 1) currentState = 0;
        else if(currentState == 2) currentState = 1;
    }
    else if (event->key() == Qt::Key_X || event->key() == Qt::Key_Return){
        if (currentState  == 0 && currentCategory == 0) {
            currentState = 1;
        }
        else if (currentState == 1 && currentCategory == 0 && currentSubCategory == 0){
            currentState = 2;
            avatarGridIndex = userAvatar - 1;
        } else if(currentState == 1 && currentCategory == 0 && currentSubCategory == 3) {
            QSettings settings("Y2K_Corp", "XMB_Interface");
            settings.sync();
            QApplication::quit();
        }
        else if(currentState == 2){
            userAvatar = avatarGridIndex + 1;
            QSettings settings("Y2K_Corp", "XMB_Interface");
            settings.setValue("userAvatar", userAvatar);
            currentState = 1;
        }
        else if (currentState == 0 && currentCategory == 2) {
            currentState = 4;
            musicWindow->move((width() - musicWindow->width()) / 2, (height() - musicWindow->height()) / 2);

            musicWindow->initView();

            musicWindow->show();
        }
    }
    else if (event->key() == Qt::Key_Q || event->text().toLower() == "q" || event->text().toLower() == "й"){
        if (currentState == 1 && currentCategory == 0 && currentSubCategory == 0){
            currentState = 2;
            avatarGridIndex = userAvatar - 1;
            if(avatarGridIndex > 10) avatarGridIndex = 0;
        }
    }
    else if (event->key() == Qt::Key_E || event->text().toLower() == "e" || event->text().toLower() == "у"){
        if (currentState == 1 && currentCategory == 0 && currentSubCategory == 0){
            int randomNum = QRandomGenerator::global()->bounded(1000, 9999);
            userName = "User_" + QString::number(randomNum);
            QSettings settings("Y2K_Corp", "XMB_Interface");
            settings.setValue("userName", userName);
        }
    }
    else if (event->key() == Qt::Key_R || event->text().toLower() == "r" || event->text().toLower() == "к"){
        if(currentState == 1 && currentCategory == 0 && currentSubCategory == 0){
            currentState = 3;
            editName = userName;
        }
    }
    else if (event->key() == Qt::Key_Up || event->key() == Qt::Key_W) {
        if(currentState == 0 && currentCategory > 0) currentCategory--;
        else if (currentState == 1 && currentSubCategory > 0) currentSubCategory--;
        else if(currentState == 2 && avatarGridIndex - 6 >= 0) avatarGridIndex -= 6;
    }
    else if (event->key() == Qt::Key_Down || event->key() == Qt::Key_S){
        if(currentState == 0 && currentCategory < categories.size() - 1) currentCategory++;
        else if (currentState == 1 && currentSubCategory < systemSubCategories.size() - 1) currentSubCategory++;
        else if (currentState == 2 && avatarGridIndex + 6 < 11) avatarGridIndex += 6;
    }
    else if (event->key() == Qt::Key_Left || event->key() == Qt::Key_A){
        if (currentState == 2 && avatarGridIndex > 0) {
            avatarGridIndex--;
        }
        else if (currentState == 1 && currentSubCategory == 1 && uiTheme > 0) {
            uiTheme--;
            QSettings settings("Y2K_Corp", "XMB_Interface");
            settings.setValue("uiTheme", uiTheme);
        }
        else if (currentState == 1 && currentSubCategory == 2 && sysVolume > 0) {
            sysVolume--;
            QSettings settings("Y2K_Corp", "XMB_Interface");
            settings.setValue("sysVolume", sysVolume);
        }
    }
    else if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D){
        if (currentState == 2 && avatarGridIndex < 10) {
            avatarGridIndex++;
        }
        else if (currentState == 1 && currentSubCategory == 1 && uiTheme < 3) {
            uiTheme++;
            QSettings settings("Y2K_Corp", "XMB_Interface");
            settings.setValue("uiTheme", uiTheme);
        }
        else if (currentState == 1 && currentSubCategory == 2 && sysVolume < 10) {
            sysVolume++;
            QSettings settings("Y2K_Corp", "XMB_Interface");
            settings.setValue("sysVolume", sysVolume);
        }
    }
    else if(event->key() == Qt::Key_Delete){
        QSettings settings("Y2K_Corp", "XMB_Interface");
        settings.clear();
        QApplication::quit();
    }
    update();
}