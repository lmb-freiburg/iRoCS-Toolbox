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

#include "HDF5SelectionDialog.hh"

#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QMessageBox>

#include "MultiChannelModel.hh"
#include "ColorMap.hh"

// #define HDF5SELECTIONDIALOG_DEBUG

/*-----------------------------------------------------------------------
 *  The general tree widget for hdf5 file content visualization and
 *  selection
 *-----------------------------------------------------------------------*/

HDF5TreeWidget::HDF5TreeWidget(QWidget *parent)
        : QTreeWidget(parent), _fileName(""), _itemTypes(), _itemChannelTypes(),
          _valid(false), _errorString("")
{
  _generateTree();
}

HDF5TreeWidget::HDF5TreeWidget(
    std::string const &fileName, QWidget *parent)
        : QTreeWidget(parent), _fileName(fileName), _itemTypes(),
          _itemChannelTypes(), _valid(false), _errorString("")
{
  _generateTree();
}

HDF5TreeWidget::~HDF5TreeWidget()
{}
  
void HDF5TreeWidget::setFileName(std::string const &fileName)
{
  if (_fileName == fileName) return;
  _fileName = fileName;
  _generateTree();
}

std::string const &HDF5TreeWidget::fileName() const
{
  return _fileName;
}

bool HDF5TreeWidget::isValid() const
{
  return _valid;
}

QString const &HDF5TreeWidget::errorString() const
{
  return _errorString;
}

void HDF5TreeWidget::setSelectable(ItemType type, bool enable)
{
  for (std::map<QTreeWidgetItem*,ItemType>::iterator it = _itemTypes.begin();
       it != _itemTypes.end(); ++it)
  {
    if (it->second == type)
    {
      if (enable)
          it->first->setFlags(it->first->flags() | Qt::ItemIsSelectable);
      else
          it->first->setFlags(it->first->flags() & ~Qt::ItemIsSelectable);
    }
  }
}

void HDF5TreeWidget::setSelectable(
    ChannelSpecs::ChannelTypes types, bool enable)
{
  for (std::map<QTreeWidgetItem*,ChannelSpecs::ChannelType>::iterator it =
           _itemChannelTypes.begin(); it != _itemChannelTypes.end(); ++it)
  {
    if (types.testFlag(it->second))
    {
      if (enable)
          it->first->setFlags(it->first->flags() | Qt::ItemIsSelectable);
      else
          it->first->setFlags(it->first->flags() & ~Qt::ItemIsSelectable);
    }
  }
}

std::string HDF5TreeWidget::absoluteItemPath(QTreeWidgetItem const *item) const
{
  std::string absName;
  const QTreeWidgetItem* current = item;
  while (current->text(0) != "/") 
  {
    absName = "/" + current->text(0).toStdString() + absName;
    current = current->parent();
  }
  return absName;
}

void HDF5TreeWidget::adjustColumnWidths()
{
  for (int c = 0; c < columnCount(); ++c) resizeColumnToContents(c);
}

