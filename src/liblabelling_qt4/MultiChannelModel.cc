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

// Make Qt happy
#include <QtGui/QFont>

#include "MultiChannelModel.hh"

#include <QtGui/QMessageBox>
#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <QtGui/QComboBox>
#include <QtGui/QStackedWidget>
#include <QtGui/QLineEdit>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>

#include "ChannelSelectionControlElement.hh"

#include "DataChannelSpecs.hh"
#include "RGBChannelSpecs.hh"
#include "VisualizationChannelSpecs.hh"
#include "AnnotationChannelSpecs.hh"
#include "SplineCurveChannelSpecs.hh"
#include "IRoCSChannelSpecs.hh"
#include "IRoCSSCTChannelSpecs.hh"
#include "ViewWidget.hh"
#include "OpenGlRenderingViewWidget.hh"
#include "OpenGlRenderingWidget.hh"
#include "DataChannelSpecsOrthoViewRenderer.hh"
#include "AnnotationChannelSpecsOrthoViewRenderer.hh"
#include "VisualizationChannelSpecsOrthoViewRenderer.hh"
#include "SplineCurveChannelSpecsOrthoViewRenderer.hh"
#include "IRoCSChannelSpecsOrthoViewRenderer.hh"
#include "IRoCSSCTChannelSpecsOrthoViewRenderer.hh"
#include "ColorMapEditorWidget.hh"

MultiChannelModel::MultiChannelModel()
        : _channels(std::vector<ChannelSpecs*>()),
          _views(std::vector<ViewWidget*>()),
          _lowerBoundUm(0.0), _upperBoundUm(0.0),
          _elementSizeUm(std::numeric_limits<double>::infinity()),
          p_channelEditorWidget(NULL), p_channelSelectionControl(NULL),
          p_channelControlStack(NULL), p_infoWidget(NULL), 
          p_positionVxXLineEdit(NULL), p_positionVxYLineEdit(NULL),
          p_positionVxZLineEdit(NULL), p_positionUmXLineEdit(NULL),
          p_positionUmYLineEdit(NULL), p_positionUmZLineEdit(NULL),
          p_valueLineEdit(NULL), _modified(false)
{
  // Channel Editor Widget
  p_channelEditorWidget = new QWidget;
  QVBoxLayout* channelEditorLayout = new QVBoxLayout;
  p_channelEditorWidget->setLayout(channelEditorLayout);
  p_channelEditorWidget->setSizePolicy(QSizePolicy::Preferred,
                                       QSizePolicy::Maximum);

  QHBoxLayout* channelSelectorLayout = new QHBoxLayout;
  channelSelectorLayout->setContentsMargins(0, 0, 0, 0);
  p_channelSelectionControl = new ChannelSelectionControlElement(
      tr("Channel:"), this);
  p_channelSelectionControl->setEditable(true);
  p_channelSelectionControl->setToolTip(
      tr("Select channel or edit channel name"));
  connect(p_channelSelectionControl, SIGNAL(currentIndexChanged(int)),
          SLOT(setActiveChannel(int)));
  connect(p_channelSelectionControl, SIGNAL(editingFinished()),
          SLOT(setActiveChannelName()));
  channelSelectorLayout->addWidget(p_channelSelectionControl);

  p_deleteChannelButton = new QToolButton;
  p_deleteChannelButton->setIcon(
      QIcon::fromTheme("edit-delete", QIcon(":/edit-delete.png")));
  p_deleteChannelButton->setText(tr("Delete"));
  p_deleteChannelButton->setToolTip(tr("Delete selected channel"));
  connect(p_deleteChannelButton, SIGNAL(clicked()),
          SLOT(deleteActiveChannel()));
  p_deleteChannelButton->setEnabled(false);
  channelSelectorLayout->addWidget(p_deleteChannelButton);

  channelEditorLayout->addLayout(channelSelectorLayout);
  
  p_channelControlStack = new QStackedWidget;
  channelEditorLayout->addWidget(p_channelControlStack);
  
  // Info widget
  QGridLayout* infoLayout = new QGridLayout;

  QLabel* positionVxLabel = new QLabel(tr("Position:"));
  infoLayout->addWidget(positionVxLabel, 0, 0);
  p_positionVxXLineEdit = new QLineEdit;
  p_positionVxXLineEdit->setReadOnly(true);
  p_positionVxXLineEdit->setMinimumWidth(40);
  infoLayout->addWidget(p_positionVxXLineEdit, 0, 1);
  p_positionVxYLineEdit = new QLineEdit;
  p_positionVxYLineEdit->setReadOnly(true);
  p_positionVxYLineEdit->setMinimumWidth(40);
  infoLayout->addWidget(p_positionVxYLineEdit, 0, 2);
  p_positionVxZLineEdit = new QLineEdit;
  p_positionVxZLineEdit->setReadOnly(true);
  p_positionVxZLineEdit->setMinimumWidth(40);
  infoLayout->addWidget(p_positionVxZLineEdit, 0, 3);
  QLabel* vxLabel = new QLabel(tr("voxel"));
  infoLayout->addWidget(vxLabel, 0, 4);

  p_positionUmXLineEdit = new QLineEdit;
  p_positionUmXLineEdit->setReadOnly(true);
  p_positionUmXLineEdit->setMinimumWidth(40);
  infoLayout->addWidget(p_positionUmXLineEdit, 1, 1);
  p_positionUmYLineEdit = new QLineEdit;
  p_positionUmYLineEdit->setReadOnly(true);
  p_positionUmYLineEdit->setMinimumWidth(40);
  infoLayout->addWidget(p_positionUmYLineEdit, 1, 2);
  p_positionUmZLineEdit = new QLineEdit;
  p_positionUmZLineEdit->setReadOnly(true);
  p_positionUmZLineEdit->setMinimumWidth(40);
  infoLayout->addWidget(p_positionUmZLineEdit, 1, 3);
  QLabel* umLabel = new QLabel(tr("micron"));
  infoLayout->addWidget(umLabel, 1, 4);

  QLabel* valueLabel = new QLabel(tr("Value:"));
  infoLayout->addWidget(valueLabel, 2, 0);
  p_valueLineEdit = new QLineEdit;
  p_valueLineEdit->setReadOnly(true);
  p_valueLineEdit->setMinimumWidth(200);
  infoLayout->addWidget(p_valueLineEdit, 2, 1, 1, 4);

  QVBoxLayout* infoOuterLayout = new QVBoxLayout;
  infoOuterLayout->addLayout(infoLayout);
  infoOuterLayout->addStretch(1);

  p_infoWidget = new QWidget;
  p_infoWidget->setLayout(infoOuterLayout);
  p_infoWidget->setSizePolicy(QSizePolicy::Preferred,
                              QSizePolicy::Maximum);
}

