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

#include "SphereMarker.hh"

#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>

#include "MultiChannelModel.hh"
#include "AnnotationChannelSpecs.hh"

#include "OrthoViewWidget.hh"
#include "SphereMarkerOrthoViewRenderer.hh"

#include "OpenGlRenderingViewWidget.hh"
#include "SphereMarkerOpenGlRenderingViewRenderer.hh"

#include "DoubleControlElement.hh"

SphereMarkerPresetWidget::SphereMarkerPresetWidget(
    AnnotationChannelSpecs* channel, QWidget* parent)
        : PointMarkerPresetWidget(channel, parent)
{
  p_radiusControl = new DoubleControlElement(tr("Radius:"));
  p_radiusControl->setSwitchable(true);
  p_radiusControl->setActive(false);
  if (channel != NULL && channel->model() != NULL)
  {
    p_radiusControl->setRange(0.01 * channel->model()->elementSizeUm()(1),
                              1000.0 * channel->model()->elementSizeUm()(1));
    p_radiusControl->setSingleStep(0.1 * channel->model()->elementSizeUm()(1));
    p_radiusControl->setValue(10.0 * channel->model()->elementSizeUm()(1));
  }
  p_layout->addWidget(p_radiusControl);
}

SphereMarkerPresetWidget::~SphereMarkerPresetWidget()
{}

void SphereMarkerPresetWidget::getValues(Marker* marker) const 
{
  PointMarkerPresetWidget::getValues(marker);
  if (marker == NULL || !marker->inherits(Marker::Sphere)) return;
  SphereMarker* s = reinterpret_cast<SphereMarker*>(marker);
  if (p_radiusControl->isActive()) s->setRadiusUm(p_radiusControl->value());
}

void SphereMarkerPresetWidget::setValues(const Marker* marker)
{
  PointMarkerPresetWidget::setValues(marker);
  if (marker == NULL || !marker->inherits(Marker::Sphere)) return;
  const SphereMarker* s = reinterpret_cast<const SphereMarker*>(marker);
  p_radiusControl->setValue(s->radiusUm());
}


SphereMarkerControlWidget::SphereMarkerControlWidget(
    AnnotationChannelSpecs* channel, QWidget* parent)
        : PointMarkerControlWidget(channel, parent)
{
  p_radiusControl = new DoubleControlElement(tr("Radius"));
  if (channel != NULL && channel->model() != NULL)
  {
    p_radiusControl->setRange(0.01 * channel->model()->elementSizeUm()(1),
                              1000.0 * channel->model()->elementSizeUm()(1));
    p_radiusControl->setSingleStep(0.1 * channel->model()->elementSizeUm()(1));
    p_radiusControl->setValue(10.0 * channel->model()->elementSizeUm()(1));
  }
  connect(p_radiusControl, SIGNAL(valueChanged()), SIGNAL(contentsChanged()));
  p_layout->addWidget(p_radiusControl);
}

SphereMarkerControlWidget::~SphereMarkerControlWidget()
{}

void SphereMarkerControlWidget::getValues(Marker* marker) const 
{
  PointMarkerControlWidget::getValues(marker);
  if (!marker->inherits(Marker::Sphere)) return;
  SphereMarker* s = reinterpret_cast<SphereMarker*>(marker);
  s->setRadiusUm(p_radiusControl->value());
}

void SphereMarkerControlWidget::setValues(const Marker* marker)
{
  PointMarkerControlWidget::setValues(marker);
  if (!marker->inherits(Marker::Sphere)) return;
  const SphereMarker* s = reinterpret_cast<const SphereMarker*>(marker);
  disconnect(p_radiusControl, SIGNAL(valueChanged()),
             this, SIGNAL(contentsChanged()));
  p_radiusControl->setValue(s->radiusUm());
  connect(p_radiusControl, SIGNAL(valueChanged()), SIGNAL(contentsChanged()));
}


SphereMarker::SphereMarker(AnnotationChannelSpecs* channel)
        : PointMarker(channel), _radiusUm(5.0)
{}

SphereMarker::SphereMarker(const blitz::TinyVector<double,3>& position,
                           const double radius, AnnotationChannelSpecs* channel)
        : PointMarker(position, channel), _radiusUm(radius)
{}

SphereMarker::SphereMarker(const SphereMarker& marker)
        : PointMarker(marker), _radiusUm(marker.radiusUm())
{}

SphereMarker::~SphereMarker() 
{}

SphereMarker &SphereMarker::operator=(const SphereMarker& marker)
{
  PointMarker::operator=(static_cast<PointMarker const &>(marker));
  _radiusUm = marker._radiusUm;
  return *this;
}

Marker::MarkerType SphereMarker::markerType() const 
{
  return Marker::Sphere;
}

bool SphereMarker::inherits(Marker::MarkerType type) const
{
  return type == Marker::Sphere || PointMarker::inherits(type);
}