void HDF5TreeWidget::_createGroupItem(
    QTreeWidgetItem *item, BlitzH5File const &inFile,
    std::string const &groupName)
{
  // Groups are always enabled to traverse the tree
  item->setFlags(Qt::ItemIsEnabled);

  // Initially set the item Incompatible. If it is an annotation or colormap
  // it will be changed accordingly
  _itemTypes[item] = Incompatible;
  
  if (inFile.existsDataset(groupName + "/position_um"))
  {
    if (inFile.existsDataset(groupName + "/radius_um"))
    {
      if (inFile.existsDataset(groupName + "/mitotic"))
          item->setText(1, tr("Annotation (Nuclei)"));
      else if (inFile.existsDataset(groupName + "/coefficients"))
          item->setText(1, tr("Annotation (Spherical Harmonic Surfaces)"));
      else if (inFile.existsDataset(groupName + "/length_um"))
          item->setText(1, tr("Annotation (Cylinders)"));
      else item->setText(1, tr("Annotation (Spheres)"));
    }
    else
    {
      if (inFile.existsDataset(groupName + "/mitotic"))
          item->setText(1, tr("Annotation (Cells)"));
      else if (inFile.existsDataset(groupName + "/nElements"))
          item->setText(1, tr("Annotation (Surfaces)"));
      else item->setText(1, tr("Annotation (Points)"));
    }
    _itemTypes[item] = Annotation;
    _itemChannelTypes[item] = ChannelSpecs::Annotation;
  }
  else
  {
    try
    {
      atb::BSpline< blitz::TinyVector<double,3> > spline;
      spline.load(inFile, groupName);
      item->setText(1, tr("B-Spline"));
      _itemTypes[item] = Annotation;
      _itemChannelTypes[item] = ChannelSpecs::SplineCurve;
    }
    catch(BlitzH5Error &)
    {}  
    try
    {
      atb::IRoCS rct;
      rct.load(inFile, groupName);
      item->setText(1, tr("iRoCS"));
      _itemTypes[item] = Annotation;
      _itemChannelTypes[item] = ChannelSpecs::IRoCS;      
    }
    catch(BlitzH5Error &)
    {}
    try
    {
      ShellCoordinateTransform sct;
      sct.load(inFile, groupName);
      item->setText(1, tr("iRoCS Shell Coordinate Transform"));
      _itemTypes[item] = Annotation;
      _itemChannelTypes[item] = ChannelSpecs::IRoCSSCT;
    }
    catch(BlitzH5Error &)
    {}
    try
    {
      ColorMap colorMap;
      colorMap.load(inFile, groupName);
      item->setText(1, tr("Colormap"));
      _itemTypes[item] = Colormap;
    }
    catch(BlitzH5Error &)
    {}
  }
}

void HDF5TreeWidget::_createDatasetItem(
    QTreeWidgetItem *item, BlitzH5File const &inFile,
    std::string const &dsName)
{
  // Initially disable the item. The caller must enable dataset items he wants
  item->setFlags(Qt::NoItemFlags);

  hid_t datasetTypeId = inFile.getDatasetType(dsName);
  if (H5Tget_class(datasetTypeId) == H5T_INTEGER)
  {
    item->setText(1, tr("Dataset (Indexed)"));
    if (H5Tget_sign(datasetTypeId) == H5T_SGN_2)
    {
      switch (H5Tget_size(datasetTypeId))
      {
      case 1 :
        item->setText(2, tr("8-Bit Signed Integer"));
        break;
      case 2 :
        item->setText(2, tr("16-Bit Signed Integer"));
        break;
      case 4 :
        item->setText(2, tr("32-Bit Signed Integer"));
        break;
      default :
        item->setText(2, tr("64-Bit Signed Integer"));
      }
    }
    else
    {
      switch (H5Tget_size(datasetTypeId))
      {
      case 1 :
        item->setText(2, tr("8-Bit Unsigned Integer"));
        break;
      case 2 :
        item->setText(2, tr("16-Bit Unsigned Integer"));
        break;
      case 4 :
        item->setText(2, tr("32-Bit Unsigned Integer"));
        break;
      default :
        item->setText(2, tr("64-Bit Unsigned Integer"));
      }
    }
    _itemChannelTypes[item] = ChannelSpecs::Visualization;
  }
  else
  {
    item->setText(1, tr("Dataset (Real)"));
    switch (H5Tget_size(datasetTypeId))
    {
    case 4 :
      item->setText(2, tr("32-Bit Float"));
    case 8 :
      item->setText(2, tr("64-Bit Float"));
    }
    _itemChannelTypes[item] = ChannelSpecs::Data;
  }
  H5Tclose(datasetTypeId);
  
  std::vector<hsize_t> extents(inFile.getDatasetShape(dsName));
  size_t nDims = extents.size();

  if (nDims <= 1) item->setText(3, tr("Scalar"));
  else
  {
    if (extents.back() == 3)
    {
      // 4-D data with three components in the last dimension will always be
      // treated as RGB data. For other dimensions check the dim_interpretation
      // attribute if it exists
      bool isRGB = false;
      if (extents.size() == 4) isRGB = true;
      else
      {
        try
        {
          std::string dimInterpretation;
          inFile.readAttribute(dimInterpretation, "dim_interpretation", dsName);
          if (dimInterpretation[dimInterpretation.size() - 1] == 'c')
              isRGB = true;
        }
        catch (BlitzH5Error &)
        {}
      }
      if (isRGB)
      {
        nDims = extents.size() - 1;
        item->setText(1, tr("Dataset (RGB)"));
        _itemChannelTypes[item] = ChannelSpecs::RGB;
      }
    }
    std::stringstream shapeStream;
    for (size_t d = 0; d < nDims - 1; ++d) shapeStream << extents[d] << " x ";
    shapeStream << extents[nDims - 1];
    item->setText(3, shapeStream.str().c_str());
  }

  // Finally check whether the dataset is compatible with labelling
  if (nDims == 2 || nDims == 3) _itemTypes[item] = Dataset;
  else
  {
    _itemTypes[item] = Incompatible;
    _itemChannelTypes.erase(_itemChannelTypes.find(item));
  }
  item->setDisabled(_itemTypes[item] == Incompatible);

  try
  {
    std::vector<double> elementSize;
    inFile.readAttribute(elementSize, "element_size_um", dsName);
    std::stringstream elSizeStream;
    for (size_t i = 0; i < elementSize.size() - 1; ++i)
        elSizeStream << elementSize[i] << " x ";
    elSizeStream << elementSize.back();
    item->setText(4, elSizeStream.str().c_str());
  }
  catch (BlitzH5Error &)
  {
    item->setText(4, tr("N/A"));
  }
}