MultiChannelModel::~MultiChannelModel()
{
  while (_channels.size() != 0) removeChannel(_channels.back());

  if (p_channelEditorWidget->parent() == 0) delete p_channelEditorWidget;
  if (p_infoWidget->parent() == 0) delete p_infoWidget;
}

blitz::TinyVector<double,3> const &MultiChannelModel::lowerBoundUm() const
{
  return _lowerBoundUm;
}

blitz::TinyVector<double,3> const &MultiChannelModel::upperBoundUm() const
{
  return _upperBoundUm;
}

blitz::TinyVector<double,3> MultiChannelModel::boundingBoxShapeUm() const
{
  return _upperBoundUm - _lowerBoundUm;
}

blitz::TinyVector<double,3> const &MultiChannelModel::elementSizeUm() const
{
  return _elementSizeUm;
}

void MultiChannelModel::setElementSizeUm(
    const blitz::TinyVector<double,3>& elementSizeUm)
{
  if (blitz::all(_elementSizeUm == elementSizeUm)) return;
  _elementSizeUm = elementSizeUm;
  emit elementSizeChanged(elementSizeUm);
}

QWidget* MultiChannelModel::channelEditorWidget() const
{
  return p_channelEditorWidget;
}

QWidget* MultiChannelModel::infoWidget() const
{
  return p_infoWidget;
}

