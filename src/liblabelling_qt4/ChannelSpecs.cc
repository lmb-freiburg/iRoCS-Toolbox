/**************************************************************************
**       Title: Generic Channel Specs
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    Channel Descriptor for one channel of multi-channel data
**
**************************************************************************/

#include "ChannelSpecs.hh"

#include <QtGui/QVBoxLayout>

#include "StringControlElement.hh"
#include "DoubleControlElement.hh"
#include "BoolControlElement.hh"
#include "Double4x4ControlElement.hh"

#include "ViewWidget.hh"
#include "ChannelSpecsRenderer.hh"
#include "MultiChannelModel.hh"

// #include <blitz/tinyvec-et.h>

ChannelSpecs::ChannelTypes const ChannelSpecs::Any =
    ChannelSpecs::Data | ChannelSpecs::Visualization |
    ChannelSpecs::RGB | ChannelSpecs::Annotation |
    ChannelSpecs::SplineCurve | ChannelSpecs::IRoCS | ChannelSpecs::IRoCSSCT;

ChannelSpecs::ChannelSpecs(MultiChannelModel *model) 
        : QObject(), p_model(model), _oldVisibilityState(false),
          _hasNewData(true),
          _lowerBoundUm(std::numeric_limits<double>::infinity()),
          _upperBoundUm(-std::numeric_limits<double>::infinity()),
          _elementSizeUm(std::numeric_limits<double>::infinity()),
          _originalTransformation(
              atb::traits< blitz::TinyMatrix<double,4,4> >::one),
          _transformation(atb::traits< blitz::TinyMatrix<double,4,4> >::one),
          _updatesEnabled(true)
{
  p_channelControlLayout = new QVBoxLayout;
  p_channelControlLayout->setContentsMargins(0, 0, 0, 0);

  p_alphaControl = new DoubleControlElement(tr("Alpha:"), 1.0);
  p_alphaControl->setRange(0.0, 1.0);
  p_alphaControl->setSingleStep(0.01);
  p_alphaControl->setSliderViewEnabled(true);
  p_alphaControl->setToolTip(tr("Set the selected channel's transparency"));
  connect(p_alphaControl, SIGNAL(valueChanged()), SLOT(emitUpdateRequest()));
  p_channelControlLayout->addWidget(p_alphaControl);

  p_visibleControl = new BoolControlElement(tr("Visible:"), true);
  p_visibleControl->setToolTip(tr("Show/Hide the selected channel"));
  connect(p_visibleControl, SIGNAL(valueChanged()), SLOT(emitUpdateRequest()));
  p_channelControlLayout->addWidget(p_visibleControl);

  p_channelControlWidget = new QWidget;
  p_channelControlWidget->setLayout(p_channelControlLayout);

  p_toggleAdvancedButton = new QToolButton;
  p_toggleAdvancedButton->setFixedHeight(15);
  p_toggleAdvancedButton->setArrowType(Qt::DownArrow);
  p_toggleAdvancedButton->setSizePolicy(
      QSizePolicy::Expanding, QSizePolicy::Fixed);
  p_toggleAdvancedButton->setCheckable(true);
  p_toggleAdvancedButton->setToolTip(tr("Advanced channel settings"));
  connect(p_toggleAdvancedButton, SIGNAL(toggled(bool)),
          SLOT(toggleAdvanced(bool)));
  p_channelControlLayout->addWidget(p_toggleAdvancedButton);

  p_advancedControlWidget = new QWidget;
  p_advancedControlWidget->setContentsMargins(0, 0, 0, 0);
  p_advancedControlWidget->setSizePolicy(
      QSizePolicy::Expanding, QSizePolicy::Expanding);
  p_advancedControlWidget->setVisible(false);

  p_advancedControlLayout = new QVBoxLayout;
  p_advancedControlLayout->setContentsMargins(0, 0, 0, 0);
  p_advancedControlWidget->setLayout(p_advancedControlLayout);

  p_transformationControl = new Double4x4ControlElement(
      "Transform:", _transformation);
  p_transformationControl->setSwitchable(true);
  p_transformationControl->setActive(false);
  connect(p_transformationControl, SIGNAL(valueChanged()),
          SLOT(setTransformationFromControl()));
  p_advancedControlLayout->addWidget(p_transformationControl);

  p_channelControlLayout->addWidget(p_advancedControlWidget);

  _icon = QIcon();
}

ChannelSpecs::~ChannelSpecs()
{}

void ChannelSpecs::setModel(MultiChannelModel *model)
{
  if (p_model == model) return;
  if (p_model != NULL) p_model->removeChannel(this, false);
  p_model = model;
}

