/**************************************************************************
**       Title: Widget for selecting groups/datasets in an HDF5 file
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#ifndef HDF5SELECTIONDIALOG_HH
#define HDF5SELECTIONDIALOG_HH

#include <QtGui/QDialog>

#include "ChannelSpecs.hh"

#include <libBlitzHdf5/BlitzHdf5Light.hh>

class QTreeWidget;
class QTreeWidgetItem;

class MultiChannelModel;

class HDF5ColorMapSelectionDialog : public QDialog
{

  Q_OBJECT
  
public:

  static std::string selectColorMap(
      QString const &filename, QWidget *parent = NULL);

private:
  
  HDF5ColorMapSelectionDialog(QString const &fileName, QWidget *parent);
  ~HDF5ColorMapSelectionDialog();

  std::string absoluteGroupName(const QTreeWidgetItem* group) const;
  void addH5Objects(QTreeWidgetItem* group, const BlitzH5File& inFile);
  std::string selectedItem() const;

private slots:
  
  void adjustColumnWidths();

private:

  QString _filename;
  bool _valid;
  QTreeWidget* p_h5TreeWidget;
  
};

class HDF5SelectionDialog : public QDialog
{

  Q_OBJECT
  
public:

  static std::vector<std::string> selectObjects(
      const QString& filename, MultiChannelModel *model = NULL,
      QWidget* parent = NULL, ChannelSpecs::ChannelTypes types =
      ChannelSpecs::Any);

  static std::string selectObject(
      const QString& filename, MultiChannelModel *model = NULL,
      QWidget* parent = NULL, ChannelSpecs::ChannelTypes types =
      ChannelSpecs::Any);

private:
  
  HDF5SelectionDialog(
      const QString& fileName, MultiChannelModel *model,
      QWidget *parent, ChannelSpecs::ChannelTypes types);
  ~HDF5SelectionDialog();

  std::string absoluteGroupName(const QTreeWidgetItem* group) const;
  void addH5Objects(QTreeWidgetItem* group, const BlitzH5File& inFile);
  std::vector<std::string> selectedItems() const;

private slots:
  
  void adjustColumnWidths();

private:

  MultiChannelModel *p_model;
  QString _filename;
  bool _valid;
  QTreeWidget* p_h5TreeWidget;
  ChannelSpecs::ChannelTypes _channelTypes;
  
};

#endif
