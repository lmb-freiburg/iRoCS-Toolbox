/**************************************************************************
**       Title: Widget for editing RGB colors
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#ifndef COLORCONTROLELEMENT_HH
#define COLORCONTROLELEMENT_HH

#include "AbstractControlElement.hh"

#include <blitz/array.h>
// #include <blitz/tinyvec-et.h>

class QSpinBox;
class QToolButton;

class ColorControlElement : public AbstractControlElement
{

  Q_OBJECT
  
public:
  
  ColorControlElement(
      const QString& label, const blitz::TinyVector<unsigned char,3>& value = 
      (blitz::TinyVector<unsigned char,3>(0, 0, 0)), QWidget* parent = 0);
  ~ColorControlElement();

  blitz::TinyVector<unsigned char,3> value() const;
  std::string toStdString() const;

public slots:
  
  void setValue(const blitz::TinyVector<unsigned char,3>& value);
  void setValue(const std::string& value);

private slots:
  
  void chooseColor();
  void updateToolButtonColor();

private:
  
  blitz::TinyVector<QSpinBox*,3> _spinbox;
  QToolButton* p_colorToolButton;
  
};

#endif
