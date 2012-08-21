#include "overlayinfo.h"

#include <iostream>

overlayInfo::overlayInfo(QWidget *pWidget) : overlayBase(pWidget)
{
    this->imageNumber = -1;
    this->numTotalImages = -1;
}

void overlayInfo::setImageNumberAndNumberOfImages(int num, int total)
{
    this->imageNumber = num;
    this->numTotalImages = total;
}

void overlayInfo::ggT (float fl, int & numerator, int & denominator)
{
   float newFL = fl;

   int c = 0;
   while(newFL - int(newFL) != 0)
   {
       newFL *= 10;

       c++;
       if (c > 8)
           break;
   }

   int num = int(newFL);
   int den = int(pow(float(10), float(c)));

   if (num >= den)
   {
       numerator = num;
       denominator = den;
       return;
   }

   std::cout << "newFL: " << int(newFL) << " denom: " << den << " (" << c << ")" << std::endl;

   int a = num;
   int b = den;

   int rest;
   do
   {
       rest = b % a;
       b = a;
       std::cout << "b: " << b << " rest: " << rest << std::endl;
       a = rest;
   } while (rest > 200);

   std::cout << "b: " << b << std::endl;

   numerator = num/b;
   denominator = den/b;
}

void overlayInfo::generatePixmap()
{
    if (this->m_background.isNull())
        return;

    this->m_overlay = QPixmap(this->m_paintWidget->width(), this->m_paintWidget->height());
    this->m_overlay.fill(QColor(0, 0, 0, 0));

    double lightOpacity = 0.8;
    double veryLightOpacity = 0.6;

    int rectWidth;
    int rectHeight;
    int rectPosX;
    int rectPosY;

    rectHeight = int(this->m_paintWidget->height() * 0.17);
    rectWidth =  int(this->m_paintWidget->width() * 0.9);
    rectPosX = int((this->m_paintWidget->width() - rectWidth) / 2.0);
    rectPosY = int(this->m_paintWidget->height() * 0.93);

    int smaller = int(this->m_paintWidget->height() * 0.016);
    if (smaller % 2 != 0)
        smaller += 1;
    int rect2Width = rectWidth - smaller;
    int rect2Height = rectHeight - smaller;
    int rect2PosX = rectPosX + smaller/2;
    int rect2PosY = rectPosY + smaller/2;

    int borderwidth = int(rectWidth - rect2Width);
    double radius1 = this->m_paintWidget->width() / 70.0;
    double radius2 = this->m_paintWidget->width() / 90.0;

    QBrush rectBrush(QColor::fromRgb(255, 255, 255, 80));
    QPen rectPen(QColor::fromRgb(255, 255, 255, 0));
    rectPen.setWidth(0);

    QLinearGradient linearGrad(0.0, 0.0, this->m_paintWidget->width(), this->m_paintWidget->height());
    linearGrad.setColorAt(0, QColor::fromRgb(0, 0, 0, 199));
    linearGrad.setColorAt(1, QColor::fromRgb(0, 0, 0, 240));
    QBrush rect2Brush(linearGrad);

    QPen rect2Pen(QColor::fromRgb(0, 0, 0, 0));
    rect2Pen.setWidth(0);

    QPixmap infoLogo(":/images/img/Button-info-256.png");
    infoLogo = infoLogo.scaledToHeight(int(rect2Height*0.56), Qt::SmoothTransformation);

    QString imageNumber = tr("%1/%2").arg(this->imageNumber).arg(this->numTotalImages);
    QFont fontNumber(QString("Helvetica"), int(rect2Height*0.15));
    QFontMetrics fm(fontNumber);
    QPen textPen(QColor::fromRgb(255, 255, 255, 200));
    QRect rectImageNumber(rectPosX+borderwidth*2+infoLogo.width(), rectPosY+borderwidth, fm.width(imageNumber), int(rect2Height*0.3));

    QPainter p;
    p.begin(&this->m_overlay);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::HighQualityAntialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);
    p.setOpacity(lightOpacity);
    p.setPen(rectPen);
    p.setBrush(rectBrush);
    p.drawRoundedRect(rectPosX, rectPosY, rectWidth, rectHeight, radius1, radius1);
    p.setPen(rect2Pen);
    p.setBrush(rect2Brush);
    p.drawRoundedRect(rect2PosX, rect2PosY, rect2Width, rect2Height, radius2, radius2);
    p.drawPixmap(rectPosX+borderwidth, rectPosY-borderwidth, infoLogo);
    p.setPen(textPen);
    p.setFont(fontNumber);
    p.drawText(rectImageNumber, Qt::AlignLeft | Qt::AlignVCenter, imageNumber);

    p.end();
}

void overlayInfo::paint(QPaintDevice * device)
{
    // actually paint
    QPainter p;
    p.begin(device);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::HighQualityAntialiasing);
    p.setOpacity(1.0);
    p.drawPixmap(0, 0, device->width(), device->height(), this->m_background);

    int fadeHeight = int(this->m_paintWidget->height() * 0.03);

    p.setPen(QPen(QColor(0, 0, 0)));
    p.setBrush(QBrush(QColor(0,0,0)));
    p.setOpacity(this->m_currentBlendValue);
    p.drawPixmap(0, 0 + (fadeHeight * (1 - this->m_currentBlendValue)), device->width(), device->height(), this->m_overlay);
    p.end();
}
