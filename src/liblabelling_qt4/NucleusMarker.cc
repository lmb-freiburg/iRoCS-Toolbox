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

#include "NucleusMarker.hh"

#include <QtGui/QVBoxLayout>

#include "MultiChannelModel.hh"
#include "AnnotationChannelSpecs.hh"

#include "OrthoViewWidget.hh"
#include "NucleusMarkerOrthoViewRenderer.hh"

#include "OpenGlRenderingViewWidget.hh"
#include "NucleusMarkerOpenGlRenderingViewRenderer.hh"

#include "BoolControlElement.hh"
#include "IntControlElement.hh"
#include "DoubleControlElement.hh"

// #include <blitz/tinyvec-et.h>

NucleusMarkerPresetWidget::NucleusMarkerPresetWidget(
    AnnotationChannelSpecs* channel, QWidget* parent)
        : SHSurfaceMarkerPresetWidget(channel, parent)
{
  connect(p_labelControl, SIGNAL(valueChanged()),
          SLOT(updateLabelSuffix()));

  p_subtypeControl = new IntControlElement(tr("Subtype"));
  p_subtypeControl->setSwitchable(true);
  p_subtypeControl->setRange(-1, INT_MAX);
  p_subtypeControl->setSpecialValueText(tr("Unspecified"));
  connect(p_subtypeControl, SIGNAL(valueChanged()),
          SLOT(updateSubtypeSuffix()));
  p_layout->addWidget(p_subtypeControl);

  p_mitoticControl = new BoolControlElement(tr("Mitotic"));
  p_mitoticControl->setSwitchable(true);
  p_layout->addWidget(p_mitoticControl);
  
  p_continuityControl = new IntControlElement(tr("Cell file"));
  p_continuityControl->setSwitchable(true);
  p_continuityControl->setRange(-1, INT_MAX);
  p_continuityControl->setSpecialValueText(tr("Unspecified"));
  p_layout->addWidget(p_continuityControl);
}

NucleusMarkerPresetWidget::~NucleusMarkerPresetWidget()
{}

void NucleusMarkerPresetWidget::getValues(Marker* marker) const 
{
  SHSurfaceMarkerPresetWidget::getValues(marker);
  if (!marker->inherits(Marker::Nucleus)) return;
  NucleusMarker* nc = reinterpret_cast<NucleusMarker*>(marker);
  if (p_subtypeControl->isActive()) nc->setSubtype(p_subtypeControl->value());
  if (p_mitoticControl->isActive()) nc->setMitotic(p_mitoticControl->value());
  if (p_continuityControl->isActive())
      nc->setContinuity(p_continuityControl->value());
}

void NucleusMarkerPresetWidget::setValues(const Marker* marker) 
{
  SHSurfaceMarkerPresetWidget::setValues(marker);
  if (!marker->inherits(Marker::Nucleus)) return;
  const NucleusMarker* nc = reinterpret_cast<const NucleusMarker*>(marker);
  p_subtypeControl->setValue(nc->subtype());
  p_mitoticControl->setValue(nc->mitotic());
  p_continuityControl->setValue(nc->continuity());
}

void NucleusMarkerPresetWidget::updateLabelSuffix()
{
  switch (p_labelControl->value())
  {
  case 0:
    p_labelControl->setSuffix(tr(" (background)"));
    break;
  case 1:
    p_labelControl->setSuffix(tr(" (root cap)"));
    break;
  case 2:
    p_labelControl->setSuffix(tr(" (epidermis)"));
    break;
  case 3:
    p_labelControl->setSuffix(tr(" (cortex)"));
    break;
  case 4:
    p_labelControl->setSuffix(tr(" (endodermis)"));
    break;
  case 5:
    p_labelControl->setSuffix(tr(" (pericycle)"));
    break;
  case 6:
    p_labelControl->setSuffix(tr(" (vasculature)"));
    break;
  case 7:
    p_labelControl->setSuffix(tr(" (QC)"));
    break;
  default:
    p_labelControl->setSuffix("");
  }
  updateSubtypeSuffix();
}

