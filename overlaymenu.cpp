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
    this->emitNewDirectoryOnBlendOut = false;
    connect(this, SIGNAL(blendOutFinished()), this, SLOT(emitOpenDirectory()));
}

void overlayMenu::addCategory(const QString & name, const QList<QFileInfo>& list)
{
    QPair< QString, QList<QFileInfo> > newPair;
    newPair.first = QString(name);
    newPair.second = QList<QFileInfo>(list);
    this->m_categoryContainer.append(newPair);

    qSort(this->m_categoryContainer.begin(), this->m_categoryContainer.end(), groupNameLess);
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
    case Qt::Key_Enter:
    case Qt::Key_Return:
        this->emitNewDirectoryOnBlendOut = true;
        this->blendOut();
    break;
    }
}

void overlayMenu::loadingFinished()
{
    this->isLoading = false;
    this->update(this->m_background);
}

void overlayMenu::emitOpenDirectory()
{
    if (this->emitNewDirectoryOnBlendOut)
        if (this->m_categoryContainer.size() > this->m_currentGroup)
            if (this->m_categoryContainer.at(this->m_currentGroup).second.size() > this->m_currentIndex)
            {
                QString current_dir_path = this->m_categoryContainer.at(this->m_currentGroup).second.at(this->m_currentIndex).absoluteFilePath();
                emit openDirectory(current_dir_path);
            }

    this->emitNewDirectoryOnBlendOut = false;
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

    QBrush rectBrush(QColor::fromRgb(255, 255, 255, 140));
    QPen rectPen(QColor::fromRgb(255, 255, 255, 0));
    rectPen.setWidth(0);

    QLinearGradient linearGrad(rectPosX, rectPosY, rectPosX+rectWidth, rectPosY+rectHeight);
    linearGrad.setColorAt(0, QColor::fromRgb(0, 0, 0, 150));
    linearGrad.setColorAt(1, QColor::fromRgb(0, 0, 0, 240));
    QBrush rect2Brush(linearGrad);

    QPen rect2Pen(QColor::fromRgb(0, 0, 0, 0));
    rect2Pen.setWidth(0);

    QRect titleRect(int(rect2PosX + rect2Width * .03), int(rect2PosY + rect2Height * .05), int(rect2Width * .94), int(rect2Height * .12));
    QRect dirRect(int(rect2PosX + rect2Width * .03), int(rect2PosY + (rect2Height * .12) + (rect2Height * .1)), int(rect2Width * .6), int(rect2Height * .44));
    QRect groupRect(int(rect2PosX + rect2Width * .04), int(rect2PosY + (rect2Height * .12) + (rect2Height * .1)), int(rect2Width * .58), int(rect2Height * .06));
    QRect listRect(int(rect2PosX + rect2Width * .04), int(rect2PosY + (rect2Height * .12) + (rect2Height * .14)), int(rect2Width * .58), int(rect2Height * .44));
    QRect infoRect(int(rect2PosX + rect2Width * .06) + int(rect2Width * .6), int(rect2PosY + (rect2Height * .12) + (rect2Height * .1)), int(rect2Width * .31), int(rect2Height * .44));

    QRect setRect1(int(rect2PosX + rect2Width * .03), int(rect2PosY + (rect2Height * .49) + (rect2Height * .12) + (rect2Height * .1)), int(rect2Width * .29), int(rect2Height * .24));
    QRect setRect2(int(rect2PosX + rect2Width * .35), int(rect2PosY + (rect2Height * .49) + (rect2Height * .12) + (rect2Height * .1)), int(rect2Width * .29), int(rect2Height * .24));
    QRect setRect3(int(rect2PosX + rect2Width * .67), int(rect2PosY + (rect2Height * .49) + (rect2Height * .12) + (rect2Height * .1)), int(rect2Width * .30), int(rect2Height * .24));

    QPixmap headerImage(":/images/img/library-256.png");
    headerImage = headerImage.scaledToHeight(titleRect.height()*2, Qt::SmoothTransformation);
    QFont fontTitle(QString("Helvetica"), int(titleRect.height() * 0.3));
    fontTitle.setStyleStrategy(QFont::PreferAntialias);
    QString titleText = tr("Bildbibliothek beta");

    QPixmap dirListing = this->generateDirList(listRect.size());
    QPixmap groupListing = this->generateGroupList(groupRect.size());
    QPixmap infoArea = this->generateInfoArea(infoRect.size());
    QPixmap setting1 = this->generateTransOptions(setRect1.size());


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

//    p.setPen(QPen(Qt::red));
//    p.drawRect(titleRect);
//    p.drawRect(dirRect);
//    p.drawRect(groupRect);
//    p.drawRect(listRect);
    p.drawRect(setRect1);
    p.drawRect(setRect2);
    p.drawRect(setRect3);
//    p.drawRect(infoRect);

    p.setOpacity(lightOpacity-0.1);
    p.drawPixmap(titleRect.x(), titleRect.y()-int(titleRect.height()/2.0), headerImage.width(), headerImage.height(), headerImage);
    p.setOpacity(lightOpacity);
    p.setFont(fontTitle);
    p.setPen(QPen(Qt::white));
    p.drawText(titleRect.x() + int(headerImage.width()*1.2), titleRect.y(), titleRect.width() - int(headerImage.width()*1.4), titleRect.height(), Qt::AlignVCenter, titleText);

    if (!dirListing.isNull())
        p.drawPixmap(listRect.x(), listRect.y(), dirListing.width(), dirListing.height(), dirListing);
    if (!groupListing.isNull())
        p.drawPixmap(groupRect.x(), groupRect.y(), groupListing.width(), groupListing.height(), groupListing);
    if (!infoArea.isNull())
        p.drawPixmap(infoRect.x(), infoRect.y(), infoArea.width(), infoArea.height(), infoArea);
    if (!setting1.isNull())
        p.drawPixmap(setRect1.x(), setRect1.y(), setting1.width(), setting1.height(), setting1);

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
    QFontMetrics fontTextMetric (fontText);

    QPixmap folderImage(":/images/img/pictureFolder-64.png");
    folderImage = folderImage.scaledToHeight(int(size.height()*0.7), Qt::SmoothTransformation);
    QRect rectText(int(size.width()*0.12), int(size.height() * .2), int(size.width()*0.8), int(size.height() * 0.6));

    QString name = dirInfo.fileName();
    if (name == ".")
        name = tr(". <Hauptverzeichnis>");

    while (fontTextMetric.boundingRect(name).width() > rectText.width())
        name = name.left(name.size()-4) + "...";

    QPainter p;
    p.begin(&pix);
    p.setOpacity(0.7);
    p.drawPixmap(int(size.width()*0.02), int(size.height()*0.15), folderImage.width(), folderImage.height(), folderImage);
    p.setOpacity(0.95);
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

        //        p.setOpacity(1.0 - (fabs(double(i-this->m_currentIndex))) / 3.5);
        if (abs(i-this->m_currentIndex) == 3)
            p.setOpacity(0.1);
        else if (i != this->m_currentIndex)
            p.setOpacity(0.3);
        else
        {
            p.setOpacity(1.0);
            p.setPen(QPen(Qt::NoPen));
            p.setBrush(QBrush(QColor(Qt::darkGray)));
            p.drawRect(0, curYpos, size.width(), cur_dir.height());
        }

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
        if (i != this->m_currentGroup)
        {
            p.setOpacity(0.3);
            fontText.setBold(false);
        }
        else
        {
            p.setOpacity(0.9);
            fontText.setBold(true);
        }

        p.setFont(fontText);
        p.setPen(QPen(Qt::white));
        p.drawText(curXpos, 0, groupListingSize.width(), groupListingSize.height(), Qt::AlignHCenter | Qt::AlignVCenter, groupName);
        curXpos += groupListingSize.width();
    }

    p.end();

    return pix;
}

