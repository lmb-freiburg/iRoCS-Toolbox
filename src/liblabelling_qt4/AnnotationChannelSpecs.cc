/**************************************************************************
**       Title: Channel Specs for Annotation Channels
**    $RCSfile$
**   $Revision: 3883 $ $Name$
**       $Date: 2008-01-21 14:28:13 +0100 (Mon, 21 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    Channel Descriptor for one channel of multi-class annotation
**
**************************************************************************/

#include "AnnotationChannelSpecs.hh"

#include <QtGui/QComboBox>
#include <QtGui/QStackedWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtCore/QThread>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QInputDialog>
#include <QtGui/QApplication>
#include <QtGui/QPushButton>

#include "DoubleControlElement.hh"
#include "BoolControlElement.hh"

#include "PointMarker.hh"
#include "SphereMarker.hh"
#include "NucleusMarker.hh"
#include "SHSurfaceMarker.hh"
#include "CylinderMarker.hh"
#include "SurfaceMarker.hh"
#include "CellMarker.hh"
#include "MarkerRenderer.hh"
#include "MarkerOrthoViewRenderer.hh"
#include "OrthoViewWidget.hh"
#include "AnnotationChannelSpecsOrthoViewRenderer.hh"
#include "AnnotationChannelSpecsOpenGlRenderingViewRenderer.hh"
#include "MultiChannelModel.hh"
#include "IRoCSChannelSpecs.hh"
#include "IRoCSSCTChannelSpecs.hh"

// #include <blitz/tinyvec-et.h>