void NucleusMarkerPresetWidget::updateSubtypeSuffix()
{
  QString suffix = "";
  switch (p_labelControl->value())
  {
  case 1:
    if (p_subtypeControl->value() == 0)
        suffix = tr(" (lateral)");
    else if (p_subtypeControl->value() == 1)
        suffix = tr(" (columella)");
    else if (p_subtypeControl->value() == 2)
        suffix = tr(" (columella initial)");
    break;
  case 2:
    if (p_subtypeControl->value() == 0)
        suffix = tr(" (atrichoblast - AT)");
    else if (p_subtypeControl->value() == 1)
        suffix = tr(" (trichoblast - T)");
    else if (p_subtypeControl->value() == 2)
        suffix = tr(" (lateral RC / epidermis initial)");
    break;
  case 3:
    if (p_subtypeControl->value() == 2)
        suffix = tr(" (cortex/endodermis initial)");
    break;
  case 6:
    if (p_subtypeControl->value() == 0)
        suffix = tr(" (procambium)");
    else if (p_subtypeControl->value() == 1)
        suffix = tr(" (metaxylem)");
    else if (p_subtypeControl->value() == 2)
        suffix = tr(" (protoxylem)");
    else if (p_subtypeControl->value() == 3)
        suffix = tr(" (phloem)");
    break;
  }
  p_subtypeControl->setSuffix(suffix);
}


NucleusMarkerControlWidget::NucleusMarkerControlWidget(
    AnnotationChannelSpecs* channel, QWidget* parent)
        : SHSurfaceMarkerControlWidget(channel, parent)
{
  connect(p_labelControl, SIGNAL(valueChanged()),
          SLOT(updateLabelSuffix()));
  connect(p_predictedLabelControl, SIGNAL(valueChanged()),
          SLOT(updatePredictedLabelSuffix()));

  p_subtypeControl = new IntControlElement(tr("Subtype"));
  p_subtypeControl->setRange(-1, INT_MAX);
  p_subtypeControl->setSpecialValueText(tr("Unspecified"));
  connect(p_subtypeControl, SIGNAL(valueChanged()), SIGNAL(contentsChanged()));
  connect(p_subtypeControl, SIGNAL(valueChanged()),
          SLOT(updateSubtypeSuffix()));
  p_layout->addWidget(p_subtypeControl);

  p_predictedSubtypeControl = new IntControlElement(tr("Predicted Subtype"));
  p_predictedSubtypeControl->setEnabled(false);
  p_predictedSubtypeControl->setSpecialValueText(tr("Unspecified"));
  p_predictedSubtypeControl->setVisible(p_showReadOnlyControl->value());
  connect(p_predictedSubtypeControl, SIGNAL(valueChanged()),
          SLOT(updatePredictedSubtypeSuffix()));
  p_layout->addWidget(p_predictedSubtypeControl);

  p_mitoticControl = new BoolControlElement(tr("Mitotic"));
  connect(p_mitoticControl, SIGNAL(valueChanged()), SIGNAL(contentsChanged()));
  p_layout->addWidget(p_mitoticControl);
  
  p_predictedMitoticControl = new BoolControlElement(tr("Predicted Mitotic"));
  p_predictedMitoticControl->setEnabled(false);
  p_predictedMitoticControl->setVisible(p_showReadOnlyControl->value());
  p_layout->addWidget(p_predictedMitoticControl);

  p_continuityControl = new IntControlElement(tr("Cell file"));
  p_continuityControl->setRange(-1, INT_MAX);
  p_continuityControl->setSpecialValueText(tr("Unspecified"));
  connect(p_continuityControl, SIGNAL(valueChanged()),
          SIGNAL(contentsChanged()));
  p_layout->addWidget(p_continuityControl);

  p_predictedContinuityControl = new IntControlElement(
      tr("Predicted cell file"));
  p_predictedContinuityControl->setEnabled(false);
  p_predictedContinuityControl->setSpecialValueText(tr("Unspecified"));
  p_predictedContinuityControl->setVisible(p_showReadOnlyControl->value());
  p_layout->addWidget(p_predictedContinuityControl);
  
  p_valueControl = new DoubleControlElement(tr("Value"));
  p_valueControl->setEnabled(false);
  p_valueControl->setVisible(p_showReadOnlyControl->value());
  p_layout->addWidget(p_valueControl);
  
  p_confidenceControl = new DoubleControlElement(tr("Confidence"));
  p_confidenceControl->setEnabled(false);
  p_confidenceControl->setVisible(p_showReadOnlyControl->value());
  p_layout->addWidget(p_confidenceControl);
  
  p_sPhaseControl = new DoubleControlElement(tr("S-Phase"));
  p_sPhaseControl->setEnabled(false);
  p_sPhaseControl->setVisible(p_showReadOnlyControl->value());
  p_layout->addWidget(p_sPhaseControl);
  
  p_borderDistanceUmControl =
      new DoubleControlElement(tr("Epidermis distance"));
  p_borderDistanceUmControl->setEnabled(false);
  p_borderDistanceUmControl->setVisible(p_showReadOnlyControl->value());
  p_borderDistanceUmControl->setSuffix(tr(" micron"));
  p_layout->addWidget(p_borderDistanceUmControl);
  
  p_volumeUm3Control = new DoubleControlElement(tr("Nucleolus volume"));
  p_volumeUm3Control->setEnabled(false);
  p_volumeUm3Control->setVisible(p_showReadOnlyControl->value());
  p_volumeUm3Control->setSuffix(tr(" micron^3"));
  p_layout->addWidget(p_volumeUm3Control);
}

