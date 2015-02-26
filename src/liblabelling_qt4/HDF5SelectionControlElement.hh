/**************************************************************************
**       Title: Widget for entering or selecting an hdf5 object
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#ifndef HDF5SELECTIONCONTROLELEMENT_HH
#define HDF5SELECTIONCONTROLELEMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "AbstractControlElement.hh"

#include "ChannelSpecs.hh"

class QLineEdit;

class MultiChannelModel;

class HDF5SelectionControlElement : public AbstractControlElement
{

  Q_OBJECT
  
public:
  
  HDF5SelectionControlElement(
      const QString& label, QString const &filename,
      const std::string& value = std::string(),
      MultiChannelModel *model = NULL,
      ChannelSpecs::ChannelTypes types = ChannelSpecs::Any,
      QWidget* parent = NULL);
  ~HDF5SelectionControlElement();

  QString const &fileName() const;

  std::string value() const;
  std::string toStdString() const;

public slots:

  void setFileName(QString const &filename);
  void setValue(const std::string& value);

private slots:
  
  void openHDF5SelectionDialog();

private:
  
  QString _filename;
  MultiChannelModel *p_model;
  ChannelSpecs::ChannelTypes _channelTypes;
  QLineEdit* p_lineedit;
  QWidget *p_parent;
  
};

#endif
