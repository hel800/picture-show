#include "overlayinfo.h"

#include <iostream>


overlayInfo::overlayInfo(QWidget *pWidget) : overlayBase(pWidget)
{
    this->imageNumber = -1;
    this->numTotalImages = -1;
    this->m_fadeSpeed = 0.075;
}

void overlayInfo::setImageNumberAndNumberOfImages(int num, int total)
{
    this->imageNumber = num;
    this->numTotalImages = total;
}

void overlayInfo::setCurrentExifInformation(EXIFInfo info)
{
    this->currentInformation = info;
}

void overlayInfo::setCurrentFileInformation(QFileInfo info)
{
    this->currentFileInformation = info;
}

void overlayInfo::setCurrentImageResolution(const QString & imgRes)
{
    this->currentImageResolution = QString(imgRes);
}

void overlayInfo::ggT (float fl, int & numerator, int & denominator)
{
   float newFL = fl;

   if (newFL == 0.0)
       return;

   int c = 0;
   while(newFL - int(newFL) != 0)
   {
       newFL *= 10;

       c++;
       if (c > 8)
           break;
   }

   int num = int(newFL);
   int den = int(qPow(float(10), float(c)));

   if (num >= den)
   {
       numerator = num;
       denominator = den;
       return;
   }

   int a = num;
   int b = den;

//   std::cout << "num: " << num << " den: " << den << std::endl;

   int rest;
   do
   {
       rest = b % a;
       b = a;
       a = rest;
//       std::cout << "a: " << a << std::endl;
   } while (rest > 200);

//   std::cout << std::endl;

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

    // Image size and resolution
    QString imageSizeAndResolution = this->currentImageResolution + " (";
    imageSizeAndResolution += QString::number(double(this->currentFileInformation.size())/1024.0, 'f', 0) + " kb)";

    // EXIF INFO
    int exp_time1, exp_time2 = 0;
    this->ggT(this->currentInformation.exposureTime, exp_time1, exp_time2);

    QString imageFileName = this->currentFileInformation.fileName();

    QString cameraString = this->currentInformation.cameraModel_st.trimmed();
    if (!this->currentInformation.cameraMake_st.trimmed().isEmpty())
        cameraString += " (" + this->currentInformation.cameraMake_st.trimmed() + ")";

    QString imageDescription = this->currentInformation.imgDescription_st.trimmed();

    QString expFStop;
    if (this->currentInformation.FStop != 0.0)
        expFStop += "f/" + QString::number(this->currentInformation.FStop, 'f', 1);

    if (this->currentInformation.exposureTime > 0.5 && this->currentInformation.exposureTime != 0.0)
    {
        expFStop += "   " + QString::number(this->currentInformation.exposureTime, 'f', 1) + " sec";
    }
    else if (this->currentInformation.exposureTime <= 0.5 && this->currentInformation.exposureTime != 0.0)
    {
        double den = 1.0 / this->currentInformation.exposureTime;
        expFStop += "   1/" + QString::number(int(den + .5)) + " sec";
    }

    QString imageFocalLength;
    if (this->currentInformation.focalLength >= 1.0)
        imageFocalLength += QString::number(this->currentInformation.focalLength, 'f', 0) + " mm";

    QString dateTime = this->currentInformation.dateTimeOriginal_st.trimmed();
    QRegExp exp("^[0-9]{4}:[0-9]{2}:[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}$");
    if (exp.exactMatch(dateTime))
    {
        QDateTime dt = QDateTime::fromString(dateTime, "yyyy:MM:dd hh:mm:ss");
        if (dt.isValid())
        {
            dateTime = dt.toString("dd.MM.yyyy hh:mm:ss");
        }
        else
        {
            dateTime = "";
        }
    }


//    // get image dimensions from file
//    QString imageDimensions = "";
//    int t0 = clock();

//    {
//        QImage temp = QImage(this->currentFileInformation.absoluteFilePath());
//        if (!temp.isNull())
//        {
//            imageDimensions = tr("(%1 x %2)").arg(temp.width()).arg(temp.height());
//        }
//    }

//    int t1 = clock();

//    printf ("It took me %d clicks (%f seconds).\n",t1-t0,((float)t1-t0)/CLOCKS_PER_SEC);

    int rectWidth;
    int rectHeight;
    int rectPosX;
    int rectPosY;

    rectWidth =  int(this->m_paintWidget->width() * 0.9);
    rectHeight = rectWidth * 0.1;
    rectPosX = int((this->m_paintWidget->width() - rectWidth) / 2.0);
    rectPosY = int(this->m_paintWidget->height() - (rectHeight * 0.45));

    int smaller = int(this->m_paintWidget->width() * 0.010);
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

    QPixmap infoLogo(":/images/img/info-logo.png");
    infoLogo = infoLogo.scaledToHeight(int(this->m_paintWidget->width() * 0.045), Qt::SmoothTransformation);

    QString imageNumber = tr("%1/%2").arg(this->imageNumber).arg(this->numTotalImages);
    QFont fontNumber(QString("Helvetica"), int(rect2Height*0.18));
    QFontMetrics fm(fontNumber);
    QPen textPen(QColor::fromRgb(255, 255, 255, 200));
    QRect rectImageNumber(rectPosX + borderwidth*3 + infoLogo.width(), rectPosY+borderwidth, fm.width(imageNumber), int(rect2Height*0.3));

    int rectInfoWidth = int((rect2Width - int(rectImageNumber.width() + infoLogo.width() + borderwidth * 8.0)) / 4.0) - borderwidth;

    int rectInfoWidth1 = int(rectInfoWidth * 1.3);
    int rectInfoWidth2 = int(rectInfoWidth * 0.7);
    int rectInfoWidth3 = int(rectInfoWidth * 1.3);
    int rectInfoWidth4 = int(rectInfoWidth * 0.7);

    QRect rectInfo1(rectImageNumber.x() + fm.width(imageNumber) + borderwidth * 4, rectPosY+borderwidth, rectInfoWidth1, int(rect2Height*0.3 + .5));
    QRect rectInfo2(rectInfo1.x() + rectInfoWidth1 + borderwidth, rectPosY+borderwidth, rectInfoWidth2, int(rect2Height*0.3 + .5));
    QRect rectInfo3(rectInfo2.x() + rectInfoWidth2 + borderwidth, rectPosY+borderwidth, rectInfoWidth3, int(rect2Height*0.3 + .5));
    QRect rectInfo4(rectInfo3.x() + rectInfoWidth3 + borderwidth, rectPosY+borderwidth, rectInfoWidth4, int(rect2Height*0.3 + .5));

    QRect rectImageName(rectInfo1.x(), rectInfo1.y(), rectInfo1.width(), int(rectInfo1.height() / 2.0));
    QRect rectImageSize(rectInfo1.x(), rectInfo1.y() + int(rectInfo1.height() / 2.0), rectInfo1.width(), int(rectInfo1.height() / 2.0));

    QRect rectExpFstop(rectInfo2.x(), rectInfo2.y(), rectInfo2.width(), int(rectInfo2.height() / 2.0));
    QRect rectFocalLength(rectInfo2.x(), rectInfo2.y() + int(rectInfo2.height() / 2.0), rectInfo2.width(), int(rectInfo2.height() / 2.0));

    QRect rectCameraModel(rectInfo3.x(), rectInfo3.y(), rectInfo3.width(), int(rectInfo3.height() / 2.0));
    QRect rectDescription(rectInfo3.x(), rectInfo3.y() + int(rectInfo3.height() / 2.0), rectInfo3.width() + borderwidth + rectInfo4.width(),  int(rectInfo3.height() / 2.0));

    QRect rectDateTaken(rectInfo4.x(), rectInfo4.y(), rectInfo4.width(), int(rectInfo4.height() / 2.0));

    QFont fontText(QString("Helvetica"), int(rectInfo1.height() / 3.0));
    QFontMetrics ftm(fontText);

    while(ftm.width(imageFileName) > rectInfo1.width())
    {
        imageFileName = "..." + imageFileName.right(imageFileName.length()-4);
    }

    while(ftm.width(imageDescription) > rectDescription.width())
    {
        imageDescription = imageDescription.left(imageDescription.length()-4) + "...";
    }

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
    p.drawPixmap(rectPosX + 2*borderwidth, int(rectPosY - borderwidth *0.75), infoLogo);
    p.setPen(textPen);
    p.setFont(fontNumber);
    p.drawText(rectImageNumber, Qt::AlignLeft | Qt::AlignVCenter, imageNumber);
//    p.drawRect(rectInfo1);
//    p.drawRect(rectInfo2);
//    p.drawRect(rectInfo3);
//    p.drawRect(rectDescription);
    p.setFont(fontText);
    p.drawText(rectImageName, Qt::AlignRight | Qt::AlignVCenter, imageFileName);
    p.drawText(rectImageSize, Qt::AlignRight | Qt::AlignVCenter, imageSizeAndResolution);

    p.drawText(rectExpFstop, Qt::AlignRight | Qt::AlignVCenter, expFStop);
    p.drawText(rectFocalLength, Qt::AlignRight | Qt::AlignVCenter, imageFocalLength);

    p.drawText(rectCameraModel, Qt::AlignRight | Qt::AlignVCenter, cameraString);
    p.drawText(rectDescription, Qt::AlignRight | Qt::AlignVCenter, imageDescription);

    p.drawText(rectDateTaken, Qt::AlignRight | Qt::AlignVCenter, dateTime);

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

    int fadeHeight = int(this->m_paintWidget->width() * 0.09);

    p.setPen(QPen(QColor(0, 0, 0)));
    p.setBrush(QBrush(QColor(0,0,0)));
    p.setOpacity(this->m_currentBlendValue);
    p.drawPixmap(0, fadeHeight * (0.15 - 0.15 * this->m_currentBlendValue), device->width(), device->height(), this->m_overlay);
    p.end();
}