NucleusMarkerControlWidget::~NucleusMarkerControlWidget()
{}

void NucleusMarkerControlWidget::getValues(Marker* marker) const 
{
  SHSurfaceMarkerControlWidget::getValues(marker);
  if (!marker->inherits(Marker::Nucleus)) return;
  NucleusMarker* nc = reinterpret_cast<NucleusMarker*>(marker);
  nc->setSubtype(p_subtypeControl->value());
  nc->setMitotic(p_mitoticControl->value());
  nc->setContinuity(p_continuityControl->value());
}

void NucleusMarkerControlWidget::setValues(const Marker* marker) 
{
  SHSurfaceMarkerControlWidget::setValues(marker);
  if (!marker->inherits(Marker::Nucleus)) return;
  const NucleusMarker* nc = reinterpret_cast<const NucleusMarker*>(marker);

  if (nc->subtype() != p_subtypeControl->value())
  {
    disconnect(p_subtypeControl, SIGNAL(valueChanged()),
               this, SIGNAL(contentsChanged()));
    p_subtypeControl->setValue(nc->subtype());
    connect(p_subtypeControl, SIGNAL(valueChanged()),
            SIGNAL(contentsChanged()));
  }
  
  if (nc->predictedSubtype() != p_predictedSubtypeControl->value())
      p_predictedSubtypeControl->setValue(nc->predictedSubtype());

  if (nc->mitotic() != p_mitoticControl->value())
  {
    disconnect(p_mitoticControl, SIGNAL(valueChanged()),
               this, SIGNAL(contentsChanged()));
    p_mitoticControl->setValue(nc->mitotic());
    connect(p_mitoticControl, SIGNAL(valueChanged()),
            SIGNAL(contentsChanged()));
  }
  
  if (nc->predictedMitotic() != p_predictedMitoticControl->value())
      p_predictedMitoticControl->setValue(nc->predictedMitotic());

  if (nc->continuity() != p_continuityControl->value())
  {
    disconnect(p_continuityControl, SIGNAL(valueChanged()),
               this, SIGNAL(contentsChanged()));
    p_continuityControl->setValue(nc->continuity());
    connect(p_continuityControl, SIGNAL(valueChanged()),
            SIGNAL(contentsChanged()));
  }
  
  if (nc->predictedContinuity() != p_predictedContinuityControl->value())
      p_predictedContinuityControl->setValue(nc->predictedContinuity());

  if (nc->value() != p_valueControl->value())
      p_valueControl->setValue(nc->value());

  if (nc->confidence() != p_confidenceControl->value())
      p_confidenceControl->setValue(nc->confidence());

  if (nc->sPhase() != p_sPhaseControl->value())
      p_sPhaseControl->setValue(nc->sPhase());

  if (nc->borderDistanceUm() != p_borderDistanceUmControl->value())
      p_borderDistanceUmControl->setValue(nc->borderDistanceUm());

  if (nc->volumeUm3() != p_volumeUm3Control->value())
      p_volumeUm3Control->setValue(nc->volumeUm3());
}

void NucleusMarkerControlWidget::toggleReadOnly()
{
  SHSurfaceMarkerControlWidget::toggleReadOnly();
  p_predictedSubtypeControl->setVisible(p_showReadOnlyControl->value());
  p_predictedMitoticControl->setVisible(p_showReadOnlyControl->value());
  p_predictedContinuityControl->setVisible(p_showReadOnlyControl->value());
  p_valueControl->setVisible(p_showReadOnlyControl->value());
  p_confidenceControl->setVisible(p_showReadOnlyControl->value());
  p_sPhaseControl->setVisible(p_showReadOnlyControl->value());
  p_borderDistanceUmControl->setVisible(p_showReadOnlyControl->value());
  p_volumeUm3Control->setVisible(p_showReadOnlyControl->value());
}

void NucleusMarkerControlWidget::updateLabelSuffix()
{
  switch (p_labelControl->value())
  {
  case 0:
    p_labelControl->setSuffix(tr(" (background)"));
    break;
  case 1:
    p_labelControl->setSuffix(tr(" (root cap)"));
    break;
  case 2:
    p_labelControl->setSuffix(tr(" (epidermis)"));
    break;
  case 3:
    p_labelControl->setSuffix(tr(" (cortex)"));
    break;
  case 4:
    p_labelControl->setSuffix(tr(" (endodermis)"));
    break;
  case 5:
    p_labelControl->setSuffix(tr(" (pericycle)"));
    break;
  case 6:
    p_labelControl->setSuffix(tr(" (vasculature)"));
    break;
  case 7:
    p_labelControl->setSuffix(tr(" (QC)"));
    break;
  default:
    p_labelControl->setSuffix("");
  }
  updateSubtypeSuffix();
}

