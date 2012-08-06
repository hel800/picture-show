/* ------------------------------------------------------------------
animation.cpp is part of picture-show.
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

#include "animation.h"

// FEATURES:
// --> // Jump funktion
// --> // problem beim letzten bild --> immer noch!!
// --> // reihenfolge von Bildern (Filename, Datum)
// --> // prüfen ob bilder in einem Ordner sind
// --> // paintToWaiting am Anfang bei sortierung anzeigen
// --> // Zeitfunktion
// Info anzeigen, wenn "i" gedrückt wird
// Panoramafunktion
// --> // Hilfe einblenden
// Übersetzung ins Englische
// Code kommentieren und License einfügen
// --> // Ausschnitt wählen: Ganzes Bild/Schirm füllen

// FEHLER:
// --> // kleine Ruckler bei Slideeffekt

animation::animation(QWidget * pWidget)
{
    this->m_paintWidget = pWidget;
    this->m_bType = FADE;
    this->m_sType = IMAGE;
    this->m_current_blend = 0.0;
    this->m_nextBlendForward = true;
    this->m_current_waiting_blend = 1.0;

    this->m_fadeSpeed = 0.01;
    this->m_doingWhat = NOTHING;

    this->m_blendFrom = QPixmap();
    this->m_blendTo = QPixmap();

    this->m_blendTimer = new QTimer();
    connect(this->m_blendTimer, SIGNAL(timeout()), this, SLOT(paint()));

    this->m_waitTimer = new QTimer();
    connect(this->m_waitTimer, SIGNAL(timeout()), this, SLOT(paintWaiting()));

    this->m_textbarTimer = new QTimer();
    connect(this->m_textbarTimer, SIGNAL(timeout()), this, SLOT(paintTextbar()));

    this->m_textWaitTimer = new QTimer();
    connect(this->m_textWaitTimer, SIGNAL(timeout()), this, SLOT(textBarOff()));

    this->m_tbReason = TIMER_ON;
    this->m_current_textbar_blend = 0.0;
    this->m_textbarVisible = false;
    this->m_nothingEnteredYet = true;

    this->m_staticLogo = QPixmap(":/images/img/static.png");
    this->m_stopwatch_icon = QPixmap(":/images/img/stopwatch-icon.png");
    this->m_stopwatch_off_icon = QPixmap(":/images/img/stopwatchOff-icon.png");
    this->m_jumpto_icon = QPixmap(":/images/img/jumpto-icon.png");
    this->m_logoText = QPixmap(":/images/img/logoText.png");

    this->m_waitingPix.append(QPixmap(":/images/img/waiting01.png"));
    this->m_waitingPix.append(QPixmap(":/images/img/waiting02.png"));
    this->m_waitingPix.append(QPixmap(":/images/img/waiting03.png"));
    this->m_waitingPix.append(QPixmap(":/images/img/waiting04.png"));
    this->m_waitingPix.append(QPixmap(":/images/img/waiting05.png"));
    this->m_waitingPix.append(QPixmap(":/images/img/waiting06.png"));
    this->m_waitingPix.append(QPixmap(":/images/img/waiting07.png"));
    this->m_waitingPix.append(QPixmap(":/images/img/waiting08.png"));

    this->m_textbarPix = QPixmap(":/images/img/textbar_big.png");
    this->backgroundColor = QColor(25, 25, 25);
}

void animation::reset()
{
    if (this->isDoingNothing())
    {
        this->m_blendFrom = QPixmap();
        this->m_blendTo = QPixmap();
    }
}

bool animation::isBusy()
{
    return this->m_doingWhat == BLENDING;
}

bool animation::isWaiting()
{
    return this->m_doingWhat == WAITING;
}

bool animation::isDoingNothing()
{
    return this->m_doingWhat == NOTHING;
}

bool animation::isShowingTextbar()
{
    return this->m_textbarVisible;
}

animation::~animation()
{
    delete this->m_blendTimer;
}

void animation::setCurrentBlendType(BlendType type)
{
    this->m_bType = type;
}

void animation::setFadeSteps(double step)
{
    this->m_fadeSpeed = step;
}

void animation::setScaleTypeToImage()
{
    this->m_sType = IMAGE;
}

void animation::setScaleTypeToScreen()
{
    this->m_sType = SCREEN;
}

void animation::setScaleTypeTo(ScaleType type)
{
    this->m_sType = type;
}

void animation::paintStartScreen()
{
    if (this->isBusy())
        return;

    this->m_waitTimer->stop();

    QPainter p;
    p.begin(this->m_paintWidget);
    this->drawStartScreen(&p);
    p.end();

    this->m_doingWhat = NOTHING;
}

void animation::paintToWaiting(const QPixmap &from)
{
    if (this->isWaiting())
        return;

    if (this->isBusy())
        this->m_blendTimer->stop();

    this->m_blendFrom = QPixmap(from);
    this->m_current_waiting_blend = 1.0;
    this->m_current_wait_angle = 0.0;

    this->m_doingWhat = WAITING;
    this->m_waitTimer->start(5);
}


void animation::paintFromWaiting(const QPixmap &to)
{
    if (this->isWaiting())
    {
        this->m_waitTimer->stop();
    }
    else if (this->isBusy())
    {
        return;
    }

    // animation sperren
    this->m_doingWhat = BLENDING;

    this->m_blendTo = QPixmap(to);

    if (!this->m_blendTo.isNull())
    {
        this->m_blendFrom = QPixmap(this->m_paintWidget->width(), this->m_paintWidget->height());
        QPainter p;
        p.begin(&this->m_blendFrom);
        p.fillRect(0, 0, this->m_paintWidget->width(), this->m_paintWidget->height(), this->backgroundColor);
        p.drawPixmap(int(this->m_paintWidget->width()/2)-140, int(this->m_paintWidget->height()/2)-140, this->m_waitingPix.at(this->m_current_wait_angle));
        p.setOpacity(0.3);
        p.drawPixmap(int(this->m_paintWidget->width()/2)-226, int(this->m_paintWidget->height()/2)+150, this->m_logoText);
        p.end();

        this->m_nextBlendForward = true;
        this->blendImages();
    }
}

void animation::blendImages(QPixmap &from, QPixmap &to, bool forward)
{
    if (this->isBusy())
        return;

    this->m_doingWhat = BLENDING;
    this->m_blendFrom = QPixmap(from);
    this->m_blendTo = QPixmap(to);

    this->m_nextBlendForward = forward;

    this->m_current_blend = 0.0;
    this->m_blendTimer->start(5);
}

void animation::repaintState()
{
    if (this->isBusy() || this->isWaiting())
        return;

    QPainter p;
    p.begin(this->m_paintWidget);

    if (this->m_blendTo.isNull())
        this->drawStartScreen(&p);
    else
        this->drawPixmap(&p, this->m_blendTo);

    p.end();
}

void animation::startTimerBar(int secs)
{
    if (this->isBusy() || this->isWaiting() || this->m_blendTo.isNull())
        return;

    if (secs < 4)
        secs = 4;

    if (this->m_textbarVisible && this->m_tbReason == TIMER_ON)
    {
        this->m_textWaitTimer->setSingleShot(true);
        this->m_textWaitTimer->start(2500);

        this->m_tbReason = TIMER_OFF;
        this->m_textbarText = tr("Timer aus");
        this->paintTextbar();
        return;
    }
    else if (this->m_textbarVisible && this->m_tbReason == TIMER_OFF)
    {
        this->m_textWaitTimer->setSingleShot(true);
        this->m_textWaitTimer->start(2500);

        this->m_tbReason = TIMER_ON;
        this->m_textbarText = tr("%1 Sekunden").arg(secs);
        this->paintTextbar();
        return;
    }
    else if (this->m_textbarVisible)
    {
        return;
    }

    // restart textWaitTimer, because TextBar might not be faded in, because it is already visible
    this->m_textWaitTimer->setSingleShot(true);
    this->m_textWaitTimer->start(2500);

    // if Textbar is not yet there fade it in
    this->blendTextbar(tr("%1 Sekunden").arg(secs));
    this->m_tbReason = TIMER_ON;
}

void animation::stopTimerBar()
{
    if (this->isBusy() || this->isWaiting() || this->m_blendTo.isNull())
        return;

    this->m_tbReason = TIMER_OFF;
    this->blendTextbar(tr("Timer aus"));

    this->m_textWaitTimer->setSingleShot(true);
    this->m_textWaitTimer->start(2500);
}

void animation::startJumptoBar(int currentImage, int numImages)
{
    if (this->isBusy() || this->isWaiting() || this->m_blendTo.isNull())
        return;

    if (this->m_textbarVisible)
        return;

    this->m_tbReason = JUMP;
    this->m_nothingEnteredYet = true;
    this->blendTextbar(tr("Jump to: %1/%2").arg(currentImage).arg(numImages));
}

void animation::numberEntered(int num)
{
    if (this->isBusy() || this->isWaiting() || this->m_blendTo.isNull())
        return;

    if (!this->m_textbarVisible || this->m_tbReason == TIMER_OFF)
        return;

    if (this->m_tbReason == TIMER_ON)
    {
        if (this->m_textbarText.count("_") == 0)
            this->setTextbarText(tr("%1_ Sekunden").arg(num));
        else if (this->m_textbarText.count("_") == 1)
            this->setTextbarText(tr("%1%2 Sekunden").arg(this->m_textbarText.left(1)).arg(num));

        this->m_textWaitTimer->setSingleShot(true);
        this->m_textWaitTimer->start(2500);
    }
    else if (this->m_tbReason == JUMP)
    {
        QRegExp rx("(\\d*)/(\\d+)");
        rx.indexIn(this->m_textbarText);
        QStringList list = rx.capturedTexts();

        if (list.size() < 3)
            return;

        bool ok = false;
        int numImages = list.at(2).toInt(&ok);
        if (!ok)
            return;
        QString newNumber = list.at(1) + QString::number(num);
        int newNum = newNumber.toInt(&ok);
        if (!ok)
            return;

        if (newNum > numImages || this->m_nothingEnteredYet)
        {
            if (num <= numImages && num > 0)
                this->setTextbarText(tr("Jump to: %1/%2").arg(num).arg(numImages));
        }
        else
        {
            this->setTextbarText(tr("Jump to: %1/%2").arg(newNum).arg(numImages));
        }

        this->m_nothingEnteredYet = false;
    }
}

void animation::enterPressed()
{
    this->m_textWaitTimer->stop();
    this->textBarOff();
}

void animation::escPressed()
{
    if (this->m_textbarVisible)
        this->blendTextbar(this->m_textbarText);
}

void animation::textBarReset()
{
    if (this->isDoingNothing())
    {
        this->m_textWaitTimer->stop();
        this->m_textbarVisible = false;
        this->m_current_textbar_blend = 0.0;
    }
}

QPixmap animation::currentDisplay()
{    
    QPixmap pixmap = QPixmap(this->m_paintWidget->width(), this->m_paintWidget->height());

    if (!this->isDoingNothing())
        return pixmap;

    QPainter p;
    p.begin(&pixmap);
    if (this->m_blendTo.isNull())
        this->drawStartScreen(&p);
    else
        this->drawPixmap(&p, this->m_blendTo);
    p.end();

    return pixmap;
}

void animation::blendTextbar(const QString & text)
{
    if (this->isBusy() || this->isWaiting())
        return;

    this->m_textbarText = QString(text);
    this->m_doingWhat = BLENDING;
    this->m_textbarTimer->start(10);
}

void animation::setTextbarText(const QString &text)
{
    if (this->isBusy() || this->isWaiting())
        return;

    if (!this->m_textbarVisible)
        return;

    this->m_textbarText = QString(text);

    QPainter p;
    p.begin(this->m_paintWidget);

    if (this->m_tbReason == TIMER_ON)
        this->drawTextbar(&p, STOPWATCH);
    else if (this->m_tbReason == TIMER_OFF)
        this->drawTextbar(&p, STOPWATCH_OFF);
    else if (this->m_tbReason == JUMP)
        this->drawTextbar(&p, JUMP_TO);

    p.end();
}

// INTERN USE ONLY
void animation::blendImages()
{
    if (this->m_blendFrom.isNull() || this->m_blendTo.isNull())
        return;

    this->m_current_blend = 0.0;
    this->m_blendTimer->start(5);
}

void animation::paint()
{
    if (this->m_blendFrom.isNull() || this->m_blendTo.isNull())
    {
        this->m_blendTimer->stop();
        this->m_doingWhat = NOTHING;
        return;
    }

    if (this->m_current_blend >= 1.0)
    {
        this->m_blendTimer->stop();
        this->m_current_blend = 1.0;

        // paint for a last time
        if (this->m_bType == FADE)
            this->paintFADE();
        else if (this->m_bType == SLIDE)
            this->paintSLIDE();
        else
            this->paintHARD();

        this->m_doingWhat = NOTHING;
        emit finishedBlending();
        return;
    }

    // in case textbar is still visible turn it off globally
    this->m_textbarVisible = false;

    // chose Blend type here
    if (this->m_bType == FADE)
        this->paintFADE();
    else if (this->m_bType == SLIDE)
        this->paintSLIDE();
    else
        this->paintHARD();
}

void animation::paintFADE()
{
    QPainter p;
    p.begin(this->m_paintWidget);

    this->drawPixmap(&p, this->m_blendFrom, 1.0 - this->m_current_blend);
    this->drawPixmap(&p, this->m_blendTo, this->m_current_blend, false);

    p.end();

    this->m_current_blend += this->m_fadeSpeed;
}

void animation::paintHARD()
{
    QPainter p;
    p.begin(this->m_paintWidget);

    this->drawPixmap(&p, this->m_blendTo);

    p.end();

    this->m_current_blend = 1.0;
}

void animation::paintSLIDE()
{
    QPainter p;
    p.begin(this->m_paintWidget);

//    int fromWidth = this->m_blendFrom.size().width();
    int toWidth = this->m_blendTo.size().width();
    int screenWidth = this->m_paintWidget->width();

    double t = (this->m_current_blend * 16.0) - 8.0;
    double sig = 1 / (1 + exp(-t));
    double sig_max = 1 / (1 + exp(-8.0));
    double sig_min = 1 / (1 + exp(8.0));
    double sig_new = (sig - sig_min) * (1/(sig_max - sig_min));

//    qDebug(QString::number(sig).toStdString().c_str());

    int current_slide = int((screenWidth * sig_new) + .5);
    int extra = screenWidth - toWidth;
    if (extra < 0)
        extra = 0;

    if (this->m_nextBlendForward)
    {
        this->drawPixmap(&p, this->m_blendFrom, 1. - sig_new, true, -current_slide);
        this->drawPixmap(&p, this->m_blendTo, sig_new, false, screenWidth - current_slide);
    }
    else
    {
        this->drawPixmap(&p, this->m_blendFrom, 1. - sig_new, true, current_slide);
        this->drawPixmap(&p, this->m_blendTo, sig_new, false, (-1 * screenWidth) + current_slide);
    }

    p.end();

    this->m_current_blend += this->m_fadeSpeed;
}

void animation::paintWaiting()
{
    this->m_current_wait_angle += 0.1;

    int angle = (int)this->m_current_wait_angle;

    angle %= 8;
    if (this->m_current_wait_angle >= 8)
        this->m_current_wait_angle = 0.0;

    this->m_current_waiting_blend -= 0.035;

    if (this->m_current_waiting_blend < 0)
        this->m_current_waiting_blend = 0;

    // in case textbar is still visible turn it off globally
    this->m_textbarVisible = false;

    QPainter p;
    p.begin(this->m_paintWidget);
    p.setOpacity(1.0);
    p.fillRect(0, 0, this->m_paintWidget->width(), this->m_paintWidget->height(), this->backgroundColor);
    p.drawPixmap(int(this->m_paintWidget->width()/2)-140, int(this->m_paintWidget->height()/2)-140, this->m_waitingPix.at(angle));
    p.setOpacity(0.3);
    p.drawPixmap(int(this->m_paintWidget->width()/2)-226, int(this->m_paintWidget->height()/2)+150, this->m_logoText);

    if (!this->m_blendFrom.isNull() && this->m_current_waiting_blend > 0)
        this->drawPixmap(&p, this->m_blendFrom, this->m_current_waiting_blend, false);

    p.end();
}

void animation::paintTextbar()
{
    if (this->m_current_textbar_blend >= 1.0 || this->m_current_textbar_blend < 0.0)
    {
        this->m_textbarTimer->stop();
        // eingeblendet
        if (this->m_current_textbar_blend >= 1.0)
        {
            this->m_current_textbar_blend = 0.999;

//            this->m_textWaitTimer->setSingleShot(true);
//            this->m_textWaitTimer->start(2500);
        }
        // wieder ausgeblendet
        else
        {
            this->m_current_textbar_blend = 0.0;
        }

        this->m_textbarVisible = !this->m_textbarVisible;
        this->m_doingWhat = NOTHING;
    }

    QPainter p;
    p.begin(this->m_paintWidget);

    if (this->m_tbReason == TIMER_ON)
        this->drawTextbar(&p, STOPWATCH, this->m_current_textbar_blend);
    else if (this->m_tbReason == TIMER_OFF)
        this->drawTextbar(&p, STOPWATCH_OFF, this->m_current_textbar_blend);
    else if (this->m_tbReason == JUMP)
        this->drawTextbar(&p, JUMP_TO, this->m_current_textbar_blend);

    p.end();

    if (this->m_textbarTimer->isActive())
    {
        if (this->m_textbarVisible)
            this->m_current_textbar_blend -= 0.1;
        else
            this->m_current_textbar_blend += 0.1;
    }
}

void animation::textBarOff()
{
    if (this->isBusy() || this->isWaiting())
        return;

    if (!this->m_textbarVisible)
        return;

    this->m_textbarText.replace("_", "");

    if (this->m_tbReason == TIMER_ON)
    {
        bool ok = false;
        int secs = this->m_textbarText.left(2).toInt(&ok);

        if (secs < 4)
            secs = 4;

        this->m_textbarText = tr("%1 Sekunden").arg(secs);

        if (ok)
            emit automaticTimer(secs*1000);
    }
    else if (this->m_tbReason == JUMP)
    {
        QRegExp rx("(\\d*)/(\\d+)");
        rx.indexIn(this->m_textbarText);
        QStringList list = rx.capturedTexts();

        if (list.size() >= 3)
        {
            bool ok = false;
            QString newNumber = list.at(1);
            int newNum = newNumber.toInt(&ok);
            if (ok)
                emit jumptoImage(newNum);
        }
    }

    // fade textbar out
    this->blendTextbar(this->m_textbarText);
}

void animation::drawStartScreen(QPainter * p)
{
    p->setOpacity(1.0);
    p->fillRect(0, 0, this->m_paintWidget->width(), this->m_paintWidget->height(), this->backgroundColor);
    p->drawPixmap(int(this->m_paintWidget->width()/2)-140, int(this->m_paintWidget->height()/2)-140, this->m_staticLogo);
    p->setOpacity(0.3);
    p->drawPixmap(int(this->m_paintWidget->width()/2)-226, int(this->m_paintWidget->height()/2)+150, this->m_logoText);
}

void animation::drawPixmap(QPainter * p, QPixmap & pix, double opacity, bool background, int offset)
{
    if (background)
    {
        p->setOpacity(1.0);
        p->fillRect(0, 0, this->m_paintWidget->width(), this->m_paintWidget->height(), this->backgroundColor);
    }


    QPoint p2;
    if (this->m_sType == IMAGE || pix.width() < pix.height())
    {
        int diff3 = pix.width() - this->m_paintWidget->width();
        if (diff3 < 0)
        {
            p2.setX((abs(diff3)/2) + offset);
            p2.setY(0);
        }
        else
        {
            int diff4 = abs(pix.height() - this->m_paintWidget->height());
            p2.setY(diff4/2);
            p2.setX(0 + offset);
        }
    }
    else
    {
        int diff3 = pix.width() - this->m_paintWidget->width();
        if (diff3 > 0)
        {
            p2.setX(0 - int(abs(diff3)/2) + offset);
            p2.setY(0);
        }
        else
        {
            int diff4 = abs(pix.height() - this->m_paintWidget->height());
            p2.setY(0 - int(diff4/2));
            p2.setX(0 + offset);
        }
    }

    p->setOpacity(opacity);
    p->drawPixmap(p2, pix);
}

void animation::drawTextbar(QPainter * p, IconType icon, double opacity)
{
    this->drawPixmap(p, this->m_blendTo);

    p->setOpacity(opacity);
    p->drawPixmap(int(this->m_paintWidget->width()/2)-236, int(this->m_paintWidget->height()/2)-65, this->m_textbarPix);

    if (icon == STOPWATCH)
        p->drawPixmap(int(this->m_paintWidget->width()/2)-190, int(this->m_paintWidget->height()/2)-50, this->m_stopwatch_icon);
    else if (icon == STOPWATCH_OFF)
        p->drawPixmap(int(this->m_paintWidget->width()/2)-190, int(this->m_paintWidget->height()/2)-50, this->m_stopwatch_off_icon);
    else if (icon == JUMP_TO)
        p->drawPixmap(int(this->m_paintWidget->width()/2)-190, int(this->m_paintWidget->height()/2)-50, this->m_jumpto_icon);



    QFont f("Helvetica");
    f.setPixelSize(26);
    f.setStyleStrategy(QFont::PreferAntialias);

    QFontMetrics fm(f);
    int nWidth = fm.width(this->m_textbarText);

    if (nWidth > 250)
        this->m_textbarText = QString("[zu lang]");

    p->setPen(QColor(255, 255, 255, 220));
    p->setFont(f);
    p->setRenderHint(QPainter::TextAntialiasing);
    p->drawText(int(this->m_paintWidget->width()/2)-int(nWidth/2) + 50, int(this->m_paintWidget->height()/2 + 4), this->m_textbarText);
}
