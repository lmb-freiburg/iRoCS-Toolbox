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

#include "ColorControlElement.hh"

#include <QtGui/QHBoxLayout>
#include <QtGui/QSpinBox>
#include <QtGui/QToolButton>
#include <QtGui/QColorDialog>

#if defined HAVE_CONFIG_H && !defined PACKAGE
#include <config.hh>
#endif

#ifdef HAVE_BLITZ_V9
#include <blitz/tinyvec-et.h>
#endif

ColorControlElement::ColorControlElement(
    const QString& label, const blitz::TinyVector<unsigned char,3>& value,
    QWidget* parent)
        : AbstractControlElement(label, parent)
{
  for (int d = 0; d < 3; ++d)
  {
    _spinbox(d) = new QSpinBox;
    _spinbox(d)->setRange(0, 255);
    _spinbox(d)->setValue(value(d));
    _spinbox(d)->setFixedWidth(60);
    _spinbox(d)->setKeyboardTracking(false);
    p_controlLayout->addWidget(_spinbox(d), 1);
    connect(_spinbox(d), SIGNAL(valueChanged(int)), SIGNAL(valueChanged()));
    connect(_spinbox(d), SIGNAL(valueChanged(int)),
            SLOT(updateToolButtonColor()));
  }

  p_colorToolButton = new QToolButton;
  updateToolButtonColor();
  connect(p_colorToolButton, SIGNAL(clicked()), SLOT(chooseColor()));
  p_colorToolButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  p_controlLayout->addWidget(p_colorToolButton);
  p_controlLayout->addStretch(1);
}

ColorControlElement::~ColorControlElement()
{}

blitz::TinyVector<unsigned char,3> ColorControlElement::value() const
{
  return blitz::TinyVector<unsigned char,3>(
      static_cast<unsigned char>(_spinbox(0)->value()),
      static_cast<unsigned char>(_spinbox(1)->value()),
      static_cast<unsigned char>(_spinbox(2)->value()));
}

std::string ColorControlElement::toStdString() const
{
  std::stringstream outStream;
  outStream << value();
  return outStream.str();
}

void ColorControlElement::setValue(
    const blitz::TinyVector<unsigned char,3>& value)
{
  if (blitz::all(value == this->value())) return;
  for (int d = 0; d < 3; ++d)
  {
    disconnect(_spinbox(d), SIGNAL(valueChanged(int)),
               this, SIGNAL(valueChanged()));
    disconnect(_spinbox(d), SIGNAL(valueChanged(int)),
               this, SLOT(updateToolButtonColor()));
    _spinbox(d)->setValue(value(d));
    connect(_spinbox(d), SIGNAL(valueChanged(int)), SIGNAL(valueChanged()));
    connect(_spinbox(d), SIGNAL(valueChanged(int)),
            SLOT(updateToolButtonColor()));
  }
  updateToolButtonColor();
  emit valueChanged();
}

void ColorControlElement::setValue(const std::string& value)
{
  blitz::TinyVector<unsigned char,3> vec;
  std::stringstream inStream(value);
  inStream >> vec;
  setValue(vec);
}

void ColorControlElement::chooseColor()
{
  blitz::TinyVector<unsigned char,3> col(value());
  QColor color = QColorDialog::getColor(
      QColor(static_cast<int>(col(0)), static_cast<int>(col(1)),
             static_cast<int>(col(2))), this);
  if (color.isValid())
      setValue(blitz::TinyVector<unsigned char,3>(
                   static_cast<unsigned char>(color.red()),
                   static_cast<unsigned char>(color.green()),
                   static_cast<unsigned char>(color.blue())));
}

void ColorControlElement::updateToolButtonColor()
{
  blitz::TinyVector<unsigned char,3> col = value();
  QPixmap pixmap(16, 16);
  pixmap.fill(QColor(static_cast<int>(col(0)), static_cast<int>(col(1)),
                     static_cast<int>(col(2))));
  p_colorToolButton->setIcon(QIcon(pixmap));
}
