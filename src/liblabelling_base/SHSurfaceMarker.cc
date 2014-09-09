/**************************************************************************
**       Title: Spherical harmonic surface marker
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#include "SHSurfaceMarker.hh"

#include <QtGui/QVBoxLayout>

#include "AnnotationChannelSpecs.hh"

#include "MultiChannelModel.hh"
#include "OrthoViewWidget.hh"
#include "SHSurfaceMarkerOrthoViewRenderer.hh"

#include "OpenGlRenderingViewWidget.hh"
#include "SHSurfaceMarkerOpenGlRenderingViewRenderer.hh"

#include "StringControlElement.hh"
#include "BoolControlElement.hh"

#include <SH_backward.hh>
#include <ATBSphericalHarmonics.hh>

// #include <blitz/tinyvec-et.h>

SHSurfaceMarkerPresetWidget::SHSurfaceMarkerPresetWidget(
    AnnotationChannelSpecs* channel, QWidget* parent)
        : SphereMarkerPresetWidget(channel, parent)
{}

SHSurfaceMarkerPresetWidget::~SHSurfaceMarkerPresetWidget()
{}


SHSurfaceMarkerControlWidget::SHSurfaceMarkerControlWidget(
    AnnotationChannelSpecs* channel, QWidget* parent)
        : SphereMarkerControlWidget(channel, parent)
{  
  p_coefficientsControl = new StringControlElement(tr("SH coefficients:"));
  p_coefficientsControl->setEnabled(false);
  p_coefficientsControl->setVisible(p_showReadOnlyControl->value());
  p_layout->addWidget(p_coefficientsControl);
}

SHSurfaceMarkerControlWidget::~SHSurfaceMarkerControlWidget()
{}

void SHSurfaceMarkerControlWidget::setValues(const Marker* marker) 
{
  SphereMarkerControlWidget::setValues(marker);
  if (!marker->inherits(Marker::SHSurface)) return;
  const SHSurfaceMarker* m = reinterpret_cast<const SHSurfaceMarker*>(marker);
  std::stringstream value;
  value << m->coefficients();
  p_coefficientsControl->setValue(value.str());
}

void SHSurfaceMarkerControlWidget::toggleReadOnly()
{
  SphereMarkerControlWidget::toggleReadOnly();
  p_coefficientsControl->setVisible(p_showReadOnlyControl->value());
}


SHSurfaceMarker::SHSurfaceMarker(
    blitz::TinyVector<double,3> const& position,
    double radius, segmentation::FrequencyArray const &coefficients,
    AnnotationChannelSpecs* channel)
        : SphereMarker(position, radius, channel), _coefficients(), _surface()
{
  if (coefficients.size() != 0)
  {
    _coefficients.resize(coefficients.shape());
    std::memcpy(_coefficients.data(), coefficients.data(),
                _coefficients.size() * sizeof(std::complex<double>));
    _updateTriangles();
  }
}
  
SHSurfaceMarker::SHSurfaceMarker(const SHSurfaceMarker& marker)
        : SphereMarker(marker), _coefficients(), _surface(marker._surface)
{
  if (marker._coefficients.size() != 0)
  {
    _coefficients.resize(marker._coefficients.shape());
    std::memcpy(_coefficients.data(), marker._coefficients.data(),
                _coefficients.size() * sizeof(std::complex<double>));
  }
}
  
SHSurfaceMarker::~SHSurfaceMarker() 
{}

SHSurfaceMarker &SHSurfaceMarker::operator=(const SHSurfaceMarker& marker)
{
  SphereMarker::operator=(static_cast<SphereMarker const &>(marker));
  _coefficients.resize(marker._coefficients.shape());
  if (marker._coefficients.size() != 0)
      std::memcpy(_coefficients.data(), marker._coefficients.data(),
                  _coefficients.size() * sizeof(std::complex<double>));
  _surface = marker._surface;
  return *this;
}

Marker::MarkerType SHSurfaceMarker::markerType() const 
{
  return Marker::SHSurface;
}

bool SHSurfaceMarker::inherits(Marker::MarkerType type) const
{
  return type == Marker::SHSurface || SphereMarker::inherits(type);
}

MarkerRenderer *SHSurfaceMarker::addRenderer(ViewWidget* view)
{ 
  if (_renderers.find(view) != _renderers.end()) return _renderers[view];
  
  switch (view->viewType())
  {
  case ViewWidget::OrthoView:
    _renderers[view] = new SHSurfaceMarkerOrthoViewRenderer(
        this, static_cast<OrthoViewWidget*>(view));
    return _renderers[view];
  case ViewWidget::OpenGlRenderingView:
    _renderers[view] = new SHSurfaceMarkerOpenGlRenderingViewRenderer(
        this, static_cast<OpenGlRenderingViewWidget*>(view));
    return _renderers[view];
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": Implementation missing"
              << std::endl;
    break;
  }
  return NULL;
}

int SHSurfaceMarker::bw() const
{
  return static_cast<int>(std::sqrt(static_cast<double>(_coefficients.size())));
}

segmentation::FrequencyArray const &SHSurfaceMarker::coefficients() const
{
  return _coefficients;
}

void SHSurfaceMarker::setCoefficients(
    segmentation::FrequencyArray const &coefficients)
{
  if (_coefficients.size() != coefficients.size())
      _coefficients.resize(coefficients.size());
  std::memcpy(_coefficients.data(), coefficients.data(),
              _coefficients.size() * sizeof(std::complex<double>));
  if (p_channel != NULL)
  {
    if (_selected && p_channel->markerControlWidget() != NULL)
        p_channel->markerControlWidget()->setValues(this);
    if (p_channel->model() != NULL) p_channel->model()->setModified(true);
  }
  _updateTriangles();
  _boundingBoxUpToDate = false;
}

std::vector<atb::SurfaceGeometry::VertexT> const
&SHSurfaceMarker::vertices() const
{
  if (_coefficients.size() != 0 && _surface.vertices.size() == 0)
      _updateTriangles();
  return _surface.vertices;
}

std::vector<atb::SurfaceGeometry::NormalT> const
&SHSurfaceMarker::normals() const
{
  if (_coefficients.size() != 0 && _surface.vertices.size() == 0)
      _updateTriangles();
  return _surface.normals;
}

std::vector<atb::SurfaceGeometry::IndexT> const
&SHSurfaceMarker::indices() const
{
  if (_coefficients.size() != 0 && _surface.vertices.size() == 0)
      _updateTriangles();
  return _surface.indices;
}

void SHSurfaceMarker::save(
    const AnnotationChannelSpecs* channel,
    BlitzH5File& outFile, const std::string& group)
{
  if (channel->markers().size() == 0) return;
  SphereMarker::save(channel, outFile, group);
  if (!channel->markers()[0]->inherits(Marker::SHSurface)) return;

  ptrdiff_t maxCoeffs = 0;
  for (std::vector<Marker*>::const_iterator it = channel->markers().begin();
       it != channel->markers().end(); ++it)
  {
    SHSurfaceMarker* m = reinterpret_cast<SHSurfaceMarker*>(*it);
    if (m->coefficients().size() > maxCoeffs)
        maxCoeffs = m->coefficients().size();
  }

  blitz::Array<int,1> bandwidths(channel->markers().size());

  blitz::Array<std::complex<double>,2> coeffs;
  if (maxCoeffs != 0) coeffs.resize(channel->markers().size(), maxCoeffs);

  ptrdiff_t i = 0;
  for (std::vector<Marker*>::const_iterator it = channel->markers().begin();
       it != channel->markers().end(); ++it, ++i)
  {
    SHSurfaceMarker* m = reinterpret_cast<SHSurfaceMarker*>(*it);
    bandwidths(i) = m->bw();
    if (m->bw() != 0)
        coeffs(i, blitz::Range(0, m->_coefficients.size() - 1)) =
            m->coefficients();
  }
  outFile.writeDataSetSimple(bandwidths, group + "/bandwidths");
  if (coeffs.size() != 0)
      outFile.writeDataSetSimple(coeffs, group + "/coefficients");
}

void SHSurfaceMarker::load(AnnotationChannelSpecs* channel,
                         const BlitzH5File& inFile, const std::string& group)
{
  SphereMarker::load(channel, inFile, group);
  ptrdiff_t nMarkers = static_cast<ptrdiff_t>(channel->markers().size());
  if (nMarkers == 0) return;

  blitz::Array<int,1> bandwidths;
  blitz::Array<std::complex<double>,2> coeffs;
  try
  {
    inFile.readDataSetSimple(bandwidths, group + "/bandwidths");
    inFile.readDataSetSimple(coeffs, group + "/coefficients");
  }
  catch (BlitzH5Error &)
  {
    bandwidths.resize(0);
  }

  for (ptrdiff_t i = 0; i < nMarkers; ++i) 
  {
    SHSurfaceMarker* m =
        reinterpret_cast<SHSurfaceMarker*>(channel->markers()[i]);
    if (i < bandwidths.size())
    {
      m->_coefficients.resize(
          static_cast<ptrdiff_t>(bandwidths(i) * bandwidths(i)));
      m->_coefficients =
          coeffs(i, blitz::Range(0, m->_coefficients.size() - 1));
    }
    else m->_coefficients.resize(0);
    m->_updateTriangles();
  }
}

void SHSurfaceMarker::writeCSVHeader(std::ofstream &out) const
{
  SphereMarker::writeCSVHeader(out);
  out << ";coefficients";
}

void SHSurfaceMarker::saveCSV(std::ofstream &out) const
{
  SphereMarker::saveCSV(out);
  out << ";";
}

void SHSurfaceMarker::loadFromMap(
    std::map<std::string,std::string> const &values)
{
  SphereMarker::loadFromMap(values);
}

bool SHSurfaceMarker::occupiesPositionUm(
    blitz::TinyVector<double,3> const &positionUm) const
{
  if (blitz::any(positionUm < boundingBoxLowerBoundUm() ||
                 positionUm > boundingBoxUpperBoundUm())) return false;
  blitz::TinyVector<double,3> pos(positionUm - _positionUm);
  if (_coefficients.size() == 0)
      return (blitz::dot(pos, pos) < blitz::pow2(_radiusUm));
  pos = ATB::euclideanToSpherical(pos);
  int bw = this->bw();
  ptrdiff_t thetaIdx = segmentation::thetaToIndex(pos(0), bw);
  ptrdiff_t phiIdx = segmentation::phiToIndex(pos(1), bw);
  ptrdiff_t index = thetaIdx * 2 * bw + phiIdx;
  return (blitz::pow2(pos(2)) <
          blitz::dot(_surface.vertices[index], _surface.vertices[index]));
}

void SHSurfaceMarker::_updateTriangles() const
{
  _boundingBoxUpToDate = false;
  
  if (_coefficients.size() == 0)
  {
    _surface.vertices.resize(0);
    _surface.normals.resize(0);
    _surface.indices.resize(0);
    return;
  }
  
  int bw = this->bw();

  segmentation::SpatialArray surface;  
  segmentation::FrequencyArray coeffs;

  // Super-Sample if the bandwidth is below a threshold
  if (bw < 10)
  {
    coeffs.resize(10 * 10);
    coeffs = 0.0;
    for (int l = 0; l < bw; ++l)
        for (int m = -l; m <= l; ++m)
            coeffs(segmentation::linearIndex(10, l, m)) =
                _coefficients(segmentation::linearIndex(bw, l, m));
    bw = 10;
  }
  else
  {
    coeffs.resize(_coefficients.shape());
    coeffs = _coefficients;
  }
  
  surface.resize(2 * bw, 2 * bw);
  SH_backward::instance(bw).sh_semi_memo_back(
      reinterpret_cast<double*>(coeffs.data()),
      reinterpret_cast<double*>(surface.data()));

  // Generate interior vertices
  unsigned int nLat = static_cast<unsigned int>(2 * bw);
  unsigned int nLon = static_cast<unsigned int>(2 * bw);
  _surface.vertices.resize(nLat * nLon + 2);
  _surface.indices.resize(6 * (nLat - 1) * nLon + 6 * nLon);
  
  for (size_t thetaIdx = 0; thetaIdx < nLat; ++thetaIdx)
  {
    double theta = segmentation::indexToTheta(thetaIdx, bw);
    for (size_t phiIdx = 0; phiIdx < nLon; ++phiIdx)
    {
      double phi = segmentation::indexToPhi(phiIdx, bw);
      blitz::TinyVector<float,3> pos(
          ATB::sphericalToEuclidean(
              blitz::TinyVector<double,3>(
                  theta, phi,
                  surface(static_cast<ptrdiff_t>(thetaIdx),
                          static_cast<ptrdiff_t>(phiIdx)).real())));
      _surface.vertices[thetaIdx * nLon + phiIdx] =
          pos(2), pos(1), pos(0);
    }
  }

  // The poles
  _surface.vertices[nLat * nLon] = 0.0f;
  _surface.vertices[nLat * nLon + 1] = 0.0f;
  for (size_t phiIdx = 0; phiIdx < nLon; ++phiIdx)
  {
    _surface.vertices[nLat * nLon] +=
        _surface.vertices[phiIdx];
    _surface.vertices[nLat * nLon + 1] +=
        _surface.vertices[(nLat - 1) * nLon + phiIdx];
  }
  _surface.vertices[nLat * nLon] /= static_cast<float>(nLon);
  _surface.vertices[nLat * nLon + 1] /= static_cast<float>(nLon);

  // And the indices
  for (unsigned int thetaIdx = 0; thetaIdx < nLat - 1; ++thetaIdx)
  {
    for (unsigned int phiIdx = 0; phiIdx < nLon; ++phiIdx)
    {
      _surface.indices[6 * (thetaIdx * nLon + phiIdx)] =
          thetaIdx * nLon + phiIdx;
      _surface.indices[6 * (thetaIdx * nLon + phiIdx) + 1] =
          (thetaIdx + 1) * nLon + phiIdx;
      _surface.indices[6 * (thetaIdx * nLon + phiIdx) + 2] =        
          (thetaIdx + 1) * nLon + ((phiIdx + 1) % nLon);
      _surface.indices[6 * (thetaIdx * nLon + phiIdx) + 3] =
          thetaIdx * nLon + phiIdx;
      _surface.indices[6 * (thetaIdx * nLon + phiIdx) + 4] =
          (thetaIdx + 1) * nLon + ((phiIdx + 1) % nLon);
      _surface.indices[6 * (thetaIdx * nLon + phiIdx) + 5] =
          thetaIdx * nLon + ((phiIdx + 1) % nLon);
    }
  }

  // Pole triangles
  for (unsigned int phiIdx = 0; phiIdx < nLon; ++phiIdx)
  {
    _surface.indices[6 * (nLat - 1) * nLon + 3 * phiIdx] =
        static_cast<unsigned int>(_surface.vertices.size() - 2);
    _surface.indices[6 * (nLat - 1) * nLon + 3 * phiIdx + 1] =
        phiIdx;
    _surface.indices[6 * (nLat - 1) * nLon + 3 * phiIdx + 2] =
        (phiIdx + 1) % nLon;
    _surface.indices[6 * (nLat - 1) * nLon + 3 * nLon + 3 * phiIdx] =
        static_cast<unsigned int>(_surface.vertices.size() - 1);
    _surface.indices[6 * (nLat - 1) * nLon + 3 * nLon + 3 * phiIdx + 1] =
        (nLat - 1) * nLon + (phiIdx + 1) % nLon;
    _surface.indices[6 * (nLat - 1) * nLon + 3 * nLon + 3 * phiIdx + 2] =
        (nLat - 1) * nLon + phiIdx;
  }

  _surface.computeDefaultNormals();
}

void SHSurfaceMarker::_updateBoundingBox() const
{
  if (_coefficients.size() == 0) SphereMarker::_updateBoundingBox();
  else
  {
    _boundingBoxLowerBoundUm = vertices()[0];
    _boundingBoxUpperBoundUm = vertices()[0];
    for (size_t i = 1; i < _surface.vertices.size(); ++i)
    {
      for (int d = 0; d < 3; ++d)
      {
        if (_surface.vertices[i](d) < _boundingBoxLowerBoundUm(d))
            _boundingBoxLowerBoundUm(d) = _surface.vertices[i](d);
        if (_surface.vertices[i](d) > _boundingBoxUpperBoundUm(d))
            _boundingBoxUpperBoundUm(d) = _surface.vertices[i](d);
      }
    }
    _boundingBoxUpToDate = true;
  }
}
