/**************************************************************************
**       Title: Spheric marker
**    $RCSfile$
**   $Revision: 3883 $ $Name$
**       $Date: 2008-01-21 14:28:13 +0100 (Mon, 21 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
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

// #include <blitz/tinyvec-et.h>

SphereMarkerPresetWidget::SphereMarkerPresetWidget(
    AnnotationChannelSpecs* channel, QWidget* parent)
        : PointMarkerPresetWidget(channel, parent)
{
  p_radiusControl = new DoubleControlElement(tr("Radius:"));
  p_radiusControl->setSwitchable(true);
  p_radiusControl->setActive(false);
  if (channel != NULL && channel->model() != NULL)
  {
    p_radiusControl->setRange(0.01 * channel->model()->elementSize()(1),
                              1000.0 * channel->model()->elementSize()(1));
    p_radiusControl->setSingleStep(0.1 * channel->model()->elementSize()(1));
    p_radiusControl->setValue(10.0 * channel->model()->elementSize()(1));
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
    p_radiusControl->setRange(0.01 * channel->model()->elementSize()(1),
                              1000.0 * channel->model()->elementSize()(1));
    p_radiusControl->setSingleStep(0.1 * channel->model()->elementSize()(1));
    p_radiusControl->setValue(10.0 * channel->model()->elementSize()(1));
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
    if (_selected && p_channel->markerControlWidget() != NULL)
        p_channel->markerControlWidget()->setValues(this);
    update();
  }
}

void SphereMarker::save(const AnnotationChannelSpecs* channel,
                        BlitzH5File& outFile, const std::string& group)
{
  if (channel->markers().size() == 0) return;
  PointMarker::save(channel, outFile, group);
  if (!channel->markers()[0]->inherits(Marker::Sphere)) return;
  blitz::Array<double,1> radii(channel->markers().size());
  ssize_t i = 0;
  for (std::vector<Marker*>::const_iterator it = channel->markers().begin();
       it != channel->markers().end(); ++it, ++i)
  {
    radii(i) = reinterpret_cast<SphereMarker*>(*it)->radiusUm();
  }
  outFile.writeDataSetSimple(radii, group + "/radius_um");
}

void SphereMarker::load(AnnotationChannelSpecs* channel,
                        const BlitzH5File& inFile, const std::string& group)
{
  PointMarker::load(channel, inFile, group);
  ssize_t nMarkers = static_cast<ssize_t>(channel->markers().size());
  if (nMarkers == 0) return;

  blitz::Array<double,1> radii;
  try
  {
    inFile.readDataSetSimple(radii, group + "/radius_um");
    if (radii.size() != nMarkers) 
    {
      blitz::Array<double,1> tmp(nMarkers);
      if (radii.size() < nMarkers)
      {
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (ptrdiff_t i = 0; i < radii.size(); ++i)
            tmp.data()[i] = radii.data()[i];
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (ptrdiff_t i = radii.size(); i < tmp.size(); ++i)
            tmp.data()[i] = -1;
      }
      else
      {
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (ptrdiff_t i = 0; i < tmp.size(); ++i)
            tmp.data()[i] = radii.data()[i];
      }
      radii.resize(tmp.size());
#ifdef _OPENMP
#pragma omp parallel for
#endif
      for (ptrdiff_t i = 0; i < tmp.size(); ++i)
          radii.data()[i] = tmp.data()[i];
    }
  }
  catch (BlitzH5Error&)
  {
    radii.resize(nMarkers);
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (ptrdiff_t i = 0; i < radii.size(); ++i)
        radii.data()[i] = -1;
  }

  for (ssize_t i = 0; i < nMarkers; ++i) 
  {
    SphereMarker* m = reinterpret_cast<SphereMarker*>(channel->markers()[i]);
    m->setRadiusUm(radii(i));
  }
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
  return (blitz::dot(positionUm - _positionUm, positionUm - _positionUm) <
          blitz::pow2(_radiusUm));
}

void SphereMarker::_updateBoundingBox() const
{
  _boundingBoxLowerBoundUm = -radiusUm();
  _boundingBoxUpperBoundUm = radiusUm();
  _boundingBoxUpToDate = true;  
}
