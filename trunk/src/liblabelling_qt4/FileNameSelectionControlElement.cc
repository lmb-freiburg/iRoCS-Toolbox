/**************************************************************************
 *
 * This file belongs to the iRoCS Toolbox.
 *
 * Copyright (C) 2015 Thorsten Falk
 *
 *        Image Analysis Lab, University of Freiburg, Germany
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **************************************************************************/

#include "FileNameSelectionControlElement.hh"

#include <QtGui/QHBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QFileDialog>
#include <QtGui/QPushButton>
#include <QtCore/QSettings>

FileNameSelectionControlElement::FileNameSelectionControlElement(
    const QString& label, const std::string& value, bool onlyExistingFiles,
    const QString& filter, QWidget* parent)
        : AbstractControlElement(label, parent),
          _onlyExisting(onlyExistingFiles), _filter(filter)
{
  p_lineedit = new QLineEdit;
  p_lineedit->setText(value.c_str());
  p_layout->addWidget(p_lineedit);
  connect(p_lineedit, SIGNAL(textChanged(QString const &)),
          SIGNAL(valueChanged(QString const &)));
  connect(p_lineedit, SIGNAL(textChanged(QString const &)),
          SLOT(emitValueChange()));
  connect(p_lineedit, SIGNAL(textChanged(QString const &)),
          SIGNAL(valueChanged()));
  
  QPushButton* browseButton = new QPushButton(tr("Browse..."));
  p_layout->addWidget(browseButton);
  connect(browseButton, SIGNAL(clicked()), SLOT(openFileSelector()));
}

FileNameSelectionControlElement::~FileNameSelectionControlElement()
{}

std::string FileNameSelectionControlElement::value() const
{
  return p_lineedit->text().toStdString();
}

std::string FileNameSelectionControlElement::toStdString() const 
{
  return value();
}

void FileNameSelectionControlElement::setValue(const std::string& value)
{
  p_lineedit->setText(value.c_str());
}

void FileNameSelectionControlElement::emitValueChange()
{
  emit valueChanged(p_lineedit->text().toStdString());
}

void FileNameSelectionControlElement::openFileSelector()
{
  QSettings settings;
  QString startPath = settings.value(
      "FileDialog/startpath", getenv("HOME")).toString();
  QString selectedFilter;
  if (_onlyExisting)
      p_lineedit->setText(
          QFileDialog::getOpenFileName(
              this, tr("Select File"), startPath, _filter, &selectedFilter));
  else
      p_lineedit->setText(
          QFileDialog::getSaveFileName(
              this, tr("Select File"), startPath, _filter, &selectedFilter,
              QFileDialog::DontConfirmOverwrite));
  std::string path = p_lineedit->text().toStdString().substr(
      0, p_lineedit->text().toStdString().rfind("/"));
  settings.setValue("FileDialog/startpath", path.c_str());
  emit AbstractControlElement::valueChanged();
}
