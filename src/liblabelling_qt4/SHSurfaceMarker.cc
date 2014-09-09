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

#include <libsegmentation/SH_backward.hh>
#include <libArrayToolbox/ATBLinAlg.hh>
#include <libArrayToolbox/TypeTraits.hh>

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
    if (selected() && p_channel->markerControlWidget() != NULL)
        p_channel->markerControlWidget()->setValues(this);
    if (p_channel->model() != NULL) p_channel->model()->setModified(true);
  }
  _updateTriangles();
  _boundingBoxUpToDate = false;
}

atb::SurfaceGeometry const &SHSurfaceMarker::geometry() const
{
  if (_coefficients.size() != 0 && _surface.vertices().size() == 0)
      _updateTriangles();
  return _surface;
}

std::vector<atb::SurfaceGeometry::VertexT> const
&SHSurfaceMarker::vertices() const
{
  if (_coefficients.size() != 0 && _surface.vertices().size() == 0)
      _updateTriangles();
  return _surface.vertices();
}

std::vector<atb::SurfaceGeometry::NormalT> const
&SHSurfaceMarker::normals() const
{
  if (_coefficients.size() != 0 && _surface.vertices().size() == 0)
      _updateTriangles();
  return _surface.normals();
}

std::vector<atb::SurfaceGeometry::IndexT> const
&SHSurfaceMarker::indices() const
{
  if (_coefficients.size() != 0 && _surface.vertices().size() == 0)
      _updateTriangles();
  return _surface.indices();
}

void SHSurfaceMarker::save(
    AnnotationChannelSpecs const *channel, BlitzH5File &outFile,
    std::string const & group, iRoCS::ProgressReporter *pr)
{
  if (channel->markers().size() == 0) return;

  int pStart = (pr != NULL) ? pr->taskProgressMin() : 0;
  int pMax = (pr != NULL) ? pr->taskProgressMax() : 1;
  int pRange = pMax - pStart;

  if (pr != NULL)
      pr->setTaskProgressMax(pStart + static_cast<int>(0.3f * pRange));
  SphereMarker::save(channel, outFile, group, pr);
  if (pr != NULL)
  {
    pStart = pr->taskProgressMax();
    pr->setTaskProgressMin(pStart);
    pr->setTaskProgressMax(pMax);
    pRange = pMax - pStart;
    if (pr->isAborted()) return;
  }
  if (!channel->markers()[0]->inherits(Marker::SHSurface)) return;

  atb::BlitzIndexT maxCoeffs = 0;
  for (std::vector<Marker*>::const_iterator it = channel->markers().begin();
       it != channel->markers().end(); ++it)
  {
    SHSurfaceMarker* m = reinterpret_cast<SHSurfaceMarker*>(*it);
    if (static_cast<atb::BlitzIndexT>(m->coefficients().size()) > maxCoeffs)
        maxCoeffs = m->coefficients().size();
  }
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.10f * pRange))) return;

  blitz::Array<int,1> bandwidths(channel->markers().size());

  blitz::Array<std::complex<double>,2> coeffs;
  if (maxCoeffs != 0) coeffs.resize(channel->markers().size(), maxCoeffs);

  atb::BlitzIndexT i = 0;
  for (std::vector<Marker*>::const_iterator it = channel->markers().begin();
       it != channel->markers().end(); ++it, ++i)
  {
    if (pr != NULL && !pr->updateProgress(
            pStart + static_cast<int>(
                (0.1f + 0.5f * static_cast<float>(i) /
                 static_cast<float>(channel->markers().size() - 1)) * pRange)))
        return;
    SHSurfaceMarker* m = reinterpret_cast<SHSurfaceMarker*>(*it);
    bandwidths(i) = m->bw();
    if (m->bw() != 0)
        coeffs(i, blitz::Range(0, m->_coefficients.size() - 1)) =
            m->coefficients();
  }
  outFile.writeDataset(bandwidths, group + "/bandwidths");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.8f * pRange))) return;
  if (coeffs.size() != 0)
      outFile.writeDataset(coeffs, group + "/coefficients");
  if (pr != NULL && !pr->updateProgress(pStart + pRange)) return;
}

