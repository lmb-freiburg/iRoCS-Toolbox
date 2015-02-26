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

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QDialog>
#include <QtGui/QTreeWidget>

#include "ChannelSpecs.hh"

#include <libBlitzHdf5/BlitzHdf5Light.hh>

class QTreeWidgetItem;

class MultiChannelModel;

class HDF5TreeWidget : public QTreeWidget
{

  Q_OBJECT

public:
  
  enum ItemType { Dataset, Annotation, Colormap, Incompatible };

  HDF5TreeWidget(QWidget *parent = NULL);
  HDF5TreeWidget(std::string const &fileName, QWidget *parent = NULL);
  ~HDF5TreeWidget();
  
  void setFileName(std::string const &fileName);
  std::string const &fileName() const;

  bool isValid() const;
  QString const &errorString() const;

  void setSelectable(ItemType type, bool enable = true);
  void setSelectable(ChannelSpecs::ChannelTypes types, bool enable = true);

  std::string absoluteItemPath(QTreeWidgetItem const *item) const;

public slots:
  
  void adjustColumnWidths();

private:

  void _createGroupItem(
      QTreeWidgetItem *item, BlitzH5File const &inFile,
      std::string const &groupName);
  void _createDatasetItem(
      QTreeWidgetItem *item, BlitzH5File const &inFile,
      std::string const &dsName);
  void _addH5Objects(QTreeWidgetItem* group, const BlitzH5File& inFile);
  void _generateTree();

  std::string _fileName;
  std::map<QTreeWidgetItem*, ItemType> _itemTypes;
  std::map<QTreeWidgetItem*, ChannelSpecs::ChannelType> _itemChannelTypes;
  bool _valid;
  QString _errorString;

};

class HDF5ColorMapSelectionDialog : public QDialog
{

  Q_OBJECT
  
public:

  static std::string selectColorMap(
      QString const &filename, QWidget *parent = NULL);

private:
  
  HDF5ColorMapSelectionDialog(QString const &fileName, QWidget *parent);
  ~HDF5ColorMapSelectionDialog();

  std::string selectedItem() const;

private:

  QString _filename;
  HDF5TreeWidget* p_h5TreeWidget;
  
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

  std::vector<std::string> selectedItems() const;

private:

  MultiChannelModel *p_model;
  QString _filename;
  HDF5TreeWidget* p_h5TreeWidget;
  ChannelSpecs::ChannelTypes _channelTypes;
  
};

#endif
