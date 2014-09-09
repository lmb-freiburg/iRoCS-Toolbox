/**************************************************************************
**       Title: Widget for editing booleans
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#ifndef BOOLCONTROLELEMENT_HH
#define BOOLCONTROLELEMENT_HH

#include "AbstractControlElement.hh"

class QCheckBox;

class BoolControlElement : public AbstractControlElement
{

  Q_OBJECT
  
public:
  
  BoolControlElement(
      const QString& label, const bool value = false, QWidget* parent = 0);
  ~BoolControlElement();

  bool value() const;
  std::string toStdString() const;

signals:
        
  void valueChanged(bool);

public slots:
  
  void setValue(const bool value);
  void setValue(const std::string& value);

private:
  
  QCheckBox* p_checkbox;
  
};

#endif