void MultiChannelModel::addChannelSpecs(ChannelSpecs* specs)
{
  // Check whether the channel needs a new name, and generate it
  if (specs->name() == "New Channel" || specs->name() == "")
      specs->setName(newChannelName());

  // Append it to the model
  specs->setModel(this);
  _channels.push_back(specs);

  // And add its control widget to the control widget stack
  p_channelControlStack->addWidget(specs->channelControlWidget());

  // Enable the "Delete channel button"
  p_deleteChannelButton->setEnabled(true);

  // Connect SIGNALS of added channel
  connect(specs, SIGNAL(viewChanged()), SLOT(updateViews()));
  connect(specs, SIGNAL(nameChanged()), SIGNAL(channelNamesOrIconsChanged()));
  connect(specs, SIGNAL(iconChanged()), SIGNAL(channelNamesOrIconsChanged()));
  connect(specs, SIGNAL(boundingBoxChanged(
                            blitz::TinyVector<double,3> const &,
                            blitz::TinyVector<double,3> const &)),
          SLOT(setBoundingBoxUm(blitz::TinyVector<double,3> const &,
                                blitz::TinyVector<double,3> const &)));

  // Set the modified flag of the MultiChannelModel
  setModified(true);

  // Update the bounding box if necessary
  blitz::TinyVector<double,3> lb(_channels[0]->lowerBoundUm());
  blitz::TinyVector<double,3> ub(_channels[0]->upperBoundUm());
  for (size_t i = 1; i < _channels.size(); ++i)
  {
    for (int d = 0; d < 3; ++d)
    {
      if (_channels[i]->lowerBoundUm()(d) < lb(d))
          lb(d) = _channels[i]->lowerBoundUm()(d);
      if (_channels[i]->upperBoundUm()(d) > ub(d))
          ub(d) = _channels[i]->upperBoundUm()(d);
    }
  }
  bool bBoxChanged =
      blitz::any(lb != _lowerBoundUm || ub != _upperBoundUm);
  if (bBoxChanged)
  {
#ifdef DEBUG
    std::cerr << "MultiChannelModel::addChannel(" << specs << " '"
              << specs->name() << "'): Model bounding box changed from ("
              << _lowerBoundUm << " - " << _upperBoundUm << ") to (" << lb
              << " - " << ub << ")" << std::endl;
#endif
    _lowerBoundUm = lb;
    _upperBoundUm = ub;
  }

  // Update the element size if necessary
  bool elemSizeChanged = blitz::any(specs->elementSizeUm() < _elementSizeUm);
  if (elemSizeChanged) _elementSizeUm = specs->elementSizeUm();

  // Generate ChannelSpecsRenderers for all active views
  // at this point the bounding box and element size of the model must be
  // already updated
  for (std::vector<ViewWidget*>::iterator it = _views.begin();
       it != _views.end(); ++it) specs->addRenderer(*it);

  // emit SIGNALS to broadcast the model change
  emit channelAdded(specs);

  // Make the newly added channel the active channel for editing
  setActiveChannel(static_cast<int>(_channels.size()) - 1);

  if (_channels.size() == 1)
  {
#ifdef DEBUG
    std::cerr << "MultiChannelModel::addChannelSpecs(" << specs
              << "): emitting isEmpty(false)" << std::endl;
#endif
    emit isEmpty(false);
  }
  if (specs->channelType() == ChannelSpecs::Annotation &&
      nChannels(ChannelSpecs::Annotation) == 1)
  {
#ifdef DEBUG
    std::cerr << "MultiChannelModel::addChannelSpecs(" << specs
              << "): emitting containsAnnotationsChanged(true)" << std::endl;
#endif
    emit containsAnnotationsChanged(true);
  }

  if (elemSizeChanged) emit elementSizeChanged(_elementSizeUm);

  // Reset the cross hair position to the center if the bounding box shape
  // changed
  if (bBoxChanged)
  {
#ifdef DEBUG
    std::cerr << "MultiChannelModel::addChannelSpecs(" << specs
              << "): emitting boundingBoxChanged(" << _lowerBoundUm << ","
              << _upperBoundUm << ")" << std::endl;
#endif
    emit boundingBoxChanged(_lowerBoundUm, _upperBoundUm);
#ifdef DEBUG
    std::cerr << "MultiChannelModel::addChannelSpecs(" << specs
              << "): setting crosshair position of all views to the center "
              << "of the bounding box" << std::endl;
#endif
  }
  if (_channels.size() == 1)
  {
    for (std::vector<ViewWidget*>::iterator it = _views.begin();
         it != _views.end(); ++it)
        (*it)->setPositionUm(
            blitz::TinyVector<double,3>((_lowerBoundUm + _upperBoundUm) / 2.0));
  }
}

DataChannelSpecs* MultiChannelModel::addDataChannel(
    atb::Array<float,3> *channel) 
{
  DataChannelSpecs* specs = new DataChannelSpecs(channel, this);
  addChannelSpecs(specs);
  std::cout << "Added new data channel with shape " << channel->shape()
            << " vx and element size " << channel->elementSizeUm() << " um/vx"
            << std::endl;
  return specs;
}
  
