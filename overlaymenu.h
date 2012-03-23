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
    void addCategory(const QString & name, const QList<QFileInfo>& list);

public slots:
    void keyPressEvent ( QKeyEvent * event );
    
protected:
    void generatePixmap();

private:
    QList< QPair< QString, QList<QFileInfo> > > m_categoryContainer;
    int m_currentIndex;
    int m_currentGroup;

    QPixmap generateFolderAndName(int group, int index, QSize size);
    QPixmap generateDirList(QSize size);


};

#endif // OVERLAYMENU_H
