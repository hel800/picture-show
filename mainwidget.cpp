/* ------------------------------------------------------------------
mainwidget.cpp is part of picture-show.
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
along with Foobar. If not, see <http://www.gnu.org/licenses/>.

......................................................................

author: Sebastian Schäfer
February 2012

--------------------------------------------------------------------*/

#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent) :
    QGLWidget(parent),
    ui(new Ui::MainWidget)
{
    this->effectEngine = new animation(this);
    connect(this->effectEngine, SIGNAL(finishedBlending()), this, SLOT(restoreDisplayState()));
    connect(this->effectEngine, SIGNAL(finishedBlending()), this, SLOT(processQueuedCommands()));
    connect(this->effectEngine, SIGNAL(textBarFadeOutFinished()), this, SLOT(processQueuedCommands()));
    connect(this->effectEngine, SIGNAL(textBarFadeOutFinished()), this, SLOT(restoreDisplayState()));

    this->displayHelp = new overlayHelp(this);
    connect(this->displayHelp, SIGNAL(blendOutFinished()), this, SLOT(overlayBlendOutFinished()));
    connect(this->displayHelp, SIGNAL(blendOutFinished()), this, SLOT(restoreDisplayState()));

    this->displayInfo = new overlayInfo(this);
    connect(this->displayInfo, SIGNAL(blendOutFinished()), this, SLOT(overlayBlendOutFinished()));

    ui->setupUi(this);

    this->settingsDial = new SettingsDialog(this);
    connect(this->settingsDial, SIGNAL(accepted()), this, SLOT(initialize()));
    connect(this->settingsDial, SIGNAL(applyClicked()), this, SLOT(applyNewOptions()));

    if (this->settingsDial->getLanguage() != "de")
    {
        if (translator.load(QCoreApplication::applicationDirPath() + "/picture-show_" + this->settingsDial->getLanguage(), "."))
            qApp->installTranslator(&translator);
    }

    this->settingsDial->updateLanguage();

    connect(this->settingsDial, SIGNAL(languageChanged(QString)), this, SLOT(changeLanguage(QString)));

    this->resizeTimer = new QTimer(this);
    connect(this->resizeTimer, SIGNAL(timeout()), this, SLOT(reloadImages()));

    this->automaticForward = new QTimer(this);
    connect(this->automaticForward, SIGNAL(timeout()), this, SLOT(advanceImages()));
    connect(this->effectEngine, SIGNAL(automaticTimer(int)), this->automaticForward, SLOT(start(int)));
    connect(this->effectEngine, SIGNAL(automaticTimer(int)), this, SLOT(setValueOfAutomaticTimer(int)));
    connect(this->effectEngine, SIGNAL(jumptoImage(int)), this, SLOT(jumpToImage(int)));

    this->displayState = NORMAL;

    this->current_blend = 0.0;
    this->forward = true;
    this->current_position = 0;
    this->current_blendType = FADE;
    this->current_scaleType = IMAGE;

    this->current_task = NONE;
    this->imagesLoaded = false;
    this->active_recalc_processes = 0;
    this->m_queuedComm = EMPTY;
    this->m_queuedBlendComm = FADE;

    this->m_displayInfo_active = false;

    this->img = QPixmap();
    this->img_next = QPixmap();
    this->img_prev = QPixmap();

    this->load_directory = new loadDirectory();
    this->load_directory->setDirectoryList(&this->current_directory_list);

    this->load_img_prev = new loadImage();
    this->load_img = new loadImage();
    this->load_img_next = new loadImage();

    this->mouseControl = false;

    connect(this->load_img_prev, SIGNAL(finished()), this, SLOT(loadImagePrevFinished()));
    connect(this->load_img, SIGNAL(finished()), this, SLOT(loadImageFinished()));
    connect(this->load_img_next, SIGNAL(finished()), this, SLOT(loadImageNextFinished()));

    connect(this->load_directory, SIGNAL(loadDirectoryFinished(bool)), this, SLOT(directoryLoadFinished(bool)));

    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "Picture Show");

    QDesktopWidget * desktop = QApplication::desktop();
    QSize mainW_size = settings.value("MainWindow_size", QSize(800, 600)).toSize();
    QPoint mainW_pos = settings.value("MainWindow_pos", QPoint(200, 200)).toPoint();

    this->last_nonFullscreen_size = mainW_size;
    this->last_nonFullscreen_pos = mainW_pos;

    // Move it to the saved location, then check if it is on a visible screen
    this->resize(mainW_size);
    this->move(mainW_pos);

    // MainWidget is not visible on a screen. That means virtual screens from previous
    // session are no longer available. The widget has to be moved back to the primary screen
    if (desktop->screenNumber(this) == -1)
    {
        QSize primaryScreen_size = desktop->availableGeometry(desktop->primaryScreen()).size();
        if (primaryScreen_size.width() < mainW_size.width())
            mainW_size.setWidth(primaryScreen_size.width());

        if (primaryScreen_size.height() < mainW_size.height())
            mainW_size.setHeight(primaryScreen_size.height());

        mainW_pos.setX((primaryScreen_size.width() - mainW_size.width()) / 2);
        mainW_pos.setY((primaryScreen_size.height() - mainW_size.height()) / 2);
    }

    this->resize(mainW_size);
    this->move(mainW_pos);

    if (settings.value("fullscreenState", QVariant(false)).toBool())
    {
        this->setWindowState(Qt::WindowFullScreen);
        this->setCursor(Qt::BlankCursor);
    }

    this->settingsDial->show();

    this->automaticForwardInverval = settings.value("automaticTimerInterval", QVariant(10000)).toInt();

    this->show();
    this->effectEngine->setFirstStart(settings.value("firstStart", QVariant(true)).toBool());
    this->effectEngine->paintStartScreen();
    this->settingsDial->raise();
}