RGBChannelSpecs* MultiChannelModel::addRGBChannel(
    atb::Array<blitz::TinyVector<float,3>,3>* channel) 
{
  RGBChannelSpecs* specs = new RGBChannelSpecs(channel, this);
  addChannelSpecs(specs);
  std::cout << "Added new RGB channel with shape " << channel->shape()
            << " vx and element size " << channel->elementSizeUm() << " um/vx"
            << std::endl;
  return specs;
}

VisualizationChannelSpecs *MultiChannelModel::addVisualizationChannel(
    atb::Array<int,3> *channel)
{
  if (channel == NULL)
  {
    if (_channels.size() == 0)
    {
      std::cerr << "Cannot create new VisualizationChannel for empty model"
                << std::endl;
      return NULL;
    }    
    blitz::TinyVector<atb::BlitzIndexT,3> dataShapePx(
        ((_upperBoundUm - _lowerBoundUm) / _elementSizeUm) + 1);
    channel = new atb::Array<int,3>(dataShapePx);
    std::memset(channel->data(), 0, channel->size() * sizeof(int));
  }
  VisualizationChannelSpecs *specs =
      new VisualizationChannelSpecs(channel, this);

  // Use nearest Interpolation for Visualization channels
  specs->data()->setInterpolator(atb::NearestInterpolator<int,3>(atb::ValueBT));

  addChannelSpecs(specs);
  std::cout << "Added new Visualization channel with shape " << channel->shape()
            << " vx and element size " << channel->elementSizeUm() << " um/vx"
            << std::endl;
  return specs;
}

AnnotationChannelSpecs* MultiChannelModel::addAnnotationChannel(
    Marker::MarkerType markerType) 
{
  if (_channels.size() == 0) return NULL;
  AnnotationChannelSpecs* specs = new AnnotationChannelSpecs(markerType, this);
  addChannelSpecs(specs);
  return specs;
}

SplineCurveChannelSpecs* MultiChannelModel::addSplineCurveChannel(
    atb::BSpline< blitz::TinyVector<double,3> > *spline)
{
  if (_channels.size() == 0) return NULL;
  SplineCurveChannelSpecs *specs = new SplineCurveChannelSpecs(spline, this);
  addChannelSpecs(specs);
  return specs;
}

IRoCSChannelSpecs* MultiChannelModel::addIRoCSChannel(
    atb::IRoCS *rct)
{
  if (_channels.size() == 0) return NULL;
  IRoCSChannelSpecs *specs = new IRoCSChannelSpecs(rct, this);
  addChannelSpecs(specs);

  // Enable the "Update coordinates" button of all annotation channels
  for (size_t i = 0; i < _channels.size() - 1; ++i)
  {
    if (_channels[i]->channelType() == ChannelSpecs::Annotation)
        static_cast<AnnotationChannelSpecs*>(
            _channels[i])->setCoordinateUpdateButtonEnabled(true);
  }

  return specs;
}

IRoCSSCTChannelSpecs* MultiChannelModel::addIRoCSSCTChannel(
    ShellCoordinateTransform *sct)
{
  if (_channels.size() == 0) return NULL;
  IRoCSSCTChannelSpecs *specs = new IRoCSSCTChannelSpecs(sct, this);
  addChannelSpecs(specs);

  // Enable the "Update coordinates" button of all annotation channels
  for (size_t i = 0; i < _channels.size() - 1; ++i)
  {
    if (_channels[i]->channelType() == ChannelSpecs::Annotation)
        static_cast<AnnotationChannelSpecs*>(
            _channels[i])->setCoordinateUpdateButtonEnabled(true);
  }

  return specs;
}

ChannelSpecs *MultiChannelModel::addChannel(ChannelSpecs *channel)
{
  if ((channel->channelType() == ChannelSpecs::Annotation ||
       channel->channelType() == ChannelSpecs::SplineCurve ||
       channel->channelType() == ChannelSpecs::IRoCS) &&
      _channels.size() == 0) return NULL;

  addChannelSpecs(channel);
  return channel;
}