void SHSurfaceMarker::load(
    AnnotationChannelSpecs *channel, BlitzH5File const &inFile,
    std::string const &group, iRoCS::ProgressReporter *pr)
{
  int pStart = (pr != NULL) ? pr->taskProgressMin() : 0;
  int pMax = (pr != NULL) ? pr->taskProgressMax() : 1;
  int pRange = pMax - pStart;

  if (pr != NULL)
      pr->setTaskProgressMax(pStart + static_cast<int>(0.3f * pRange));
  SphereMarker::load(channel, inFile, group, pr);
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

  blitz::Array<int,1> bandwidths(nMarkers);
  _loadFixedSize(bandwidths, inFile, group + "/bandwidths");
  blitz::Array<std::complex<double>,2> coefficients;
  try 
  {
    inFile.readDataset(coefficients, group + "/coefficients");
    if (coefficients.extent(0) != nMarkers) 
    {
      blitz::Array<std::complex<double>,2> tmp(
          nMarkers, coefficients.extent(1));
      if (coefficients.extent(0) < nMarkers)
      {
        tmp = -1;
        tmp(blitz::Range(0, nMarkers - 1), blitz::Range::all()) =
            coefficients;
      }
      else
      {
        tmp = coefficients(blitz::Range(0, tmp.extent(0) - 1),
                           blitz::Range::all());
      }
      coefficients.resize(tmp.extent(0), tmp.extent(1));
      coefficients = tmp;
    }
  }
  catch (BlitzH5Error&)
  {}

  for (atb::BlitzIndexT i = 0; i < nMarkers; ++i) 
  {
    if (pr != NULL && !pr->updateProgress(
            pStart + static_cast<int>(
                (0.5f + 0.5f * static_cast<float>(i) /
                 static_cast<float>(nMarkers - 1)) * pRange)))
        return;
    SHSurfaceMarker* m =
        reinterpret_cast<SHSurfaceMarker*>(channel->markers()[i]);
    if (coefficients.size() != 0)
    {
      if (bandwidths(i) == -1) m->_coefficients.resize(0);
      else
      {
        m->_coefficients.resize(bandwidths(i) * bandwidths(i));
        for (atb::BlitzIndexT j = 0;
             j < static_cast<atb::BlitzIndexT>(m->_coefficients.size()); ++j)
            m->_coefficients(j) = coefficients(i, j);
      }
    }
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
  blitz::TinyVector<double,3> pos(positionUm - this->positionUm());
  if (_coefficients.size() == 0)
      return (blitz::dot(pos, pos) < blitz::pow2(_radiusUm));
  pos = atb::euclideanToSpherical(pos);
  int bw = this->bw();
  atb::BlitzIndexT thetaIdx = segmentation::thetaToIndex(pos(0), bw);
  atb::BlitzIndexT phiIdx = segmentation::phiToIndex(pos(1), bw);
  atb::BlitzIndexT index = thetaIdx * 2 * bw + phiIdx;
  return (blitz::pow2(pos(2)) <
          blitz::dot(_surface.vertices()[index], _surface.vertices()[index]));
}

void SHSurfaceMarker::copyToATBNucleus(atb::Nucleus &nc) const
{
  SphereMarker::copyToATBNucleus(nc);
  nc.shCoefficients() = _coefficients;
}

void SHSurfaceMarker::copyFromATBNucleus(atb::Nucleus const &nc)
{
  SphereMarker::copyFromATBNucleus(nc);
  _coefficients = nc.shCoefficients();
}

void SHSurfaceMarker::_updateTriangles() const
{
  std::vector<atb::SurfaceGeometry::VertexT> &V = _surface.vertices();
  std::vector<atb::SurfaceGeometry::IndexT> &I = _surface.indices();

  _boundingBoxUpToDate = false;
  
  if (_coefficients.size() == 0)
  {
    V.resize(0);
    _surface.normals().resize(0);
    I.resize(0);
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
  V.resize(nLat * nLon + 2);
  I.resize(6 * (nLat - 1) * nLon + 6 * nLon);
  
  for (size_t thetaIdx = 0; thetaIdx < nLat; ++thetaIdx)
  {
    double theta = segmentation::indexToTheta(thetaIdx, bw);
    for (size_t phiIdx = 0; phiIdx < nLon; ++phiIdx)
    {
      double phi = segmentation::indexToPhi(phiIdx, bw);
      blitz::TinyVector<float,3> pos(
          atb::sphericalToEuclidean(
              blitz::TinyVector<double,3>(
                  theta, phi,
                  surface(static_cast<atb::BlitzIndexT>(thetaIdx),
                          static_cast<atb::BlitzIndexT>(phiIdx)).real())));
      V[thetaIdx * nLon + phiIdx] = pos(2), pos(1), pos(0);
    }
  }

  // The poles
  V[nLat * nLon] = 0.0f;
  V[nLat * nLon + 1] = 0.0f;
  for (size_t phiIdx = 0; phiIdx < nLon; ++phiIdx)
  {
    V[nLat * nLon] += V[phiIdx];
    V[nLat * nLon + 1] += V[(nLat - 1) * nLon + phiIdx];
  }
  V[nLat * nLon] /= static_cast<float>(nLon);
  V[nLat * nLon + 1] /= static_cast<float>(nLon);

  // And the indices
  for (unsigned int thetaIdx = 0; thetaIdx < nLat - 1; ++thetaIdx)
  {
    for (unsigned int phiIdx = 0; phiIdx < nLon; ++phiIdx)
    {
      I[6 * (thetaIdx * nLon + phiIdx)] = thetaIdx * nLon + phiIdx;
      I[6 * (thetaIdx * nLon + phiIdx) + 1] = (thetaIdx + 1) * nLon + phiIdx;
      I[6 * (thetaIdx * nLon + phiIdx) + 2] =
          (thetaIdx + 1) * nLon + ((phiIdx + 1) % nLon);
      I[6 * (thetaIdx * nLon + phiIdx) + 3] = thetaIdx * nLon + phiIdx;
      I[6 * (thetaIdx * nLon + phiIdx) + 4] =
          (thetaIdx + 1) * nLon + ((phiIdx + 1) % nLon);
      I[6 * (thetaIdx * nLon + phiIdx) + 5] =
          thetaIdx * nLon + ((phiIdx + 1) % nLon);
    }
  }

  // Pole triangles
  for (unsigned int phiIdx = 0; phiIdx < nLon; ++phiIdx)
  {
    I[6 * (nLat - 1) * nLon + 3 * phiIdx] =
        static_cast<unsigned int>(V.size() - 2);
    I[6 * (nLat - 1) * nLon + 3 * phiIdx + 1] = phiIdx;
    I[6 * (nLat - 1) * nLon + 3 * phiIdx + 2] = (phiIdx + 1) % nLon;
    I[6 * (nLat - 1) * nLon + 3 * nLon + 3 * phiIdx] =
        static_cast<unsigned int>(V.size() - 1);
    I[6 * (nLat - 1) * nLon + 3 * nLon + 3 * phiIdx + 1] =
        (nLat - 1) * nLon + (phiIdx + 1) % nLon;
    I[6 * (nLat - 1) * nLon + 3 * nLon + 3 * phiIdx + 2] =
        (nLat - 1) * nLon + phiIdx;
  }

  _surface.computeDefaultNormals();
}

void SHSurfaceMarker::_updateBoundingBox() const
{
  if (_coefficients.size() == 0) SphereMarker::_updateBoundingBox();
  else
  {
    std::vector<atb::SurfaceGeometry::VertexT> const &V = _surface.vertices();
    _boundingBoxLowerBoundUm = _boundingBoxUpperBoundUm = V[0];
    for (size_t i = 1; i < V.size(); ++i)
    {
      for (int d = 0; d < 3; ++d)
      {
        if (V[i](d) < _boundingBoxLowerBoundUm(d))
            _boundingBoxLowerBoundUm(d) = V[i](d);
        if (V[i](d) > _boundingBoxUpperBoundUm(d))
            _boundingBoxUpperBoundUm(d) = V[i](d);
      }
    }
    _boundingBoxUpToDate = true;
  }
}
