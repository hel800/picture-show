#ifndef ANIMATION_H
/* ------------------------------------------------------------------
animation.h is part of picture-show.
.....................................................................

picture-show is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

picture-show is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with picture-show. If not, see <http://www.gnu.org/licenses/>.

......................................................................

author: Sebastian Schäfer
February 2012

--------------------------------------------------------------------*/

#define ANIMATION_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QGraphicsTextItem>
#include <QStyleOptionGraphicsItem>

#include "global.h"
#include <math.h>

class animation : public QObject
{
    Q_OBJECT

public:
    enum BusyType {
        BLENDING,
        WAITING,
        NOTHING
    };

    enum IconType {
        STOPWATCH,
        STOPWATCH_OFF,
        JUMP_TO,
        EXIT_ICO
    };

    enum TextBarReason {
        TIMER_ON,
        TIMER_OFF,
        JUMP,
        EXIT
    };

    animation(QWidget * pWidget);
    ~animation();

    void reset();

    bool isBusy();
    bool isWaiting();
    bool isDoingNothing();
    bool isShowingTextbar();

    void setCurrentBlendType(BlendType type);
    void setFadeSteps(double step);

    void setScaleTypeToImage();
    void setScaleTypeToScreen();
    void setScaleTypeTo(ScaleType type);

    void setFirstStart(bool fs);

    void paintStartScreen();
    void paintToWaiting(const QPixmap & from = QPixmap());
    void paintFromWaiting(const QPixmap & to = QPixmap());
    void blendImages(QPixmap &from, QPixmap &to, bool forward = true);
    void repaintState();

    void startTimerBar(int secs = 10);
    void stopTimerBar();
    void startJumptoBar(int currentImage, int numImages);
    void startExitApp();

    bool exitRequested();

    void numberEntered(int num);
    void enterPressed();
    void escPressed();
    void textBarReset();

    QPixmap currentDisplay();

signals:
    void automaticTimer(int secs);
    void jumptoImage(int number);
    void finishedBlending();
    void textBarFadeOutFinished();

private slots:
    void paint();
    void paintWaiting();
    void paintTextbar();
    void textBarOff();

private:
    void blendImages();
    void paintFADE();
    void paintHARD();
    void paintSLIDE();
    void paintSLIDE_FADE();

    void blendTextbar(const QString &text);
    void setTextbarText(const QString &text);

    void drawStartScreen(QPainter * p);
    void drawPixmap(QPainter * p, QPixmap & pix, double opacity = 1.0, bool background = true, int offset = 0);
    void drawTextbar(QPainter * p, IconType icon, double opacity = 1.0);

    QWidget * m_paintWidget;
    QColor backgroundColor;

    QTimer * m_blendTimer;
    QTimer * m_waitTimer;
    QTimer * m_textbarTimer;
    QTimer * m_textWaitTimer;

    BusyType m_doingWhat;

    double m_current_blend;
    bool m_nextBlendForward;
    QPixmap m_blendFrom;
    QPixmap m_blendTo;

    QPixmap m_staticLogo;
    QPixmap m_stopwatch_icon;
    QPixmap m_stopwatch_off_icon;
    QPixmap m_jumpto_icon;
    QPixmap m_exit_icon;
    QPixmap m_logoText;

    QVector<QPixmap> m_waitingPix;
    double m_current_wait_angle;
    double m_current_waiting_blend;

    TextBarReason m_tbReason;

    QPixmap m_textbarPix;
    double m_current_textbar_blend;
    bool m_textbarVisible;
    bool m_nothingEnteredYet;
    QString m_textbarText;

    double m_fadeSpeed;
    bool m_isAskingExit;

    BlendType m_bType;
    ScaleType m_sType;

    bool m_firstStart;

};

#endif // ANIMATION_H