MainWidget::~MainWidget()
{
    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture Show");
    settings.setValue("fullscreenState", QVariant(this->windowState() == Qt::WindowFullScreen));
    settings.setValue("automaticTimerInterval", QVariant(this->automaticForwardInverval));

    if (this->windowState() != Qt::WindowFullScreen)
    {
        settings.setValue("MainWindow_size", this->size());
        settings.setValue("MainWindow_pos", this->pos());
    }
    else
    {
        settings.setValue("MainWindow_size", this->last_nonFullscreen_size);
        settings.setValue("MainWindow_pos", this->last_nonFullscreen_pos);
    }

    delete ui;

    delete this->settingsDial;

    delete this->effectEngine;
    delete this->resizeTimer;

    delete this->load_img;
    delete this->load_img_next;
    delete this->load_img_prev;
}

void MainWidget::initialize()
{
    if (!QDir(this->settingsDial->getCurrentDirectory()).exists())
        return;

    this->current_task = START;

    this->effectEngine->reset();
    this->effectEngine->paintToWaiting();

    // stop timer, if active
    this->automaticForward->stop();

    this->effectEngine->setFadeSteps(this->settingsDial->getCurrentFadeTime());
    this->effectEngine->setCurrentBlendType(this->settingsDial->getBlendType());
    this->effectEngine->setScaleTypeTo(this->settingsDial->getScaleType());
    this->current_blendType = this->settingsDial->getBlendType();
    this->current_scaleType = this->settingsDial->getScaleType();
    this->mouseControl = this->settingsDial->getMouseControl();

    this->img = QPixmap();
    this->img_next = QPixmap();
    this->img_prev = QPixmap();

    this->current_position = 0;

    this->load_directory->setDirectory(this->settingsDial->getCurrentDirectory());
    this->load_directory->setSorting(this->settingsDial->getDirectorySorting());
    this->load_directory->start();
}

