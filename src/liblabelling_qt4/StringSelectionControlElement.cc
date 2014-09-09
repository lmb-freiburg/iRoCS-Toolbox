/**************************************************************************
**       Title: Widget for choosing a string from a string list
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#include "StringSelectionControlElement.hh"

#include <QtGui/QHBoxLayout>
#include <QtGui/QComboBox>
#include <QtCore/QStringList>

StringSelectionControlElement::StringSelectionControlElement(
    const QString& label, QStringList const &elements, QWidget* parent)
        : AbstractControlElement(label, parent)
{
  p_combobox = new QComboBox;
  p_combobox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  p_combobox->addItems(elements);
  p_combobox->setEditable(false);
  p_layout->addWidget(p_combobox);

  connect(p_combobox, SIGNAL(activated(int)), SIGNAL(activated(int)));
  connect(p_combobox, SIGNAL(activated(int)), SIGNAL(valueChanged(int)));
  connect(p_combobox, SIGNAL(activated(int)), SIGNAL(valueChanged()));
  connect(p_combobox, SIGNAL(activated(const QString&)),
          SIGNAL(activated(const QString&)));
  connect(p_combobox, SIGNAL(currentIndexChanged(int)),
          SIGNAL(currentIndexChanged(int)));
  connect(p_combobox, SIGNAL(currentIndexChanged(const QString&)),
          SIGNAL(currentIndexChanged(const QString&)));
  connect(p_combobox, SIGNAL(highlighted(int)), SIGNAL(highlighted(int)));
  connect(p_combobox, SIGNAL(highlighted(const QString&)),
          SIGNAL(highlighted(const QString&)));
}

StringSelectionControlElement::~StringSelectionControlElement()
{}

int StringSelectionControlElement::value() const
{
  return p_combobox->currentIndex();
}

int StringSelectionControlElement::nValues() const
{
  return p_combobox->count();
}

std::string StringSelectionControlElement::toStdString() const
{
  return p_combobox->currentText().toStdString();
}
  
void StringSelectionControlElement::setValue(const int value)
{
  if (value >= 0 && value < p_combobox->count())
      p_combobox->setCurrentIndex(value);
}

void StringSelectionControlElement::setValue(const std::string& value)
{
  int index = p_combobox->findText(value.c_str());
  setValue(index);
}