AnnotationChannelSpecs::AnnotationChannelSpecs(
    Marker::MarkerType markerType, MultiChannelModel *model) 
        : ChannelSpecs(model), _markerType(markerType), _selectedMarker(NULL)
{
  disconnect(
      p_alphaControl, SIGNAL(valueChanged()), this, SLOT(emitUpdateRequest()));
  disconnect(
      p_visibleControl, SIGNAL(valueChanged()),
      this, SLOT(emitUpdateRequest()));
  connect(p_alphaControl, SIGNAL(valueChanged()), SIGNAL(viewChanged()));
  connect(p_visibleControl, SIGNAL(valueChanged()), SIGNAL(viewChanged()));

  QHBoxLayout *viewTypeLayout = new QHBoxLayout;
  QLabel *viewTypeLabel = new QLabel(tr("View Type:"));
  viewTypeLayout->addWidget(viewTypeLabel);
  p_viewTypeComboBox = new QComboBox;
  p_viewTypeComboBox->setSizePolicy(
      QSizePolicy::Expanding, QSizePolicy::Fixed);
  p_viewTypeComboBox->addItem(tr("Label"));
  p_viewTypeComboBox->addItem(tr("Predicted Label"));
  p_viewTypeComboBox->addItem(tr("Confusion"));
  p_viewTypeComboBox->addItem(tr("Random"));
  connect(p_viewTypeComboBox, SIGNAL(currentIndexChanged(int)),
          SIGNAL(viewChanged()));
  viewTypeLayout->addWidget(p_viewTypeComboBox);
  p_channelControlLayout->addLayout(viewTypeLayout);

  QHBoxLayout *operationLayout = new QHBoxLayout;
  QLabel *operationLabel = new QLabel(tr("Operation:"));
  operationLayout->addWidget(operationLabel);
  p_operationComboBox = new QComboBox;
  p_operationComboBox->setSizePolicy(
      QSizePolicy::Expanding, QSizePolicy::Fixed);
  p_operationComboBox->addItem(tr("Add marker"));
  p_operationComboBox->addItem(tr("Label marker"));
  p_operationComboBox->addItem(tr("Edit marker"));
  p_operationComboBox->addItem(tr("Delete marker"));
  operationLayout->addWidget(p_operationComboBox);
  p_channelControlLayout->addLayout(operationLayout);

  p_coordinateUpdateButton = new QPushButton(tr("Update coordinates"));
  p_coordinateUpdateButton->setEnabled(
      p_model != NULL && p_model->nChannels(
          ChannelSpecs::IRoCS | ChannelSpecs::IRoCSSCT) != 0);
  connect(p_coordinateUpdateButton, SIGNAL(clicked()),
          SLOT(updateCoordinates()));
  p_channelControlLayout->addWidget(p_coordinateUpdateButton);

  switch (markerType) 
  {
  case Marker::Point :
    p_presetWidget = new PointMarkerPresetWidget(this);
    p_controlWidget = new PointMarkerControlWidget(this);
    break;
  case Marker::Sphere :
    p_presetWidget = new SphereMarkerPresetWidget(this);
    p_controlWidget = new SphereMarkerControlWidget(this);
    break;
  case Marker::Cylinder :
    p_presetWidget = new CylinderMarkerPresetWidget(this);
    p_controlWidget = new CylinderMarkerControlWidget(this);
    break;
  case Marker::SHSurface :
    p_presetWidget = new SHSurfaceMarkerPresetWidget(this);
    p_controlWidget = new SHSurfaceMarkerControlWidget(this);
    break;
  case Marker::Nucleus :
    p_presetWidget = new NucleusMarkerPresetWidget(this);
    p_controlWidget = new NucleusMarkerControlWidget(this);
    break;
  case Marker::Surface :
    p_presetWidget = new SurfaceMarkerPresetWidget(this);
    p_controlWidget = new SurfaceMarkerControlWidget(this);
    break;
  case Marker::Cell :
    p_presetWidget = new CellMarkerPresetWidget(this);
    p_controlWidget = new CellMarkerControlWidget(this);
    break;
  default:
    std::cerr << "Error in " << __FILE__ << ":" << __LINE__
              << ": implementation missing for the selected MarkerType"
              << std::endl;
    throw std::runtime_error("Missing Implementation error");
  }

  // Setup default root colormap (jet)
  setColorForLabel(-1, blitz::TinyVector<float,3>(1.0f, 1.0f, 1.0f));
  setColorForLabel(0, blitz::TinyVector<float,3>(0.0f, 0.0f, 0.0f));
  setColorForLabel(1, blitz::TinyVector<float,3>(0.0f, 0.0f, 1.0f));
  setColorForLabel(2, blitz::TinyVector<float,3>(0.0f, 1.0f, 1.0f));
  setColorForLabel(3, blitz::TinyVector<float,3>(0.0f, 1.0f, 0.0f));
  setColorForLabel(4, blitz::TinyVector<float,3>(1.0f, 1.0f, 0.0f));
  setColorForLabel(5, blitz::TinyVector<float,3>(1.0f, 0.0f, 0.0f));
  setColorForLabel(
      6, blitz::TinyVector<float,3>(2.0f / 3.0f, 1.0f / 3.0f, 0.0f));

  p_channelControlLayout->addWidget(p_presetWidget);
  connect(p_presetWidget, SIGNAL(colormapChanged()), SIGNAL(viewChanged()));
  p_channelControlLayout->addWidget(p_controlWidget);
  p_controlWidget->setSizePolicy(
      QSizePolicy::Expanding, QSizePolicy::Expanding);
  connect(p_controlWidget, SIGNAL(contentsChanged()),
          SLOT(updateSelectedMarker()));
  connect(this, SIGNAL(featureSizeChanged(int)),
          p_controlWidget, SLOT(resizeFeatures(int)));
  p_controlWidget->setEnabled(false);
  
  p_channelControlLayout->addStretch(1);
}

AnnotationChannelSpecs::~AnnotationChannelSpecs()
{
  for (iterator it = _markers.begin(); it != _markers.end(); ++it) delete *it;
}

ChannelSpecs::ChannelType AnnotationChannelSpecs::channelType() const
{
  return ChannelSpecs::Annotation;
}

float AnnotationChannelSpecs::valueAt(
    blitz::TinyVector<double,3> const &positionUm) const
{
  for (const_iterator it = _markers.begin(); it != _markers.end(); ++it)
  {
    if (blitz::all((*it)->positionUm() == positionUm))
        return static_cast<float>((*it)->label());
  }
  return 0.0f;
}

std::string AnnotationChannelSpecs::stringValueAt(
    blitz::TinyVector<double,3> const &positionUm) const
{
  for (const_iterator it = _markers.begin(); it != _markers.end(); ++it)
  {
    if (blitz::all((*it)->positionUm() == positionUm))
    {
      std::stringstream os;
      os << (*it)->label();
      return os.str();
    }
  }
  return "0";
}

int AnnotationChannelSpecs::selectedViewType() const
{
  return p_viewTypeComboBox->currentIndex();
}