void MainWidget::applyNewOptions()
{
    if (!this->imagesLoaded)
        this->initialize();

    if (this->load_directory->getSorting() != this->settingsDial->getDirectorySorting())
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Frage"));
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText(tr("Der Sortierungsmodus wurde geändert. Damit die neue Sortierung angewendet wird, muss die Show neu geladen werden. Sie startet neu von Beginn."));
        msgBox.setInformativeText(tr("\"Ja\" klicken, zum neustarten, \"Nein\" um alte Sortierung weiter zu verwenden."));

        QPushButton *yesButton = msgBox.addButton(tr("Ja"), QMessageBox::YesRole);
        QPushButton *noButton = msgBox.addButton(tr("Nein"), QMessageBox::NoRole);
        msgBox.setDefaultButton(noButton);

        msgBox.exec();

        if (msgBox.clickedButton() == dynamic_cast<QAbstractButton*>(yesButton))
        {
            this->initialize();
            return;
        }
        else
        {
            this->settingsDial->setDirectorySorting(this->load_directory->getSorting());
        }
    }

    this->effectEngine->setFadeSteps(this->settingsDial->getCurrentFadeTime());
    this->effectEngine->setCurrentBlendType(this->settingsDial->getBlendType());
    this->current_blendType = this->settingsDial->getBlendType();
    this->mouseControl = this->settingsDial->getMouseControl();

    if (this->current_scaleType != this->settingsDial->getScaleType())
    {
        this->current_scaleType = this->settingsDial->getScaleType();
        this->effectEngine->setScaleTypeTo(this->settingsDial->getScaleType());

        this->effectEngine->paintToWaiting();
        this->reloadImages();
    }
}

void MainWidget::directoryLoadFinished(bool success)
{
    // if any overlays have been shown, now they're set back
    // this is for security only, they should have been faded out before image change
    this->displayState = NORMAL;

    if (!success)
    {
        this->effectEngine->paintStartScreen();
        this->current_task = NONE;
        this->imagesLoaded = false;

        QMessageBox::warning(this, tr("Fehler beim laden des Ordners"), this->load_directory->getErrorMsg());

        this->settingsDial->show();
        return;
    }

    if (this->current_directory_list.size() == 0)
    {
        this->effectEngine->paintStartScreen();
        this->current_task = NONE;
        this->imagesLoaded = false;

        QMessageBox::warning(this, tr("Keine Bilder"), tr("In dem angegebenen Ordner wurden keine Bilder gefunden!"));

        this->settingsDial->show();
        return;
    }

    this->current_list_imageSizes.clear();
    this->current_task = RECALC;

    this->settingsDial->addDirectoryToHistory(this->load_directory->getDirectory());

    if (this->current_directory_list.size() > 0)
    {
        this->active_recalc_processes++;
        this->load_img->setTask(this->current_directory_list.at(0).absoluteFilePath(), this->width(), this->height(), this->current_scaleType);
        this->load_img->start();
    }

    if (this->current_directory_list.size() > 1)
    {
        this->active_recalc_processes++;
        this->load_img_next->setTask(this->current_directory_list.at(1).absoluteFilePath(), this->width(), this->height(), this->current_scaleType);
        this->load_img_next->start();
    }
}

void MainWidget::loadImageFinished()
{
    this->active_recalc_processes--;
    this->img = QPixmap::fromImage(*this->load_img->getImage());

    if (this->active_recalc_processes == 0)
        this->current_task = NONE;

    if (this->load_img->getSourceWidth() > 0 && this->load_img->getSourceWidth() > 0)
        this->current_list_imageSizes.insert(this->current_position, tr("%1 x %2").arg(this->load_img->getSourceWidth()).arg(this->load_img->getSourceHeight()));


    if (this->img.isNull())
    {
        this->automaticForward->stop();
        QMessageBox::warning(this, tr("Fehler"), tr("Das Bild \"%1\" konnte nicht geladen werden!").arg(this->current_directory_list.at(this->current_position).fileName()));
        this->settingsDial->show();
    }
    else
    {
        this->imagesLoaded = true;

        // Set firstStart to false for future sessions
        QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture Show");
        settings.setValue("firstStart", QVariant(false));
        this->effectEngine->setFirstStart(settings.value("firstStart", QVariant(true)).toBool());

        this->effectEngine->paintFromWaiting(this->img);
    }
}

