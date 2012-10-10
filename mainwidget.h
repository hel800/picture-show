/* ------------------------------------------------------------------
mainwidget.h is part of picture-show.
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

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QGLWidget>
#include <QPainter>
#include <QPixmap>
#include <QImage>
#include <QTimer>
#include <QKeyEvent>
#include <QQueue>
#include <QList>
#include <QPair>
#include <QTranslator>
#include <QDesktopWidget>

#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QMessageBox>
#include <QMouseEvent>

#include "global.h"
#include "loaddirectory.h"
#include "loadimage.h"
#include "animation.h"
#include "settingsdialog.h"
#include "overlayhelp.h"
#include "overlayinfo.h"

namespace Ui {
class MainWidget;
}

class MainWidget : public QGLWidget
{
    Q_OBJECT
    
public:
    enum nextTask {
        START,
        PREV,
        NEXT,
        RECALC,
        NONE
    };

    enum queuedCommand {
        EMPTY,
        PREV_IMG,
        NEXT_IMG
    };

    enum state {
        NORMAL,
        SHOW_HELP,
        SHOW_INFO,
        SHOW_TEXTBAR
    };

    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

public slots:
    void initialize();
    void directoryLoadFinished(bool success);
    void overlayBlendOutFinished();
    void changeLanguage(QString language_short);
    void advanceImages(bool forward = true, bool hard = false);
    void jumpToImage(int num);
    void setValueOfAutomaticTimer(int msecs);

private slots:
    void loadImageFinished();
    void loadImagePrevFinished();
    void loadImageNextFinished();
    void reloadImages();
    void processQueuedCommands();
    void restoreDisplayState();

protected:
    void resizeEvent ( QResizeEvent * event );
    void changeEvent ( QEvent * event );
    void paintEvent ( QPaintEvent * event );
    void keyPressEvent ( QKeyEvent * event );

    void keyPressEvent_showingHelp ( QKeyEvent * event );
    void keyPressEvent_showingInfo ( QKeyEvent * event );

    void mouseReleaseEvent(QMouseEvent *event);
    
private:
    QPixmap scale(const QPixmap &map);
    QPixmap loadAndScale(QString & fn);

    QSize calcNewSize(QSize oldsize);

    Ui::MainWidget *ui;
    SettingsDialog *settingsDial;

    state displayState;

    animation * effectEngine;
    overlayHelp * displayHelp;
    overlayInfo * displayInfo;

    QTimer * resizeTimer;
    double current_blend;
    bool forward;

    QTimer * automaticForward;
    int automaticForwardInverval;

    nextTask current_task;
    bool imagesLoaded;

    bool mouseControl;

    QList<QFileInfo> current_directory_list;
    int current_position;

    QMap<int, QString> current_list_imageSizes  ;

    BlendType current_blendType;
    ScaleType current_scaleType;

    queuedCommand m_queuedComm;
    BlendType m_queuedBlendComm;

    bool m_displayInfo_active;

    QPixmap img_prev;
    QPixmap img;
    QPixmap img_next;

    loadDirectory * load_directory;

    QTranslator translator;

    loadImage * load_img_prev;
    loadImage * load_img;
    loadImage * load_img_next;

    int active_recalc_processes;
};

#endif // MAINWIDGET_H