void MultiChannelModel::removeChannel(
    ChannelSpecs* channel, bool freeMemory)
{
  if (channel == NULL) return;
  
  // Find iterator and index of the channel in the model
  std::vector<ChannelSpecs*>::iterator it = _channels.begin();
  int channelIndex = 0;
  for (; it != _channels.end() && *it != channel; ++it, ++channelIndex);

  // If the model does not contain the channel, return immediately
  if (it == _channels.end()) return;

  // Disable cache updates of the channel
  channel->setUpdatesEnabled(false);

  // Remove the control widget from the control widget stack
  p_channelControlStack->removeWidget(channel->channelControlWidget());
  channel->channelControlWidget()->setParent(NULL);

  // Remove the channel from the model
  _channels.erase(it);
  
  // Disconnect SIGNALS of removed channel
  disconnect(channel, SIGNAL(viewChanged()), this, SLOT(updateViews()));
  disconnect(channel, SIGNAL(nameChanged()),
             this, SIGNAL(channelNamesOrIconsChanged()));
  disconnect(channel, SIGNAL(iconChanged()),
             this, SIGNAL(channelNamesOrIconsChanged()));
  disconnect(channel, SIGNAL(boundingBoxChanged(
                                 blitz::TinyVector<double,3> const &,
                                 blitz::TinyVector<double,3> const &)),
             this, SLOT(setBoundingBoxUm(blitz::TinyVector<double,3> const &,
                                         blitz::TinyVector<double,3> const &)));
  
  // Update the model bounding box if necessary
  bool bBoxChanged = false;
  bool elementSizeUmChanged = false;
  if (_channels.size() != 0)
  {
    blitz::TinyVector<double,3> elSizeUm(_channels[0]->elementSizeUm());
    blitz::TinyVector<double,3> lb(_channels[0]->lowerBoundUm());
    blitz::TinyVector<double,3> ub(_channels[0]->upperBoundUm());
    for (size_t i = 1; i < _channels.size(); ++i)
    {
      for (int d = 0; d < 3; ++d)
      {
        if (_channels[i]->elementSizeUm()(d) < elSizeUm(d))
            elSizeUm(d) = _channels[i]->elementSizeUm()(d);
        if (_channels[i]->lowerBoundUm()(d) < lb(d))
            lb(d) = _channels[i]->lowerBoundUm()(d);
        if (_channels[i]->upperBoundUm()(d) > ub(d))
            ub(d) = _channels[i]->upperBoundUm()(d);
      }
    }
    bBoxChanged = blitz::any(lb != _lowerBoundUm || ub != _upperBoundUm);
    elementSizeUmChanged = blitz::any(elSizeUm != _elementSizeUm);
    if (bBoxChanged)
    {
#ifdef DEBUG
      std::cerr << "MultiChannelModel::removeChannel(" << channel << " '"
                << channel->name() << "', " << freeMemory
                << "): Model bounding box changed from (" << _lowerBoundUm
                << " - " << _upperBoundUm << ") to (" << lb << " - " << ub
                << ")" << std::endl;
#endif
      _lowerBoundUm = lb;
      _upperBoundUm = ub;
    }
    if (elementSizeUmChanged)
    {
#ifdef DEBUG
      std::cerr << "MultiChannelModel::removeChannel(" << channel << " '"
                << channel->name() << "', " << freeMemory
                << "): Model element size changed from " << _elementSizeUm
                << " to " << elSizeUm << std::endl;
#endif
      _elementSizeUm = elSizeUm;
    }
  }
  else
  {
    bBoxChanged = true;
    _lowerBoundUm = 0.0;
    _upperBoundUm = 0.0;
    elementSizeUmChanged = true;
    _elementSizeUm = std::numeric_limits<double>::infinity();
    p_deleteChannelButton->setEnabled(false);
  }

  // Remove the channel from all views (This triggers operations needed to
  // synchronize the view with the model, e.g. cache updates)
  for (std::vector<ViewWidget*>::iterator itV = _views.begin();
       itV != _views.end(); ++itV) (*itV)->removeChannel(channel);

  // Delete the channel data if requested
  if (freeMemory) delete channel;

  // If the model is not empty set its modified flag
  setModified(_channels.size() != 0);

  // If there are no iRoCS channels left, disable the "Update coordinates"
  // button of all annotation channels
  if (nChannels(ChannelSpecs::IRoCS) == 0)
  {
    for (size_t i = 0; i < _channels.size(); ++i)
    {
      if (_channels[i]->channelType() == ChannelSpecs::Annotation)
          static_cast<AnnotationChannelSpecs*>(
              _channels[i])->setCoordinateUpdateButtonEnabled(false);
    }
  }

  // emit signals to inform listeners of the model change
  if (bBoxChanged) emit boundingBoxChanged(_lowerBoundUm, _upperBoundUm);
  if (elementSizeUmChanged) emit elementSizeChanged(_elementSizeUm);

  // If the model becomes empty emit the isEmpty SIGNAL
  if (_channels.size() == 0) emit isEmpty(true);

  // emit the channelRemoved SIGNAL
  emit channelRemoved(channel);
  
  // If the last annotation channel was just removed send a corresponding
  // SIGNAL
  if (nChannels(ChannelSpecs::Annotation) == 0)
      emit containsAnnotationsChanged(false);
}

