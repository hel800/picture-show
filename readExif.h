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
#include <stdio.h>
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
    unsigned char *buf = new unsigned char[fsize];

    if (file.read((char*)buf, cappedSize) != cappedSize)
    {
        qDebug("Can't read file2");
        delete[] buf;
        file.close();
        return originalDate;
    }

    //EXIFInfo result;
    //ParseEXIF(buf, fsize, result);
    QString date;
    ExtractDateTime(buf, fsize, date);

    if(!date.isEmpty())
    {
        originalDate = QDateTime::fromString(date, QString("yyyy:MM:dd HH:mm:ss"));
    }
    else
    {
        if (file.read((char*)buf, fsize) != fsize)
        {
            qDebug("Can't read file2");
            delete[] buf;
            file.close();
            return originalDate;
        }

        ExtractDateTime(buf, fsize, date);

        if(!date.isEmpty())
            originalDate = QDateTime::fromString(date, QString("yyyy:MM:dd HH:mm:ss"));
    }

    delete[] buf;
    file.close();

    return originalDate;
}


#endif // READEXIF_H