void NucleusMarkerControlWidget::updatePredictedLabelSuffix()
{
  switch (p_predictedLabelControl->value())
  {
  case 0:
    p_predictedLabelControl->setSuffix(tr(" (background)"));
    break;
  case 1:
    p_predictedLabelControl->setSuffix(tr(" (root cap)"));
    break;
  case 2:
    p_predictedLabelControl->setSuffix(tr(" (epidermis)"));
    break;
  case 3:
    p_predictedLabelControl->setSuffix(tr(" (cortex)"));
    break;
  case 4:
    p_predictedLabelControl->setSuffix(tr(" (endodermis)"));
    break;
  case 5:
    p_predictedLabelControl->setSuffix(tr(" (pericycle)"));
    break;
  case 6:
    p_predictedLabelControl->setSuffix(tr(" (vasculature)"));
    break;
  case 7:
    p_predictedLabelControl->setSuffix(tr(" (QC)"));
    break;
  default:
    p_predictedLabelControl->setSuffix("");
  }
  updatePredictedSubtypeSuffix();
}

void NucleusMarkerControlWidget::updateSubtypeSuffix()
{
  QString suffix = "";
  switch (p_labelControl->value())
  {
  case 1:
    if (p_subtypeControl->value() == 0)
        suffix = tr(" (lateral)");
    else if (p_subtypeControl->value() == 1)
        suffix = tr(" (columella)");
    else if (p_subtypeControl->value() == 2)
        suffix = tr(" (columella initial)");
    break;
  case 2:
    if (p_subtypeControl->value() == 0)
        suffix = tr(" (atrichoblast - AT)");
    else if (p_subtypeControl->value() == 1)
        suffix = tr(" (trichoblast - T)");
    else if (p_subtypeControl->value() == 2)
        suffix = tr(" (lateral RC / epidermis initial)");
    break;
  case 3:
    if (p_subtypeControl->value() == 2)
        suffix = tr(" (cortex/endodermis initial)");
    break;
  case 6:
    if (p_subtypeControl->value() == 0)
        suffix = tr(" (procambium)");
    else if (p_subtypeControl->value() == 1)
        suffix = tr(" (metaxylem)");
    else if (p_subtypeControl->value() == 2)
        suffix = tr(" (protoxylem)");
    else if (p_subtypeControl->value() == 3)
        suffix = tr(" (phloem)");
    break;
  }
  p_subtypeControl->setSuffix(suffix);
}

void NucleusMarkerControlWidget::updatePredictedSubtypeSuffix()
{
  QString suffix = "";
  switch (p_predictedLabelControl->value())
  {
  case 1:
    if (p_predictedSubtypeControl->value() == 0)
        suffix = tr(" (lateral)");
    else if (p_predictedSubtypeControl->value() == 1)
        suffix = tr(" (columella)");
    else if (p_predictedSubtypeControl->value() == 2)
        suffix = tr(" (columella initial)");
    break;
  case 2:
    if (p_predictedSubtypeControl->value() == 0)
        suffix = tr(" (atrichoblast - AT)");
    else if (p_predictedSubtypeControl->value() == 1)
        suffix = tr(" (trichoblast - T)");
    else if (p_predictedSubtypeControl->value() == 2)
        suffix = tr(" (lateral RC / epidermis initial)");
    break;
  case 3:
    if (p_predictedSubtypeControl->value() == 2)
        suffix = tr(" (cortex/endodermis initial)");
    break;
  case 6:
    if (p_predictedSubtypeControl->value() == 0)
        suffix = tr(" (procambium)");
    else if (p_predictedSubtypeControl->value() == 1)
        suffix = tr(" (metaxylem)");
    else if (p_predictedSubtypeControl->value() == 2)
        suffix = tr(" (protoxylem)");
    else if (p_predictedSubtypeControl->value() == 3)
        suffix = tr(" (phloem)");
    break;
  }
  p_predictedSubtypeControl->setSuffix(suffix);
}


NucleusMarker::NucleusMarker(AnnotationChannelSpecs* channel)
        : SHSurfaceMarker(channel), _value(-1), _confidence(-1), _sphase(-1),
          _borderDistance(-1), _volume(-1), _mitotic(false),
          _predictedMitotic(false), _subtype(-1), _predictedSubtype(-1),
          _continuity(-1), _predictedContinuity(-1)
{}
  
