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
    this->m_currentIndex = 0;
    this->m_currentGroup = 0;

    this->isLoading = true;
}

void overlayMenu::addCategory(const QString & name, const QList<QFileInfo>& list)
{
    QPair< QString, QList<QFileInfo> > newPair;
    newPair.first = QString(name);
    newPair.second = QList<QFileInfo>(list);
    this->m_categoryContainer.append(newPair);
}

void overlayMenu::keyPressEvent ( QKeyEvent * event )
{
    switch (event->key())
    {
    case Qt::Key_Left:
        if (this->m_currentGroup > 0)
        {
            this->m_currentGroup--;
            this->m_currentIndex = 0;
            this->update();
        }
    break;
    case Qt::Key_Right:
        if (this->m_categoryContainer.size() > this->m_currentGroup+1)
        {
            this->m_currentGroup++;
            this->m_currentIndex = 0;
            this->update();
        }
    break;
    case Qt::Key_Up:
        if (this->m_currentIndex > 0)
            this->m_currentIndex--;
        this->update(this->m_background);
    break;
    case Qt::Key_Down:
        if (this->m_categoryContainer.size() > this->m_currentGroup)
        {
            if (this->m_currentIndex+1 < this->m_categoryContainer.at(this->m_currentGroup).second.size())
                this->m_currentIndex++;
            this->update(this->m_background);
        }
    break;
    case Qt::Key_PageUp:
        this->m_currentIndex -= 5;
        if (this->m_currentIndex < 0)
            this->m_currentIndex = 0;
        this->update(this->m_background);
    break;
    case Qt::Key_PageDown:
        if (this->m_categoryContainer.size() > this->m_currentGroup)
        {
            this->m_currentIndex += 5;
            if (this->m_currentIndex >= this->m_categoryContainer.at(this->m_currentGroup).second.size())
                this->m_currentIndex = this->m_categoryContainer.at(this->m_currentGroup).second.size()-1;
            this->update(this->m_background);
        }
    break;
    }
}

void overlayMenu::loadingFinished()
{
    this->isLoading = false;
    this->update(this->m_background);
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

    QRect titleRect(int(rect2PosX + rect2Width * .03), int(rect2PosY + rect2Height * .05), int(rect2Width * .94), int(rect2Height * .12));
    QRect dirRect(int(rect2PosX + rect2Width * .03), int(rect2PosY + (rect2Height * .12) + (rect2Height * .1)), int(rect2Width * .6), int(rect2Height * .44));
    QRect groupRect(int(rect2PosX + rect2Width * .04), int(rect2PosY + (rect2Height * .12) + (rect2Height * .1)), int(rect2Width * .58), int(rect2Height * .06));
    QRect listRect(int(rect2PosX + rect2Width * .04), int(rect2PosY + (rect2Height * .12) + (rect2Height * .14)), int(rect2Width * .58), int(rect2Height * .44));
    QRect setRect(int(rect2PosX + rect2Width * .03), int(rect2PosY + (rect2Height * .49) + (rect2Height * .12) + (rect2Height * .1)), int(rect2Width * .94), int(rect2Height * .24));
    QRect infoRect(int(rect2PosX + rect2Width * .06) + int(rect2Width * .6), int(rect2PosY + (rect2Height * .12) + (rect2Height * .1)), int(rect2Width * .31), int(rect2Height * .44));

    QPixmap headerImage(":/images/img/library-256.png");
    headerImage = headerImage.scaledToHeight(titleRect.height()*2, Qt::SmoothTransformation);
    QFont fontTitle(QString("Helvetica"), int(titleRect.height() * 0.3));
    fontTitle.setStyleStrategy(QFont::PreferAntialias);
    QString titleText = tr("Bildbibliothek beta");

    QPixmap dirListing = this->generateDirList(listRect.size());
    QPixmap groupListing = this->generateGroupList(groupRect.size());


    // Painting
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

    p.setPen(QPen(Qt::red));
    p.drawRect(titleRect);
    p.drawRect(dirRect);
    p.drawRect(groupRect);
    p.drawRect(listRect);
    p.drawRect(setRect);
    p.drawRect(infoRect);

    p.setOpacity(lightOpacity);
    p.drawPixmap(titleRect.x(), titleRect.y()-int(titleRect.height()/2.0), headerImage.width(), headerImage.height(), headerImage);
    p.setFont(fontTitle);
    p.setPen(QPen(Qt::white));
    p.drawText(titleRect.x() + int(headerImage.width()*1.2), titleRect.y(), titleRect.width() - int(headerImage.width()*1.4), titleRect.height(), Qt::AlignVCenter, titleText);


    if (!dirListing.isNull())
        p.drawPixmap(listRect.x(), listRect.y(), dirListing.width(), dirListing.height(), dirListing);
    if (!groupListing.isNull())
        p.drawPixmap(groupRect.x(), groupRect.y(), groupListing.width(), groupListing.height(), groupListing);

    // if still loading print loading...
    if (this->isLoading)
    {
        QFont fontText(QString("Helvetica"), int(double(rect2Height) * 0.03));
        fontText.setStyleStrategy(QFont::PreferAntialias);
        p.setFont(fontText);
        p.setPen(QPen(Qt::white));
        p.drawText(listRect, Qt::AlignVCenter | Qt::AlignHCenter, tr("Bibliothek wird geladen..."));
    }

    p.end();
}

