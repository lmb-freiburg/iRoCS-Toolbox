/**************************************************************************
**       Title: Widget for entering or selecting an hdf5 object
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#include "HDF5SelectionControlElement.hh"

#include <QtGui/QHBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include "HDF5SelectionDialog.hh"
#include "MultiChannelModel.hh"

HDF5SelectionControlElement::HDF5SelectionControlElement(
    const QString& label, QString const &filename, const std::string& value,
    MultiChannelModel *model, ChannelSpecs::ChannelTypes types, QWidget* parent)
        : AbstractControlElement(label, parent),
          _filename(filename), p_model(model), _channelTypes(types),
          p_parent(parent)
{
  p_lineedit = new QLineEdit;
  p_lineedit->setText(value.c_str());
  p_layout->addWidget(p_lineedit);
  connect(p_lineedit, SIGNAL(editingFinished()), SIGNAL(valueChanged()));
  
  QPushButton* browseButton = new QPushButton(tr("Choose..."));
  p_layout->addWidget(browseButton);
  connect(browseButton, SIGNAL(clicked()), SLOT(openHDF5SelectionDialog()));
}

HDF5SelectionControlElement::~HDF5SelectionControlElement()
{}

QString const &HDF5SelectionControlElement::fileName() const
{
  return _filename;
}

std::string HDF5SelectionControlElement::value() const
{
  return p_lineedit->text().toStdString();
}

std::string HDF5SelectionControlElement::toStdString() const 
{
  return value();
}

void HDF5SelectionControlElement::setFileName(QString const &filename)
{
  _filename = filename;
}

void HDF5SelectionControlElement::setValue(const std::string& value)
{
  p_lineedit->setText(value.c_str());
}

void HDF5SelectionControlElement::openHDF5SelectionDialog()
{
  std::string res = HDF5SelectionDialog::selectObject(
      _filename, p_model, p_parent, _channelTypes);
  if (res == "") return;
  p_lineedit->setText(res.c_str());
  emit valueChanged();
}
