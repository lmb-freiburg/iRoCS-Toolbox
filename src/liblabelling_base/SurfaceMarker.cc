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

#include "AnnotationChannel.hh"

namespace atb
{

  SurfaceMarker::SurfaceMarker(
      blitz::TinyVector<double,3> const &positionUm,
      SurfaceGeometry const * const surface,
      AnnotationChannel* channel)
          : PointMarker(position, channel), _surface()
  {
    if (surface != NULL) _surface = *surface;
  }
  
  SurfaceMarker::SurfaceMarker(SurfaceMarker const &marker)
          : PointMarker(marker), _surface(marker._surface)
  {}
  
  SurfaceMarker::~SurfaceMarker() 
  {}
  
  SurfaceMarker &SurfaceMarker::operator=(SurfaceMarker const &marker)
  {
    PointMarker::operator=(static_cast<PointMarker const &>(marker));
    _surface = marker._surface;
    return *this;
  }
  
  Marker::MarkerType SurfaceMarker::type() const 
  {
    return Marker::Surface;
  }

  bool SurfaceMarker::isA(Marker::MarkerType type) const
  {
    return type == Marker::Surface || PointMarker::isA(type);
  }

  SurfaceGeometry const &SurfaceMarker::surface() const
  {
    return _surface;
  }
  
  SurfaceGeometry &SurfaceMarker::surface()
  {
    _boundingBoxUpToDate = false;
    if (p_channel != NULL) p_channel->setModified(true);
    return _surface;
  }
  
  void SurfaceMarker::save(
      AnnotationChannel const *channel,
      BlitzH5File &outFile, std::string const &group)
  {
    // Assert that a valid channel is given and that it is not empty
    if (channel == NULL || channel->markers().size() == 0) return;

    // Save attributes of super class
    PointMarker::save(channel, outFile, group);

    // Ensure that the given channel really contains SurfaceMarkers
    if (!channel->markers()[0]->inherits(Marker::Surface)) return;
    
    ptrdiff_t nVertices = 0;
    ptrdiff_t nNormals = 0;
    ptrdiff_t nIndices = 0;
    for (std::vector<Marker*>::const_iterator it = channel->markers().begin();
         it != channel->markers().end(); ++it)
    {
      SurfaceMarker* m = reinterpret_cast<SurfaceMarker*>(*it);
      nVertices += m->vertices().size();
      nNormals += m->normals().size();
      nIndices += m->indices().size();
    }
    
    blitz::Array<blitz::TinyVector<int,3>,1> nElements(
        channel->markers().size());
    blitz::Array<SurfaceGeometry::VertexT,1> vertices(nVertices);
    blitz::Array<SurfaceGeometry::NormalT,1> normals(nNormals);
    blitz::Array<SurfaceGeometry::IndexT,1> indices(nIndices);

    ptrdiff_t i = 0;
    ptrdiff_t vertexPtr = 0, normalPtr = 0, indexPtr = 0;
    for (std::vector<Marker*>::const_iterator it = channel->markers().begin();
         it != channel->markers().end(); ++it, ++i)
    {
      SurfaceMarker* m = reinterpret_cast<SurfaceMarker*>(*it);
      nElements.data()[i] = blitz::TinyVector<int,3>(
          static_cast<int>(m->vertices().size()),
          static_cast<int>(m->normals().size()),
          static_cast<int>(m->indices().size()));
      std::memcpy(vertices.data() + vertexPtr, &m->vertices()[0],
                  m->vertices().size() * sizeof(SurfaceGeometry::VertexT));
      vertexPtr += m->vertices().size();
      std::memcpy(normals.data() + normalPtr, &m->normals()[0],
                  m->normals().size() * sizeof(SurfaceGeometry::NormalT));
      normalPtr += m->normals().size();
      std::memcpy(indices.data() + indexPtr, &m->indices()[0],
                  m->indices().size() * sizeof(SurfaceGeometry::IndexT));
      indexPtr += m->indices().size();
    }
  
    outFile.writeDataSetSimple(nElements, group + "/nElements");
    if (vertices.size() != 0)
        outFile.writeDataSetSimple(vertices, group + "/vertices");
    if (normals.size() != 0)
        outFile.writeDataSetSimple(normals, group + "/normals");
    if (indices.size() != 0)
        outFile.writeDataSetSimple(indices, group + "/indices");
  }