void MainWidget::loadImagePrevFinished()
{
    this->active_recalc_processes--;

    this->img_prev = QPixmap::fromImage(*this->load_img_prev->getImage());

    if (this->active_recalc_processes == 0)
        this->current_task = NONE;

    if (this->load_img_prev->getSourceWidth() > 0 && this->load_img_prev->getSourceWidth() > 0)
        this->current_list_imageSizes.insert(this->current_position-1, tr("%1 x %2").arg(this->load_img_prev->getSourceWidth()).arg(this->load_img_prev->getSourceHeight()));

    this->processQueuedCommands();
}

void MainWidget::loadImageNextFinished()
{
    this->active_recalc_processes--;

    this->img_next = QPixmap::fromImage(*this->load_img_next->getImage());

    if (this->active_recalc_processes == 0)
        this->current_task = NONE;

    if (this->load_img_next->getSourceWidth() > 0 && this->load_img_next->getSourceWidth() > 0)
        this->current_list_imageSizes.insert(this->current_position+1, tr("%1 x %2").arg(this->load_img_next->getSourceWidth()).arg(this->load_img_next->getSourceHeight()));


    this->processQueuedCommands();
}

void MainWidget::overlayBlendOutFinished()
{
    this->displayState = NORMAL;
    this->processQueuedCommands();
}

void MainWidget::changeLanguage(QString language_short)
{
    qApp->removeTranslator(&translator);
    if (language_short != "de")
        if (translator.load(QCoreApplication::applicationDirPath() + "/picture-show_" + language_short))
            qApp->installTranslator(&translator);
    this->settingsDial->updateLanguage();
}

void MainWidget::reloadImages()
{
    this->resizeTimer->stop();

    if (this->current_task != NONE || this->current_directory_list.size() == 0)
        return;

    this->active_recalc_processes = 0;
    this->current_task = RECALC;

    this->active_recalc_processes++;
    this->load_img->setTask(this->current_directory_list.at(this->current_position).absoluteFilePath(), this->width(), this->height(), this->current_scaleType);
    this->load_img->start();

    if (this->current_position > 0)
    {
        this->active_recalc_processes++;
        this->load_img_prev->setTask(this->current_directory_list.at(this->current_position-1).absoluteFilePath(), this->width(), this->height(), this->current_scaleType);
        this->load_img_prev->start();
    }

    if (this->current_directory_list.size() > this->current_position+1)
    {
        this->active_recalc_processes++;
        this->load_img_next->setTask(this->current_directory_list.at(this->current_position+1).absoluteFilePath(), this->width(), this->height(), this->current_scaleType);
        this->load_img_next->start();
    }
}