MarkerRenderer *SphereMarker::addRenderer(ViewWidget* view)
{ 
  if (_renderers.find(view) != _renderers.end()) return _renderers[view];
  
  switch (view->viewType())
  {
  case ViewWidget::OrthoView:
    _renderers[view] = new SphereMarkerOrthoViewRenderer(
        this, static_cast<OrthoViewWidget*>(view));
    return _renderers[view];
  case ViewWidget::OpenGlRenderingView:
    _renderers[view] = new SphereMarkerOpenGlRenderingViewRenderer(
        this, static_cast<OpenGlRenderingViewWidget*>(view));
    return _renderers[view];
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": Implementation missing"
              << std::endl;
    break;
  }
  return NULL;
}

double SphereMarker::radiusUm() const 
{
  return _radiusUm;
}

void SphereMarker::setRadiusUm(const double radius) 
{
  if (_radiusUm == radius) return;
  _radiusUm = radius;
  _boundingBoxUpToDate = false;
  if (p_channel != NULL && p_channel->model() != NULL)
  {
    p_channel->model()->setModified(true);
    if (selected() && p_channel->markerControlWidget() != NULL)
        p_channel->markerControlWidget()->setValues(this);
    update();
  }
}

void SphereMarker::save(
    AnnotationChannelSpecs const *channel,
    BlitzH5File &outFile, std::string const &group,
    iRoCS::ProgressReporter *pr)
{
  if (channel->markers().size() == 0) return;

  int pStart = (pr != NULL) ? pr->taskProgressMin() : 0;
  int pMax = (pr != NULL) ? pr->taskProgressMax() : 1;
  int pRange = pMax - pStart;

  if (pr != NULL)
      pr->setTaskProgressMax(pStart + static_cast<int>(0.9f * pRange));
  PointMarker::save(channel, outFile, group, pr);
  if (pr != NULL)
  {
    pStart = pr->taskProgressMax();
    pr->setTaskProgressMin(pStart);
    pr->setTaskProgressMax(pMax);
    pRange = pMax - pStart;
    if (pr->isAborted()) return;
  }
  if (!channel->markers()[0]->inherits(Marker::Sphere)) return;

  blitz::Array<double,1> radii(channel->markers().size());
  ssize_t i = 0;
  for (std::vector<Marker*>::const_iterator it = channel->markers().begin();
       it != channel->markers().end(); ++it, ++i)
  {
    radii(i) = reinterpret_cast<SphereMarker*>(*it)->radiusUm();
  }
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.50f * pRange))) return;
  outFile.writeDataset(radii, group + "/radius_um");
  if (pr != NULL && !pr->updateProgress(pStart + pRange)) return;
}

void SphereMarker::load(
    AnnotationChannelSpecs *channel, BlitzH5File const &inFile,
    std::string const &group, iRoCS::ProgressReporter *pr)
{
  int pStart = (pr != NULL) ? pr->taskProgressMin() : 0;
  int pMax = (pr != NULL) ? pr->taskProgressMax() : 1;
  int pRange = pMax - pStart;

  if (pr != NULL)
      pr->setTaskProgressMax(pStart + static_cast<int>(0.9f * pRange));
  PointMarker::load(channel, inFile, group, pr);
  if (pr != NULL)
  {
    pStart = pr->taskProgressMax();
    pr->setTaskProgressMin(pStart);
    pr->setTaskProgressMax(pMax);
    pRange = pMax - pStart;
    if (pr->isAborted()) return;
  }
  size_t nMarkers = static_cast<size_t>(channel->markers().size());

  if (nMarkers == 0) return;

  blitz::Array<double,1> radii(nMarkers);
  _loadFixedSize(radii, inFile, group + "/radius_um");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.5f * pRange))) return;

  for (size_t i = 0; i < nMarkers; ++i) 
  {
    SphereMarker* m = reinterpret_cast<SphereMarker*>(channel->markers()[i]);
    m->setRadiusUm(radii(i));
  }
  if (pr != NULL && !pr->updateProgress(pStart + pRange)) return;
}

void SphereMarker::writeCSVHeader(std::ofstream &out) const
{
  Marker::writeCSVHeader(out);
  out << ";radius (micron)";
}

void SphereMarker::saveCSV(std::ofstream &out) const
{
  PointMarker::saveCSV(out);
  out << ";" << _radiusUm;
}

void SphereMarker::loadFromMap(std::map<std::string,std::string> const &values)
{
  PointMarker::loadFromMap(values);
  _radiusUm = atof(values.find("radius (micron)")->second.c_str());
}

bool SphereMarker::occupiesPositionUm(
    blitz::TinyVector<double,3> const &positionUm) const
{
  if (blitz::any(positionUm < boundingBoxLowerBoundUm() ||
                 positionUm > boundingBoxUpperBoundUm())) return false;
  return (blitz::dot(positionUm - this->positionUm(),
                     positionUm - this->positionUm()) < blitz::pow2(_radiusUm));
}

void SphereMarker::copyToATBNucleus(atb::Nucleus &nc) const
{
  PointMarker::copyToATBNucleus(nc);
  nc.setRadiusUm(_radiusUm);
}

void SphereMarker::copyFromATBNucleus(atb::Nucleus const &nc)
{
  PointMarker::copyFromATBNucleus(nc);
  _radiusUm = nc.radiusUm();
}

void SphereMarker::_updateBoundingBox() const
{
  _boundingBoxLowerBoundUm = -radiusUm();
  _boundingBoxUpperBoundUm = radiusUm();
  _boundingBoxUpToDate = true;  
}
