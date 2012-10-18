/* ------------------------------------------------------------------
overlayhelp.h is part of picture-show.
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

#ifndef OVERLAYHELP_H
#define OVERLAYHELP_H

#include "overlaybase.h"
#include <QStringList>

class overlayHelp : public overlayBase
{
    Q_OBJECT
public:
    overlayHelp(QWidget * pWidget);

protected:
    void generatePixmap();

    QStringList helpItems();
    int calcTextSize(int targetSize);
};

#endif // OVERLAYHELP_H