void MainWidget::advanceImages(bool forward, bool hard)
{
    // queue command
    if (this->effectEngine->isBusy() || this->current_task != NONE)
    {
        if (!forward)
            this->m_queuedComm = PREV_IMG;
        else
            this->m_queuedComm = NEXT_IMG;

        if (!hard)
            this->m_queuedBlendComm = this->current_blendType;
        else
            this->m_queuedBlendComm = HARD;
        return;
    }

    if (hard)
        this->effectEngine->setCurrentBlendType(HARD);
    else
        this->effectEngine->setCurrentBlendType(this->current_blendType);

    if (!forward)
    {
        if (this->current_position > 0)
        {
            if (this->img_prev.isNull())
            {
                this->automaticForward->stop();
                QMessageBox::warning(this, tr("Fehler"), tr("Der Inhalt des Bilderordners wurde geändert, Bilder wurden entfernt oder sind nicht mehr zugänglich!\nDie Timerfunktion wurde gestoppt, sofern sie aktiv war. Der Bilderordner muss neu geöffnet und eingelesen werden..."));
                this->settingsDial->show();
                return;
            }

            // CHECK if dislay_info or a textbar is blend in and blend it out first
            if (this->displayState == SHOW_INFO || this->displayState == SHOW_HELP || this->effectEngine->isShowingTextbar())
            {
                this->m_queuedComm = PREV_IMG;
                if (!hard)
                    this->m_queuedBlendComm = this->current_blendType;
                else
                    this->m_queuedBlendComm = HARD;

                if (this->displayState == SHOW_INFO)
                {
                    this->m_displayInfo_active = true;
                    this->displayInfo->blendOut();
                }
                else if (this->displayState == SHOW_HELP)
                {
                    this->displayHelp->blendOut();
                }
                else
                {
                    this->effectEngine->escPressed();
                }

                return;
            }

            this->current_position--;

            this->effectEngine->blendImages(this->img, this->img_prev, false);
            if (this->automaticForward->isActive())
                this->automaticForward->start(this->automaticForwardInverval);

            this->img_next = this->img;
            this->img = this->img_prev;
            this->img_prev = QPixmap();

            if (this->current_position > 0)
            {
                this->active_recalc_processes++;
                this->current_task = PREV;
                this->load_img_prev->setTask(this->current_directory_list.at(this->current_position-1).absoluteFilePath(), this->width(), this->height(), this->current_scaleType);
                this->load_img_prev->start();
            }
        }
    }
    else
    {
        if (this->current_position < this->current_directory_list.size()-1)
        {
            if (this->img_next.isNull())
            {
                this->automaticForward->stop();
                QMessageBox::warning(this, tr("Fehler"), tr("Der Inhalt des Bilderordners wurde geändert, Bilder wurden entfernt oder sind nicht mehr zugänglich!\nDie Timerfunktion wurde gestoppt, sofern sie aktiv war. Der Bilderordner muss neu geöffnet und eingelesen werden..."));
                this->settingsDial->show();
                return;
            }

            // CHECK if dislay_info or a textbar is blend in and blend it out first
            if (this->displayState == SHOW_INFO || this->displayState == SHOW_HELP || this->effectEngine->isShowingTextbar())
            {
                this->m_queuedComm = NEXT_IMG;
                if (!hard)
                    this->m_queuedBlendComm = this->current_blendType;
                else
                    this->m_queuedBlendComm = HARD;

                if (this->displayState == SHOW_INFO)
                {
                    this->m_displayInfo_active = true;
                    this->displayInfo->blendOut();
                }
                else if (this->displayState == SHOW_HELP)
                {
                    this->displayHelp->blendOut();
                }
                else
                {
                    this->effectEngine->escPressed();
                }

                return;
            }

            this->current_position++;

            this->effectEngine->blendImages(this->img, this->img_next);
            if (this->automaticForward->isActive())
                this->automaticForward->start(this->automaticForwardInverval);

//            if (this->current_position >= this->current_directory_list.size()-1)
//                return;

            this->img_prev = this->img;
            this->img = this->img_next;
            this->img_next = QPixmap();

            if (this->current_position + 1 < this->current_directory_list.size())
            {
                this->active_recalc_processes++;
                this->current_task = NEXT;
                this->load_img_next->setTask(this->current_directory_list.at(this->current_position+1).absoluteFilePath(), this->width(), this->height(), this->current_scaleType);
                this->load_img_next->start();
            }
        }
    }
}

void MainWidget::jumpToImage(int num)
{
    if (this->current_directory_list.size() < num || num < 1)
        return;

    if (this->current_position == num -1)
        return;

    this->current_position = num - 1;
    if (!this->img.isNull())
        this->effectEngine->paintToWaiting(this->img);
    else
        this->effectEngine->paintToWaiting();
    this->resizeTimer->start(500);
}

void MainWidget::setValueOfAutomaticTimer(int msecs)
{
    this->automaticForwardInverval = msecs;
}