AnnotationChannelSpecs::Operation
AnnotationChannelSpecs::selectedOperation() const
{
  switch (p_operationComboBox->currentIndex())
  {
  case 0 :
    return AddMarker;
  case 1:
    return LabelMarker;
  case 2:
    return EditMarker;
  case 3:
    return DeleteMarker;
  default:
    std::cerr << __FILE__ << ":" << __LINE__
              << ": Missing implementation for operation "
              << p_operationComboBox->currentIndex() << std::endl;
  }
  return UnknownOperation;
}

MarkerPresetWidget *AnnotationChannelSpecs::markerPresetWidget()
{
  return p_presetWidget;
}

MarkerControlWidget *AnnotationChannelSpecs::markerControlWidget()
{
  return p_controlWidget;
}

ChannelSpecsRenderer *AnnotationChannelSpecs::addRenderer(ViewWidget *view)
{
  if (_renderers.find(view) != _renderers.end())
  {
    std::cout << "AnnotationChannelSpecs::addRenderer(): WARNING: The channel "
              << "already has an associated renderer" << std::endl;
    return _renderers[view];
  }
  switch (view->viewType())
  {
  case ViewWidget::OrthoView:
    _renderers[view] = new AnnotationChannelSpecsOrthoViewRenderer(
        this, static_cast<OrthoViewWidget*>(view));
    break;
  case ViewWidget::OpenGlRenderingView:
    _renderers[view] = new AnnotationChannelSpecsOpenGlRenderingViewRenderer(
        this, reinterpret_cast<OpenGlRenderingViewWidget*>(view));
    break;
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": Implementation missing"
              << std::endl;
    break;
  }
  for (iterator it = _markers.begin(); it != _markers.end(); ++it)
  {
    MarkerRenderer *renderer = (*it)->addRenderer(view);
    renderer->updateCache();
  }
  return _renderers[view];
}

Marker *AnnotationChannelSpecs::closestMarker(
    blitz::TinyVector<double,3> const &positionUm, bool intersecting)
{
  if (_markers.size() == 0) return NULL;
  
  Marker *res = NULL;
  double minSqDistance = std::numeric_limits<double>::infinity();
  for (const_iterator it = _markers.begin(); it != _markers.end(); ++it)
  {
    if (intersecting && !(*it)->occupiesPositionUm(positionUm)) continue;
    double sqDistance = blitz::dot(
        positionUm - (*it)->positionUm(), positionUm - (*it)->positionUm());
    if (sqDistance < minSqDistance)
    {
      minSqDistance = sqDistance;
      res = *it;
    }
  }
  return res;
}

Marker *AnnotationChannelSpecs::addMarker(
    blitz::TinyVector<double,3> const &positionUm) 
{
  Marker *marker = NULL;
  switch(_markerType) 
  {
  case Marker::Point : 
    marker = new PointMarker(positionUm);
    break;
  case Marker::Sphere :
    marker = new SphereMarker(positionUm);
    break;
  case Marker::Cylinder :
    marker = new CylinderMarker(positionUm);
    break;
  case Marker::SHSurface :
    marker = new SHSurfaceMarker(positionUm);
    break;
  case Marker::Nucleus :
    marker = new NucleusMarker(positionUm);
    break;
  case Marker::Surface :
    marker = new SurfaceMarker(positionUm);
    break;
  case Marker::Cell :
    marker = new CellMarker(positionUm);
    break;
  default:
    std::cerr << "Error in " << __FILE__ << ":" << __LINE__
              << ": implementation of addMarker() missing for the selected "
              << "MarkerType" << std::endl;
    throw std::runtime_error("Missing Implementation error");
  }
  addMarker(marker);
  p_presetWidget->getValues(marker);
  return marker;
}

