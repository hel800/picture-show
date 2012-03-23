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

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);    
    this->setModal(true);

    this->languageChangeSignalOff = true;
    this->loadSettings();
    this->languageChangeSignalOff = false;
}

SettingsDialog::~SettingsDialog()
{
    this->saveSettings();
    delete ui;
}

QString SettingsDialog::getCurrentDirectory()
{
    return ui->lineEdit_directory->text();
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

void SettingsDialog::updateLanguage()
{
    this->languageChangeSignalOff = true;
    this->saveSettings();
    ui->retranslateUi(this);
    this->loadSettings();
    this->languageChangeSignalOff = false;
}

void SettingsDialog::on_pushButton_browse_clicked()
{
    ui->lineEdit_directory->setText(QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                     ui->lineEdit_directory->text(),
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks));
}

void SettingsDialog::loadSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture Show");
    ui->lineEdit_directory->setText(settings.value("lastDir", QVariant(QString(""))).toString());
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

    ui->comboBox_language->setCurrentIndex(languageID);
}

void SettingsDialog::saveSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "bsSoft", "picture Show");
    settings.setValue("lastDir", QVariant(ui->lineEdit_directory->text()));
    settings.setValue("effect", QVariant(ui->comboBox_effect->currentIndex()));
    settings.setValue("fadeTime", QVariant(ui->comboBox_fadeTime->currentIndex()));
    settings.setValue("sortOrder", QVariant(ui->comboBox_sort->currentIndex()));
    settings.setValue("scaleType", QVariant(ui->comboBox_scaling->currentIndex()));
    settings.setValue("languageID", QVariant(ui->comboBox_language->currentIndex()));
}

void SettingsDialog::on_comboBox_language_currentIndexChanged(int index)
{
    if (!this->languageChangeSignalOff)
    {
        QString lang = this->getLanguage();
        emit this->languageChanged(lang);
    }
}