void MainWidget::processQueuedCommands()
{
    if (this->m_queuedComm != EMPTY && this->current_task == NONE && !this->effectEngine->isBusy())
    {
        if (this->m_queuedComm == NEXT_IMG)
        {
            this->advanceImages(true, this->m_queuedBlendComm == HARD);
        }
        else if (this->m_queuedComm == PREV_IMG)
        {
            this->advanceImages(false, this->m_queuedBlendComm == HARD);
        }
        else if (this->m_queuedComm == INFO_BAR)
        {
            if (this->displayInfo->blendIn(this->effectEngine->currentDisplay()))
            {
                this->displayState = SHOW_INFO;
                this->effectEngine->textBarReset();
            }
        }
        else if (this->m_queuedComm == JUMP_TO)
        {
            this->effectEngine->startJumptoBar(this->current_position+1, this->current_directory_list.size());
            this->displayState = SHOW_TEXTBAR;
        }
        else if (this->m_queuedComm == TIMER)
        {
            if (this->automaticForward->isActive())
            {
                this->automaticForward->stop();
                this->effectEngine->stopTimerBar();
            }
            else if (this->imagesLoaded)
            {
                this->effectEngine->startTimerBar(int(this->automaticForwardInverval/1000));
            }
        }
        else if (this->m_queuedComm == HELP_WINDOW)
        {
            if (this->displayHelp->blendIn(this->effectEngine->currentDisplay()))
            {
                QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture Show");
                settings.setValue("firstStart", QVariant(false));
                this->effectEngine->setFirstStart(false);
                this->displayState = SHOW_HELP;
                this->automaticForward->stop();
                this->effectEngine->textBarReset();
            }
        }

        this->m_queuedComm = EMPTY;
    }
}

void MainWidget::restoreDisplayState()
{
    if (this->effectEngine->isBusy())
        return;

    if (this->displayState == SHOW_HELP)
        this->displayHelp->blendIn(this->effectEngine->currentDisplay());

    if ((this->m_displayInfo_active || this->displayState == SHOW_INFO) && this->m_queuedComm == EMPTY)
    {
        this->displayInfo->setImageNumberAndNumberOfImages(this->current_position+1, this->current_directory_list.size());

        QString fname = this->current_directory_list.at(this->current_position).absoluteFilePath();
        EXIFInfo exif = readExifHeader(fname);
        this->displayInfo->setCurrentExifInformation(exif);
        this->displayInfo->setCurrentFileInformation(this->current_directory_list.at(this->current_position));
        this->displayInfo->setCurrentImageResolution(this->current_list_imageSizes.value(this->current_position, QString()));

        if (this->displayInfo->blendIn(this->effectEngine->currentDisplay()))
        {
            this->displayState = SHOW_INFO;
            this->effectEngine->textBarReset();
        }
    }
}

void MainWidget::resizeEvent ( QResizeEvent * event )
{
    if (event->oldSize() != event->size())
    {
        if (this->imagesLoaded)
        {
            this->effectEngine->paintToWaiting();
            this->resizeTimer->start(500);
        }
        else
        {
            if (this->displayState == SHOW_HELP && this->effectEngine->isDoingNothing())
                this->displayHelp->update(this->effectEngine->currentDisplay());
            else
                this->effectEngine->paintStartScreen();
        }
    }
}

void MainWidget::changeEvent ( QEvent * event )
{
//    if (event->type() != QEvent::WindowStateChange)
//        return;

//    qDebug(QString::number(event->type()).toStdString().c_str());

//    if (this->displayState == SHOW_HELP && this->effectEngine->isDoingNothing())
//        this->displayHelp->update(this->effectEngine->currentDisplay());
//    else
//        this->effectEngine->repaintState();
    event->ignore();
}

void MainWidget::paintEvent ( QPaintEvent * event )
{
    if (this->displayState == SHOW_HELP && this->effectEngine->isDoingNothing())
        this->displayHelp->update(this->effectEngine->currentDisplay());
    else if (this->displayState == SHOW_INFO && this->effectEngine->isDoingNothing())
        this->displayInfo->update(this->effectEngine->currentDisplay());
    else
        this->effectEngine->repaintState();

    event->accept();
}

