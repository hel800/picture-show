#include "overlayinfo.h"

overlayInfo::overlayInfo(QWidget *pWidget) : overlayBase(pWidget)
{
}

void overlayInfo::generatePixmap()
{
    if (this->m_background.isNull())
        return;

    this->m_overlay = QPixmap(this->m_paintWidget->width(), this->m_paintWidget->height());
    this->m_overlay.fill(QColor(0, 0, 0, 0));

    double lightOpacity = 0.8;

    int rectWidth;
    int rectHeight;
    int rectPosX;
    int rectPosY;

    rectHeight = int(this->m_paintWidget->height() * 0.17);
    rectWidth =  int(this->m_paintWidget->width() * 0.9);
    rectPosX = int((this->m_paintWidget->width() - rectWidth) / 2.0);
    rectPosY = int(this->m_paintWidget->height() * 0.92);

    int rect2Width = rectWidth - this->m_paintWidget->height() * 0.016;
    int rect2Height = rectHeight - this->m_paintWidget->height() * 0.016;
    int rect2PosX = rectPosX + this->m_paintWidget->height() * 0.008;
    int rect2PosY = rectPosY + this->m_paintWidget->height() * 0.008;

    QBrush rectBrush(QColor::fromRgb(255, 255, 255, 80));
    QPen rectPen(QColor::fromRgb(255, 255, 255, 0));
    rectPen.setWidth(0);

    QLinearGradient linearGrad(0.0, 0.0, this->m_paintWidget->width(), this->m_paintWidget->height());
    linearGrad.setColorAt(0, QColor::fromRgb(0, 0, 0, 199));
    linearGrad.setColorAt(1, QColor::fromRgb(0, 0, 0, 240));
    QBrush rect2Brush(linearGrad);

    QPen rect2Pen(QColor::fromRgb(0, 0, 0, 0));
    rect2Pen.setWidth(0);

    QPixmap logo(":/images/img/logo256x256.png");
    logo = logo.scaledToHeight(int(rect2Height*0.15), Qt::SmoothTransformation);

    QPixmap logoText(":/images/img/logoText.png");
    logoText = logoText.scaledToHeight(int(rect2Height*0.05), Qt::SmoothTransformation);

    QString version = tr("version: %1").arg(qApp->applicationVersion());
    QFont fontVersion(QString("Helvetica"), int(rectHeight*0.015));
    QFontMetrics fm(fontVersion);
    int widthVersion = fm.width(version);
    int heightVersion = fm.height();

    QRect rectVersion((int((this->m_paintWidget->width() / 2.0) - (widthVersion / 2.0))),
                      rect2PosY + 6 + logo.height() + logoText.height(),
                      widthVersion, heightVersion);

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
