/* ------------------------------------------------------------------
loaddirectory.cpp is part of picture-show.
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

author: Sebastian Sch�fer
March 2012

--------------------------------------------------------------------*/

#include "loaddirectory.h"

loadDirectory::loadDirectory() : QThread()
{
    this->m_dirList = NULL;
}

void loadDirectory::setDirectory(const QString &path)
{
    this->m_path = path;
}

void loadDirectory::setDirectoryList(QList<QFileInfo> * list)
{
    this->m_dirList = list;
}

void loadDirectory::setSorting(Sorting sortType)
{
    this->m_sorting = sortType;
}

QString& loadDirectory::getErrorMsg()
{
    return this->m_error_msg;
}

void loadDirectory::run()
{
    QStringList filters;
    filters << "*.jpeg" << "*.jpg" << "*.JPG" << "*.JPEG";

    QDir current_dir = QDir(this->m_path);
    if (!current_dir.exists())
    {
        this->m_error_msg = tr("Das angegebene Verzeichnis existiert nicht!");
        emit loadDirectoryFinished(false);
        return;
    }

    this->m_dirList->clear();

    QList<QFileInfo> tempList = current_dir.entryInfoList(filters, QDir::Files, QDir::Name | QDir::IgnoreCase);

    if (this->m_sorting == DATE_CREATED)
    {
        QList< QPair<QFileInfo, QDateTime> > temp2list;

        foreach(QFileInfo info, tempList)
        {
            QDateTime date = readOriginalDate(info.absoluteFilePath());
            QPair<QFileInfo, QDateTime> pair;
            pair.first = info;
            pair.second = date;
            temp2list << pair;
        }

        qSort(temp2list.begin(), temp2list.end(), fileCreateLessThan);

        for (int i = 0; i < temp2list.size(); i++)
            this->m_dirList->append(temp2list.at(i).first);
    }
    else
    {
        for (int i = 0; i < tempList.size(); i++)
            this->m_dirList->append(tempList.at(i));
    }

    emit loadDirectoryFinished(true);
}

bool fileCreateLessThan(const QPair<QFileInfo, QDateTime> &f1, const QPair<QFileInfo, QDateTime> &f2)
{
    QDateTime f1_date = f1.second;
    QDateTime f2_date = f2.second;

    if (f1_date.isValid() && f2_date.isValid())
        return f1_date < f2_date;
    else if (f2_date.isValid() && !f1_date.isValid())
        return false;
    else if (f1_date.isValid() && !f2_date.isValid())
        return true;
    else
        return f1.first.fileName() < f2.first.fileName();
}
