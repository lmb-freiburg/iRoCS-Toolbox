/**************************************************************************
**       Title: Widget for choosing a string from a string list
**    $RCSfile$
**   $Revision:$ $Name$
**       $Date:$
**   Copyright: GPL $Author:$
** Description:
**
**************************************************************************/

#ifndef STRINGSELECTIONCONTROLELEMENT_HH
#define STRINGSELECTIONCONTROLELEMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "AbstractControlElement.hh"

class QComboBox;
class QStringList;

class StringSelectionControlElement : public AbstractControlElement
{
  
  Q_OBJECT

public:

  StringSelectionControlElement(
      const QString& label, QStringList const &elements, QWidget* parent = 0);
  ~StringSelectionControlElement();
  
  int value() const;
  int nValues() const;
  std::string toStdString() const;
  
signals:
        
  void activated(int index);
  void activated(const QString& text);
  void currentIndexChanged(int index);
  void currentIndexChanged(const QString& text);
  void highlighted(int index);
  void highlighted(const QString& text);
  void valueChanged(int index);

public slots:
  
  void setValue(int index);
  void setValue(const std::string& value);
  
private:

  QComboBox* p_combobox;

};

#endif