void MainWidget::keyPressEvent ( QKeyEvent * event )
{
    if (this->current_task == START && event->key() == Qt::Key_Escape)
        this->close();

    if (this->imagesLoaded)
    {
        switch (event->key())
        {
        case Qt::Key_Left:
            this->advanceImages(false);
        break;
        case Qt::Key_Right:
            this->advanceImages();
        break;
        case Qt::Key_PageUp:
            this->advanceImages(false, true);
        break;
        case Qt::Key_PageDown:
            this->advanceImages(true, true);
        break;
        }
    }

    switch (event->key())
    {
        case Qt::Key_O:
        if (this->effectEngine->isBusy() && this->current_task == NONE)
            return;

        if (this->settingsDial->isHidden())
        {
            this->automaticForward->stop();
            this->settingsDial->show();
        }
        else
            this->settingsDial->hide();
        break;
        case Qt::Key_F:
        if (this->current_task == NONE)
        {
            if (this->windowState() != Qt::WindowFullScreen)
            {
                this->last_nonFullscreen_size = this->size();
                this->last_nonFullscreen_pos = this->pos();
            }

            this->setWindowState(this->windowState() ^ Qt::WindowFullScreen);
            if ((this->windowState() == Qt::WindowFullScreen) || (this->windowState() == (Qt::WindowFullScreen | Qt::WindowMaximized)))
                this->setCursor(Qt::BlankCursor);
            else
                this->unsetCursor();
        }
        break;
    }

    // KEY bindings in HELP and INFO mode
    if (this->displayState == SHOW_HELP)
        return this->keyPressEvent_showingHelp( event );
    else if (this->displayState == SHOW_INFO)
        return this->keyPressEvent_showingInfo( event );


    // KEY bindings in NORMAL display mode
    switch (event->key())
    {
    case Qt::Key_Escape:
        if (this->effectEngine->exitRequested())
            this->close();
        else if (this->effectEngine->isShowingTextbar())
            this->effectEngine->escPressed();
        else
            this->effectEngine->startExitApp();
    break;

    case Qt::Key_Space:
        if (this->automaticForward->isActive() && !this->effectEngine->isBusy())
        {
            this->automaticForward->stop();
            this->effectEngine->stopTimerBar();
        }
        else if (this->imagesLoaded)
        {
            this->effectEngine->startTimerBar(int(this->automaticForwardInverval/1000));
        }
    break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        this->effectEngine->enterPressed();
    break;
    case Qt::Key_0:
    case Qt::Key_1:
    case Qt::Key_2:
    case Qt::Key_3:
    case Qt::Key_4:
    case Qt::Key_5:
    case Qt::Key_6:
    case Qt::Key_7:
    case Qt::Key_8:
    case Qt::Key_9:
        this->effectEngine->numberEntered(event->text().toInt());
    break;
    case Qt::Key_J:
        if (this->effectEngine->isDoingNothing() && this->imagesLoaded)
        {
            this->automaticForward->stop();
            this->effectEngine->startJumptoBar(this->current_position+1, this->current_directory_list.size());
        }
    break;
    case Qt::Key_I:
        if (this->effectEngine->isDoingNothing() && this->imagesLoaded)
        {
            this->displayInfo->setImageNumberAndNumberOfImages(this->current_position+1, this->current_directory_list.size());

            QString fname = this->current_directory_list.at(this->current_position).absoluteFilePath();
            EXIFInfo exif = readExifHeader(fname);
            this->displayInfo->setCurrentExifInformation(exif);
            this->displayInfo->setCurrentFileInformation(this->current_directory_list.at(this->current_position));
            this->displayInfo->setCurrentImageResolution(this->current_list_imageSizes.value(this->current_position, QString()));

            if (this->effectEngine->isShowingTextbar())
            {
                this->effectEngine->escPressed();
                this->m_queuedComm = INFO_BAR;
            }
            else
            {
                if (this->displayInfo->blendIn(this->effectEngine->currentDisplay()))
                {
                    this->displayState = SHOW_INFO;
                    this->effectEngine->textBarReset();
                }
            }
        }
    break;
    case Qt::Key_H:
    case Qt::Key_F1:
        if (this->current_task != RECALC && this->current_task != START && !this->effectEngine->isBusy())
        {
            if (this->displayHelp->blendIn(this->effectEngine->currentDisplay()))
            {
                QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture Show");
                settings.setValue("firstStart", QVariant(false));
                this->effectEngine->setFirstStart(false);
                this->displayState = SHOW_HELP;
                this->automaticForward->stop();
                this->effectEngine->textBarReset();
            }
        }
    break;
    }
}