NucleusMarker::NucleusMarker(
    blitz::TinyVector<double,3> const& position, double radius,
    segmentation::FrequencyArray const &coefficients,
    AnnotationChannelSpecs* channel)
        : SHSurfaceMarker(position, radius, coefficients, channel),
          _value(-1), _confidence(-1), _sphase(-1), _borderDistance(-1),
          _volume(-1), _mitotic(false), _predictedMitotic(false),
          _subtype(-1), _predictedSubtype(-1), _continuity(-1),
          _predictedContinuity(-1)
{}
  
NucleusMarker::NucleusMarker(const NucleusMarker& marker)
        : SHSurfaceMarker(marker), _value(marker._value),
          _confidence(marker._confidence), _sphase(marker._sphase),
          _borderDistance(marker._borderDistance), _volume(marker._volume),
          _mitotic(marker._mitotic),
          _predictedMitotic(marker._predictedMitotic),
          _subtype(marker._subtype),
          _predictedSubtype(marker._predictedSubtype), 
          _continuity(marker._continuity),
          _predictedContinuity(marker._predictedContinuity)
{}
  
NucleusMarker::~NucleusMarker() 
{}

NucleusMarker &NucleusMarker::operator=(const NucleusMarker& marker)
{
  SHSurfaceMarker::operator=(static_cast<SHSurfaceMarker const &>(marker));
  _value = marker._value;
  _confidence = marker._confidence;
  _sphase = marker._sphase;
  _borderDistance = marker._borderDistance;
  _volume = marker._volume;
  _mitotic = marker._mitotic;
  _predictedMitotic = marker._predictedMitotic;
  _subtype = marker._subtype;
  _predictedSubtype = marker._predictedSubtype;
  _continuity = marker._continuity;
  _predictedContinuity = marker._predictedContinuity;
  return *this;
}

Marker::MarkerType NucleusMarker::markerType() const 
{
  return Marker::Nucleus;
}

bool NucleusMarker::inherits(Marker::MarkerType type) const
{
  return type == Marker::Nucleus || SHSurfaceMarker::inherits(type);
}

MarkerRenderer *NucleusMarker::addRenderer(ViewWidget* view)
{ 
  if (_renderers.find(view) != _renderers.end()) return _renderers[view];
  
  switch (view->viewType())
  {
  case ViewWidget::OrthoView:
    _renderers[view] = new NucleusMarkerOrthoViewRenderer(
        this, static_cast<OrthoViewWidget*>(view));
    return _renderers[view];
  case ViewWidget::OpenGlRenderingView:
    _renderers[view] = new NucleusMarkerOpenGlRenderingViewRenderer(
        this, static_cast<OpenGlRenderingViewWidget*>(view));
    return _renderers[view];
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": Implementation missing"
              << std::endl;
    break;
  }
  return NULL;
}

int NucleusMarker::subtype() const
{
  return _subtype;
}

void NucleusMarker::setSubtype(const int subtype)
{
  if (_subtype == subtype) return;
  _subtype = subtype;
  if (p_channel != NULL)
  {
    if (selected() && p_channel->markerControlWidget() != NULL)
        p_channel->markerControlWidget()->setValues(this);
    if (p_channel->model() != NULL)
    {
      p_channel->model()->setModified(true);
      update();
    }
  }
}

int NucleusMarker::predictedSubtype() const
{
  return _predictedSubtype;
}

void NucleusMarker::setPredictedSubtype(const int subtype)
{
  if (_predictedSubtype == subtype) return;
  _predictedSubtype = subtype;
  if (p_channel != NULL)
  {
    if (selected() && p_channel->markerControlWidget() != NULL)
        p_channel->markerControlWidget()->setValues(this);
    if (p_channel->model() != NULL)
    {
      p_channel->model()->setModified(true);
      update();
    }
  }  
}
  
double NucleusMarker::value() const
{
  return _value;
}

void NucleusMarker::setValue(const double value)
{
  if (_value == value) return;
  _value = value;
  if (p_channel != NULL)
  {
    if (selected() && p_channel->markerControlWidget() != NULL)
        p_channel->markerControlWidget()->setValues(this);
    if (p_channel->model() != NULL) p_channel->model()->setModified(true);
  }
}
  
double NucleusMarker::confidence() const
{
  return _confidence;
}

void NucleusMarker::setConfidence(const double confidence)
{
  if (_confidence == confidence) return;
  _confidence = confidence;
  if (p_channel != NULL)
  {
    if (selected() && p_channel->markerControlWidget() != NULL)
        p_channel->markerControlWidget()->setValues(this);
    if (p_channel->model() != NULL) p_channel->model()->setModified(true);
  }
}
  
double NucleusMarker::sPhase() const
{
  return _sphase;
}