void AnnotationChannelSpecs::addMarker(Marker *marker) 
{
  if (marker == NULL) return;

  marker->moveToThread(qApp->thread());

  if (_markers.size() != 0 &&
      marker->_features.size() != _markers.front()->_features.size())
      marker->_features.resize(_markers.front()->_features.size());
  if (_colorMap.find(marker->label()) == _colorMap.end()) 
  {
    _colorMap.insert(
        std::pair< int, blitz::TinyVector<float,3> >(
            marker->label(), blitz::TinyVector<float,3>(1, 1, 1)));
    _alphaMap.insert(std::pair<int,float>(marker->label(), 1.0));
  }
  _markers.push_back(marker);
  marker->setChannel(this);

  if (p_model != NULL)
  {
    for (std::vector<ViewWidget*>::const_iterator it = p_model->viewsBegin();
         it != p_model->viewsEnd(); ++it)
    {
      MarkerRenderer *renderer = marker->addRenderer(*it);
      renderer->updateCache();
    }
  }

  blitz::TinyVector<double,3> lb(_lowerBoundUm), ub(_upperBoundUm);
  for (int d = 0; d < 3; ++d)
  {
    if (marker->boundingBoxLowerBoundUm()(d) < lb(d))
        lb(d) = marker->boundingBoxLowerBoundUm()(d);
    if (marker->boundingBoxUpperBoundUm()(d) > ub(d))
        ub(d) = marker->boundingBoxUpperBoundUm()(d);
  }
  if (blitz::any(lb != _lowerBoundUm || ub != _upperBoundUm))
  {
#ifdef DEBUG
    std::cerr << "Bounding box of annotation channel changed from ("
              << _lowerBoundUm << " - " << _upperBoundUm << ") to ("
              << lb << " - " << ub << ")" << std::endl;
#endif
    _lowerBoundUm = lb;
    _upperBoundUm = ub;
    emit boundingBoxChanged(_lowerBoundUm, _upperBoundUm);
  }

  if (p_model != NULL) p_model->setModified(true);
}

void AnnotationChannelSpecs::labelMarker(Marker *marker)
{
  if (marker == NULL) return;

  bool oldUpdatesEnabledState = marker->updatesEnabled();
  marker->setUpdatesEnabled(false);
  p_presetWidget->getValues(marker);
  if (qApp->thread() == QThread::currentThread() &&
      marker == _selectedMarker) p_controlWidget->setValues(marker);
  marker->setUpdatesEnabled(oldUpdatesEnabledState);
  marker->update();
  if (p_model != NULL) p_model->setModified(true);
}

void AnnotationChannelSpecs::removeMarker(Marker *marker)
{
  if (marker == NULL) return;

  iterator it = _markers.begin();
  while (*it != marker) ++it;
  if (it != _markers.end()) _markers.erase(it);
  if (marker == _selectedMarker) selectMarker(NULL);
  delete marker;
  if (p_model != NULL) p_model->setModified(true);
  updateBoundingBox();
  emit viewChanged();
}

void AnnotationChannelSpecs::selectMarker(Marker *marker)
{
  if (marker == _selectedMarker) return;

  _selectedMarker = marker;

  p_controlWidget->setEnabled(_selectedMarker != NULL);

  if (_selectedMarker != NULL && qApp->thread() == QThread::currentThread())
      p_controlWidget->setValues(marker);
  emit viewChanged();
}

Marker *AnnotationChannelSpecs::selectedMarker() 
{
  return _selectedMarker;
}

Marker const *AnnotationChannelSpecs::selectedMarker() const
{
  return _selectedMarker;
}

Marker::MarkerType AnnotationChannelSpecs::markerType() const 
{
  return _markerType;
}

void AnnotationChannelSpecs::setColorForLabel(
    int label, blitz::TinyVector<float,3> const &color)
{
  std::map< int,blitz::TinyVector<float,3> >::iterator it =
      _colorMap.find(label);
  if (it == _colorMap.end())
      _colorMap.insert(
          std::pair< int, blitz::TinyVector<float,3> >(label, color));
  else
  {
    if (blitz::all(it->second == color)) return;
    it->second = color;
  }
  if (p_model != NULL) p_model->setModified(true);
}

blitz::TinyVector<float,3>
AnnotationChannelSpecs::colorForLabel(int label) const
{
  if (_colorMap.find(label) != _colorMap.end())
      return _colorMap.find(label)->second;
  return blitz::TinyVector<float,3>(1.0f);
}

std::map< int,blitz::TinyVector<float,3> > &AnnotationChannelSpecs::colorMap()
{
  return _colorMap;
}

void AnnotationChannelSpecs::setAlphaForLabel(int label, float alpha)
{
  std::map< int,float >::iterator it = _alphaMap.find(label);
  if (it == _alphaMap.end())
      _alphaMap.insert(std::pair< int, float >(label, alpha));
  else
  {
    if (it->second == alpha) return;
    it->second = alpha;
  }
  if (p_model != NULL) p_model->setModified(true);
}

