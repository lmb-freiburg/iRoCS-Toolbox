/**************************************************************************
**       Title: Single Point marker
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#include "PointMarker.hh"

#include "AnnotationChannelSpecs.hh"

#include "OrthoViewWidget.hh"
#include "PointMarkerOrthoViewRenderer.hh"

#include "OpenGlRenderingViewWidget.hh"
#include "PointMarkerOpenGlRenderingViewRenderer.hh"

// #include <blitz/tinyvec-et.h>

PointMarkerPresetWidget::PointMarkerPresetWidget(
    AnnotationChannelSpecs* channel, QWidget* parent) 
        : MarkerPresetWidget(channel, parent)
{}

PointMarkerPresetWidget::~PointMarkerPresetWidget()
{}


PointMarkerControlWidget::PointMarkerControlWidget(
    AnnotationChannelSpecs* channel, QWidget* parent)
        : MarkerControlWidget(channel, parent)
{}

PointMarkerControlWidget::~PointMarkerControlWidget()
{}


PointMarker::PointMarker(const blitz::TinyVector<double,3>& position,
                         AnnotationChannelSpecs* channel)
        : Marker(position, channel)
{}
  
PointMarker::PointMarker(const PointMarker& marker)
        : Marker(marker)
{}
  
PointMarker::~PointMarker() 
{}

PointMarker &PointMarker::operator=(const PointMarker& marker)
{
  Marker::operator=(static_cast<Marker const &>(marker));
  return *this;
}

Marker::MarkerType PointMarker::markerType() const 
{
  return Marker::Point;
}

bool PointMarker::inherits(Marker::MarkerType type) const 
{
  return type == Marker::Point;
}

MarkerRenderer *PointMarker::addRenderer(ViewWidget* view)
{ 
  if (_renderers.find(view) != _renderers.end()) return _renderers[view];
  
  switch (view->viewType())
  {
  case ViewWidget::OrthoView:
    _renderers[view] = new PointMarkerOrthoViewRenderer(
        this, static_cast<OrthoViewWidget*>(view));
    return _renderers[view];
  case ViewWidget::OpenGlRenderingView:
    _renderers[view] = new PointMarkerOpenGlRenderingViewRenderer(
        this, static_cast<OpenGlRenderingViewWidget*>(view));
    return _renderers[view];
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": Implementation missing"
              << std::endl;
    break;
  }
  return NULL;
}

void PointMarker::save(
    AnnotationChannelSpecs const *channel,
    BlitzH5File &outFile, std::string const &group,
    iRoCS::ProgressReporter *pr)
{
  Marker::save(channel, outFile, group, pr);
}

void PointMarker::load(
    AnnotationChannelSpecs *channel, BlitzH5File const &inFile,
    std::string const &group, iRoCS::ProgressReporter *pr)
{
  Marker::load(channel, inFile, group, pr);
}

void PointMarker::saveCSV(std::ofstream &out) const
{
  Marker::saveCSV(out);
}

void PointMarker::loadFromMap(
    std::map<std::string,std::string> const &values)
{
  Marker::loadFromMap(values);
}

bool PointMarker::occupiesPositionUm(
    blitz::TinyVector<double,3> const &positionUm) const
{
  return blitz::all(positionUm > boundingBoxLowerBoundUm() - 5.0 &&
                    positionUm < boundingBoxUpperBoundUm() + 5.0);
}

void PointMarker::_updateBoundingBox() const
{
  _boundingBoxLowerBoundUm = 0.0;
  _boundingBoxUpperBoundUm = 0.0;
  _boundingBoxUpToDate = true;
}