QPixmap overlayMenu::generateInfoArea(QSize size)
{
    QPixmap pix(size);
    pix.fill(QColor(0, 0, 0, 30));

    QPainter p;
    p.begin(&pix);

    int pEdgeWidth = int(size.height()*0.02);
    int pEdgeLength = int(size.height()*0.15);

    QPolygon pEdge;
    pEdge << QPoint(0,0) << QPoint(0, pEdgeLength) << QPoint(pEdgeWidth, pEdgeLength);
    pEdge << QPoint(pEdgeWidth, pEdgeWidth) << QPoint(pEdgeLength, pEdgeWidth);
    pEdge << QPoint(pEdgeLength, 0);

    QPolygon pEdge2;
    pEdge2 << QPoint(size.width(),size.height()) << QPoint(size.width(), size.height()-pEdgeLength) << QPoint(size.width()-pEdgeWidth, size.height()-pEdgeLength);
    pEdge2 << QPoint(size.width()-pEdgeWidth, size.height()-pEdgeWidth) << QPoint(size.width()-pEdgeLength, size.height()-pEdgeWidth);
    pEdge2 << QPoint(size.width()-pEdgeLength, size.height());

    QPixmap images(":/images/img/images-256.png");
    images = images.scaledToHeight(int(size.height()*0.9), Qt::SmoothTransformation);

    p.setPen(QPen(Qt::NoPen));
    p.setBrush(QBrush(QColor(Qt::gray)));
    p.setOpacity(0.8);
    p.drawPolygon(pEdge);
    p.drawPolygon(pEdge2);
    p.setOpacity(0.1);
    p.drawPixmap(int((size.width()-images.width())/2.0), int(size.height()*0.05), images.width(), images.height(), images);

    p.end();

    return pix;
}

QPixmap overlayMenu::generateTransOptions(QSize size)
{
    QPixmap pix(size);
    pix.fill(QColor(0, 0, 0, 30));

    QPainter p;
    p.begin(&pix);

    QFont fontText(QString("Helvetica"), int(double(size.height()) * 0.1));
    fontText.setStyleStrategy(QFont::PreferAntialias);

    QPixmap button(":/images/img/key_button_F4.png");
    button = button.scaledToHeight(int(size.height()*0.25), Qt::SmoothTransformation);

    p.setFont(fontText);
    p.setOpacity(0.8);
    p.setPen(QPen(QColor(Qt::white)));
    p.drawText(QRect(0, 0, size.width(), size.height()), Qt::AlignTop | Qt::AlignLeft, tr("Übergangseffekt:"));
    p.drawPixmap(0, int((size.height()/2.0) - (button.height()/2.0)), button.width(), button.height(), button);

    p.end();

    return pix;
}

QPixmap overlayMenu::generateIntervalOptions(QSize size)
{
    return QPixmap();
}

QPixmap overlayMenu::generatePictureZoom(QSize size)
{
    return QPixmap();
}

static bool groupNameLess(const QPair< QString, QList<QFileInfo> > &g1, const QPair< QString, QList<QFileInfo> > &g2)
{
    QString g1_string = g1.first;
    QString g2_string = g2.first;

    if (g1_string.at(0).isDigit() && !g2_string.at(0).isDigit())
        return true;
    else if (!g1_string.at(0).isDigit() && g2_string.at(0).isDigit())
        return false;
    else if (g1_string.at(0).isDigit() && g2_string.at(0).isDigit())
        return g1_string > g2_string;
    else
        return g1_string < g2_string;
}