void NucleusMarker::setSPhase(const double sPhase)
{
  if (_sphase == sPhase) return;
  _sphase = sPhase;
  if (p_channel != NULL)
  {
    if (selected() && p_channel->markerControlWidget() != NULL)
        p_channel->markerControlWidget()->setValues(this);
    if (p_channel->model() != NULL) p_channel->model()->setModified(true);
  }
}
  
bool NucleusMarker::mitotic() const
{
  return _mitotic;
}

void NucleusMarker::setMitotic(const bool mitotic)
{
  if (_mitotic == mitotic) return;
  _mitotic = mitotic;
  if (p_channel != NULL)
  {
    if (selected() && p_channel->markerControlWidget() != NULL)
        p_channel->markerControlWidget()->setValues(this);
    if (p_channel->model() != NULL)
    {
      p_channel->model()->setModified(true);
      update();
    }
  }
}
  
bool NucleusMarker::predictedMitotic() const
{
  return _predictedMitotic;
}

void NucleusMarker::setPredictedMitotic(const bool mitotic)
{
  if (_predictedMitotic == mitotic) return;
  _predictedMitotic = mitotic;
  if (p_channel != NULL)
  {
    if (selected() && p_channel->markerControlWidget() != NULL)
        p_channel->markerControlWidget()->setValues(this);
    if (p_channel->model() != NULL)
    {
      p_channel->model()->setModified(true);
      update();
    }
  }
}
  
int NucleusMarker::continuity() const
{
  return _continuity;
}

void NucleusMarker::setContinuity(const int continuity)
{
  if (_continuity == continuity) return;
  _continuity = continuity;
  if (p_channel != NULL)
  {
    if (selected() && p_channel->markerControlWidget() != NULL)
        p_channel->markerControlWidget()->setValues(this);
    if (p_channel->model() != NULL)
    {
      p_channel->model()->setModified(true);
      update();
    }
  }
}
  
int NucleusMarker::predictedContinuity() const
{
  return _predictedContinuity;
}

void NucleusMarker::setPredictedContinuity(const int continuity)
{
  if (_predictedContinuity == continuity) return;
  _predictedContinuity = continuity;
  if (p_channel != NULL)
  {
    if (selected() && p_channel->markerControlWidget() != NULL)
        p_channel->markerControlWidget()->setValues(this);
    if (p_channel->model() != NULL)
    {
      p_channel->model()->setModified(true);
      update();
    }
  }
}
  
double NucleusMarker::borderDistanceUm() const
{
  return _borderDistance;
}

void NucleusMarker::setBorderDistanceUm(const double borderDistanceUm)
{
  if (_borderDistance == borderDistanceUm) return;
  _borderDistance = borderDistanceUm;
  if (p_channel != NULL)
  {
    if (selected() && p_channel->markerControlWidget() != NULL)
        p_channel->markerControlWidget()->setValues(this);
    if (p_channel->model() != NULL) p_channel->model()->setModified(true);
  }
}
  
double NucleusMarker::volumeUm3() const
{
  return _volume;
}

void NucleusMarker::setVolumeUm3(const double volumeUm3)
{
  if (_volume == volumeUm3) return;
  _volume = volumeUm3;
  if (p_channel != NULL)
  {
    if (selected() && p_channel->markerControlWidget() != NULL)
        p_channel->markerControlWidget()->setValues(this);
    if (p_channel->model() != NULL) p_channel->model()->setModified(true);
  }
}

bool NucleusMarker::operator<(const NucleusMarker& rhs) const
{
  return _value < rhs.value();
}

