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

#include "ViewWidget.hh"
#include "ChannelSpecsRenderer.hh"
#include "MultiChannelModel.hh"

// #include <blitz/tinyvec-et.h>

ChannelSpecs::ChannelTypes const ChannelSpecs::Any =
    ChannelSpecs::Data | ChannelSpecs::Visualization |
    ChannelSpecs::RGB | ChannelSpecs::Annotation |
    ChannelSpecs::SplineCurve | ChannelSpecs::IRoCS;

ChannelSpecs::ChannelSpecs(MultiChannelModel *model) 
        : QObject(), p_model(model), _oldVisibilityState(false),
          _hasNewData(true), _updatesEnabled(true)
{
  p_channelControlLayout = new QVBoxLayout;
  p_channelControlLayout->setContentsMargins(0, 0, 0, 0);

  p_alphaControl = new DoubleControlElement(tr("Alpha:"), 1.0);
  p_alphaControl->setRange(0.0, 1.0);
  p_alphaControl->setSingleStep(0.01);
  p_alphaControl->setSliderViewEnabled(true);
  connect(p_alphaControl, SIGNAL(valueChanged()), SLOT(emitUpdateRequest()));
  p_channelControlLayout->addWidget(p_alphaControl);

  p_visibleControl = new BoolControlElement(tr("Visible:"), true);
  connect(p_visibleControl, SIGNAL(valueChanged()), SLOT(emitUpdateRequest()));
  p_channelControlLayout->addWidget(p_visibleControl);

  p_channelControlWidget = new QWidget;
  p_channelControlWidget->setLayout(p_channelControlLayout);

  _icon = QIcon();
}

ChannelSpecs::~ChannelSpecs()
{}

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