  void SurfaceMarker::load(
      AnnotationChannel *channel, BlitzH5File const &inFile,
      std::string const &group)
  {
    PointMarker::load(channel, inFile, group);
    ptrdiff_t nMarkers = static_cast<ptrdiff_t>(channel->markers().size());

    if (nMarkers == 0) return;

    blitz::Array<blitz::TinyVector<int,3>,1> nElements;
    blitz::Array<SurfaceGeometry::VertexT,1> vertices;
    blitz::Array<SurfaceGeometry::NormalT,1> normals;
    blitz::Array<SurfaceGeometry::IndexT,1> indices;

    try
    {
      inFile.readDataSetSimple(nElements, group + "/nElements");
      try
      {
        inFile.readDataSetSimple(vertices, group + "/vertices");
      }
      catch (BlitzH5Error &)
      {}
      try
      {
        inFile.readDataSetSimple(normals, group + "/normals");
      }
      catch (BlitzH5Error &)
      {}
      try
      {
        inFile.readDataSetSimple(indices, group + "/indices");
      }
      catch (BlitzH5Error &)
      {}
    }
    catch (BlitzH5Error &e)
    {
      std::cerr << "Error while loading SurfaceChannel: " << e.what()
                << std::endl;
      return;
    }

    ptrdiff_t vertexPtr = 0, normalPtr = 0, indexPtr = 0;
    for (ptrdiff_t i = 0; i < nMarkers; ++i) 
    {
      SurfaceMarker* m =
          reinterpret_cast<SurfaceMarker*>(channel->markers()[i]);

      m->vertices().resize(static_cast<ptrdiff_t>(nElements(i)(0)));
      std::memcpy(&m->vertices()[0], vertices.data() + vertexPtr,
                  m->vertices().size() * sizeof(SurfaceGeometry::VertexT));
      vertexPtr += m->vertices().size();
      m->normals().resize(static_cast<ptrdiff_t>(nElements(i)(1)));
      std::memcpy(&m->normals()[0], normals.data() + normalPtr,
                  m->normals().size() * sizeof(SurfaceGeometry::NormalT));
      normalPtr += m->normals().size();
      m->indices().resize(static_cast<ptrdiff_t>(nElements(i)(2)));
      std::memcpy(&m->indices()[0], indices.data() + indexPtr,
                  m->indices().size() * sizeof(SurfaceGeometry::IndexT));
      indexPtr += m->indices().size();
    }
  }

  void SurfaceMarker::writeCSVHeader(std::ostream &out) const
  {
    Marker::writeCSVHeader(out);
    out << ";vertices (micron);normals;indices";
  }

  void SurfaceMarker::saveCSV(std::ostream &out) const
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
    if (_surface.indices.size() == 0)
        return PointMarker::occupiesPositionUm(positionUm);

    if (blitz::any(positionUm < boundingBoxLowerBoundUm() ||
                   positionUm > boundingBoxUpperBoundUm())) return false;
  
    // In principle one has to search for the closest triangle and check whether
    // the dot-product of the difference vector and the normal vector of the
    // surface is negative, but that's expensive, instead the vertices are
    // searched, which is still slow
    blitz::TinyVector<double,3> pos(positionUm - _positionUm);
    ptrdiff_t closestVertex = 0;
    double minSqDist = blitz::dot(
        pos - _surface.vertices[0], pos - _surface.vertices[0]);
    for (size_t i = 1; i < _surface.vertices.size(); ++i)
    {
      double sqDist = blitz::dot(
          pos - _surface.vertices[i], pos - _surface.vertices[i]);
      if (sqDist < minSqDist)
      {
        minSqDist = sqDist;
        closestVertex = i;
      }
    }
    return blitz::dot(pos - _surface.vertices[closestVertex],
                      _surface.normals[closestVertex]) < 0.0;
  }

  void SurfaceMarker::_updateBoundingBox() const
  {
    if (_surface.indices.size() == 0) PointMarker::_updateBoundingBox();
    else
    {
      _boundingBoxLowerBoundUm = _surface.vertices[_surface.indices[0]];
      _boundingBoxUpperBoundUm = _surface.vertices[_surface.indices[0]];
      for (size_t i = 1; i < _surface.indices.size(); ++i)
      {
        for (int d = 0; d < 3; ++d)
        {
          if (_surface.vertices[_surface.indices[i]](d) <
              _boundingBoxLowerBoundUm(d))
              _boundingBoxLowerBoundUm(d) =
                  _surface.vertices[_surface.indices[i]](d);
          if (_surface.vertices[_surface.indices[i]](d) >
              _boundingBoxUpperBoundUm(d))
              _boundingBoxUpperBoundUm(d) =
                  _surface.vertices[_surface.indices[i]](d);
        }
      }
      _boundingBoxUpToDate = true;
    }
  }

}
