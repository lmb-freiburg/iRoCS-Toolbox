/**************************************************************************
**       Title: Widget for editing RGB colors
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
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
    p_layout->addWidget(_spinbox(d), 1);
    connect(_spinbox(d), SIGNAL(valueChanged(int)), SIGNAL(valueChanged()));
    connect(_spinbox(d), SIGNAL(valueChanged(int)),
            SLOT(updateToolButtonColor()));
  }

  p_colorToolButton = new QToolButton;
  updateToolButtonColor();
  connect(p_colorToolButton, SIGNAL(clicked()), SLOT(chooseColor()));
  p_colorToolButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  p_layout->addWidget(p_colorToolButton);
  p_layout->addStretch(1);
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