float AnnotationChannelSpecs::alphaForLabel(int label) const
{
  if (_alphaMap.find(label) != _alphaMap.end())
      return _alphaMap.find(label)->second;
  return 1.0f;
}

std::map< int,float > &AnnotationChannelSpecs::alphaMap()
{
  return _alphaMap;
}

void AnnotationChannelSpecs::resizeFeatures(size_t newSize)
{
  bool anyFeatureResized = false;
  for (iterator it = _markers.begin(); it != _markers.end(); ++it)
  {
    if ((*it)->_features.size() == newSize) continue;
    (*it)->_features.resize(newSize);
    anyFeatureResized = true;
  }
  if (!anyFeatureResized) return;
  if (p_model != NULL) p_model->setModified(true);
  emit featureSizeChanged(static_cast<int>(newSize));
}

const std::vector<Marker*> &AnnotationChannelSpecs::markers() const
{
  return _markers;
}

AnnotationChannelSpecs::const_iterator AnnotationChannelSpecs::begin() const
{
  return _markers.begin();
}

AnnotationChannelSpecs::iterator AnnotationChannelSpecs::begin()
{
  return _markers.begin();
}

AnnotationChannelSpecs::const_iterator AnnotationChannelSpecs::end() const
{
  return _markers.end();
}

AnnotationChannelSpecs::iterator AnnotationChannelSpecs::end()
{
  return _markers.end();
}

Marker const *AnnotationChannelSpecs::operator[](size_t index) const
{
  return _markers[index];
}

Marker *AnnotationChannelSpecs::operator[](size_t index)
{
  return _markers[index];
}

blitz::TinyVector<double,3> const
&AnnotationChannelSpecs::elementSizeUm() const
{
  if (p_model != NULL) return p_model->elementSizeUm();
  else return this->_elementSizeUm;
}

HDF5DataIO::RetVal AnnotationChannelSpecs::load(
    std::string const &fileName, iRoCS::ProgressReporter *pr)
{
  try
  {
    BlitzH5File inFile(fileName);
    load(inFile, pr);
  }
  catch (BlitzH5Error &e)
  {
    QMessageBox::critical(
        qApp->activeWindow(), tr("Annotation Channel read error"),
        tr("Error while loading annotation channel '%1:%2': %3").arg(
            fileName.c_str()).arg(name().c_str()).arg(e.what()));
    return HDF5DataIO::ReadError;
  }
  catch (std::exception &e)
  {
    QMessageBox::critical(
        qApp->activeWindow(), tr("Annotation Channel read error"),
        tr("Error while loading annotation channel '%1:%2': %3").arg(
            fileName.c_str()).arg(name().c_str()).arg(e.what()));
    return HDF5DataIO::ReadError;
  }
  return HDF5DataIO::Ok;
}

void AnnotationChannelSpecs::load(
    BlitzH5File const &inFile, iRoCS::ProgressReporter *pr)
{
  bool enable = updatesEnabled();
  if (enable) setUpdatesEnabled(false);
  try
  {
    inFile.readAttribute(_originalTransformation, "transformation", name());
    setTransformation(_originalTransformation);
  }
  catch (BlitzH5Error &e)
  {
    std::cerr << "Warning: Could not read transformation for "
              << "AnnotationChannel '" << name()
              << "'. Assuming identity transformation" << std::endl;
    _originalTransformation = atb::traits< blitz::TinyMatrix<double,4,4> >::one;
  }
  switch(_markerType)
  {
  case Marker::Point :
    PointMarker::load(this, inFile, name(), pr);
    break;
  case Marker::Sphere :
    SphereMarker::load(this, inFile, name(), pr);
    break;
  case Marker::Nucleus :
    NucleusMarker::load(this, inFile, name(), pr);
    break;
  case Marker::Cylinder :
    CylinderMarker::load(this, inFile, name(), pr);
    break;
  case Marker::SHSurface :
    SHSurfaceMarker::load(this, inFile, name(), pr);
    break;
  case Marker::Surface :
    SurfaceMarker::load(this, inFile, name(), pr);
    break;
  case Marker::Cell :
    CellMarker::load(this, inFile, name(), pr);
    break;
  default :
    std::cerr << "Error in " << __FILE__ << ":" << __LINE__
              << ": implementation missing for the selected MarkerType"
              << std::endl;
    throw std::runtime_error("Missing Implementation error");
  }
  updateBoundingBox();

  // Restore old updatesEnabled state
  setUpdatesEnabled(enable);
  update();
}

