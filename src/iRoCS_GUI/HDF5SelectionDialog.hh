/**************************************************************************
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

#ifndef IROCS_HDF5SELECTIONDIALOG_HH
#define IROCS_HDF5SELECTIONDIALOG_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QDialog>
#include <QtGui/QTreeWidget>

#include <libMultiChannelModel/Channel.hh>

#include <libBlitzHdf5/BlitzHdf5Light.hh>

class QTreeWidgetItem;

namespace iRoCS {

  class MultichannelModel;

  class HDF5TreeWidget : public QTreeWidget {

    Q_OBJECT

  public:

    enum ItemType { Dataset, Annotation, Colormap, Incompatible };

    HDF5TreeWidget(iRoCS::ProgressReporter *pr = NULL, QWidget *parent = NULL);
    HDF5TreeWidget(
      std::string const &fileName, iRoCS::ProgressReporter *pr = NULL,
      QWidget *parent = NULL);
    ~HDF5TreeWidget();

    void setFileName(std::string const &fileName);
    std::string const &fileName() const;

    bool isValid() const;
    QString const &errorString() const;

    void setSelectable(ItemType type, bool enable = true);
    void setSelectable(Channel::Types types, bool enable = true);

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
    iRoCS::ProgressReporter *p_progress;
    std::map<QTreeWidgetItem*, ItemType> _itemTypes;
    std::map<QTreeWidgetItem*, Channel::Type> _itemChannelTypes;
    bool _valid;
    QString _errorString;

  };

  // class HDF5ColorMapSelectionDialog : public QDialog
  // {

  //   Q_OBJECT

  // public:

  //   static std::string selectColorMap(
  //       QString const &filename, QWidget *parent = NULL);

  // private:

  //   HDF5ColorMapSelectionDialog(QString const &fileName, QWidget *parent);
  //   ~HDF5ColorMapSelectionDialog();

  //   std::string selectedItem() const;

  // private:

  //   QString _filename;
  //   HDF5TreeWidget* p_h5TreeWidget;

  // };

  class HDF5SelectionDialog : public QDialog {

    Q_OBJECT

  public:

    static std::vector<std::string> selectObjects(
      const QString& filename, MultichannelModel *model = NULL,
      QWidget* parent = NULL, Channel::Types types = Channel::Any,
      iRoCS::ProgressReporter *pr = NULL);

    static std::string selectObject(
      const QString& filename, MultichannelModel *model = NULL,
      QWidget* parent = NULL, Channel::Types types = Channel::Any,
      iRoCS::ProgressReporter *pr = NULL);

  private:

    HDF5SelectionDialog(
      const QString& fileName, MultichannelModel *model,
      QWidget *parent, Channel::Types types, iRoCS::ProgressReporter *pr);
    ~HDF5SelectionDialog();

    std::vector<std::string> selectedItems() const;

  private:

    MultichannelModel *p_model;
    QString _filename;
    HDF5TreeWidget* p_h5TreeWidget;
    Channel::Types _channelTypes;

  };

}

#endif