MultiChannelModel *ChannelSpecs::model() const
{
  return p_model;
}

std::string ChannelSpecs::name() const 
{
  return _name;
}

QIcon ChannelSpecs::icon() const 
{
  return _icon;
}

float ChannelSpecs::alpha() const 
{
  return static_cast<float>(p_alphaControl->value());
}

bool ChannelSpecs::visible() const 
{
  return p_visibleControl->value();
}

bool ChannelSpecs::hasNewData() const
{
  return _hasNewData;
}

blitz::TinyVector<double,3> const &ChannelSpecs::lowerBoundUm() const
{
  return _lowerBoundUm;
}

blitz::TinyVector<double,3> const &ChannelSpecs::upperBoundUm() const
{
  return _upperBoundUm;
}

blitz::TinyVector<double,3> const &ChannelSpecs::elementSizeUm() const
{
  return _elementSizeUm;
}

blitz::TinyMatrix<double,4,4> const &ChannelSpecs::transformation() const
{
  return _transformation;
}

void ChannelSpecs::setTransformation(
    blitz::TinyMatrix<double,4,4> const &transformation)
{
  if (blitz::all(_transformation == transformation)) return;
  _transformation = transformation;
  p_transformationControl->setValue(transformation);
  if (p_model != NULL) p_model->setModified(true);
  emitUpdateRequest();
}

QWidget *ChannelSpecs::channelControlWidget() const 
{
  return p_channelControlWidget;
}

void ChannelSpecs::removeRenderer(ViewWidget *view)
{
  std::map<ViewWidget*,ChannelSpecsRenderer*>::iterator it =
      _renderers.find(view);
  if (it != _renderers.end()) _renderers.erase(it);
}

ChannelSpecsRenderer *ChannelSpecs::renderer(ViewWidget *view) const
{
  if (_renderers.find(view) == _renderers.end()) return NULL;
  return _renderers.find(view)->second;
}

std::map<ViewWidget*,ChannelSpecsRenderer*> const
&ChannelSpecs::renderers() const
{
  return _renderers;
}

std::map<ViewWidget*,ChannelSpecsRenderer*>
&ChannelSpecs::renderers()
{
  return _renderers;
}

bool ChannelSpecs::updatesEnabled() const
{
  return _updatesEnabled;
}

void ChannelSpecs::setName(std::string const &name) 
{
  if (this->name() == name) return;
  _name = name;
  if (p_model != NULL) p_model->setModified(true);
  emit nameChanged();
}

void ChannelSpecs::setIcon(QIcon const &icon) 
{
  _icon = icon;
  emit iconChanged();
}

void ChannelSpecs::setAlpha(float alpha) 
{
  if (this->alpha() == alpha) return;
  p_alphaControl->setValue(alpha);
  if (p_model != NULL) p_model->setModified(true);
}
  
void ChannelSpecs::setVisible(bool visible) 
{
  if (this->visible() == visible) return;
  p_visibleControl->setValue(visible);
  if (p_model != NULL) p_model->setModified(true);  
}

void ChannelSpecs::setHasNewData(bool hasNewData)
{
  _hasNewData = hasNewData;
}

void ChannelSpecs::setUpdatesEnabled(bool enable)
{
  for (std::map<ViewWidget*,ChannelSpecsRenderer*>::iterator it =
           _renderers.begin(); it != _renderers.end(); ++it)
      it->second->setCacheUpdatesEnabled(enable);
  _updatesEnabled = enable;
}

void ChannelSpecs::update()
{
  if (!_updatesEnabled) return;
  for (std::map<ViewWidget*,ChannelSpecsRenderer*>::iterator it =
           _renderers.begin(); it != _renderers.end(); ++it)
      it->second->updateCache();
}

void ChannelSpecs::revalidate()
{
  p_transformationControl->setValue(_transformation);
  if (p_model != NULL) p_model->setModified(true);
  emitUpdateRequest();
}

void ChannelSpecs::emitUpdateRequest()
{
  _hasNewData = true;
  update();
  if (visible() || _oldVisibilityState != visible())
  {
    _oldVisibilityState = visible();
    emit viewChanged();
  }
}

void ChannelSpecs::toggleAdvanced(bool toggle)
{
  if (toggle) p_toggleAdvancedButton->setArrowType(Qt::UpArrow);
  else p_toggleAdvancedButton->setArrowType(Qt::DownArrow);
  p_advancedControlWidget->setVisible(toggle);
}

void ChannelSpecs::setTransformationFromControl()
{
  setTransformation(p_transformationControl->value());
}