HDF5DataIO::RetVal AnnotationChannelSpecs::save(
    std::string const &fileName, iRoCS::ProgressReporter *pr) const
{
  try
  {
    BlitzH5File outFile(fileName, BlitzH5File::WriteOrNew);
    save(outFile, pr);
  }
  catch (BlitzH5Error &e)
  {
    QMessageBox::critical(
        qApp->activeWindow(), tr("Annotation Channel write error"),
        tr("Error while saving annotation channel '%1:%2': %3").arg(
            fileName.c_str()).arg(name().c_str()).arg(e.what()));
    return HDF5DataIO::WriteError;
  }
  catch (std::exception &e)
  {
    QMessageBox::critical(
        qApp->activeWindow(), tr("Annotation Channel write error"),
        tr("Error while saving annotation channel '%1:%2': %3").arg(
            fileName.c_str()).arg(name().c_str()).arg(e.what()));
    return HDF5DataIO::WriteError;
  }
  return HDF5DataIO::Ok;
}

void AnnotationChannelSpecs::save(
    BlitzH5File &outFile, iRoCS::ProgressReporter *pr) const
{
  outFile.writeAttribute(_transformation, "transformation", name());
  switch(_markerType)
  {
  case Marker::Point :
    PointMarker::save(this, outFile, name(), pr);
    break;
  case Marker::Sphere :
    SphereMarker::save(this, outFile, name(), pr);
    break;
  case Marker::Nucleus :
    NucleusMarker::save(this, outFile, name(), pr);
    break;
  case Marker::Cylinder :
    CylinderMarker::save(this, outFile, name(), pr);
    break;
  case Marker::SHSurface :
    SHSurfaceMarker::save(this, outFile, name(), pr);
    break;
  case Marker::Surface :
    SurfaceMarker::save(this, outFile, name(), pr);
    break;
  case Marker::Cell :
    CellMarker::save(this, outFile, name(), pr);
    break;
  default :
    std::cerr << "Error in " << __FILE__ << ":" << __LINE__
              << ": implementation missing for the selected MarkerType"
              << std::endl;
    throw std::runtime_error("Missing Implementation error");
  }
}

void AnnotationChannelSpecs::saveCSV(std::string const &fileName) const
{
  if (_markers.size() == 0)
  {
    QMessageBox::critical(
        qApp->activeWindow(), tr("CSV annotation export error"),
        tr("The channel '%1' does not contain markers. Nothing to save.").arg(
            name().c_str()));
    return;    
  }

 try
  {
    // Write header
    std::ofstream outFile(fileName.c_str(), std::ios::trunc);
    _markers[0]->writeCSVHeader(outFile);
    outFile << "\n";
    
    // Write markers
    for (size_t i = 0; i < _markers.size(); ++i)
    {
      _markers[i]->saveCSV(outFile);
      outFile << "\n";
    }
  }
  catch (std::exception &e)
  {
    QMessageBox::critical(
        qApp->activeWindow(), tr("CSV annotation export error"),
        tr("Error while exporting channel '%1': %2").arg(
            name().c_str()).arg(e.what()));
  }
}

