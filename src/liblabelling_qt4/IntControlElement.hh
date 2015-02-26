/**************************************************************************
**       Title: Widget for editing integer numbers
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#ifndef INTCONTROLELEMENT_HH
#define INTCONTROLELEMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "AbstractControlElement.hh"

class QSpinBox;

class IntControlElement : public AbstractControlElement
{

  Q_OBJECT
  
public:
  
  IntControlElement(
      const QString& label, const int value = -1, QWidget* parent = 0);
  ~IntControlElement();

  void setRange(const int min, const int max);

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

  int value() const;
  std::string toStdString() const;

signals:
  void valueChanged(int);
  
public slots:
  
  void setValue(const int value);
  void setValue(const std::string& value);

private:
  
  QSpinBox* p_spinbox;
  
};

#endif