void NucleusMarker::save(
    AnnotationChannelSpecs const *channel, BlitzH5File &outFile,
    std::string const &group, iRoCS::ProgressReporter *pr)
{
  if (channel->markers().size() == 0) return;

  int pStart = (pr != NULL) ? pr->taskProgressMin() : 0;
  int pMax = (pr != NULL) ? pr->taskProgressMax() : 1;
  int pRange = pMax - pStart;

  if (pr != NULL)
      pr->setTaskProgressMax(pStart + static_cast<int>(0.4f * pRange));
  SHSurfaceMarker::save(channel, outFile, group, pr);
  if (pr != NULL)
  {
    pStart = pr->taskProgressMax();
    pr->setTaskProgressMin(pStart);
    pr->setTaskProgressMax(pMax);
    pRange = pMax - pStart;
    if (pr->isAborted()) return;
  }
  if (!channel->markers()[0]->inherits(Marker::Nucleus)) return;
  blitz::Array<int,1> subtypes(channel->markers().size());
  blitz::Array<int,1> predictedsubtypes(channel->markers().size());
  blitz::Array<double,1> values(channel->markers().size());
  blitz::Array<double,1> confidences(channel->markers().size());
  blitz::Array<double,1> sphases(channel->markers().size());
  blitz::Array<double,1> borderdistances(channel->markers().size());
  blitz::Array<double,1> volumes(channel->markers().size());
  blitz::Array<unsigned char,1> mitotics(channel->markers().size());
  blitz::Array<unsigned char,1> predictedmitotics(channel->markers().size());
  blitz::Array<int,1> continuities(channel->markers().size());
  blitz::Array<int,1> predictedcontinuities(channel->markers().size());
  ssize_t i = 0;
  for (std::vector<Marker*>::const_iterator it = channel->markers().begin();
       it != channel->markers().end(); ++it, ++i)
  {
    NucleusMarker* m = reinterpret_cast<NucleusMarker*>(*it);
    subtypes(i) = m->subtype();
    predictedsubtypes(i) = m->predictedSubtype();
    values(i) = m->value();
    confidences(i) = m->confidence();
    sphases(i) = m->sPhase();
    borderdistances(i) = m->borderDistanceUm();
    volumes(i) = m->volumeUm3();
    mitotics(i) = (m->mitotic()) ? 1 : 0;
    predictedmitotics(i) = (m->predictedMitotic()) ? 1 : 0;
    continuities(i) = m->continuity();
    predictedcontinuities(i) = m->predictedContinuity();
  }
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.1f * pRange))) return;
  outFile.writeDataset(subtypes, group + "/subtype");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.18f * pRange))) return;
  outFile.writeDataset(predictedsubtypes, group + "/predicted_subtype");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.26f * pRange))) return;
  outFile.writeDataset(values, group + "/value");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.34f * pRange))) return;
  outFile.writeDataset(confidences, group + "/confidence");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.42f * pRange))) return;
  outFile.writeDataset(sphases, group + "/sphase");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.50f * pRange))) return;
  outFile.writeDataset(borderdistances, group + "/borderDistance_um");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.58f * pRange))) return;
  outFile.writeDataset(volumes, group + "/volume_um3");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.66f * pRange))) return;
  outFile.writeDataset(mitotics, group + "/mitotic");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.74f * pRange))) return;
  outFile.writeDataset(predictedmitotics, group + "/predicted_mitotic");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.82f * pRange))) return;
  outFile.writeDataset(continuities, group + "/continuity");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.90f * pRange))) return;
  outFile.writeDataset(predictedcontinuities,
                             group + "/predicted_continuity");
  if (pr != NULL && !pr->updateProgress(pStart + pRange)) return;
}

void NucleusMarker::load(
    AnnotationChannelSpecs *channel, BlitzH5File const &inFile,
    std::string const &group, iRoCS::ProgressReporter *pr)
{

  int pStart = (pr != NULL) ? pr->taskProgressMin() : 0;
  int pMax = (pr != NULL) ? pr->taskProgressMax() : 1;
  int pRange = pMax - pStart;

  if (pr != NULL)
      pr->setTaskProgressMax(pStart + static_cast<int>(0.4f * pRange));
  SHSurfaceMarker::load(channel, inFile, group, pr);
  if (pr != NULL)
  {
    pStart = pr->taskProgressMax();
    pr->setTaskProgressMin(pStart);
    pr->setTaskProgressMax(pMax);
    pRange = pMax - pStart;
    if (pr->isAborted()) return;
  }
  atb::BlitzIndexT nMarkers =
      static_cast<atb::BlitzIndexT>(channel->markers().size());
  if (nMarkers == 0) return;

  blitz::Array<int,1> subtypes(nMarkers);
  _loadFixedSize(subtypes, inFile, group + "/subtype");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.05f * pRange))) return;

  blitz::Array<int,1> predictedsubtypes(nMarkers);
  _loadFixedSize(predictedsubtypes, inFile, group + "/predicted_subtype");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.1f * pRange))) return;

  blitz::Array<double,1> values(nMarkers);
  _loadFixedSize(values, inFile, group + "/value");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.15f * pRange))) return;

  blitz::Array<double,1> confidences(nMarkers);
  _loadFixedSize(confidences, inFile, group + "/confidence");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.2f * pRange))) return;

  blitz::Array<double,1> sphases(nMarkers);
  _loadFixedSize(sphases, inFile, group + "/sphase");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.25f * pRange))) return;

  blitz::Array<double,1> borderdistances(nMarkers);
  _loadFixedSize(borderdistances, inFile, group + "/borderDistance_um");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.3f * pRange))) return;

  blitz::Array<double,1> volumes(nMarkers);
  _loadFixedSize(volumes, inFile, group + "/volume_um3");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.35f * pRange))) return;

  blitz::Array<bool,1> mitotics(nMarkers);
  _loadFixedSize(mitotics, inFile, group + "/mitotic");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.4f * pRange))) return;

  blitz::Array<bool,1> predictedmitotics(nMarkers);
  _loadFixedSize(predictedmitotics, inFile, group + "/predicted_mitotic");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.45f * pRange))) return;

  blitz::Array<int,1> continuities(nMarkers);
  _loadFixedSize(continuities, inFile, group + "/continuity");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.5f * pRange))) return;

  blitz::Array<int,1> predictedcontinuities(nMarkers);
  _loadFixedSize(
      predictedcontinuities, inFile, group + "/predicted_continuity");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.55f * pRange))) return;

  for (atb::BlitzIndexT i = 0; i < nMarkers; ++i) 
  {
    if (pr != NULL && !pr->updateProgress(
            pStart + static_cast<int>(
                (0.55f + 0.45f * static_cast<float>(i) /
                 static_cast<float>(nMarkers - 1)) * pRange)))
        return;
    NucleusMarker* m = reinterpret_cast<NucleusMarker*>(channel->markers()[i]);
    m->setSubtype(subtypes(i));
    m->setPredictedSubtype(predictedsubtypes(i));
    m->setValue(values(i));
    m->setConfidence(confidences(i));
    m->setSPhase(sphases(i));
    m->setBorderDistanceUm(borderdistances(i));
    m->setVolumeUm3(volumes(i));
    m->setMitotic(mitotics(i));
    m->setPredictedMitotic(predictedmitotics(i));
    m->setContinuity(continuities(i));
    m->setPredictedContinuity(predictedcontinuities(i));
  }
}

