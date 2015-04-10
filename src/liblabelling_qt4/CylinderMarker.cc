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

#include "CylinderMarker.hh"

#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>

#include "MultiChannelModel.hh"
#include "AnnotationChannelSpecs.hh"

#include "OrthoViewWidget.hh"
#include "CylinderMarkerOrthoViewRenderer.hh"

#include "OpenGlRenderingViewWidget.hh"
#include "CylinderMarkerOpenGlRenderingViewRenderer.hh"

#include "DoubleControlElement.hh"

// #include <blitz/tinyvec-et.h>

CylinderMarkerPresetWidget::CylinderMarkerPresetWidget(
    AnnotationChannelSpecs* channel, QWidget* parent)
        : SphereMarkerPresetWidget(channel, parent)
{
  p_lengthControl = new DoubleControlElement(tr("Length:"));
  p_lengthControl->setSwitchable(true);
  p_lengthControl->setActive(false);
  if (channel != NULL && channel->model() != NULL)
  {
    p_lengthControl->setRange(0.01 * channel->model()->elementSizeUm()(1),
                              1000.0 * channel->model()->elementSizeUm()(1));
    p_lengthControl->setSingleStep(0.1 * channel->model()->elementSizeUm()(1));
    p_lengthControl->setValue(10.0 * channel->model()->elementSizeUm()(1));
  }
  p_layout->addWidget(p_lengthControl);

  p_thetaControl = new DoubleControlElement(tr("Theta (deg):"));
  p_thetaControl->setSliderViewEnabled(true);
  p_thetaControl->setSwitchable(true);
  p_thetaControl->setActive(false);
  if (channel != NULL && channel->model() != NULL)
  {
    p_thetaControl->setRange(0.0, 179.9);
    p_thetaControl->setSingleStep(0.1);
    p_thetaControl->setValue(90.0);
  }
  p_layout->addWidget(p_thetaControl);

  p_phiControl = new DoubleControlElement(tr("Phi (deg):"));
  p_phiControl->setSliderViewEnabled(true);
  p_phiControl->setSwitchable(true);
  p_phiControl->setActive(false);
  if (channel != NULL && channel->model() != NULL)
  {
    p_phiControl->setRange(0.0, 359.9);
    p_phiControl->setSingleStep(0.1);
    p_phiControl->setValue(0.0);
  }
  p_layout->addWidget(p_phiControl);  
}

CylinderMarkerPresetWidget::~CylinderMarkerPresetWidget()
{}

void CylinderMarkerPresetWidget::getValues(Marker* marker) const 
{
  SphereMarkerPresetWidget::getValues(marker);
  if (marker == NULL || !marker->inherits(Marker::Cylinder)) return;
  CylinderMarker* s = reinterpret_cast<CylinderMarker*>(marker);
  if (p_lengthControl->isActive()) s->setLengthUm(p_lengthControl->value());
  if (p_thetaControl->isActive()) s->setTheta(
      p_thetaControl->value() / 180.0 * M_PI);
  if (p_phiControl->isActive()) s->setPhi(
      p_phiControl->value() / 180.0 * M_PI);
}

void CylinderMarkerPresetWidget::setValues(const Marker* marker)
{
  SphereMarkerPresetWidget::setValues(marker);
  if (!marker->inherits(Marker::Cylinder)) return;
  CylinderMarker const *s = reinterpret_cast<CylinderMarker const*>(marker);
  p_lengthControl->setValue(s->lengthUm());
  p_thetaControl->setValue(s->theta() / M_PI * 180.0);
  p_phiControl->setValue(s->phi() / M_PI * 180.0);
}


