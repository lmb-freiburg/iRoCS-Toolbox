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

#include "HDF5SelectionControlElement.hh"

#include <QtGui/QHBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include "HDF5SelectionDialog.hh"
#include "MultiChannelModel.hh"

HDF5SelectionControlElement::HDF5SelectionControlElement(
    const QString& label, QString const &filename, const std::string& value,
    MultiChannelModel *model, ChannelSpecs::ChannelTypes types, QWidget* parent)
        : AbstractControlElement(label, parent),
          _filename(filename), p_model(model), _channelTypes(types),
          p_parent(parent)
{
  p_lineedit = new QLineEdit;
  p_lineedit->setText(value.c_str());
  p_controlLayout->addWidget(p_lineedit);
  connect(p_lineedit, SIGNAL(editingFinished()), SIGNAL(valueChanged()));

  QPushButton* browseButton = new QPushButton(tr("Choose..."));
  p_controlLayout->addWidget(browseButton);
  connect(browseButton, SIGNAL(clicked()), SLOT(openHDF5SelectionDialog()));
}

HDF5SelectionControlElement::~HDF5SelectionControlElement()
{}

QString const &HDF5SelectionControlElement::fileName() const
{
  return _filename;
}

std::string HDF5SelectionControlElement::value() const
{
  return p_lineedit->text().toStdString();
}

std::string HDF5SelectionControlElement::toStdString() const
{
  return value();
}

void HDF5SelectionControlElement::setFileName(QString const &filename)
{
  _filename = filename;
}

void HDF5SelectionControlElement::setValue(const std::string& value)
{
  p_lineedit->setText(value.c_str());
}

void HDF5SelectionControlElement::openHDF5SelectionDialog()
{
  std::string res = HDF5SelectionDialog::selectObject(
      _filename, p_model, p_parent, _channelTypes);
  if (res == "") return;
  p_lineedit->setText(res.c_str());
  emit valueChanged();
}