void MultiChannelModel::removeChannel(size_t index)
{
  if (index < _channels.size()) removeChannel(_channels[index]);
}

void MultiChannelModel::removeChannel(const std::string& name)
{
  removeChannel(channel(name));
}

size_t MultiChannelModel::nChannels(ChannelSpecs::ChannelTypes types) const
{
  if (types == ChannelSpecs::Any) return _channels.size();

  size_t res = 0;
  for (size_t i = 0; i < _channels.size(); ++i)
      if (types.testFlag(_channels[i]->channelType())) ++res;
  return res;
}

ChannelSpecs* MultiChannelModel::selectedChannel()
{
  int index = p_channelControlStack->currentIndex();
  if (index < 0 || index > static_cast<int>(_channels.size())) return NULL;
  return _channels[index];
}

ChannelSpecs* MultiChannelModel::channel(size_t index)
{
  return (index < _channels.size()) ? _channels[index] : NULL;
}

ChannelSpecs* MultiChannelModel::channel(const std::string& name)
{
  std::vector<ChannelSpecs*>::iterator it = _channels.begin();
  while (it != _channels.end() && (*it)->name() != name) ++it;
  return (it != _channels.end()) ? *it : NULL;
}

MultiChannelModel::iterator MultiChannelModel::begin()
{
  return _channels.begin();
}

MultiChannelModel::iterator MultiChannelModel::end()
{
  return _channels.end();
}

void MultiChannelModel::addView(ViewWidget* view)
{
  for (std::vector<ChannelSpecs*>::iterator it = begin(); it != end(); ++it)
      (*it)->addRenderer(view);
  _views.push_back(view);
}

void MultiChannelModel::removeView(ViewWidget* view)
{
  std::vector<ViewWidget*>::iterator viewIt = _views.begin();
  while (viewIt != _views.end() && *viewIt != view) ++viewIt;
  if (viewIt != _views.end())
  {
    for (std::vector<ChannelSpecs*>::iterator it =
             begin(); it != end(); ++it) (*it)->removeRenderer(view);
    _views.erase(viewIt);
  }
}

std::vector<ViewWidget*>::iterator MultiChannelModel::viewsBegin()
{
  return _views.begin();
}

std::vector<ViewWidget*>::iterator MultiChannelModel::viewsEnd()
{
  return _views.end();
}

bool MultiChannelModel::channelNamesUnique() 
{
  std::vector<ChannelSpecs*>::const_iterator it = begin();
  std::vector<std::string> readNames;
  for (; it != end(); ++it) 
  {
    bool unique = true;
    for (std::vector<std::string>::iterator it2 = readNames.begin();
         unique && it2 != readNames.end(); ++it2) 
    {
      if (*it2 == (*it)->name()) unique = false;
    }
    if (!unique) 
    {
      QMessageBox::warning(
          qApp->activeWindow(), tr("Channel names not unique"),
          tr("Channel names need to be unique for saving, please rename "
             "the channels accordingly"));
      return false;
    }
    readNames.push_back((*it)->name());
  }
  return true;
}

void MultiChannelModel::setLowerBoundUm(
    blitz::TinyVector<double,3> const &lowerBoundUm)
{
  if (blitz::all(lowerBoundUm == _lowerBoundUm)) return;
  _lowerBoundUm = _lowerBoundUm;
  emit boundingBoxChanged(_lowerBoundUm, _upperBoundUm);
}

void MultiChannelModel::setUpperBoundUm(
    blitz::TinyVector<double,3> const &upperBoundUm)
{
  if (blitz::all(upperBoundUm == _upperBoundUm)) return;
  _upperBoundUm = _upperBoundUm;
  emit boundingBoxChanged(_lowerBoundUm, _upperBoundUm);
}

