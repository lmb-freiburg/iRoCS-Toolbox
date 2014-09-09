/**************************************************************************
**       Title: Widget for editing strings
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#ifndef STRINGCONTROLELEMENT_HH
#define STRINGCONTROLELEMENT_HH

#include "AbstractControlElement.hh"

class QLineEdit;

class StringControlElement : public AbstractControlElement
{

  Q_OBJECT
  
public:
  
  StringControlElement(
      const QString& label, const std::string& value = std::string(),
      QWidget* parent = 0);
  ~StringControlElement();

  std::string value() const;
  std::string toStdString() const;

public slots:
  
  void setValue(const std::string& value);

private slots:
  
  void emitValueChange();

signals:
  
  void valueChanged(std::string const &);

private:
  
  QLineEdit* p_lineedit;
  
};

#endif