CylinderMarkerControlWidget::CylinderMarkerControlWidget(
    AnnotationChannelSpecs* channel, QWidget* parent)
        : SphereMarkerControlWidget(channel, parent)
{
  p_lengthControl = new DoubleControlElement(tr("Length:"));
  if (channel != NULL && channel->model() != NULL)
  {
    p_lengthControl->setRange(0.01 * channel->model()->elementSizeUm()(1),
                              1000.0 * channel->model()->elementSizeUm()(1));
    p_lengthControl->setSingleStep(0.1 * channel->model()->elementSizeUm()(1));
    p_lengthControl->setValue(10.0 * channel->model()->elementSizeUm()(1));
  }
  connect(p_lengthControl, SIGNAL(valueChanged()), SIGNAL(contentsChanged()));
  p_layout->addWidget(p_lengthControl);

  p_thetaControl = new DoubleControlElement(tr("Theta (deg):"));
  p_thetaControl->setSliderViewEnabled(true);
  if (channel != NULL && channel->model() != NULL)
  {
    p_thetaControl->setRange(0.0, 179.9);
    p_thetaControl->setSingleStep(0.1);
    p_thetaControl->setValue(90.0);
  }
  connect(p_thetaControl, SIGNAL(valueChanged()), SIGNAL(contentsChanged()));
  p_layout->addWidget(p_thetaControl);

  p_phiControl = new DoubleControlElement(tr("Phi (deg):"));
  p_phiControl->setSliderViewEnabled(true);
  if (channel != NULL && channel->model() != NULL)
  {
    p_phiControl->setRange(0.0, 359.9);
    p_phiControl->setSingleStep(0.1);
    p_phiControl->setValue(0.0);
  }
  connect(p_phiControl, SIGNAL(valueChanged()), SIGNAL(contentsChanged()));
  p_layout->addWidget(p_phiControl);  
}

CylinderMarkerControlWidget::~CylinderMarkerControlWidget()
{}

void CylinderMarkerControlWidget::getValues(Marker* marker) const 
{
  SphereMarkerControlWidget::getValues(marker);
  if (marker == NULL || !marker->inherits(Marker::Cylinder)) return;
  CylinderMarker* s = reinterpret_cast<CylinderMarker*>(marker);
  s->setLengthUm(p_lengthControl->value());
  s->setTheta(p_thetaControl->value() / 180.0 * M_PI);
  s->setPhi(p_phiControl->value() / 180.0 * M_PI);
}

void CylinderMarkerControlWidget::setValues(const Marker* marker)
{
  SphereMarkerControlWidget::setValues(marker);
  if (marker == NULL || !marker->inherits(Marker::Cylinder)) return;
  const CylinderMarker* s = reinterpret_cast<const CylinderMarker*>(marker);
  disconnect(p_lengthControl, SIGNAL(valueChanged()),
             this, SIGNAL(contentsChanged()));
  p_lengthControl->setValue(s->lengthUm());
  connect(p_lengthControl, SIGNAL(valueChanged()), SIGNAL(contentsChanged()));
  disconnect(p_thetaControl, SIGNAL(valueChanged()),
             this, SIGNAL(contentsChanged()));
  p_thetaControl->setValue(s->theta() / M_PI * 180.0);
  connect(p_thetaControl, SIGNAL(valueChanged()), SIGNAL(contentsChanged()));
  disconnect(p_phiControl, SIGNAL(valueChanged()),
             this, SIGNAL(contentsChanged()));
  p_phiControl->setValue(s->phi() / M_PI * 180.0);
  connect(p_phiControl, SIGNAL(valueChanged()), SIGNAL(contentsChanged()));
}


CylinderMarker::CylinderMarker(
    const blitz::TinyVector<double,3>& positionUm,
    double radiusUm, double lengthUm, double theta, double phi,
    AnnotationChannelSpecs* channel)
        : SphereMarker(positionUm, radiusUm, channel), _lengthUm(lengthUm),
          _theta(theta), _phi(phi)
{}

CylinderMarker::CylinderMarker(const CylinderMarker& marker)
        : SphereMarker(marker._positionUm, marker._radiusUm, marker.p_channel),
          _lengthUm(marker._lengthUm), _theta(marker._theta), _phi(marker._phi)
{}

CylinderMarker::~CylinderMarker() 
{}

Marker::MarkerType CylinderMarker::markerType() const 
{
  return Marker::Cylinder;
}

bool CylinderMarker::inherits(Marker::MarkerType type) const
{
  return type == Marker::Cylinder || SphereMarker::inherits(type);
}