void HDF5TreeWidget::_addH5Objects(
    QTreeWidgetItem *group, const BlitzH5File& inFile)
{
  std::string groupName = absoluteItemPath(group);
  std::vector<std::string> objects(inFile.getObjects(groupName));

  for (std::vector<std::string>::iterator it = objects.begin();
       it != objects.end(); ++it) 
  {
    QTreeWidgetItem *newItem = new QTreeWidgetItem(group);
    newItem->setText(0, it->c_str());
    newItem->setText(1, "");
    newItem->setText(2, "");
    newItem->setText(3, "");
    newItem->setText(4, "");

    if (inFile.existsGroup(groupName + "/" + *it))
    {
      _createGroupItem(newItem, inFile, groupName + "/" + *it);
      _addH5Objects(newItem, inFile);
    }
    else if (inFile.existsDataset(groupName + "/" + *it)) 
        _createDatasetItem(newItem, inFile, groupName + "/" + *it);
  }
}

void HDF5TreeWidget::_generateTree()
{
  clear();
  setColumnCount(5);
  QStringList headerLabels;
  headerLabels.push_back(tr("Name", "getH5ObjectDialog|Name"));
  headerLabels.push_back(tr("Structure", "getH5ObjectDialog|Structure"));
  headerLabels.push_back(tr("Type", "getH5ObjectDialog|Type"));
  headerLabels.push_back(tr("Shape", "getH5ObjectDialog|Shape"));
  headerLabels.push_back(tr("Element Size [vx/micron]",
                            "getH5ObjectDialog|ElementSize"));
  setHeaderLabels(headerLabels);
  connect(this, SIGNAL(expanded(const QModelIndex&)),
          SLOT(adjustColumnWidths()));
  connect(this, SIGNAL(collapsed(const QModelIndex&)),
          SLOT(adjustColumnWidths()));  
  QTreeWidgetItem* root = new QTreeWidgetItem;
  root->setText(0, "/");
  root->setText(1, "");
  root->setText(2, "");
  root->setText(3, "");
  root->setText(4, "");
  try
  {
    BlitzH5File inFile(_fileName);
    _createGroupItem(root, inFile, "/");
    _addH5Objects(root, inFile);
    _valid = true;
  }
  catch (BlitzH5Error &e)
  {
    _valid = false;
    _errorString = e.what();
  }
  addTopLevelItem(root);
  root->setExpanded(true);
}


/*-----------------------------------------------------------------------
 *  The ColorMap selection dialog
 *-----------------------------------------------------------------------*/

std::string HDF5ColorMapSelectionDialog::selectColorMap(
    QString const &filename, QWidget *parent)
{
  HDF5ColorMapSelectionDialog* dlg =
      new HDF5ColorMapSelectionDialog(filename, parent);
  std::string result;
  if (dlg->p_h5TreeWidget->isValid())
  {
    int exitCode = dlg->exec();
    if (exitCode == QDialog::Accepted) result = dlg->selectedItem();
  }
  else
      QMessageBox::critical(
          parent, tr("HDF5 read error"),
          tr("Could not read contents of %1: %2").arg(
              filename).arg(dlg->p_h5TreeWidget->errorString()));
  dlg->setParent(NULL);
  delete dlg;
  return result;
}

