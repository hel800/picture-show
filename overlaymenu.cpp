/* ------------------------------------------------------------------
overlaymenu.cpp is part of picture-show.
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

#include "overlaymenu.h"

overlayMenu::overlayMenu(QWidget *pWidget) :
    overlayBase(pWidget)
{
}

void overlayMenu::setDirectory(const QString &path)
{
    this->m_currentDirectory = QDir(path);
}

void overlayMenu::generatePixmap()
{
    if (this->m_background.isNull())
        return;

    this->m_overlay = QPixmap(this->m_paintWidget->width(), this->m_paintWidget->height());
    this->m_overlay.fill(QColor(0, 0, 0, 0));

    double lightOpacity = 0.8;
    double darkOpacity = 0.9;

    int rectWidth;
    int rectHeight;
    int rectPosX;
    int rectPosY;

    if (this->m_paintWidget->width() > this->m_paintWidget->height())
    {
        rectHeight = int(this->m_paintWidget->height() * 0.6);
        rectWidth = int(rectHeight * 1.6);
        rectPosX = int((this->m_paintWidget->width() - rectWidth) / 2.0);
        rectPosY = int((this->m_paintWidget->height() - rectHeight) / 2.0);
    }
    else
    {
        rectWidth = rectHeight = int(this->m_paintWidget->width() * 0.8);
        rectHeight = int(rectWidth * 0.625);
        rectPosX = int((this->m_paintWidget->width() - rectWidth) / 2.0);
        rectPosY = int((this->m_paintWidget->height() - rectHeight) / 2.0);
    }

    int rect2Width = rectWidth - 40;
    int rect2Height = rectHeight - 40;
    int rect2PosX = rectPosX + 20;
    int rect2PosY = rectPosY + 20;

    int skipH = int(rect2Width * 0.05);
    int skipV = int(rect2Height * 0.05);

    QBrush rectBrush(QColor::fromRgb(255, 255, 255, 120));
    QPen rectPen(QColor::fromRgb(255, 255, 255, 0));
    rectPen.setWidth(0);

    QLinearGradient linearGrad(0.0, 0.0, this->m_paintWidget->width(), this->m_paintWidget->height());
    linearGrad.setColorAt(0, QColor::fromRgb(0, 0, 0, 100));
    linearGrad.setColorAt(1, QColor::fromRgb(0, 0, 0, 240));
    QBrush rect2Brush(linearGrad);

    QPen rect2Pen(QColor::fromRgb(0, 0, 0, 0));
    rect2Pen.setWidth(0);

    QPainter p;
    p.begin(&this->m_overlay);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::HighQualityAntialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);
    p.setOpacity(lightOpacity);
    p.setPen(rectPen);
    p.setBrush(rectBrush);
    p.drawRoundedRect(rectPosX, rectPosY, rectWidth, rectHeight, 22.0, 22.0);
    p.setPen(rect2Pen);
    p.setBrush(rect2Brush);
    p.drawRoundedRect(rect2PosX, rect2PosY, rect2Width, rect2Height, 20.0, 20.0);
    p.end();
}