MarkerRenderer *CylinderMarker::addRenderer(ViewWidget* view)
{ 
  if (_renderers.find(view) != _renderers.end()) return _renderers[view];
  
  switch (view->viewType())
  {
  case ViewWidget::OrthoView:
    _renderers[view] = new CylinderMarkerOrthoViewRenderer(
        this, static_cast<OrthoViewWidget*>(view));
    return _renderers[view];
  case ViewWidget::OpenGlRenderingView:
    _renderers[view] = new CylinderMarkerOpenGlRenderingViewRenderer(
        this, static_cast<OpenGlRenderingViewWidget*>(view));
    return _renderers[view];
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": Implementation missing"
              << std::endl;
    break;
  }
  return NULL;
}

double CylinderMarker::lengthUm() const 
{
  return _lengthUm;
}

void CylinderMarker::setLengthUm(double lengthUm) 
{
  if (_lengthUm == lengthUm) return;
  _lengthUm = lengthUm;
  _boundingBoxUpToDate = false;
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

double CylinderMarker::theta() const 
{
  return _theta;
}

void CylinderMarker::setTheta(double theta) 
{
  if (_theta == theta) return;
  _theta = theta;
  _boundingBoxUpToDate = false;
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

double CylinderMarker::phi() const 
{
  return _phi;
}

void CylinderMarker::setPhi(double phi) 
{
  if (_phi == phi) return;
  _phi = phi;
  _boundingBoxUpToDate = false;
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

blitz::TinyVector<double,3> CylinderMarker::orientation() const
{
  blitz::TinyVector<double,3> orientation;
  orientation(0) = std::cos(_theta);
  orientation(1) = std::sin(_theta) * std::sin(_phi);
  orientation(2) = std::sin(_theta) * std::cos(_phi);
  return orientation;
}

void CylinderMarker::setOrientation(
    blitz::TinyVector<double,3> orientation)
{
  _boundingBoxUpToDate = false;
  orientation /= std::sqrt(blitz::dot(orientation, orientation));
  _theta = std::acos(orientation(0));
  _phi = std::atan2(orientation(1), orientation(2));
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

void CylinderMarker::save(
    AnnotationChannelSpecs const *channel,
    BlitzH5File& outFile, std::string const &group,
    iRoCS::ProgressReporter *pr)
{
  if (channel->markers().size() == 0) return;
  SphereMarker::save(channel, outFile, group, pr);
  if (!channel->markers()[0]->inherits(Marker::Cylinder)) return;

  blitz::Array<double,1> lengths(channel->markers().size());
  ptrdiff_t i = 0;
  for (std::vector<Marker*>::const_iterator it = channel->markers().begin();
       it != channel->markers().end(); ++it, ++i)
  {
    lengths(i) = reinterpret_cast<CylinderMarker*>(*it)->lengthUm();
  }
  outFile.writeDataset(lengths, group + "/length_um");

  blitz::Array<double,1> thetas(channel->markers().size());
  i = 0;
  for (std::vector<Marker*>::const_iterator it = channel->markers().begin();
       it != channel->markers().end(); ++it, ++i)
  {
    thetas(i) = reinterpret_cast<CylinderMarker*>(*it)->theta();
  }
  outFile.writeDataset(thetas, group + "/theta");

  blitz::Array<double,1> phis(channel->markers().size());
  i = 0;
  for (std::vector<Marker*>::const_iterator it = channel->markers().begin();
       it != channel->markers().end(); ++it, ++i)
  {
    phis(i) = reinterpret_cast<CylinderMarker*>(*it)->phi();
  }
  outFile.writeDataset(phis, group + "/phi");

  blitz::Array<blitz::TinyVector<double,3>,1> orientations(
      channel->markers().size());
  i = 0;
  for (std::vector<Marker*>::const_iterator it = channel->markers().begin();
       it != channel->markers().end(); ++it, ++i)
  {
    orientations(i) = reinterpret_cast<CylinderMarker*>(*it)->orientation();
  }
  outFile.writeDataset(orientations, group + "/orientation");
}

void CylinderMarker::load(
    AnnotationChannelSpecs* channel,
    const BlitzH5File& inFile, const std::string& group,
    iRoCS::ProgressReporter *pr)
{
  SphereMarker::load(channel, inFile, group, pr);
  size_t nMarkers = channel->markers().size();
  if (nMarkers == 0) return;

  blitz::Array<double,1> lengths(nMarkers);
  _loadFixedSize(lengths, inFile, group + "/length_um");  

  for (size_t i = 0; i < nMarkers; ++i) 
      reinterpret_cast<CylinderMarker*>(channel->markers()[i])->setLengthUm(
          lengths(i));

  // If orientations are stored use them to determine theta and phi
  try
  {
    blitz::Array<blitz::TinyVector<double,3>,1> orientations;
    inFile.readDataset(orientations, group + "/orientation");
    if (orientations.size() != nMarkers) throw BlitzH5Error();
    for (size_t i = 0; i < nMarkers; ++i) 
        reinterpret_cast<CylinderMarker*>(
            channel->markers()[i])->setOrientation(orientations(i));
  }
  catch (BlitzH5Error &)
  {
    // orientations not found or in wrong format,
    // try to load theta and phi directly
    blitz::Array<double,1> thetas(nMarkers);
    _loadFixedSize(thetas, inFile, group + "/theta");  

   blitz::Array<double,1> phis(nMarkers);
    _loadFixedSize(phis, inFile, group + "/phi");

    for (size_t i = 0; i < nMarkers; ++i) 
    {
      CylinderMarker* m = reinterpret_cast<CylinderMarker*>(
          channel->markers()[i]);
      m->setTheta(thetas(i));
      m->setPhi(phis(i));
    }
  }
}

void CylinderMarker::writeCSVHeader(std::ofstream &out) const
{
  SphereMarker::writeCSVHeader(out);
  out << ";length (micron);theta (degrees);phi (degrees)";
}

void CylinderMarker::saveCSV(std::ofstream &out) const
{
  SphereMarker::saveCSV(out);
  out << ";" << _lengthUm << ";" << _theta << ";" << _phi;
}

void CylinderMarker::loadFromMap(
    std::map<std::string,std::string> const &values)
{
  SphereMarker::loadFromMap(values);
  _lengthUm = atof(values.find("length (micron)")->second.c_str());
  _theta = atof(values.find("theta (degrees)")->second.c_str());
  _phi = values.find("phi (degrees)")->second != "0";
}

bool CylinderMarker::occupiesPositionUm(
    blitz::TinyVector<double,3> const &positionUm) const
{
  return blitz::all(positionUm > boundingBoxLowerBoundUm() &&
                    positionUm < boundingBoxUpperBoundUm());
}

void CylinderMarker::_updateBoundingBox() const
{
  // Cylinder rotation
  blitz::Array<float,2> R(3, 3);
  R =
      1.0f, 0.0f, 0.0f, 
      0.0f, static_cast<float>(std::cos(theta())),
      static_cast<float>(std::sin(theta())),
      0.0f, static_cast<float>(-std::sin(theta())),
      static_cast<float>(std::cos(theta()));
  
  blitz::Array<float,2> R2(3, 3);
  R2 =
      static_cast<float>(std::cos(phi())), 0.0f,
      static_cast<float>(std::sin(phi())),
      0.0f, 1.0f, 0.0f,
      static_cast<float>(-std::sin(phi())), 0.0f,
      static_cast<float>(std::cos(phi()));
  
  R = atb::mmMult(R2, R);

  // Axis main direction
  blitz::TinyVector<float,3> e1(0.0, 0.0, 1.0);
  blitz::TinyVector<float,3> axis1(atb::mvMult(R, e1));
  
  // Cylinder axis start and end points
  blitz::TinyVector<float,3> axisStartUm(
      positionUm() - lengthUm() / 2.0 * axis1);
  blitz::TinyVector<float,3> axisEndUm(
      positionUm() + lengthUm() / 2.0 * axis1);

  // Bounding box corners
  for (int d = 0; d < 3; ++d)
  {
    if (axisStartUm(d) < axisEndUm(d))
    {
      _boundingBoxLowerBoundUm(d) = axisStartUm(d);
      _boundingBoxUpperBoundUm(d) = axisEndUm(d);
    }
    else
    {
      _boundingBoxLowerBoundUm(d) = axisEndUm(d);
      _boundingBoxUpperBoundUm(d) = axisStartUm(d);
    }
  }
  _boundingBoxUpToDate = true;
}
