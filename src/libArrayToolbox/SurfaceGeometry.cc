/**************************************************************************
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

#include "SurfaceGeometry.hh"

#if defined HAVE_CONFIG_H && !defined PACKAGE
#include <config.hh>
#endif

#ifdef HAVE_BLITZ_V9
#include <blitz/tinyvec-et.h>
#endif

namespace atb
{

  SurfaceGeometry::SurfaceGeometry()
          : _vertices(), _normals(), _indices(), _boundingBoxLowerBoundUm(0.0),
            _boundingBoxUpperBoundUm(0.0), _boundingBoxUpToDate(false)
  {}
  
  SurfaceGeometry::SurfaceGeometry(SurfaceGeometry const &geometry)
          : _vertices(geometry._vertices), _normals(geometry._normals),
            _indices(geometry._indices),
            _boundingBoxLowerBoundUm(geometry._boundingBoxLowerBoundUm),
            _boundingBoxUpperBoundUm(geometry._boundingBoxUpperBoundUm),
            _boundingBoxUpToDate(geometry._boundingBoxUpToDate)
  {}

  SurfaceGeometry::~SurfaceGeometry()
  {}

  SurfaceGeometry &SurfaceGeometry::operator=(SurfaceGeometry const &geometry)
  {
    _vertices = geometry._vertices;
    _normals = geometry._normals;
    _indices = geometry._indices;
    _boundingBoxLowerBoundUm = geometry._boundingBoxLowerBoundUm;
    _boundingBoxUpperBoundUm = geometry._boundingBoxUpperBoundUm;
    _boundingBoxUpToDate = geometry._boundingBoxUpToDate;
    return *this;
  }

  std::vector<SurfaceGeometry::VertexT> const &SurfaceGeometry::vertices() const
  {
    return _vertices;
  }
  
  std::vector<SurfaceGeometry::VertexT> &SurfaceGeometry::vertices()
  {
    _boundingBoxUpToDate = false;
    return _vertices;
  }

  std::vector<SurfaceGeometry::NormalT> const &SurfaceGeometry::normals() const
  {
    return _normals;
  }

  std::vector<SurfaceGeometry::NormalT> &SurfaceGeometry::normals()
  {
    return _normals;
  }

  std::vector<SurfaceGeometry::IndexT> const &SurfaceGeometry::indices() const
  {
    return _indices;
  }

  std::vector<SurfaceGeometry::IndexT> &SurfaceGeometry::indices()
  {
    return _indices;
  }

  void SurfaceGeometry::computeDefaultNormals()
  {
    assert(_indices.size() % 3 == 0);
    _normals.resize(_vertices.size(), NormalT(0.0f));
    for (size_t i = 0; i < _indices.size(); i += 3)
    {
      for (int d = 0; d < 3; ++d)
      {
        NormalT normal = blitz::cross(
            VertexT(_vertices[_indices[i + (d + 1) % 3]] -
                    _vertices[_indices[i + d]]),
            VertexT(_vertices[_indices[i + (d + 2) % 3]] -
                    _vertices[_indices[i + d]]));
        float n = static_cast<float>(std::sqrt(blitz::dot(normal, normal)));
        if (n > 0.0f) _normals[_indices[i + d]] += normal / n;
      }
    }
    for (size_t i = 0; i < _normals.size(); ++i)
    {
      float n = static_cast<float>(
          std::sqrt(blitz::dot(_normals[i], _normals[i])));
      if (n > 0.0f) _normals[i] /= n;
      else _normals[i] = VertexT(1.0, 0.0, 0.0);
    }    
  }

  void SurfaceGeometry::renderSTL(std::ostream &os, bool writeASCII) const
  {
    if (writeASCII)
    {
      for (size_t i = 0; i < _indices.size(); i += 3)
      {
        NormalT normal = (_normals[_indices[i]] + _normals[_indices[i + 1]] +
                          _normals[_indices[i + 2]]) / 3.0f;
        os << "facet normal " << normal(0) << " " << normal(1) << " "
           << normal(2) << std::endl
           << "outer loop" << std::endl
           << "vertex " << _vertices[_indices[i]](0) << " "
           << _vertices[_indices[i]](1) << " " << _vertices[_indices[i]](2)
           << std::endl
           << "vertex " << _vertices[_indices[i + 1]](0) << " "
           << _vertices[_indices[i + 1]](1) << " "
           << _vertices[_indices[i + 1]](2)
           << std::endl
           << "vertex " << _vertices[_indices[i + 2]](0) << " "
           << _vertices[_indices[i + 2]](1) << " "
           << _vertices[_indices[i + 2]](2)
           << std::endl
           << "endloop" << std::endl
           << "endfacet" << std::endl;
      }
    }
    else
    {
      for (size_t i = 0; i < _indices.size(); i += 3)
      {
        size_t i0 = _indices[i];
        size_t i1 = _indices[i + 1];
        size_t i2 = _indices[i + 2];
        NormalT normal = (_normals[i0] + _normals[i1] + _normals[i2]) / 3.0f;
        float vertexData[] =
            { normal(0), normal(1), normal(2),
              _vertices[i0](0), _vertices[i0](1), _vertices[i0](2),
              _vertices[i1](0), _vertices[i1](1), _vertices[i1](2),
              _vertices[i2](0), _vertices[i2](1), _vertices[i2](2),
            };
        os.write(reinterpret_cast<char*>(vertexData), 12 * sizeof(float));
        os.put(0);
        os.put(0);
      }
    }
  }
  
  void SurfaceGeometry::planeIntersection(
      blitz::TinyVector<float,3> const &, float,
      std::vector< blitz::TinyVector<blitz::TinyVector<float,2>,2> > &)
      const
  {
    // To be implemented
  }
  
  void SurfaceGeometry::planeIntersection(
      int direction, float planeOffset,
      std::vector< blitz::TinyVector<blitz::TinyVector<float,2>,2> > &lines)
      const
  {
    for (size_t i = 0; i < _indices.size(); i += 3)
    {
      blitz::TinyVector<blitz::TinyVector<float,2>,2> line;
      int p = 0;
      for (int d = 0; d < 3 && p < 2; ++d)
      {
        VertexT const &v1 = _vertices[_indices[i + d]];
        VertexT const &v2 = _vertices[_indices[i + (d + 1) % 3]];
        if ((v1(direction) <= planeOffset && v2(direction) <= planeOffset) ||
            (v1(direction) >= planeOffset && v2(direction) >= planeOffset) ||
            (v1(direction) == v2(direction))) continue;
        float alpha = (planeOffset - v1(direction)) /
            (v2(direction) - v1(direction));
        switch (direction)
        {
        case 0:
          line(p)(0) = v1(1) + alpha * (v2(1) - v1(1));
          line(p)(1) = v1(2) + alpha * (v2(2) - v1(2));
          break;
        case 1:
          line(p)(0) = v1(0) + alpha * (v2(0) - v1(0));
          line(p)(1) = v1(2) + alpha * (v2(2) - v1(2));
          break;
        case 2:
          line(p)(0) = v1(1) + alpha * (v2(1) - v1(1));
          line(p)(1) = v1(0) + alpha * (v2(0) - v1(0));
          break;
        default:
          std::cerr << __FILE__ << ":" << __LINE__
                    << ": SurfaceGeometry::planeIntersection(): "
                    << "invalid orthogonal dimension " << direction
                    << " given. It must be between 0 and 2." << std::endl;
          exit(-1);
        }
        p++;
      }
      if (p == 2) lines.push_back(line);
    }
  }
 
  blitz::TinyVector<double,3> const &
  SurfaceGeometry::boundingBoxLowerBoundUm() const
  {
    if (!_boundingBoxUpToDate) _updateBoundingBox();
    return _boundingBoxLowerBoundUm;
  }
  
  blitz::TinyVector<double,3> const &
  SurfaceGeometry::boundingBoxUpperBoundUm() const
  {
    if (!_boundingBoxUpToDate) _updateBoundingBox();
    return _boundingBoxUpperBoundUm;
  }
  
  void SurfaceGeometry::_updateBoundingBox() const
  {
    if (_vertices.size() == 0)
    {
      _boundingBoxLowerBoundUm = 0.0;
      _boundingBoxUpperBoundUm = 0.0;
    }
    else
    {
      _boundingBoxLowerBoundUm = _boundingBoxUpperBoundUm = _vertices[0];
      for (size_t i = 1; i < _vertices.size(); ++i)
      {
        for (int d = 0; d < 3; ++d)
        {
          if (_vertices[i](d) < _boundingBoxLowerBoundUm(d))
              _boundingBoxLowerBoundUm(d) = _vertices[i](d);
          if (_vertices[i](d) > _boundingBoxUpperBoundUm(d))
              _boundingBoxUpperBoundUm(d) = _vertices[i](d);          
        }
      }
    }
    _boundingBoxUpToDate = true;
  }
 
}