void NucleusMarker::writeCSVHeader(std::ofstream &out) const
{
  SHSurfaceMarker::writeCSVHeader(out);
  out << ";subtype;predicted subtype;mitotic;predicted mitotic;cellfile;"
      << "predicted cellfile;value;confidence;sphase;"
      << "distance from border (micron);volume (cube microns)";
}

void NucleusMarker::saveCSV(std::ofstream &out) const
{
  SHSurfaceMarker::saveCSV(out);
  out << ";" << _subtype << ";" << _predictedSubtype << ";"
      << ((_mitotic) ? 1 : 0) << ";" << ((_predictedMitotic) ? 1 : 0)
      << ";" << _continuity << ";" << _predictedContinuity << ";" << _value
      << ";" << _confidence << ";" << _sphase << ";" << _borderDistance << ";"
      << _volume;
}

void NucleusMarker::loadFromMap(std::map<std::string,std::string> const &values)
{
  SHSurfaceMarker::loadFromMap(values);
  _subtype = atoi(values.find("subtype")->second.c_str());
  _predictedSubtype = atoi(
      values.find("predicted subtype")->second.c_str());
  _mitotic = values.find("mitotic")->second != "0";
  _predictedMitotic = values.find("predicted mitotic")->second != "0";
  _continuity = atoi(values.find("cellfile")->second.c_str());
  _predictedContinuity = atoi(
      values.find("predicted cellfile")->second.c_str());
  _value = atof(values.find("value")->second.c_str());
  _confidence = atof(values.find("confidence")->second.c_str());
  _sphase = values.find("sphase")->second != "0";
  _borderDistance = atof(
      values.find("distance from border (micron)")->second.c_str());
  _volume = atof(values.find("volume (cube microns)")->second.c_str());
}

void NucleusMarker::copyToATBNucleus(atb::Nucleus &nc) const
{
  SHSurfaceMarker::copyToATBNucleus(nc);
  nc.setValue(_value);
  nc.setConfidence(_confidence);
  nc.setSphase(_sphase);
  nc.setBorderDistanceUm(_borderDistance);
  nc.setVolumeUm3(_volume);
  nc.setMitotic(_mitotic);
  nc.setPredictedMitotic(_predictedMitotic);
  nc.setSubtype(_subtype);
  nc.setPredictedSubtype(_predictedSubtype);
  nc.setCellfile(_continuity);
  nc.setPredictedCellfile(_predictedContinuity);
}

void NucleusMarker::copyFromATBNucleus(atb::Nucleus const &nc)
{
  SHSurfaceMarker::copyFromATBNucleus(nc);
  _value = nc.value();
  _confidence = nc.confidence();
  _sphase = nc.sphase();
  _borderDistance = nc.borderDistanceUm();
  _volume = nc.volumeUm3();
  _mitotic = nc.mitotic();
  _predictedMitotic = nc.predictedMitotic();
  _subtype = nc.subtype();
  _predictedSubtype = nc.predictedSubtype();
  _continuity = nc.cellfile();
  _predictedContinuity = nc.predictedCellfile();
}

