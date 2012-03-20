/* ------------------------------------------------------------------
loaddirectory.h is part of picture-show.
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

#ifndef LOADDIRECTORY_H
#define LOADDIRECTORY_H

#include <QThread>
#include <QList>
#include <QPair>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>

#include "readExif.h"
#include "global.h"

static bool fileCreateLessThan(const QPair<QFileInfo, QDateTime> &f1, const QPair<QFileInfo, QDateTime> &f2);

class loadDirectory : public QThread
{
    Q_OBJECT
public:
    loadDirectory();

    void setDirectory(const QString &path);
    void setDirectoryList(QList<QFileInfo> *list);
    void setSorting(Sorting sortType);
    QString& getErrorMsg();

    void run();
    
signals:
    void loadDirectoryFinished(bool success);

private:
    Sorting m_sorting;
    QString m_path;
    QString m_error_msg;
    QList<QFileInfo> * m_dirList;
    
};

#endif // LOADDIRECTORY_H
