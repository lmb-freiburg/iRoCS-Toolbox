/**************************************************************************
**       Title: Widget for selecting groups/datasets in an HDF5 file
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#include "HDF5SelectionDialog.hh"

#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QDialogButtonBox>

#include "MultiChannelModel.hh"
#include "ColorMap.hh"

// #define HDF5SELECTIONDIALOG_DEBUG

/*-----------------------------------------------------------------------
 *  The ColorMap selection dialog
 *-----------------------------------------------------------------------*/

std::string HDF5ColorMapSelectionDialog::selectColorMap(
    QString const &filename, QWidget *parent)
{
  HDF5ColorMapSelectionDialog* dlg =
      new HDF5ColorMapSelectionDialog(filename, parent);
  std::string result;
  if (dlg->_valid)
  {
    int exitCode = dlg->exec();
    if (exitCode == QDialog::Accepted) result = dlg->selectedItem();
  }
  dlg->setParent(NULL);
  delete dlg;
  return result;
}

HDF5ColorMapSelectionDialog::HDF5ColorMapSelectionDialog(
    QString const &filename, QWidget* parent)
        : QDialog(parent), _filename(filename), _valid(true)
{
  QVBoxLayout* layout = new QVBoxLayout;
  setLayout(layout);
  
  p_h5TreeWidget = new QTreeWidget;
  p_h5TreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
  p_h5TreeWidget->setColumnCount(5);

  QStringList headerLabels;
  headerLabels.push_back(tr("Name", "getH5ObjectDialog|Name"));
  headerLabels.push_back(tr("Structure", "getH5ObjectDialog|Structure"));
  headerLabels.push_back(tr("Type", "getH5ObjectDialog|Type"));
  headerLabels.push_back(tr("Shape", "getH5ObjectDialog|Shape"));
  headerLabels.push_back(tr("Element Size [vx/micron]",
                            "getH5ObjectDialog|ElementSize"));
  p_h5TreeWidget->setHeaderLabels(headerLabels);
  connect(p_h5TreeWidget, SIGNAL(expanded(const QModelIndex&)),
          SLOT(adjustColumnWidths()));
  connect(p_h5TreeWidget, SIGNAL(collapsed(const QModelIndex&)),
          SLOT(adjustColumnWidths()));  

  QTreeWidgetItem* root = new QTreeWidgetItem;
  root->setText(0, "/");
  root->setText(1, "");
  root->setText(2, "");
  root->setText(3, "");
  root->setText(4, "");
  root->setFlags(Qt::ItemIsEnabled);

  try
  {
    BlitzH5File inFile(filename.toStdString());
    if (!inFile.existsDataset("/position_um")) throw BlitzH5Error();
    if (inFile.existsDataset("/radius_um"))
    {
      if (inFile.existsDataset("/mitotic"))
          root->setText(1, "Annotation (Nuclei)");
      else if (inFile.existsDataset("/coefficients"))
          root->setText(1, "Annotation (Spherical Harmonic Surfaces)");
      else if (inFile.existsDataset("/length_um"))
          root->setText(1, "Annotation (Cylinders)");
      else root->setText(1, "Annotation (Spheres)");
    }
    else
    {
      if (inFile.existsDataset("/mitotic"))
          root->setText(1, "Annotation (Cells)");
      else if (inFile.existsDataset("/nElements"))
          root->setText(1, "Annotation (Surfaces)");
      else root->setText(1, "Annotation (Points)");
    }
  }
  catch (BlitzH5Error &)
  {}

  try
  {
    BlitzH5File inFile(filename.toStdString());
    atb::BSpline< blitz::TinyVector<double,3> > spline;
    spline.load(inFile, "/");
    root->setText(1, "B-Spline");
  }
  catch(BlitzH5Error &)
  {}

  try
  {
    BlitzH5File inFile(filename.toStdString());
    atb::IRoCS rct;
    rct.load(inFile, "/");
    root->setText(1, "iRoCS");
  }
  catch(BlitzH5Error &)
  {}

  try
  {
    BlitzH5File inFile(filename.toStdString());
    ShellCoordinateTransform sct;
    sct.load(inFile, "/");
    root->setText(1, "iRoCS Shell Coordinate Transform");
  }
  catch(BlitzH5Error &)
  {}

  try
  {
    BlitzH5File inFile(filename.toStdString());
    ColorMap colorMap;
    colorMap.load(inFile, "/");
    root->setText(1, "Colormap");
    root->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  }
  catch(BlitzH5Error &)
  {}

  p_h5TreeWidget->addTopLevelItem(root);

  try
  {
    BlitzH5File inFile(filename.toStdString());
    addH5Objects(root, inFile);
  }
  catch (BlitzH5Error&)
  {
    _valid = false;
    return;
  }
  root->setExpanded(true);

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
  
std::string
HDF5ColorMapSelectionDialog::absoluteGroupName(
    const QTreeWidgetItem* group) const
{
  std::string absName;
  const QTreeWidgetItem* current = group;
  while (current->text(0) != "/") 
  {
    absName = "/" + current->text(0).toStdString() + absName;
    current = current->parent();
  }
  return absName;
}

void HDF5ColorMapSelectionDialog::addH5Objects(
    QTreeWidgetItem* group, const BlitzH5File& inFile)
{
  std::string groupName = absoluteGroupName(group);
  std::vector<std::string> objects(inFile.getObjects(groupName));

  for (std::vector<std::string>::iterator it = objects.begin();
       it != objects.end(); ++it) 
  {
    if (inFile.existsGroup(groupName + "/" + *it))
    {
      QTreeWidgetItem* newGroup = new QTreeWidgetItem(group);
      newGroup->setText(0, it->c_str());
      newGroup->setText(1, "");
      newGroup->setText(2, "");
      newGroup->setText(3, "");
      newGroup->setText(4, "");
      newGroup->setFlags(Qt::ItemIsEnabled);

      try
      {
        if (!inFile.existsDataset(groupName + "/" + *it + "/position_um"))
            throw BlitzH5Error();
        if (inFile.existsDataset(groupName + "/" + *it + "/radius_um"))
        {
          if (inFile.existsDataset(groupName + "/" + *it + "/mitotic"))
              newGroup->setText(1, "Annotation (Nuclei)");
          else if (inFile.existsDataset(
                       groupName + "/" + *it + "/coefficients"))
              newGroup->setText(1, "Annotation (Spherical Harmonic Surfaces)");
          else if (inFile.existsDataset(groupName + "/" + *it + "/length_um"))
              newGroup->setText(1, "Annotation (Cylinders)");
          else newGroup->setText(1, "Annotation (Spheres)");
        }
        else
        {
          if (inFile.existsDataset(groupName + "/" + *it + "/mitotic"))
              newGroup->setText(1, "Annotation (Cells)");
          else if (inFile.existsDataset(groupName + "/" + *it + "/nElements"))
              newGroup->setText(1, "Annotation (Surfaces)");
          else newGroup->setText(1, "Annotation (Points)");
        }
      }
      catch (BlitzH5Error &)
      {}
      
      try
      {
        atb::BSpline< blitz::TinyVector<double,3> > spline;
        spline.load(inFile, groupName + "/" + *it);
        newGroup->setText(1, "B-Spline");
      }
      catch(BlitzH5Error &)
      {}

      try
      {
        atb::IRoCS rct;
        rct.load(inFile, groupName + "/" + *it);
        newGroup->setText(1, "iRoCS");
      }
      catch(BlitzH5Error &)
      {}

      try
      {
        ShellCoordinateTransform sct;
        sct.load(inFile, groupName + "/" + *it);
        newGroup->setText(1, "iRoCS Shell Coordinate Transform");
      }
      catch(BlitzH5Error &)
      {}

      try
      {
        ColorMap colorMap;
        colorMap.load(inFile, groupName + "/" + *it);
        newGroup->setText(1, "Colormap");
        newGroup->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      }
      catch(BlitzH5Error &)
      {}

      addH5Objects(newGroup, inFile);
    }
    else if (inFile.existsDataset(groupName + "/" + *it)) 
    {
      QTreeWidgetItem* newDataSet = new QTreeWidgetItem(group);
      newDataSet->setFlags(Qt::NoItemFlags);
      newDataSet->setText(0, it->c_str());

      hid_t datasetTypeId = inFile.getDatasetType(groupName + "/" + *it);

      if (H5Tget_class(datasetTypeId) == H5T_INTEGER)
      {
        newDataSet->setText(1, "Data (Indexed)");
        if (H5Tget_sign(datasetTypeId) == H5T_SGN_2)
        {
          switch (H5Tget_size(datasetTypeId))
          {
          case 1 :
            newDataSet->setText(2, "8-Bit Signed Integer");
            break;
          case 2 :
            newDataSet->setText(2, "16-Bit Signed Integer");
            break;
          case 4 :
            newDataSet->setText(2, "32-Bit Signed Integer");
            break;
          default :
            newDataSet->setText(2, "64-Bit Signed Integer");
          }
        }
        else
        {
          switch (H5Tget_size(datasetTypeId))
          {
          case 1 :
            newDataSet->setText(2, "8-Bit Unsigned Integer");
            break;
          case 2 :
            newDataSet->setText(2, "16-Bit Unsigned Integer");
            break;
          case 4 :
            newDataSet->setText(2, "32-Bit Unsigned Integer");
            break;
          default :
            newDataSet->setText(2, "64-Bit Unsigned Integer");
          }
        }
      }
      else
      {
        newDataSet->setText(1, "Data (Real)");
        switch (H5Tget_size(datasetTypeId))
        {
        case 4 :
          newDataSet->setText(2, "32-Bit Float");
        case 8 :
          newDataSet->setText(2, "64-Bit Float");
        }
      }
      H5Tclose(datasetTypeId);

      std::vector<hsize_t> extents(
          inFile.getDatasetShape(groupName + "/" + *it));
      
      if (extents.size() == 4 && extents[3] == 3) // 3D RGB
      {
        newDataSet->setText(1, "Data (RGB)");
        std::stringstream shapeStream;
        shapeStream << extents[0] << " x " << extents[1] << " x " << extents[2];
        newDataSet->setText(3, shapeStream.str().c_str());        
      }
      else
      {
        if (extents.size() == 0) newDataSet->setText(3, "scalar");
        else
        {
          std::stringstream shapeStream;
          for (size_t i = 0; i < extents.size() - 1; ++i)
              shapeStream << extents[i] << " x ";
          shapeStream << extents.back();
          newDataSet->setText(3, shapeStream.str().c_str());
        }
      }
      
      std::vector<double> elementSize(extents.size(), -1.0);
      try
      {
        inFile.readAttribute(
            elementSize, "element_size_um", groupName + "/" + *it);
        std::stringstream elSizeStream;
        for (size_t i = 0; i < elementSize.size() - 1; ++i)
            elSizeStream << elementSize[i] << " x ";
        elSizeStream << elementSize.back();
        newDataSet->setText(4, elSizeStream.str().c_str());
      }
      catch (BlitzH5Error &)
      {
        newDataSet->setText(4, tr("N/A"));
      }
    }
  } 
}

std::string HDF5ColorMapSelectionDialog::selectedItem() const
{
  if (p_h5TreeWidget->selectedItems().size() == 0) return "";
  return absoluteGroupName(p_h5TreeWidget->selectedItems().at(0));
}

void HDF5ColorMapSelectionDialog::adjustColumnWidths()
{
  for (int c = 0; c < p_h5TreeWidget->columnCount(); ++c)
      p_h5TreeWidget->resizeColumnToContents(c);
}


/*-----------------------------------------------------------------------
 *  The HDF5 Channel Selection Dialog
 *-----------------------------------------------------------------------*/ 

std::vector<std::string> HDF5SelectionDialog::selectObjects(
    const QString& filename, MultiChannelModel *model, QWidget* parent,
    ChannelSpecs::ChannelTypes types)
{
  HDF5SelectionDialog* dlg =
      new HDF5SelectionDialog(filename, model, parent, types);
  std::vector<std::string> result;
  if (dlg->_valid)
  {
    int exitCode = dlg->exec();
    if (exitCode == QDialog::Accepted) result = dlg->selectedItems();
  }
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
  if (dlg->_valid)
  {
    int exitCode = dlg->exec();
    if (exitCode == QDialog::Accepted)
        result = (dlg->selectedItems().size() > 0) ?
            dlg->selectedItems().front() : "";
  }
  dlg->setParent(0);
  delete dlg;
  return result;
}

HDF5SelectionDialog::HDF5SelectionDialog(
    const QString& filename, MultiChannelModel *model, QWidget* parent,
    ChannelSpecs::ChannelTypes types)
        : QDialog(parent), p_model(model), _filename(filename),
          _valid(true), _channelTypes(types)
{
#ifdef HDF5SELECTIONDIALOG_DEBUG
  std::cerr << "Constructing HDF5SelectionDialog() with ChannelTypes: "
            << int(types) << std::flush;
  if (types.testFlag(ChannelSpecs::Data))
      std::cerr << " Data" << std::flush;
  if (types.testFlag(ChannelSpecs::RGB))
      std::cerr << " RGB" << std::flush;
  if (types.testFlag(ChannelSpecs::Visualization))
      std::cerr << " Visualization" << std::flush;
  if (types.testFlag(ChannelSpecs::Annotation))
      std::cerr << " Annotation" << std::flush;
  if (types.testFlag(ChannelSpecs::SplineCurve))
      std::cerr << " SplineCurve" << std::flush;
  if (types.testFlag(ChannelSpecs::IRoCS))
      std::cerr << " IRoCS" << std::flush;
  if (types.testFlag(ChannelSpecs::IRoCSSCT))
      std::cerr << " IRoCSSCT" << std::flush;
  std::cerr << std::endl;
#endif

  QVBoxLayout* layout = new QVBoxLayout;
  setLayout(layout);
  
  p_h5TreeWidget = new QTreeWidget;
  p_h5TreeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
  p_h5TreeWidget->setColumnCount(5);

  QStringList headerLabels;
  headerLabels.push_back(tr("Name", "getH5ObjectDialog|Name"));
  headerLabels.push_back(tr("Structure", "getH5ObjectDialog|Structure"));
  headerLabels.push_back(tr("Type", "getH5ObjectDialog|Type"));
  headerLabels.push_back(tr("Shape", "getH5ObjectDialog|Shape"));
  headerLabels.push_back(tr("Element Size [vx/micron]",
                            "getH5ObjectDialog|ElementSize"));
  p_h5TreeWidget->setHeaderLabels(headerLabels);
  connect(p_h5TreeWidget, SIGNAL(expanded(const QModelIndex&)),
          SLOT(adjustColumnWidths()));
  connect(p_h5TreeWidget, SIGNAL(collapsed(const QModelIndex&)),
          SLOT(adjustColumnWidths()));  

  QTreeWidgetItem* root = new QTreeWidgetItem;
  root->setText(0, "/");
  root->setText(1, "");
  root->setText(2, "");
  root->setText(3, "");
  root->setText(4, "");
  root->setFlags(Qt::ItemIsEnabled);

  try
  {
    BlitzH5File inFile(filename.toStdString());
    if (!inFile.existsDataset("/position_um")) throw BlitzH5Error();
    if (_channelTypes.testFlag(ChannelSpecs::Annotation))
        root->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    if (inFile.existsDataset("/radius_um"))
    {
      if (inFile.existsDataset("/mitotic"))
          root->setText(1, "Annotation (Nuclei)");
      else if (inFile.existsDataset("/coefficients"))
          root->setText(1, "Annotation (Spherical Harmonic Surfaces)");
      else if (inFile.existsDataset("/length_um"))
          root->setText(1, "Annotation (Cylinders)");
      else root->setText(1, "Annotation (Spheres)");
    }
    else
    {
      if (inFile.existsDataset("/mitotic"))
          root->setText(1, "Annotation (Cells)");
      else if (inFile.existsDataset("/nElements"))
          root->setText(1, "Annotation (Surfaces)");
      else root->setText(1, "Annotation (Points)");
    }
  }
  catch (BlitzH5Error &)
  {}

  try
  {
    BlitzH5File inFile(filename.toStdString());
    atb::BSpline< blitz::TinyVector<double,3> > spline;
    spline.load(inFile, "/");
    root->setText(1, "B-Spline");
    if (_channelTypes.testFlag(ChannelSpecs::SplineCurve))
        root->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  }
  catch(BlitzH5Error &)
  {}

  try
  {
    BlitzH5File inFile(filename.toStdString());
    atb::IRoCS rct;
    rct.load(inFile, "/");
    root->setText(1, "iRoCS");
    if (_channelTypes.testFlag(ChannelSpecs::IRoCS))
        root->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  }
  catch(BlitzH5Error &)
  {}

  try
  {
    BlitzH5File inFile(filename.toStdString());
    ShellCoordinateTransform sct;
    sct.load(inFile, "/");
    root->setText(1, "iRoCS Shell Coordinate Transform");
    if (_channelTypes.testFlag(ChannelSpecs::IRoCSSCT))
        root->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  }
  catch(BlitzH5Error &)
  {}

  p_h5TreeWidget->addTopLevelItem(root);

  try
  {
    BlitzH5File inFile(filename.toStdString());
    addH5Objects(root, inFile);
  }
  catch (BlitzH5Error&)
  {
    _valid = false;
    return;
  }
  root->setExpanded(true);

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
  
std::string
HDF5SelectionDialog::absoluteGroupName(const QTreeWidgetItem* group) const
{
  std::string absName;
  const QTreeWidgetItem* current = group;
  while (current->text(0) != "/") 
  {
    absName = "/" + current->text(0).toStdString() + absName;
    current = current->parent();
  }
  return absName;
}

void HDF5SelectionDialog::addH5Objects(
    QTreeWidgetItem* group, const BlitzH5File& inFile)
{
  std::string groupName = absoluteGroupName(group);

#ifdef HDF5SELECTIONDIALOG_DEBUG
  std::cerr << "Processing HDF5SelectionDialog::addH5Objects(" << groupName
            << ")" << std::endl;
#endif

  std::vector<std::string> objects;
  try
  {
    objects = inFile.getObjects(groupName);
  }
  catch (BlitzH5Error &e)
  {
#ifdef HDF5SELECTIONDIALOG_DEBUG
    std::cerr << "  Error while reading object list: " << e.what()
              << std::endl;
#endif
    throw;
  }

#ifdef HDF5SELECTIONDIALOG_DEBUG
  std::cerr << "  Found " << objects.size() << " objects" << std::endl;
#endif
  for (std::vector<std::string>::iterator it = objects.begin();
       it != objects.end(); ++it) 
  {
#ifdef HDF5SELECTIONDIALOG_DEBUG
    std::cerr << "  Processing object '" << *it << "'" << std::endl;
#endif
    if (inFile.existsGroup(groupName + "/" + *it))
    {
      QTreeWidgetItem* newGroup = new QTreeWidgetItem(group);
      newGroup->setText(0, it->c_str());
      newGroup->setText(1, "");
      newGroup->setText(2, "");
      newGroup->setText(3, "");
      newGroup->setText(4, "");
      newGroup->setFlags(Qt::ItemIsEnabled);

#ifdef HDF5SELECTIONDIALOG_DEBUG
      std::cerr << "Flags for group '" << groupName << "/" << *it
                << "' set to ItemIsEnabled" << std::endl;
#endif

      try
      {
        if (!inFile.existsDataset(groupName + "/" + *it + "/position_um"))
            throw BlitzH5Error();
        if (_channelTypes.testFlag(ChannelSpecs::Annotation))
            newGroup->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        if (inFile.existsDataset(groupName + "/" + *it + "/radius_um"))
        {
          if (inFile.existsDataset(groupName + "/" + *it + "/mitotic"))
              newGroup->setText(1, "Annotation (Nuclei)");
          else if (inFile.existsDataset(
                       groupName + "/" + *it + "/coefficients"))
              newGroup->setText(1, "Annotation (Spherical Harmonic Surfaces)");
          else if (inFile.existsDataset(groupName + "/" + *it + "/length_um"))
              newGroup->setText(1, "Annotation (Cylinders)");
          else newGroup->setText(1, "Annotation (Spheres)");
        }
        else
        {
          if (inFile.existsDataset(groupName + "/" + *it + "/mitotic"))
              newGroup->setText(1, "Annotation (Cells)");
          else if (inFile.existsDataset(groupName + "/" + *it + "/nElements"))
              newGroup->setText(1, "Annotation (Surfaces)");
          else newGroup->setText(1, "Annotation (Points)");
        }
      }
      catch (BlitzH5Error &)
      {}
      
      try
      {
        atb::BSpline< blitz::TinyVector<double,3> > spline;
        spline.load(inFile, groupName + "/" + *it);
        if (_channelTypes.testFlag(ChannelSpecs::SplineCurve))
        {
          newGroup->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

#ifdef HDF5SELECTIONDIALOG_DEBUG
          std::cerr << "Flags for group '" << groupName << "/" << *it
                    << "' set to ItemIsEnabled | ItemIsSelectable"
                    << std::endl;
#endif

        }
        newGroup->setText(1, "B-Spline");
      }
      catch(BlitzH5Error &)
      {}

      try
      {
        atb::IRoCS rct;
        rct.load(inFile, groupName + "/" + *it);
        if (_channelTypes.testFlag(ChannelSpecs::IRoCS))
        {
          newGroup->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

#ifdef HDF5SELECTIONDIALOG_DEBUG
          std::cerr << "Flags for group '" << groupName << "/" << *it
                    << "' set to ItemIsEnabled | ItemIsSelectable"
                    << std::endl;
#endif

        }
        newGroup->setText(1, "iRoCS");
      }
      catch(BlitzH5Error &)
      {}

      try
      {
        ShellCoordinateTransform sct;
        sct.load(inFile, groupName + "/" + *it);
        if (_channelTypes.testFlag(ChannelSpecs::IRoCSSCT))
        {
          newGroup->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

#ifdef HDF5SELECTIONDIALOG_DEBUG
          std::cerr << "Flags for group '" << groupName << "/" << *it
                    << "' set to ItemIsEnabled | ItemIsSelectable"
                    << std::endl;
#endif
        }
        newGroup->setText(1, "iRoCS Shell Coordinate Transform");
      }
      catch(BlitzH5Error &)
      {}

#ifdef HDF5SELECTIONDIALOG_DEBUG
      std::cerr << "   Adding group " << *it
                << " - enabled = "
                << newGroup->flags().testFlag(Qt::ItemIsEnabled)
                << " - selectable = "
                << newGroup->flags().testFlag(Qt::ItemIsSelectable)
                << std::endl;
#endif
      
      addH5Objects(newGroup, inFile);

    }
    else if (inFile.existsDataset(groupName + "/" + *it)) 
    {
      QTreeWidgetItem* newDataSet = new QTreeWidgetItem(group);
      newDataSet->setFlags(Qt::NoItemFlags);

#ifdef HDF5SELECTIONDIALOG_DEBUG
      std::cerr << "Flags set to NoItemFlags" << std::endl;
#endif

      newDataSet->setText(0, it->c_str());

      hid_t datasetTypeId = inFile.getDatasetType(groupName + "/" + *it);

      if (H5Tget_class(datasetTypeId) == H5T_INTEGER)
      {
        newDataSet->setText(1, "Data (Indexed)");
        if (H5Tget_sign(datasetTypeId) == H5T_SGN_2)
        {
          switch (H5Tget_size(datasetTypeId))
          {
          case 1 :
            newDataSet->setText(2, "8-Bit Signed Integer");
            if (_channelTypes.testFlag(ChannelSpecs::Visualization))
            { 
              newDataSet->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
              
#ifdef HDF5SELECTIONDIALOG_DEBUG
              std::cerr << "Flags set to ItemIsEnabled | ItemIsSelectable"
                        << std::endl;
#endif
              
            }
            break;
          case 2 :
            newDataSet->setText(2, "16-Bit Signed Integer");
            if (_channelTypes.testFlag(ChannelSpecs::Visualization))
            {
              newDataSet->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
              
#ifdef HDF5SELECTIONDIALOG_DEBUG
              std::cerr << "Flags set to ItemIsEnabled | ItemIsSelectable"
                        << std::endl;
#endif
              
            }
            break;
          case 4 :
            newDataSet->setText(2, "32-Bit Signed Integer");
            if (_channelTypes.testFlag(ChannelSpecs::Visualization))
            {
              newDataSet->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
              
#ifdef HDF5SELECTIONDIALOG_DEBUG
              std::cerr << "Flags set to ItemIsEnabled | ItemIsSelectable"
                        << std::endl;
#endif
              
            }
            break;
          default :
            newDataSet->setText(2, "64-Bit Signed Integer");
          }
        }
        else
        {
          switch (H5Tget_size(datasetTypeId))
          {
          case 1 :
            newDataSet->setText(2, "8-Bit Unsigned Integer");
            break;
          case 2 :
            newDataSet->setText(2, "16-Bit Unsigned Integer");
            break;
          case 4 :
            newDataSet->setText(2, "32-Bit Unsigned Integer");
            break;
          default :
            newDataSet->setText(2, "64-Bit Unsigned Integer");
          }
          if (_channelTypes.testFlag(ChannelSpecs::Visualization))
          {
            newDataSet->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

#ifdef HDF5SELECTIONDIALOG_DEBUG
            std::cerr << "Flags set to ItemIsEnabled | ItemIsSelectable"
                      << std::endl;
#endif

          }
        }
      }
      else
      {
        newDataSet->setText(1, "Data (Real)");
        switch (H5Tget_size(datasetTypeId))
        {
        case 4 :
          newDataSet->setText(2, "32-Bit Float");
        case 8 :
          newDataSet->setText(2, "64-Bit Float");
        }
        if (_channelTypes.testFlag(ChannelSpecs::Data))
        {
          newDataSet->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

#ifdef HDF5SELECTIONDIALOG_DEBUG
          std::cerr << "Flags set to ItemIsEnabled | ItemIsSelectable"
                    << std::endl;
#endif

        }
      }
      H5Tclose(datasetTypeId);

      std::vector<hsize_t> extents(
          inFile.getDatasetShape(groupName + "/" + *it));
      if (!(extents.size() == 3 || (extents.size() == 4 && extents[3] == 3)))
      {
        newDataSet->setFlags(Qt::NoItemFlags);

#ifdef HDF5SELECTIONDIALOG_DEBUG
        std::cerr << "Flags set to NoItemFlags" << std::endl;
#endif

      }
      
      if (extents.size() == 4 && extents[3] == 3) // 3D RGB
      {
        newDataSet->setText(1, "Data (RGB)");
        std::stringstream shapeStream;
        shapeStream << extents[0] << " x " << extents[1] << " x " << extents[2];
        newDataSet->setText(3, shapeStream.str().c_str());        
      }
      else
      {
        if (extents.size() == 0) newDataSet->setText(3, "scalar");
        else
        {
          std::stringstream shapeStream;
          for (size_t i = 0; i < extents.size() - 1; ++i)
              shapeStream << extents[i] << " x ";
          shapeStream << extents.back();
          newDataSet->setText(3, shapeStream.str().c_str());
        }
      }
      
      std::vector<double> elementSize(extents.size(), -1.0);
      try
      {
        inFile.readAttribute(
            elementSize, "element_size_um", groupName + "/" + *it);
        std::stringstream elSizeStream;
        for (size_t i = 0; i < elementSize.size() - 1; ++i)
            elSizeStream << elementSize[i] << " x ";
        elSizeStream << elementSize.back();
        newDataSet->setText(4, elSizeStream.str().c_str());
      }
      catch (BlitzH5Error &)
      {
        newDataSet->setText(4, tr("N/A"));
      }
      if (p_model != NULL)
      {
        bool compatible =
            (extents.size() == 3 ||
             (extents.size() == 4 && extents[3] == 3));
        if (!compatible)
        {
          newDataSet->setFlags(Qt::NoItemFlags);

#ifdef HDF5SELECTIONDIALOG_DEBUG
          std::cerr << "Flags set to NoItemFlags" << std::endl;
#endif

        }
      }

#ifdef HDF5SELECTIONDIALOG_DEBUG
      std::cerr << "   Adding dataset " << *it
                << " - enabled = "
                << newDataSet->flags().testFlag(Qt::ItemIsEnabled)
                << " - selectable = "
                << newDataSet->flags().testFlag(Qt::ItemIsSelectable)
                << std::endl;
#endif

    }
  } 
}

std::vector<std::string> HDF5SelectionDialog::selectedItems() const
{
  int nSelectedItems =  p_h5TreeWidget->selectedItems().size();
  std::vector<std::string> result;
  for (int i = 0; i < nSelectedItems; ++i) 
  {
    result.push_back(absoluteGroupName(p_h5TreeWidget->selectedItems().at(i)));
  }
  return result;
}

void HDF5SelectionDialog::adjustColumnWidths()
{
  for (int c = 0; c < p_h5TreeWidget->columnCount(); ++c)
      p_h5TreeWidget->resizeColumnToContents(c);
}