QPixmap overlayMenu::generateFolderAndName(int group, int index, QSize size)
{
    if (this->m_categoryContainer.size() <= group)
        return QPixmap();

    QList<QFileInfo> dirList = this->m_categoryContainer.at(group).second;

    if (dirList.size() <= index)
        return QPixmap();

    QFileInfo dirInfo = dirList.at(index);

    QPixmap pix(size);
    pix.fill(QColor(0, 0, 0, 0));

    QFont fontText(QString("Helvetica"), int(double(size.height()) * 0.4));
    fontText.setStyleStrategy(QFont::PreferAntialias);
    QFontMetrics fmHeader(fontText);
    QRect rectText(int(size.width()*0.1), int(size.height() * .2), int(size.width()*0.8), int(size.height() * 0.6));

    QString name = dirInfo.fileName();
    if (name == ".")
        name = tr(". <Hauptverzeichnis>");

    QPainter p;
    p.begin(&pix);
    p.setOpacity(0.7);
    p.setFont(fontText);
    p.setPen(QPen(Qt::white));
    p.drawText(rectText, name);
    p.end();

    return pix;
}

QPixmap overlayMenu::generateDirList(QSize size)
{
    if (this->m_categoryContainer.size() <= this->m_currentGroup)
        return QPixmap();

    QPixmap pix(size);
    pix.fill(QColor(0, 0, 0, 0));

    QList<QFileInfo> dirList = this->m_categoryContainer.at(this->m_currentGroup).second;

    QSize dirListingSize(size.width(), int(size.height() / 7.0));

    QPainter p;
    p.begin(&pix);

    int curYpos = 0;

    for (int i = this->m_currentIndex-3; i < this->m_currentIndex + 4; i++)
    {
        if (i < 0)
        {
            curYpos += dirListingSize.height();
            continue;
        }

        if (i >= dirList.size())
            break;

        QPixmap cur_dir = this->generateFolderAndName(this->m_currentGroup, i, dirListingSize);
        p.setOpacity(1.0 - (fabs(double(i-this->m_currentIndex))) / 3.5);
        p.drawPixmap(0, curYpos, cur_dir.width(), cur_dir.height(), cur_dir);
        curYpos += dirListingSize.height();
    }

    p.end();

    return pix;
}

QPixmap overlayMenu::generateGroupList(QSize size)
{
    QPixmap pix(size);
    pix.fill(QColor(0, 0, 0, 0));

    QPainter p;
    p.begin(&pix);

    QFont fontText(QString("Helvetica"), int(double(size.height()) * 0.5));
    fontText.setStyleStrategy(QFont::PreferAntialias);

    QSize groupListingSize(int(size.width()/5.0), size.height());
    int curXpos = 0;

    for (int i = this->m_currentGroup-2; i < this->m_currentGroup + 3; i++)
    {
        if (i < 0)
        {
            curXpos += groupListingSize.width();
            continue;
        }

        if (i >= this->m_categoryContainer.size())
            break;

        QString groupName = this->m_categoryContainer.at(i).first;
        p.setOpacity(1.0 - (fabs(double(i-this->m_currentGroup))) / 3.5);
        p.setFont(fontText);
        p.setPen(QPen(Qt::white));
        p.drawText(curXpos, 0, groupListingSize.width(), groupListingSize.height(), Qt::AlignHCenter | Qt::AlignVCenter, groupName);
        curXpos += groupListingSize.width();
    }

    p.end();

    return pix;
}


