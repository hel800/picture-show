/* ------------------------------------------------------------------
overlaybase.cpp is part of picture-show.
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
March 2012

--------------------------------------------------------------------*/

#include "overlaybase.h"

overlayBase::overlayBase(QWidget *pWidget)
{
    this->m_paintWidget = pWidget;
    this->m_overlayTimer = new QTimer(this);
    this->m_currentState = READY;
    this->m_fadeSpeed = 0.1;

    connect(this->m_overlayTimer, SIGNAL(timeout()), this, SLOT(timedOut()));
}

overlayBase::~overlayBase()
{
    delete this->m_overlayTimer;
}

void overlayBase::setFadeSteps(double step)
{
    this->m_fadeSpeed = step;
}

bool overlayBase::blendIn(QPixmap& bg)
{
    if (this->m_currentState == READY)
    {
        this->m_currentBlendValue = 0.0;
        this->m_background = QPixmap(bg);
        this->generatePixmap();
        this->m_currentState = BLEND_IN;

        this->m_overlayTimer->start(10);
        return true;
    }

    return false;
}

bool overlayBase::blendOut()
{
    if (this->m_currentState == READY && this->m_currentBlendValue == 1.0)
    {
        this->generatePixmap();
        this->m_currentState = BLEND_OUT;
        this->m_overlayTimer->start(10);
        return true;
    }

    return false;
}

void overlayBase::update(QPixmap& bg)
{
    if (this->m_currentState != READY)
        return;

    this->m_background = QPixmap(bg);
    this->generatePixmap();

    this->paint(this->m_paintWidget);
}

void overlayBase::timedOut()
{
    if (this->m_background.isNull() || this->m_overlay.isNull())
    {
        this->m_overlayTimer->stop();
        this->m_currentState = READY;
        emit blendInFinished();
        return;
    }

    if (this->m_currentState == BLEND_IN)
        this->m_currentBlendValue += this->m_fadeSpeed;
    else if (this->m_currentState = BLEND_OUT)
        this->m_currentBlendValue -= this->m_fadeSpeed;

    if (this->m_currentBlendValue + 0.001 >= 1.0)
    {
        this->m_overlayTimer->stop();
        this->m_currentBlendValue = 1.0;
        this->m_currentState = READY;
        this->paint(this->m_paintWidget);
        emit blendInFinished();
    }
    else if (this->m_currentBlendValue - 0.001 <= 0.0)
    {
        this->m_overlayTimer->stop();
        this->m_currentBlendValue = 0.0;
        this->m_currentState = READY;
        this->paint(this->m_paintWidget);
        emit blendOutFinished();
    }
    else
    {
        this->paint(this->m_paintWidget);
    }
}

void overlayBase::paint(QPaintDevice * device)
{
    // actually paint
    QPainter p;
    p.begin(device);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::HighQualityAntialiasing);
    p.setOpacity(1.0);
    p.drawPixmap(0, 0, device->width(), device->height(), this->m_background);

    p.setOpacity(this->m_currentBlendValue);
    p.setPen(QPen(QColor(0, 0, 0)));
    p.setBrush(QBrush(QColor(0,0,0)));    

    p.drawPixmap(0, 0, device->width(), device->height(), this->m_overlay);
    p.end();
}
