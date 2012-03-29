/* ------------------------------------------------------------------
loadlibrary.cpp is part of picture-show.
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

#include "loadlibrary.h"

loadLibrary::loadLibrary(overlayMenu * oMenu) : QThread()
{
    this->m_menu = oMenu;
}

void loadLibrary::setPath(const QString& dirpath)
{
    this->m_path = dirpath;
}

QString& loadLibrary::getErrorMsg()
{
    return this->m_error_msg;
}

void loadLibrary::run()
{
    QDir mainDirectory(this->m_path);
    if (!mainDirectory.exists())
    {
        this->m_error_msg = tr("Der angegebene Ordner existiert nicht: %1").arg(this->m_path);
        emit loadingLibraryFinished(false);
        return;
    }

    // container for sorting and grouping items
    QMap< QString, QList< QPair< QDateTime, QFileInfo> > * > libraryPrototype;


    // looking for folders, reading the date
    foreach(QFileInfo info, mainDirectory.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QStringList filters;
        filters << "*.jpeg" << "*.jpg" << "*.JPG" << "*.JPEG";
        QFileInfoList items = QDir(info.absoluteFilePath()).entryInfoList(filters, QDir::Files, QDir::Name | QDir::IgnoreCase);

        QString dirName = QDir(info.absoluteFilePath()).dirName();

        if (items.size() > 0)
        {
            QFileInfo firstFile = items.first();
            QString fname = firstFile.absoluteFilePath();
            QDateTime date = readOriginalDate(firstFile.absoluteFilePath());

            QString key = tr("unbekannt");

            if (date.isValid())
            {
                key = date.toString("yyyy");
            }

            QMap< QString, QList< QPair< QDateTime, QFileInfo> > * >::iterator it;
            it = libraryPrototype.find(key);
            if (it == libraryPrototype.end())
            {
                QList< QPair< QDateTime, QFileInfo > > * newList = new QList< QPair< QDateTime, QFileInfo > >();
                newList->append(QPair< QDateTime, QFileInfo >(date, info));

                libraryPrototype.insert(key, newList);
            }
            else
            {
                QList< QPair< QDateTime, QFileInfo > > * existingList = it.value();
                existingList->append(QPair< QDateTime, QFileInfo >(date, info));
            }
        }
    }

    // sort folder lists
    // insert groups with items in display menu
    QMapIterator< QString, QList< QPair< QDateTime, QFileInfo> > * > i(libraryPrototype);
    while(i.hasNext())
    {
        i.next();
        QList< QPair< QDateTime, QFileInfo> > * cur_list = i.value();
        qSort(cur_list->begin(), cur_list->end(), fileCreateLessThan);

        QList<QFileInfo> newList;
        for (int k = 0; k < cur_list->size(); k++)
        {
            QPair< QDateTime, QFileInfo> pair = cur_list->at(k);
            newList << pair.second;
        }

        this->m_menu->addCategory(i.key(), newList);

        delete cur_list;
    }

    libraryPrototype.clear();
    emit loadingLibraryFinished(true);
}

bool fileCreateLessThan(const QPair< QDateTime, QFileInfo > &f1, const QPair< QDateTime, QFileInfo > &f2)
{
    QDateTime f1_date = f1.first;
    QDateTime f2_date = f2.first;

    if (f1_date.isValid() && f2_date.isValid())
        return f1_date < f2_date;
    else if (f2_date.isValid() && !f1_date.isValid())
        return false;
    else if (f1_date.isValid() && !f2_date.isValid())
        return true;
    else
        return f1.second.fileName() < f2.second.fileName();
}