AnnotationChannelSpecs *AnnotationChannelSpecs::loadCSV(
    std::string const &fileName)
{
  AnnotationChannelSpecs* channel = NULL;
  try
  {
    std::ifstream inFile(fileName.c_str());

    // Read header
    std::vector<std::string> headerStrings;
    std::string headerLine;
    std::getline(inFile, headerLine);
    if (!inFile.good())
    {
      QMessageBox::critical(
          qApp->activeWindow(), tr("CSV Read error"),
          tr("Could not read file header. The file seems to be empty."));
      return NULL;
    }

    bool hasPosX = false;
    bool hasPosY = false;
    bool hasPosZ = false;
    bool isSphere = false;
    bool isSHSurface = false;
    bool isCylinder = false;
    bool isSurface = false;
    bool isBiological = false;
    while (headerLine.size() > 0)
    {
      headerStrings.push_back(headerLine.substr(0, headerLine.find(";")));
      if (headerLine.find(";") == std::string::npos) headerLine = "";
      else headerLine = headerLine.substr(
          headerLine.find(";") + 1, std::string::npos);
      if (headerStrings.back() == "x (micron)") hasPosX = true;
      if (headerStrings.back() == "y (micron)") hasPosY = true;
      if (headerStrings.back() == "z (micron)") hasPosZ = true;      
      if (headerStrings.back() == "radius (micron)") isSphere = true;
      if (headerStrings.back() == "coefficients") isSHSurface = true;
      if (headerStrings.back() == "length (micron)" ||
          headerStrings.back() == "theta (degree)" ||
          headerStrings.back() == "phi (degree)") isCylinder = true;
      if (headerStrings.back() == "vertices (micron)" ||
          headerStrings.back() == "normals" ||
          headerStrings.back() == "indices") isSurface = true;
      if (headerStrings.back() == "subtype" ||
          headerStrings.back() == "predicted subtype" ||
          headerStrings.back() == "mitotic" ||
          headerStrings.back() == "predicted_mitotic" ||
          headerStrings.back() == "cellfile" ||
          headerStrings.back() == "predicted cellfile" ||
          headerStrings.back() == "value" ||
          headerStrings.back() == "confidence" ||
          headerStrings.back() == "sphase" ||
          headerStrings.back() == "distance from border (micron)" ||
          headerStrings.back() == "volume (cube microns)") isBiological = true;
    }
    
    if (!hasPosX || !hasPosY || !hasPosZ)
    {
      QMessageBox::critical(
          qApp->activeWindow(), tr("CSV Read error"),
          tr("The file header contains no position information. At least the "
             "columns 'x (micron)', 'y (micron)' and 'z (micron)' must be "
             "given."));
      return NULL;
    }

    Marker::MarkerType markerType = Marker::Point;
    if (isSphere)
    {
      if (isSHSurface)
      {
        if (isBiological) markerType = Marker::Nucleus;
        else markerType = Marker::SHSurface;
      }
      else
      {
        if (isCylinder) markerType = Marker::Cylinder;
        else markerType = Marker::Sphere;
      }
    }
    else
    {
      if (isSurface)
      {
        if (isBiological) markerType = Marker::Cell;
        else markerType = Marker::Surface;
      }
      else markerType = Marker::Point;
    }

    std::cout << "  Creating new annotation channel with type "
              << Marker::markerTypeToString(markerType).toStdString()
              << std::endl;

    channel = new AnnotationChannelSpecs(markerType);
    channel->setName(fileName);

    std::map<std::string,std::string> attValueMap;
    for (size_t i = 0; i < headerStrings.size(); ++i)
        attValueMap[headerStrings[i]] = "";
    
    // Read file data
    int lineNo = 2;
    while (inFile.good())
    {
      std::string line;
      if (!std::getline(inFile, line)) break;
      std::string fullLine = line;

      for (std::vector<std::string>::const_iterator it = headerStrings.begin();
           it != headerStrings.end(); ++it)
      {
        if (line == "$") 
        {
          std::stringstream headerStream;
          headerStream << "Header fields: ";
          for (size_t i = 0; i < headerStrings.size(); ++i)
              headerStream << "'" << headerStrings[i] << "' ";
          headerStream << "\n";
          QMessageBox::critical(
              qApp->activeWindow(), tr("CSV read error"),
              tr("Error while reading CSV data in line %1. Line contains too "
                 "few values. Abort\n").arg(lineNo) +
              headerStream.str().c_str() + "Read line: " + fullLine.c_str());
          delete channel;
          return NULL;
        }
        attValueMap[*it] = line.substr(0, line.find(";"));
        if (line.find(";") == std::string::npos) line = "$";
        else line = line.substr(line.find(";") + 1, std::string::npos);        
      }

      Marker *marker = channel->addMarker(blitz::TinyVector<double,3>(0.0));
      marker->loadFromMap(attValueMap);
      ++lineNo;
    }
  }
  catch (std::exception &e)
  {
    QMessageBox::warning(
        qApp->activeWindow(), tr("CSV Read error"),
        tr("Error while parsing CSV file: ") + e.what());
    delete channel;
    return NULL;
  }
  return channel;
}

void AnnotationChannelSpecs::setCoordinateUpdateButtonEnabled(bool enable)
{
  p_coordinateUpdateButton->setEnabled(enable);
}

