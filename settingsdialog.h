/* ------------------------------------------------------------------
settingsdialog.h is part of picture-show.
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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QFileDialog>
#include <QSettings>
#include <QHideEvent>

#include "global.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    QString getCurrentDirectory();
    void setCurrentDirectory(const QString & dir);
    double getCurrentFadeTime();
    BlendType getBlendType();
    Sorting getDirectorySorting();
    void setDirectorySorting(Sorting sort);
    bool getIncludeSubdirs();
    void setIncludeSubdirs(bool inc);
    bool getLoopSlideShow();
    void setLoopSlideShow(bool loop);
    ScaleType getScaleType();
    QString getLanguage();
    bool getMouseControl();

    void updateLanguage();
    void updateHistory();

    void addDirectoryToHistory(const QString & dir);

signals:
    void languageChanged(QString lang);
    void applyClicked();
    
private slots:
    void on_pushButton_browse_clicked();
    void on_comboBox_language_currentIndexChanged(int index);

    void on_pushButton_deleteHistory_clicked();

private:
    Ui::SettingsDialog *ui;
    bool languageChangeSignalOff;

    void loadSettings();
    void saveSettings();
};

#endif // SETTINGSDIALOG_H
