/**************************************************************************
**       Title: Widget for editing floating point numbers in double 
**              precision
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#include "DoubleControlElement.hh"

#include <QtGui/QHBoxLayout>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QSlider>

#include <limits>

DoubleControlElement::DoubleControlElement(
    const QString& label, const double value, QWidget* parent)
        : AbstractControlElement(label, parent)
{
  p_spinbox = new QDoubleSpinBox;
  // p_spinbox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  p_spinbox->setRange(-std::numeric_limits<double>::infinity(),
                      std::numeric_limits<double>::infinity());
  p_spinbox->setSingleStep(0.1);
  p_spinbox->setDecimals(6);
  p_spinbox->setValue(value);
  p_spinbox->setMinimumWidth(80);
  p_spinbox->setKeyboardTracking(false);
  p_layout->addWidget(p_spinbox);
  connect(p_spinbox, SIGNAL(valueChanged(double)), SIGNAL(valueChanged()));
  connect(p_spinbox, SIGNAL(valueChanged(double)),
          SIGNAL(valueChanged(double)));

  p_slider = NULL;

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

DoubleControlElement::~DoubleControlElement()
{}

bool DoubleControlElement::sliderViewEnabled() const
{
  return p_slider != NULL;
}

void DoubleControlElement::setSliderViewEnabled(bool enable)
{
  if (p_slider == NULL && !enable) return;

  if (enable && static_cast<int>(
          (p_spinbox->maximum() - p_spinbox->minimum()) /
          p_spinbox->singleStep()) < 1000000)
  {
    p_slider = new QSlider;
    // p_slider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    p_slider->setRange(
        0, static_cast<int>(
            (p_spinbox->maximum() - p_spinbox->minimum()) /
            p_spinbox->singleStep()));
    p_slider->setSingleStep(1);
    p_slider->setPageStep(
        (p_slider->maximum() / 100 > 1) ? p_slider->maximum() / 100 : 1);
    p_slider->setOrientation(Qt::Horizontal);
    updateSlider(p_spinbox->value());
    connect(p_slider, SIGNAL(sliderMoved(int)), SLOT(updateSpinBox(int)));
    connect(p_spinbox, SIGNAL(valueChanged(double)),
            SLOT(updateSlider(double)));
    p_layout->insertWidget(p_layout->count() - 1, p_slider);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  }
  else
  {
    disconnect(p_spinbox, SIGNAL(valueChanged(double)),
               this, SLOT(updateSlider(double)));
    p_layout->removeWidget(p_slider);
    delete p_slider;
    p_slider = NULL;
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  }
}

void DoubleControlElement::setRange(const double min, const double max)
{
  p_spinbox->setRange(min, max);
  if (p_slider != NULL && static_cast<int>(
          (p_spinbox->maximum() - p_spinbox->minimum()) /
          p_spinbox->singleStep()) < 1000000)
  {
    p_slider->setRange(
        0, static_cast<int>(
            (p_spinbox->maximum() - p_spinbox->minimum()) /
            p_spinbox->singleStep()));
    p_slider->setPageStep(
        (p_slider->maximum() / 100 > 1) ? p_slider->maximum() / 100 : 1);
    updateSlider(p_spinbox->value());    
  }
  else
  {
    if (p_slider != NULL)
    {
      disconnect(p_spinbox, SIGNAL(valueChanged(double)),
                 this, SLOT(updateSlider(double)));
      p_layout->removeWidget(p_slider);
      delete p_slider;
      p_slider = NULL;
    }
  }
}

void DoubleControlElement::setSingleStep(const double step)
{
  p_spinbox->setSingleStep(step);
  if (p_slider != NULL && static_cast<int>(
          (p_spinbox->maximum() - p_spinbox->minimum()) /
          p_spinbox->singleStep()) < 1000000)
  {
    p_slider->setRange(
        0, static_cast<int>(
            (p_spinbox->maximum() - p_spinbox->minimum()) /
            p_spinbox->singleStep()));
    p_slider->setPageStep(
        (p_slider->maximum() / 100 > 1) ? p_slider->maximum() / 100 : 1);
    updateSlider(p_spinbox->value());    
  }
  else
  {
    if (p_slider != NULL)
    {
      disconnect(p_spinbox, SIGNAL(valueChanged(double)),
                 this, SLOT(updateSlider(double)));
      p_layout->removeWidget(p_slider);
      delete p_slider;
      p_slider = NULL;
    }
  }
}

void DoubleControlElement::setSpecialValueText(QString const &text)
{
  p_spinbox->setSpecialValueText(text);
}

void DoubleControlElement::setSuffix(QString const &text)
{
  p_spinbox->setSuffix(text);
}

double DoubleControlElement::value() const 
{
  return p_spinbox->value();
}

std::string DoubleControlElement::toStdString() const 
{
  return QString::number(value()).toStdString();
}

void DoubleControlElement::setValue(const double& value)
{
  p_spinbox->setValue(value);
}

void DoubleControlElement::setValue(const std::string& value)
{
  p_spinbox->setValue(atof(value.c_str()));
}

void DoubleControlElement::updateSpinBox(int value)
{
  if (static_cast<double>(value) * p_spinbox->singleStep() +
      p_spinbox->minimum() == p_spinbox->value()) return;
  // disconnect(p_gammaSpinBox, SIGNAL(valueChanged(double)),
  //            this, SLOT(updateGammaSlider(double)));
  p_spinbox->setValue(
      static_cast<double>(value) * p_spinbox->singleStep() +
      p_spinbox->minimum());
  // connect(p_gammaSpinBox, SIGNAL(valueChanged(double)),
  //         SLOT(updateGammaSlider(double)));
}

void DoubleControlElement::updateSlider(double value)
{
  if (static_cast<int>((value - p_spinbox->minimum()) /
                       p_spinbox->singleStep()) == p_slider->value()) return;
  // disconnect(p_gammaSlider, SIGNAL(sliderMoved(int)),
  //            this, SLOT(updateGammaSpinBox(int)));
  p_slider->setValue(
      static_cast<int>(
          (value - p_spinbox->minimum()) / p_spinbox->singleStep()));
  // connect(p_gammaSlider, SIGNAL(sliderMoved(int)),
  //         SLOT(updateGammaSpinBox(int)));
}

