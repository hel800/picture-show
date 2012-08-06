/* ------------------------------------------------------------------
overlaybase.h is part of picture-show.
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

#ifndef OVERLAYBASE_H
#define OVERLAYBASE_H

#include <QObject>
#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QTimer>
#include <QApplication>

class overlayBase : public QObject
{
    Q_OBJECT
public:
    enum State
    {
        READY,
        BLEND_IN,
        BLEND_OUT
    };

    overlayBase(QWidget * pWidget);
    ~overlayBase();

    void setFadeSteps(double step);
    
signals:
    void blendInFinished();
    void blendOutFinished();
    
public slots:
    virtual bool blendIn(const QPixmap& bg);
    virtual bool blendOut();

    virtual void update(const QPixmap &bg);

protected slots:
    virtual void timedOut();
    virtual void paint(QPaintDevice *device);

protected:
    virtual void generatePixmap() = 0;

    QWidget * m_paintWidget;
    QPixmap m_overlay;
    QPixmap m_background;
    QTimer * m_overlayTimer;

    State m_currentState;
    double m_currentBlendValue;
    double m_fadeSpeed;
};

#endif // OVERLAYBASE_H
