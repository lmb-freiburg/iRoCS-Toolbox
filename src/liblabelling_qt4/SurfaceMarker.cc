/**************************************************************************
**       Title: Surface marker for arbitrary surfaces
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#include "SurfaceMarker.hh"

#include <QtGui/QVBoxLayout>

#include "AnnotationChannelSpecs.hh"

#include "SurfaceMarkerOrthoViewRenderer.hh"
#include "OrthoViewWidget.hh"
#include "MultiChannelModel.hh"

#include "SurfaceMarkerOpenGlRenderingViewRenderer.hh"
#include "OpenGlRenderingViewWidget.hh"

SurfaceMarkerPresetWidget::SurfaceMarkerPresetWidget(
    AnnotationChannelSpecs* channel, QWidget* parent)
        : PointMarkerPresetWidget(channel, parent)
{}

SurfaceMarkerPresetWidget::~SurfaceMarkerPresetWidget()
{}


SurfaceMarkerControlWidget::SurfaceMarkerControlWidget(
    AnnotationChannelSpecs* channel, QWidget* parent)
        : PointMarkerControlWidget(channel, parent)
{}

SurfaceMarkerControlWidget::~SurfaceMarkerControlWidget()
{}


SurfaceMarker::SurfaceMarker(
    blitz::TinyVector<double,3> const& position,
    atb::SurfaceGeometry const * const surface,
    AnnotationChannelSpecs* channel)
        : PointMarker(position, channel), _surface()
{
  if (surface != NULL) _surface = *surface;
}
  
SurfaceMarker::SurfaceMarker(const SurfaceMarker& marker)
        : PointMarker(marker), _surface(marker._surface)
{}
  
SurfaceMarker::~SurfaceMarker() 
{}

SurfaceMarker &SurfaceMarker::operator=(const SurfaceMarker& marker)
{
  PointMarker::operator=(static_cast<PointMarker const &>(marker));
  _surface = marker._surface;
  return *this;
}

Marker::MarkerType SurfaceMarker::markerType() const 
{
  return Marker::Surface;
}

bool SurfaceMarker::inherits(Marker::MarkerType type) const
{
  return type == Marker::Surface || PointMarker::inherits(type);
}

MarkerRenderer *SurfaceMarker::addRenderer(ViewWidget* view)
{ 
  if (_renderers.find(view) != _renderers.end()) return _renderers[view];
  
  switch (view->viewType())
  {
  case ViewWidget::OrthoView:
    _renderers[view] = new SurfaceMarkerOrthoViewRenderer(
        this, static_cast<OrthoViewWidget*>(view));
    return _renderers[view];
  case ViewWidget::OpenGlRenderingView:
    _renderers[view] = new SurfaceMarkerOpenGlRenderingViewRenderer(
        this, static_cast<OpenGlRenderingViewWidget*>(view));
    return _renderers[view];
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": Implementation missing"
              << std::endl;
    break;
  }
  return NULL;
}

atb::SurfaceGeometry const &SurfaceMarker::surface() const
{
  return _surface;
}

atb::SurfaceGeometry &SurfaceMarker::surface()
{
  _boundingBoxUpToDate = false;
  if (p_channel != NULL && p_channel->model() != NULL)
      p_channel->model()->setModified(true);
  return _surface;
}

std::vector<atb::SurfaceGeometry::VertexT> const 
&SurfaceMarker::vertices() const
{
  return _surface.vertices();
}

std::vector<atb::SurfaceGeometry::VertexT> &SurfaceMarker::vertices()
{
  _boundingBoxUpToDate = false;
  if (p_channel != NULL && p_channel->model() != NULL)
      p_channel->model()->setModified(true);
  return _surface.vertices();
}

std::vector<atb::SurfaceGeometry::NormalT> const
&SurfaceMarker::normals() const
{
  return _surface.normals();
}

std::vector<atb::SurfaceGeometry::NormalT> &SurfaceMarker::normals()
{
  if (p_channel != NULL && p_channel->model() != NULL)
      p_channel->model()->setModified(true);
  return _surface.normals();
}

std::vector<atb::SurfaceGeometry::IndexT> const
&SurfaceMarker::indices() const
{
  return _surface.indices();
}

std::vector<atb::SurfaceGeometry::IndexT> &SurfaceMarker::indices()
{
  _boundingBoxUpToDate = false;
  if (p_channel != NULL && p_channel->model() != NULL)
      p_channel->model()->setModified(true);
  return _surface.indices();
}

void SurfaceMarker::setVertices(
    std::vector<atb::SurfaceGeometry::VertexT> const &vertices)
{
  _surface.vertices() = vertices;
  _boundingBoxUpToDate = false;
  if (p_channel != NULL && p_channel->model() != NULL)
      p_channel->model()->setModified(true);
}

void SurfaceMarker::setNormals(
    std::vector<atb::SurfaceGeometry::NormalT> const & normals)
{
  _surface.normals() = normals;
  if (p_channel != NULL && p_channel->model() != NULL)
      p_channel->model()->setModified(true);
}

void SurfaceMarker::setIndices(
     std::vector<atb::SurfaceGeometry::IndexT> const &indices)
{
  _surface.indices() = indices;
  _boundingBoxUpToDate = false;
  if (p_channel != NULL && p_channel->model() != NULL)
      p_channel->model()->setModified(true);
}

void SurfaceMarker::save(
    AnnotationChannelSpecs const *channel,
    BlitzH5File &outFile, std::string const &group,
    iRoCS::ProgressReporter *pr)
{
  std::vector<Marker*> const &markers = channel->markers();
  if (markers.size() == 0) return;

  int pStart = (pr != NULL) ? pr->taskProgressMin() : 0;
  int pMax = (pr != NULL) ? pr->taskProgressMax() : 1;
  int pRange = pMax - pStart;

  if (pr != NULL)
      pr->setTaskProgressMax(pStart + static_cast<int>(0.1f * pRange));
  PointMarker::save(channel, outFile, group, pr);
  if (pr != NULL)
  {
    pStart = pr->taskProgressMax();
    pr->setTaskProgressMin(pStart);
    pr->setTaskProgressMax(pMax);
    pRange = pMax - pStart;
    if (pr->isAborted()) return;
  }
  if (!markers[0]->inherits(Marker::Surface)) return;

  atb::BlitzIndexT nVertices = 0;
  atb::BlitzIndexT nNormals = 0;
  atb::BlitzIndexT nIndices = 0;
  blitz::Array<blitz::TinyVector<int,3>,1> nElements(markers.size());
  for (atb::BlitzIndexT i = 0;
       i < static_cast<atb::BlitzIndexT>(markers.size()); ++i)
  {
    SurfaceMarker* m = reinterpret_cast<SurfaceMarker*>(markers[i]);
    nVertices += m->vertices().size();
    nNormals += m->normals().size();
    nIndices += m->indices().size();
    nElements(i) = blitz::TinyVector<int,3>(
        static_cast<int>(m->vertices().size()),
        static_cast<int>(m->normals().size()),
        static_cast<int>(m->indices().size()));
  }

  if (nVertices == 0 || nNormals == 0 || nIndices == 0)
  {
    std::cout << "  No geometry information to be saved" << std::endl;
    if (pr != NULL) pr->updateProgress(pStart + pRange);
    return;
  }

  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.1f * pRange))) return;
  outFile.writeDataset(nElements, group + "/nElements");
  nElements.free();
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.2f * pRange))) return;

  blitz::Array<atb::SurfaceGeometry::VertexT,1> vertices(nVertices);
  blitz::Array<atb::SurfaceGeometry::NormalT,1> normals(nNormals);
  blitz::Array<atb::SurfaceGeometry::IndexT,1> indices(nIndices);
  atb::BlitzIndexT vertexPtr = 0, normalPtr = 0, indexPtr = 0;
  for (size_t i = 0; i < markers.size(); ++i)
  {
    if (pr != NULL && !pr->updateProgress(
            pStart + static_cast<int>(
                0.2f + 0.5f * (
                    static_cast<float>(i) /
                    static_cast<float>(markers.size() - 1)) * pRange))) return;
    SurfaceMarker* m = reinterpret_cast<SurfaceMarker*>(markers[i]);
    std::memcpy(vertices.data() + vertexPtr, &m->vertices()[0],
                m->vertices().size() * sizeof(atb::SurfaceGeometry::VertexT));
    vertexPtr += m->vertices().size();
    std::memcpy(normals.data() + normalPtr, &m->normals()[0],
                m->normals().size() * sizeof(atb::SurfaceGeometry::NormalT));
    normalPtr += m->normals().size();
    std::memcpy(indices.data() + indexPtr, &m->indices()[0],
                m->indices().size() * sizeof(atb::SurfaceGeometry::IndexT));
    indexPtr += m->indices().size();
  }

  outFile.writeDataset(vertices, group + "/vertices");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.8f * pRange))) return;
  outFile.writeDataset(normals, group + "/normals");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.9f * pRange))) return;
  outFile.writeDataset(indices, group + "/indices");

  if (pr != NULL && !pr->updateProgress(pStart + pRange)) return;
}

void SurfaceMarker::load(
    AnnotationChannelSpecs *channel, BlitzH5File const &inFile,
    std::string const &group, iRoCS::ProgressReporter *pr)
{
  int pStart = (pr != NULL) ? pr->taskProgressMin() : 0;
  int pMax = (pr != NULL) ? pr->taskProgressMax() : 1;
  int pRange = pMax - pStart;

  if (pr != NULL)
      pr->setTaskProgressMax(pStart + static_cast<int>(0.1f * pRange));
  PointMarker::load(channel, inFile, group, pr);
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

  blitz::Array<blitz::TinyVector<int,3>,1> nElements;
  blitz::Array<atb::SurfaceGeometry::VertexT,1> vertices;
  blitz::Array<atb::SurfaceGeometry::NormalT,1> normals;
  blitz::Array<atb::SurfaceGeometry::IndexT,1> indices;

  try
  {
    inFile.readDataset(nElements, group + "/nElements");
    if (pr != NULL && !pr->updateProgress(
            pStart + static_cast<int>(0.1f * pRange))) return;
    if (static_cast<atb::BlitzIndexT>(nElements.size()) != nMarkers) 
    {
      std::cout << "Incompatible surface information. Surface geometry "
                << "information for " << nElements.size() << " markers "
                << "stored, but the channel contains " << nMarkers
                << " markers. Skipping!" << std::endl;
      return;
    }
    inFile.readDataset(vertices, group + "/vertices");
    if (pr != NULL && !pr->updateProgress(
            pStart + static_cast<int>(0.25f * pRange))) return;
    inFile.readDataset(normals, group + "/normals");
    if (pr != NULL && !pr->updateProgress(
            pStart + static_cast<int>(0.4f * pRange))) return;
    inFile.readDataset(indices, group + "/indices");
    if (pr != NULL && !pr->updateProgress(
            pStart + static_cast<int>(0.55f * pRange))) return;
  }
  catch (BlitzH5Error &e)
  {
    std::cerr << "No surface geometry information found. The surface markers "
              << "will be shown as Points: " << e.what() << std::endl;
    return;
  }

  ptrdiff_t vertexPtr = 0, normalPtr = 0, indexPtr = 0;
  for (atb::BlitzIndexT i = 0; i < nMarkers; ++i) 
  {
    if (pr != NULL && !pr->updateProgress(
            pStart + static_cast<int>(
                (0.55f + 0.45f * static_cast<float>(i) /
                 static_cast<float>(nMarkers - 1)) * pRange))) return;
    SurfaceMarker* m =
        reinterpret_cast<SurfaceMarker*>(channel->markers()[i]);

    m->vertices().resize(static_cast<atb::BlitzIndexT>(nElements(i)(0)));
    std::memcpy(&m->vertices()[0], vertices.data() + vertexPtr,
                m->vertices().size() * sizeof(atb::SurfaceGeometry::VertexT));
    vertexPtr += m->vertices().size();
    m->normals().resize(static_cast<atb::BlitzIndexT>(nElements(i)(1)));
    std::memcpy(&m->normals()[0], normals.data() + normalPtr,
                m->normals().size() * sizeof(atb::SurfaceGeometry::NormalT));
    normalPtr += m->normals().size();
    m->indices().resize(static_cast<atb::BlitzIndexT>(nElements(i)(2)));
    std::memcpy(&m->indices()[0], indices.data() + indexPtr,
                m->indices().size() * sizeof(atb::SurfaceGeometry::IndexT));
    indexPtr += m->indices().size();
  }
}

void SurfaceMarker::writeCSVHeader(std::ofstream &out) const
{
  Marker::writeCSVHeader(out);
  out << ";vertices (micron);normals;indices";
}

void SurfaceMarker::saveCSV(std::ofstream &out) const
{
  PointMarker::saveCSV(out);
  out << ";;;";
}

void SurfaceMarker::loadFromMap(
    std::map<std::string,std::string> const &values)
{
  PointMarker::loadFromMap(values);
}

bool SurfaceMarker::occupiesPositionUm(
    blitz::TinyVector<double,3> const &positionUm) const
{
  if (_surface.indices().size() == 0)
      return PointMarker::occupiesPositionUm(positionUm);

  if (blitz::any(positionUm < boundingBoxLowerBoundUm() ||
                 positionUm > boundingBoxUpperBoundUm())) return false;
  
  // In principle one has to search for the closest triangle and check whether
  // the dot-product of the difference vector and the normal vector of the
  // surface is negative, but that's expensive, instead the vertices are
  // searched, which is still slow
  blitz::TinyVector<double,3> pos(positionUm - this->positionUm());
  atb::BlitzIndexT closestVertex = 0;
  double minSqDist = blitz::dot(
      pos - _surface.vertices()[0], pos - _surface.vertices()[0]);
  for (size_t i = 1; i < _surface.vertices().size(); ++i)
  {
    double sqDist = blitz::dot(
        pos - _surface.vertices()[i], pos - _surface.vertices()[i]);
    if (sqDist < minSqDist)
    {
      minSqDist = sqDist;
      closestVertex = i;
    }
  }
  return blitz::dot(pos - _surface.vertices()[closestVertex],
                    _surface.normals()[closestVertex]) < 0.0;
}

void SurfaceMarker::_updateBoundingBox() const
{
  std::vector<atb::SurfaceGeometry::IndexT> const &I = _surface.indices();
  if (I.size() == 0) PointMarker::_updateBoundingBox();
  else
  {
    std::vector<atb::SurfaceGeometry::VertexT> const &V = _surface.vertices();
    _boundingBoxLowerBoundUm = V[I[0]];
    _boundingBoxUpperBoundUm = V[I[0]];
    for (size_t i = 1; i < I.size(); ++i)
    {
      for (int d = 0; d < 3; ++d)
      {
        if (V[I[i]](d) < _boundingBoxLowerBoundUm(d))
            _boundingBoxLowerBoundUm(d) = V[I[i]](d);
        if (V[I[i]](d) > _boundingBoxUpperBoundUm(d))
            _boundingBoxUpperBoundUm(d) = V[I[i]](d);
      }
    }
    _boundingBoxUpToDate = true;
  }
}