std::string MultiChannelModel::newChannelName()
{
  std::string name;
  bool nameUnique = true;
  int i = 0;
  do 
  {
    std::stringstream nameStream;
    nameStream << "channel_" << i;
    name = nameStream.str();
    nameUnique = true;
    for (std::vector<ChannelSpecs*>::const_iterator it = begin();
         nameUnique && it != end(); ++it) 
    {
      if (name == (*it)->name()) nameUnique = false;
    }
    ++i;
  }
  while (!nameUnique);
  return name;
}

void MultiChannelModel::setBoundingBoxUm(
    blitz::TinyVector<double,3> const &lowerBoundUm,
    blitz::TinyVector<double,3> const &upperBoundUm)
{
  blitz::TinyVector<double,3> lb(_lowerBoundUm);
  blitz::TinyVector<double,3> ub(_upperBoundUm);
  for (int d = 0; d < 3; ++d)
  {
    if (lowerBoundUm(d) < lb(d)) lb(d) = lowerBoundUm(d);
    if (upperBoundUm(d) > ub(d)) ub(d) = upperBoundUm(d);
  }
  if (blitz::any(lb != _lowerBoundUm || ub != _upperBoundUm))
  {
#ifdef DEBUG
    std::cerr << "MultiChannelModel::setBoundingBoxUm(" << lowerBoundUm
              << ", " << upperBoundUm << "): Model bounding box changed from ("
              << _lowerBoundUm << " - " << _upperBoundUm << ") to ("
              << lb << " - " << ub << ")" << std::endl;
#endif
    _lowerBoundUm = lb;
    _upperBoundUm = ub;
    emit boundingBoxChanged(_lowerBoundUm, _upperBoundUm);
  }
}

void MultiChannelModel::setActiveChannel(int index)
{
  if(index < 0 || index > static_cast<int>(_channels.size())) return;

  int oldIndex = p_channelControlStack->currentIndex();
  if (oldIndex == index) return;

  p_channelControlStack->setCurrentIndex(index);
  p_channelSelectionControl->setValue(index);
  emit channelSelectionChanged(index);
  emit annotationChannelSelected(
      _channels[index]->channelType() == ChannelSpecs::Annotation);
}


void MultiChannelModel::deleteActiveChannel()
{
  if (QMessageBox::question(
          qApp->activeWindow(), tr("Delete Channel"),
          tr("Are you sure you want to delete the channel %1?").arg(
              selectedChannel()->name().c_str()), QMessageBox::Yes |
          QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
      removeChannel(selectedChannel());
}

void MultiChannelModel::setActiveChannelName()
{
  if (selectedChannel() != NULL)
      selectedChannel()->setName(p_channelSelectionControl->toStdString());
}

void MultiChannelModel::updateViews()
{
#ifdef DEBUG
  std::cerr << "MultiChannelModel::updateViews()" << std::endl;
#endif
  for (std::vector<ViewWidget*>::iterator it = _views.begin();
       it != _views.end(); ++it) (*it)->redraw();
  for (std::vector<ChannelSpecs*>::iterator it = _channels.begin();
       it != _channels.end(); ++it) (*it)->setHasNewData(false);
}

void MultiChannelModel::updateInfoWidget(
    const blitz::TinyVector<double,3>& positionUm) 
{
  p_positionVxXLineEdit->setText(
      QString::number(static_cast<ptrdiff_t>(
                          positionUm(0) / _elementSizeUm(0) + 0.5)));
  p_positionVxYLineEdit->setText(
      QString::number(static_cast<ptrdiff_t>(
                          positionUm(1) / _elementSizeUm(1) + 0.5)));
  p_positionVxZLineEdit->setText(
      QString::number(static_cast<ptrdiff_t>(
                          positionUm(2) / _elementSizeUm(2) + 0.5)));
  p_positionUmXLineEdit->setText(QString::number(positionUm(0)));
  p_positionUmYLineEdit->setText(QString::number(positionUm(1)));
  p_positionUmZLineEdit->setText(QString::number(positionUm(2)));
  std::stringstream s;
  for (size_t c = 0; c < _channels.size(); ++c) 
  {
    s << _channels[c]->stringValueAt(positionUm) << " ";
  }
  p_valueLineEdit->setText(s.str().c_str());
}

bool MultiChannelModel::modified()
{
  return _modified;
}

void MultiChannelModel::setModified(bool modified)
{
  if (_modified == modified) return;
  _modified = modified;
  emit modifiedFlagChanged(modified);
}
