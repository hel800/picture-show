/* ------------------------------------------------------------------
loadimage.cpp is part of picture-show.
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

#include "loadimage.h"

loadImage::loadImage()
{
    this->fn = "";
    this->img = NULL;

    this->target_height = 0;
    this->target_width = 0;

    this->source_width = -1;
    this->source_height = -1;
}

loadImage::loadImage(const QString & filename, int w, int h)
{    
   this->fn = filename;
   this->img = NULL;

   this->target_width = w;
   this->target_height = h;
}

void loadImage::setTask(const QString &filename, int w, int h, ScaleType type)
{
    this->fn = filename;

    this->target_width = w;
    this->target_height = h;

    this->stype = type;
}

void loadImage::run()
{
    QImage temp = QImage(fn);
    if (temp.isNull())
    {
        this->img = new QImage();
        return;
    }

    this->source_width = temp.width();
    this->source_height = temp.height();

    if (this->img != NULL)
        delete this->img;

    this->img = NULL;
    if (this->stype == IMAGE || temp.width() < temp.height())
        this->img = new QImage(temp.scaled(this->target_width, this->target_height, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    else
        this->img = new QImage(temp.scaled(this->target_width, this->target_height, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

    if (this->img == NULL)
    {
        this->img = new QImage();
    }
    else
    {
        EXIFInfo exif = readExifHeader(fn);

        if (exif.orientation != 1)
        {
            QTransform trans;
            QImage newImage(*this->img);
            switch (exif.orientation)
            {
            case 2:
                newImage = this->img->mirrored(true, false);
            break;
            case 3:
                trans.rotate(180.0);
            break;
            case 4:
                newImage = this->img->mirrored(false, true);
            break;
            case 5:
                newImage = this->img->mirrored(true, false);
                trans.rotate(270.0);
            break;
            case 6:
                trans.rotate(90.0);
            break;
            case 7:
                newImage = this->img->mirrored(true, false);
                trans.rotate(90.0);
            break;
            case 8:
                trans.rotate(270.0);
            break;
            }

            delete this->img;
            this->img = NULL;
            this->img = new QImage(newImage.transformed(trans));
        }
    }
}

QImage * loadImage::getImage()
{
    return this->img;
}

int loadImage::getSourceWidth()
{
    return this->source_width;
}

int loadImage::getSourceHeight()
{
    return this->source_height;
}