HDF5ColorMapSelectionDialog::HDF5ColorMapSelectionDialog(
    QString const &filename, QWidget* parent)
        : QDialog(parent), _filename(filename), p_h5TreeWidget(NULL)
{
  QVBoxLayout* layout = new QVBoxLayout;
  setLayout(layout);
  
  p_h5TreeWidget = new HDF5TreeWidget(_filename.toStdString());
  p_h5TreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
  p_h5TreeWidget->setSelectable(HDF5TreeWidget::Colormap);

  layout->addWidget(p_h5TreeWidget);
  
  QDialogButtonBox* buttonBox =
      new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));

  layout->addWidget(buttonBox);
  resize(640,480);
}

HDF5ColorMapSelectionDialog::~HDF5ColorMapSelectionDialog()
{}
  
std::string HDF5ColorMapSelectionDialog::selectedItem() const
{
  if (p_h5TreeWidget->selectedItems().size() == 0) return "";
  return p_h5TreeWidget->absoluteItemPath(
      p_h5TreeWidget->selectedItems().at(0));
}


/*-----------------------------------------------------------------------
 *  The HDF5 Channel Selection Dialog
 *-----------------------------------------------------------------------*/ 

std::vector<std::string> HDF5SelectionDialog::selectObjects(
    QString const &filename, MultiChannelModel *model, QWidget *parent,
    ChannelSpecs::ChannelTypes types)
{
  HDF5SelectionDialog* dlg =
      new HDF5SelectionDialog(filename, model, parent, types);
  std::vector<std::string> result;
  if (dlg->p_h5TreeWidget->isValid())
  {
    int exitCode = dlg->exec();
    if (exitCode == QDialog::Accepted) result = dlg->selectedItems();
  }
  else
      QMessageBox::critical(
          parent, tr("HDF5 read error"),
          tr("Could not read contents of %1: %2").arg(
              filename).arg(dlg->p_h5TreeWidget->errorString()));
  dlg->setParent(0);
  delete dlg;
  return result;
}

std::string HDF5SelectionDialog::selectObject(
    const QString& filename, MultiChannelModel *model, QWidget* parent,
    ChannelSpecs::ChannelTypes types)
{
  HDF5SelectionDialog* dlg =
      new HDF5SelectionDialog(filename, model, parent, types);
  dlg->p_h5TreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
  std::string result;
  if (dlg->p_h5TreeWidget->isValid())
  {
    int exitCode = dlg->exec();
    if (exitCode == QDialog::Accepted)
        result = (dlg->selectedItems().size() > 0) ?
            dlg->selectedItems().front() : "";
  }
  else
      QMessageBox::critical(
          parent, tr("HDF5 read error"),
          tr("Could not read contents of %1: %2").arg(
              filename).arg(dlg->p_h5TreeWidget->errorString()));
  dlg->setParent(0);
  delete dlg;
  return result;
}

HDF5SelectionDialog::HDF5SelectionDialog(
    const QString& filename, MultiChannelModel *model, QWidget* parent,
    ChannelSpecs::ChannelTypes types)
        : QDialog(parent), p_model(model), _filename(filename),
          _channelTypes(types)
{
  QVBoxLayout* layout = new QVBoxLayout;
  setLayout(layout);
  
  p_h5TreeWidget = new HDF5TreeWidget(filename.toStdString());
  p_h5TreeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
  p_h5TreeWidget->setSelectable(types);

  layout->addWidget(p_h5TreeWidget);
  
  QDialogButtonBox* buttonBox =
      new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));

  layout->addWidget(buttonBox);
  resize(640,480);
}

HDF5SelectionDialog::~HDF5SelectionDialog()
{}
  
std::vector<std::string> HDF5SelectionDialog::selectedItems() const
{
  int nSelectedItems =  p_h5TreeWidget->selectedItems().size();
  std::vector<std::string> result;
  for (int i = 0; i < nSelectedItems; ++i) 
  {
    result.push_back(
        p_h5TreeWidget->absoluteItemPath(
            p_h5TreeWidget->selectedItems().at(i)));
  }
  return result;
}
