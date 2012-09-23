/* ------------------------------------------------------------------
loadimage.h is part of picture-show.
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

#ifndef LOADIMAGE_H
#define LOADIMAGE_H

#include <QThread>
#include <QImage>
#include <QFile>
#include <QPicture>

#include "global.h"

class loadImage : public QThread
{
public:
    loadImage();
    loadImage(const QString & filename, int w, int h);

    void setTask(const QString & filename, int w, int h, ScaleType type = IMAGE);
    void run();

    QImage * getImage();

    int getSourceWidth();
    int getSourceHeight();

signals:
    void loadImageFinished(QImage * imgAdd);

private:
    QString fn;
    QImage * img;

    int source_width;
    int source_height;

    int target_width;
    int target_height;

    ScaleType stype;
};

#endif // LOADIMAGE_H
