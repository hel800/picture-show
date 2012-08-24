/* ------------------------------------------------------------------
overlayinfo.h is part of picture-show.
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
August 2012

--------------------------------------------------------------------*/

#ifndef OVERLAYINFO_H
#define OVERLAYINFO_H

#include <QFileInfo>

#include <overlaybase.h>
#include "exif.h"

class overlayInfo : public overlayBase
{
    Q_OBJECT
public:
    overlayInfo(QWidget * pWidget);

    void setImageNumberAndNumberOfImages(int num, int total);
    void setCurrentExifInformation(EXIFInfo info);
    void setCurrentFileInformation(QFileInfo info);

    static void ggT (float fl, int &numerator, int &denominator);

protected slots:
    void paint(QPaintDevice *device);

protected:
    void generatePixmap();

private:
    int imageNumber;
    int numTotalImages;

    EXIFInfo currentInformation;
    QFileInfo currentFileInformation;
};

#endif // OVERLAYINFO_H
