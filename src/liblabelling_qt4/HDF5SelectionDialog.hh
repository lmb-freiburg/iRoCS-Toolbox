/**************************************************************************
 *
 * This file belongs to the iRoCS Toolbox.
 *
 * Copyright (C) 2015 Thorsten Falk
 *
 *        Image Analysis Lab, University of Freiburg, Germany
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
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
