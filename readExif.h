/* ------------------------------------------------------------------
readExif.h is part of picture-show.
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

#ifndef READEXIF_H
#define READEXIF_H

#include <QString>
#include <QDateTime>
#include <QFile>
#include <iostream>
#include "exif.h"

static QDateTime readOriginalDate(const QString &fname)
{
    QDateTime originalDate;

    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug("Can't read file");
        qDebug(fname.toStdString().c_str());
        return originalDate;
    }

    qint64 fsize = file.size();
    qint64 cappedSize = 1500;
    unsigned char *buf = new unsigned char[cappedSize];

    if (file.read((char*)buf, cappedSize) != cappedSize)
    {
        qDebug("Can't read file2");
        delete[] buf;
        file.close();
        return originalDate;
    }

    QString date;
    ExtractDateTime(buf, cappedSize, date);

    if(!date.isEmpty())
    {
        originalDate = QDateTime::fromString(date, QString("yyyy:MM:dd HH:mm:ss"));
    }
    else
    {
        file.seek(0);
        delete[] buf;
        buf = new unsigned char[fsize];

        if (file.read((char*)buf, fsize) != fsize)
        {
            qDebug("Can't read file2");
            delete[] buf;
            file.close();
            return originalDate;
        }

        qDebug("Try to read Date from whole file!");
        ExtractDateTime(buf, fsize, date);

        if(!date.isEmpty())
            originalDate = QDateTime::fromString(date, QString("yyyy:MM:dd HH:mm:ss"));
        else
            qDebug("CANNOT READ DATE!!!");
    }

    delete[] buf;
    file.close();

    return originalDate;
}


#endif // READEXIF_H
