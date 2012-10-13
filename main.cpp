/* ------------------------------------------------------------------
main.cpp is part of picture-show.
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

#include <QtGui/QApplication>
#include "mainwidget.h"

#include <sstream>

int main(int argc, char *argv[])
{
    QString appVersion = QString("0.7 beta ");

    #if defined (_MSC_VER)
        #if _MSC_VER >= 1600
            appVersion += QString("(MSVC v10.0 ");
        #elif _MSC_VER >= 1500
            appVersion += QString("(MSVC v9.0 ");
        #else
            appVersion += QString("(MSVC < 9.0");
        #endif
    #elif defined (__GNUC__)
        std::ostringstream os;
        os << __GNUC__ << "." << __GNUC_MINOR__;
        appVersion += QString("GCC v") + QString(os.str().c_str());
    #elif defined (__MINGW32__)
        std::ostringstream os;
        os << __MINGW32_MAJOR_VERSION << "." << __MINGW32_MINOR_VERSION;
        appVersion += QString("MinGW v") + QString(os.str().c_str());
    #else
        appVersion += QString("(unknown compiler ");
    #endif

    #if defined (_M_X64) || defined (__x86_64__)
        appVersion += QString(" x64)");
    #else
        appVersion += QString(" x86)");
    #endif


    QApplication a(argc, argv);
    a.setApplicationName(QString("picture-show"));
    a.setApplicationVersion(appVersion);

    MainWidget w;
    w.show();
    
    return a.exec();
}
