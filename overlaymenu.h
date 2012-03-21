/* ------------------------------------------------------------------
overlaymenu.h is part of picture-show.
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

#ifndef OVERLAYMENU_H
#define OVERLAYMENU_H

#include <QString>
#include <QDir>

#include "overlaybase.h"

class overlayMenu : public overlayBase
{
    Q_OBJECT
public:
    explicit overlayMenu(QWidget * pWidget);
    void setDirectory(const QString &path);
    
protected:
    void generatePixmap();

private:
    QDir m_currentDirectory;


};

#endif // OVERLAYMENU_H
