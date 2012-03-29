/* ------------------------------------------------------------------
loadlibrary.h is part of picture-show.
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

#ifndef LOADLIBRARY_H
#define LOADLIBRARY_H

#include <QThread>
#include <QPair>
#include <QMap>

#include "global.h"
#include "overlaymenu.h"
#include "readExif.h"

static bool fileCreateLessThan(const QPair< QDateTime, QFileInfo > &f1, const QPair< QDateTime, QFileInfo > &f2);

class loadLibrary : public QThread
{
    Q_OBJECT
public:
    explicit loadLibrary(overlayMenu * oMenu);

    void setPath(const QString& dirpath);
    QString& getErrorMsg();

    void run();
    
signals:
    void loadingLibraryFinished(bool success);
public slots:

private:
    overlayMenu * m_menu;

    QString m_path;
    QString m_error_msg;
    
};

#endif // LOADLIBRARY_H