void MainWidget::keyPressEvent_showingHelp ( QKeyEvent * event )
{
    if (!this->effectEngine->isDoingNothing())
        return;

    switch (event->key())
    {
    case Qt::Key_H:
    case Qt::Key_F1:
    case Qt::Key_Enter:
    case Qt::Key_Return:
    case Qt::Key_Escape:
        this->displayHelp->blendOut();
    break;
    }
}

void MainWidget::keyPressEvent_showingInfo ( QKeyEvent * event )
{
    if (!this->effectEngine->isDoingNothing())
        return;

    switch (event->key())
    {
    case Qt::Key_I:
    case Qt::Key_Enter:
    case Qt::Key_Return:
    case Qt::Key_Escape:
        this->displayInfo->blendOut();
        this->m_displayInfo_active = false;
    break;
    case Qt::Key_Space:
        this->m_displayInfo_active = true;

        this->displayInfo->blendOut();
        this->m_queuedComm = TIMER;
    break;
    case Qt::Key_J:
        this->automaticForward->stop();
        this->m_displayInfo_active = true;

        this->displayInfo->blendOut();
        this->m_queuedComm = JUMP_TO;
    break;
    case Qt::Key_H:
    case Qt::Key_F1:
        this->automaticForward->stop();
        this->m_displayInfo_active = true;

        this->displayInfo->blendOut();
        this->m_queuedComm = HELP_WINDOW;
    break;
    }
}

void MainWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!this->mouseControl)
        return;

    if (event->button() == Qt::LeftButton)
        this->advanceImages(false);

    if (event->button() == Qt::MiddleButton)
    {
        if (this->settingsDial->isHidden())
            this->settingsDial->show();
        else
            this->settingsDial->hide();
    }

    if (event->button() == Qt::RightButton)
        this->advanceImages(true);
}

QPixmap MainWidget::scale(const QPixmap & map)
{
    return map.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

QPixmap MainWidget::loadAndScale(QString & fn)
{
    return this->scale(QPixmap(fn));
}

QSize MainWidget::calcNewSize(QSize oldsize)
{
    double oldRatio = double(oldsize.width()) / double(oldsize.height());
    double newRatio = double(this->size().width()) / double(this->size().height());

    int newWidth = 0.0;
    int newHeight = 0.0;

    if (newRatio > oldRatio)
    {
        newHeight = this->size().height();
        newWidth = int (newHeight * oldRatio);
    }
    else
    {
        newWidth = this->size().width();
        newHeight = int (double(newWidth) / oldRatio);
    }

    return QSize(newWidth, newHeight);
}



//QPixmap MainWidget::loadAndScale(QString & fn)
//{
//    cv::Mat testimg = cv::imread(fn.toStdString().c_str());
//    qDebug(QString::number(testimg.channels()).toStdString().c_str());

////    cv::Mat rgba = cv::Mat(100, 100, cv.CV_8UC4);
////    alpha = cv::Mat(100, 100, cv.CV_8UC1, 255);
////    cv.MixChannels([rgba], [bgr, alpha], [
////       (0, 2),    # rgba[0] -> bgr[2]
////       (1, 1),    # rgba[1] -> bgr[1]
////       (2, 0),    # rgba[2] -> bgr[0]
////       (3, 3)     # rgba[3] -> alpha[0]
////    ])

//    cv::Mat testMat = testimg.clone();
//    cv::cvtColor(testimg, testMat, CV_BGR2RGBA, 4);

//    qDebug(QString::number(testMat.channels()).toStdString().c_str());

//    QSize newSize = this->calcNewSize(QSize(testimg.size().width, testimg.size().height));

//    cv::gpu::GpuMat source;
//    source.upload(testMat);

//    cv::gpu::GpuMat dest = cv::gpu::GpuMat(newSize.width(), newSize.height(), testimg.type());

//    cv::gpu::resize( source, dest, cv::Size(newSize.width(),newSize.height()));

//    cv::Mat result, result2;
//    dest.download(result);

//    cv::cvtColor(result, result2, CV_RGBA2RGB, 3);

//    QImage qimg((uchar*)result2.data, result.cols, result.rows, QImage::Format_RGB32);
//    QPixmap p;
//    p.convertFromImage(qimg);

//    return p;
//}
