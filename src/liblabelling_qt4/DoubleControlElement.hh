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

#ifndef DOUBLECONTROLELEMENT_HH
#define DOUBLECONTROLELEMENT_HH

#include "AbstractControlElement.hh"

class QDoubleSpinBox;
class QSlider;

class DoubleControlElement : public AbstractControlElement
{

  Q_OBJECT
  
public:
  
  DoubleControlElement(
      const QString& label, const double value = -1, QWidget* parent = 0);
  ~DoubleControlElement();

  bool sliderViewEnabled() const;
  void setSliderViewEnabled(bool enable);

  void setRange(const double min, const double max);
  void setSingleStep(const double step);

/*======================================================================*/
/*! 
 *   If specified, the given text is shown if the spin box is at its
 *   minimum value instead of that value. If you pass a NULL (empty) string
 *   it will be reset to show the value again.
 *
 *   \param text The text to output instead of the spinbox's minimum value
 */
/*======================================================================*/
  void setSpecialValueText(QString const &text);

/*======================================================================*/
/*! 
 *   If specified, the given text is appended to the value in the spinbox.
 *
 *   \param text The text to append (e.g. unit)
 */
/*======================================================================*/
  void setSuffix(QString const &text);

  double value() const;
  std::string toStdString() const;

signals:

  void valueChanged(double);

public slots:
  
  void setValue(const double& value);
  void setValue(const std::string& value);

private slots:
  
  void updateSpinBox(int value);
  void updateSlider(double value);

private:
  
  QDoubleSpinBox* p_spinbox;
  QSlider* p_slider;
  
};

#endif