void AnnotationChannelSpecs::updateBoundingBox()
{
#ifdef DEBUG
  std::cerr << "AnnotationChannelSpecs::updateBoundingBox()" << std::endl;
#endif
  if (_markers.size() == 0)
  {
    if (blitz::all(_lowerBoundUm == 0.0 && _upperBoundUm == 0.0)) return;
    _lowerBoundUm = 0.0;
    _upperBoundUm = 0.0;
  }
  else
  {
    blitz::TinyVector<double,3> lb(_markers[0]->boundingBoxLowerBoundUm());
    blitz::TinyVector<double,3> ub(_markers[0]->boundingBoxUpperBoundUm());
    for (size_t i = 1; i < _markers.size(); ++i)
    {
      for (int d = 0; d < 3; ++d)
      {
        if (_markers[i]->boundingBoxLowerBoundUm()(d) < lb(d))
            lb(d) = _markers[i]->boundingBoxLowerBoundUm()(d);
        if (_markers[i]->boundingBoxUpperBoundUm()(d) > ub(d))
            ub(d) = _markers[i]->boundingBoxUpperBoundUm()(d);
      }
    }
    if (blitz::all(lb == _lowerBoundUm || ub == _upperBoundUm)) return;
    _lowerBoundUm = lb;
    _upperBoundUm = ub;
  }
  emit boundingBoxChanged(_lowerBoundUm, _upperBoundUm);
}

void AnnotationChannelSpecs::updateSelectedMarker() 
{
  if (_selectedMarker != NULL) p_controlWidget->getValues(_selectedMarker);
}

void AnnotationChannelSpecs::updateCoordinates()
{
  // If the channel is not associated to a model or if the model does not
  // contain an iRoCS channel give up
  if (p_model == NULL || p_model->nChannels(
          ChannelSpecs::IRoCS | ChannelSpecs::IRoCSSCT) == 0) return;
  
  atb::IRoCS *rct = NULL;
  ShellCoordinateTransform *sct = NULL;
  if (p_model->nChannels(ChannelSpecs::IRoCS | ChannelSpecs::IRoCSSCT) > 1)
  {
    // If there are multiple iRoCS channels ask the user which one to use
    QStringList iRoCSChannelNames;
    for (MultiChannelModel::iterator it = p_model->begin();
         it != p_model->end(); ++it)
    {
      if ((*it)->channelType() == ChannelSpecs::IRoCS)
          iRoCSChannelNames << (*it)->name().c_str();
      if ((*it)->channelType() == ChannelSpecs::IRoCSSCT)
          iRoCSChannelNames << (*it)->name().c_str();
    }
    bool ok;
    QString selected = QInputDialog::getItem(
        qApp->activeWindow(), tr("Choose iRoCS channel"),
        tr("Choose the iRoCS channel to use for coordinate update."),
        iRoCSChannelNames, 0, false, &ok);
    if (!ok) return;
    for (MultiChannelModel::iterator it = p_model->begin();
         it != p_model->end(); ++it)
    {
      if ((*it)->channelType() == ChannelSpecs::IRoCS &&
          (*it)->name() == selected.toStdString())
      {
        rct = static_cast<IRoCSChannelSpecs*>(*it)->rct();
        break;
      }
      if ((*it)->channelType() == ChannelSpecs::IRoCSSCT &&
          (*it)->name() == selected.toStdString())
      {
        sct = static_cast<IRoCSSCTChannelSpecs*>(*it)->sct();
        break;
      }
    }
  }
  else
  {
    // otherwise use the only iRoCS channel available
    for (MultiChannelModel::iterator it = p_model->begin();
         it != p_model->end(); ++it)
    {
      if ((*it)->channelType() == ChannelSpecs::IRoCS)
      {
        rct = static_cast<IRoCSChannelSpecs*>(*it)->rct();
        break;
      }
      if ((*it)->channelType() == ChannelSpecs::IRoCSSCT)
      {
        sct = static_cast<IRoCSSCTChannelSpecs*>(*it)->sct();
        break;
      }
    }
  }
  if (rct == NULL && sct == NULL) return;
  
  std::cout << "Updating iRoCS coordinates of channel '" << name() << "'"
            << std::endl;
  for (iterator it = _markers.begin(); it != _markers.end(); ++it)
  {
    blitz::TinyVector<double,3> coords;
    if (rct != NULL) rct->getCoordinates((*it)->positionUm());
    if (sct != NULL) sct->getCoordinates((*it)->positionUm());
    (*it)->setQcDistanceUm(coords(0));
    (*it)->setRadialDistanceUm(coords(1));
    (*it)->setPhi(coords(2));
  }
}
