/* ------------------------------------------------------------------
settingsdialog.cpp is part of picture-show.
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

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <iostream>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);    
    this->setModal(true);
    this->setWindowFlags( this->windowFlags() & ~Qt::WindowContextHelpButtonHint );

    ui->comboBox_directoryPath->setEditable(true);

    connect(ui->pushButton_apply, SIGNAL(clicked()), this, SIGNAL(applyClicked()));
    connect(ui->pushButton_apply, SIGNAL(clicked()), this, SLOT(hide()));

    this->languageChangeSignalOff = true;
    this->loadSettings();
    this->languageChangeSignalOff = false;

    ui->comboBox_directoryPath->setCurrentIndex(0);
}

SettingsDialog::~SettingsDialog()
{
    this->saveSettings();
    delete ui;
}

QString SettingsDialog::getCurrentDirectory()
{
    return ui->comboBox_directoryPath->lineEdit()->text();
}

void SettingsDialog::setCurrentDirectory(const QString & dir)
{
    ui->comboBox_directoryPath->lineEdit()->setText(dir);
}

double SettingsDialog::getCurrentFadeTime()
{
    if (ui->comboBox_fadeTime->currentIndex() == 0)
        return 0.04;
    else if (ui->comboBox_fadeTime->currentIndex() == 1)
        return 0.02;
    else if (ui->comboBox_fadeTime->currentIndex() == 2)
        return 0.01;
    else if (ui->comboBox_fadeTime->currentIndex() == 3)
        return 0.005;
    else
        return 0.01;
}

BlendType SettingsDialog::getBlendType()
{
    if (ui->comboBox_effect->currentIndex() == 1)
        return FADE;
    else if (ui->comboBox_effect->currentIndex() == 2)
        return SLIDE;
    else if (ui->comboBox_effect->currentIndex() == 3)
        return SLIDE_FADE;
    else
        return HARD;
}

Sorting SettingsDialog::getDirectorySorting()
{
    if (ui->comboBox_sort->currentIndex() == 1)
        return DATE_CREATED;
    else
        return FILENAME;
}

void SettingsDialog::setDirectorySorting(Sorting sort)
{
    if (sort == DATE_CREATED)
        ui->comboBox_sort->setCurrentIndex(1);
    else
        ui->comboBox_sort->setCurrentIndex(0);
}

bool SettingsDialog::getIncludeSubdirs()
{
    return ui->checkBox_subdirs->isChecked();
}

void SettingsDialog::setIncludeSubdirs(bool inc)
{
    if (inc)
        ui->checkBox_subdirs->setCheckState(Qt::Checked);
    else
        ui->checkBox_subdirs->setCheckState(Qt::Unchecked);
}

ScaleType SettingsDialog::getScaleType()
{
    if (ui->comboBox_scaling->currentIndex() == 0)
        return IMAGE;
    else
        return SCREEN;
}

QString SettingsDialog::getLanguage()
{
    if (ui->comboBox_language->currentIndex() == 0)
        return QString("de");
    else
        return QString("en");
}

bool SettingsDialog::getMouseControl()
{
    return ui->checkBox_mouseControl->isChecked();
}

void SettingsDialog::updateLanguage()
{
    this->languageChangeSignalOff = true;
    this->saveSettings();
    ui->retranslateUi(this);
    int currentIndex = ui->comboBox_directoryPath->currentIndex();
    this->loadSettings();
    ui->comboBox_directoryPath->setCurrentIndex(currentIndex);
    this->languageChangeSignalOff = false;
}

void SettingsDialog::updateHistory()
{
    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture Show");
    QStringList history = settings.value("historyOfDirectories").toStringList();

    ui->comboBox_directoryPath->clear();

    foreach(QString dir, history)
        ui->comboBox_directoryPath->addItem(dir);
}

void SettingsDialog::addDirectoryToHistory(const QString & dir)
{
    if (!ui->checkBox_historySave->isChecked())
        return;

    QString newDir = QString(dir);
    if (dir.endsWith(QDir::separator()))
        newDir = dir.left(dir.length()-1);

    QDir directory(newDir);

    if (!directory.isAbsolute() && !directory.exists())
        return;

    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture Show");
    QStringList history = settings.value("historyOfDirectories").toStringList();

    history.removeAll(newDir);
    history.prepend(newDir);

    settings.setValue("historyOfDirectories", QVariant(history));

    this->updateHistory();
}

void SettingsDialog::on_pushButton_browse_clicked()
{
    QString directory = QFileDialog::getExistingDirectory(this, tr("Öffne Verzeichnis"),
                                                          ui->comboBox_directoryPath->lineEdit()->text(),
                                                          QFileDialog::ShowDirsOnly);


    if (!directory.isEmpty())
        ui->comboBox_directoryPath->lineEdit()->setText(directory);
}

void SettingsDialog::loadSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture Show");
    ui->comboBox_effect->setCurrentIndex(settings.value("effect", QVariant(1)).toInt());
    ui->comboBox_fadeTime->setCurrentIndex(settings.value("fadeTime", QVariant(2)).toInt());
    ui->comboBox_sort->setCurrentIndex(settings.value("sortOrder", QVariant(0)).toInt());
    ui->comboBox_scaling->setCurrentIndex(settings.value("scaleType", QVariant(0)).toInt());

    int languageID = settings.value("languageID", QVariant(-1)).toInt();
    if (languageID == -1)
    {
        QStringList parts = QLocale::system().name().split("_", QString::SkipEmptyParts);

        qDebug(parts.at(0).toStdString().c_str());
        if (parts.size() > 0 && parts.at(0) == "de")
            languageID = 0;
        else
            languageID = 1;
    }

    QStringList history = settings.value("historyOfDirectories").toStringList();

    ui->comboBox_directoryPath->clear();
    foreach(QString dir, history)
        ui->comboBox_directoryPath->addItem(dir);

    ui->comboBox_language->setCurrentIndex(languageID);
    ui->checkBox_mouseControl->setChecked(settings.value("mouseControl", QVariant(false)).toBool());
    ui->checkBox_historySave->setChecked(settings.value("saveHistory", QVariant(true)).toBool());
    ui->checkBox_subdirs->setChecked(settings.value("includeSubdirs", QVariant(false)).toBool());
}

void SettingsDialog::saveSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture Show");
    settings.setValue("effect", QVariant(ui->comboBox_effect->currentIndex()));
    settings.setValue("fadeTime", QVariant(ui->comboBox_fadeTime->currentIndex()));
    settings.setValue("sortOrder", QVariant(ui->comboBox_sort->currentIndex()));
    settings.setValue("scaleType", QVariant(ui->comboBox_scaling->currentIndex()));
    settings.setValue("languageID", QVariant(ui->comboBox_language->currentIndex()));
    settings.setValue("mouseControl", QVariant(ui->checkBox_mouseControl->isChecked()));
    settings.setValue("saveHistory", QVariant(ui->checkBox_historySave->isChecked()));
    settings.setValue("includeSubdirs", QVariant(ui->checkBox_subdirs->isChecked()));
}

void SettingsDialog::on_comboBox_language_currentIndexChanged(int index)
{
    if (!this->languageChangeSignalOff)
    {
        QString lang = this->getLanguage();
        emit this->languageChanged(lang);
    }
}

void SettingsDialog::on_pushButton_deleteHistory_clicked()
{
    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture Show");
    QStringList emptyHistory;

    settings.setValue("historyOfDirectories", QVariant(emptyHistory));

    this->updateHistory();
}
