/**************************************************************************
**       Title: Widget for editing 3-D vectorial integer numbers
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#ifndef INT3CONTROLELEMENT_HH
#define INT3CONTROLELEMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "AbstractControlElement.hh"

#include <blitz/array.h>

class QSpinBox;

class Int3ControlElement : public AbstractControlElement
{

  Q_OBJECT
  
public:
  
  Int3ControlElement(
      const QString& label, const blitz::TinyVector<int,3>& value = -1,
      QWidget* parent = 0);
  ~Int3ControlElement();

  void setRange(const blitz::TinyVector<int,3>& min,
                const blitz::TinyVector<int,3>& max);

  blitz::TinyVector<int,3> value() const;
  std::string toStdString() const;

public slots:
  
  void setValue(const blitz::TinyVector<int,3>& value);
  void setValue(const std::string& value);

private:
  
  blitz::TinyVector<QSpinBox*,3> _spinbox;
  
};

#endif
