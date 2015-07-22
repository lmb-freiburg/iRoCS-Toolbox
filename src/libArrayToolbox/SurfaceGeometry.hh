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

/*======================================================================*/
/*!
 *  \file SurfaceGeometry.hh
 *  \brief Storage and rendering of Triangulated surfaces.
 */
/*======================================================================*/

#ifndef ATBSURFACEGEOMETRY_HH
#define ATBSURFACEGEOMETRY_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <vector>

#include <blitz/array.h>

namespace atb
{

/*======================================================================*/
/*!
 *  \class SurfaceGeometry SurfaceGeometry.hh "libArrayToolbox/SurfaceGeometry.hh"
 *  \brief The SurfaceGeometry struct provides data structures required
 *    for the storage of triangulated surface data for direct OpenGL
 *    rendering or export to STL files.
 */
/*======================================================================*/
  class SurfaceGeometry 
  {
    
  public:
    
/*======================================================================*/
/*! 
 *  \brief VertexT is the type used to store a 3D vertex position
 */
/*======================================================================*/
    typedef blitz::TinyVector<float,3> VertexT;

/*======================================================================*/
/*! 
 *  \brief NormalT is the type used to store a 3D vertex normal
 */
/*======================================================================*/
    typedef blitz::TinyVector<float,3> NormalT;

/*======================================================================*/
/*! 
 *  \brief IndexT is the type used to store the triangle corner indices in the
 *  vertex Array
 */
/*======================================================================*/
    typedef unsigned int IndexT;
    
/*======================================================================*/
/*! 
 *   \brief Default Constructor.
 *
 *   Creates a new empty SurfaceGeometry object.
 */
/*======================================================================*/
    SurfaceGeometry();

/*======================================================================*/
/*! 
 *   \brief Copy Constructor.
 *
 *   Creates a new copy of the given SurfaceGeometry object.
 *
 *   \param geometry The Surface geometry to copy
 */
/*======================================================================*/
    SurfaceGeometry(SurfaceGeometry const &geometry);

/*======================================================================*/
/*! 
 *   \brief Destructor.
 */
/*======================================================================*/
    ~SurfaceGeometry();
    
/*======================================================================*/
/*! 
 *   \brief Copy assignment operator.
 *
 *   Sets the data of this SurfaceGeometry object to the right-hand-side
 *   SurfaceGeometry.
 *
 *   \param geometry The Surface geometry to copy
 *
 *   \return A reference to this SurfaceGeometry object for operator
 *      chaining
 */
/*======================================================================*/
    SurfaceGeometry &operator=(SurfaceGeometry const &geometry);

/*======================================================================*/
/*! 
 *   \brief Get a read only reference onto the Vertex vector of this
 *   SurfaceGeometry object.
 *
 *   \return A reference to the Vertex vector of this SurfaceGeometry object
 */
/*======================================================================*/
    std::vector<VertexT> const &vertices() const;

/*======================================================================*/
/*! 
 *   \brief Get a random-access reference onto the Vertex vector of this
 *   SurfaceGeometry object.
 *
 *   When requesting a read-write reference to the vertices, the bounding
 *   will be recomputed when requested the next time. When editing a stored
 *   reference, the bounding box will be only recomputed at first bounding box
 *   request, try to avoid keeping references to the vertex vector, or
 *   request a new reference if you want to force a bounding box update.
 *
 *   \return A reference to the Vertex vector of this SurfaceGeometry object
 */
/*======================================================================*/
    std::vector<VertexT> &vertices();

/*======================================================================*/
/*! 
 *   \brief Get a read only reference onto the Normal vector of this
 *   SurfaceGeometry object.
 *
 *   \return A reference to the Normal vector of this SurfaceGeometry object
 */
/*======================================================================*/
    std::vector<NormalT> const &normals() const;

/*======================================================================*/
/*! 
 *   \brief Get a random-access reference onto the Normal vector of this
 *   SurfaceGeometry object.
 *
 *   \return A reference to the Normal vector of this SurfaceGeometry object
 */
/*======================================================================*/
    std::vector<NormalT> &normals();

/*======================================================================*/
/*! 
 *   \brief Get a read only reference onto the Index vector of this
 *   SurfaceGeometry object.
 *
 *   \return A reference to the Index vector of this SurfaceGeometry object
 */
/*======================================================================*/
    std::vector<IndexT> const &indices() const;

/*======================================================================*/
/*! 
 *   \brief Get a random-access reference onto the Index vector of this
 *   SurfaceGeometry object.
 *
 *   \return A reference to the Index vector of this SurfaceGeometry object
 */
/*======================================================================*/
    std::vector<IndexT> &indices();

/*======================================================================*/
/*! 
 *   \brief Computes the surface normals from the provided vertices and
 *     indices.
 *
 *   For each vertex the normals of all adjacent triangles are computed
 *   using the cross product of its edges and averaged giving the normal
 *   for that vertex.
 */
/*======================================================================*/
    void computeDefaultNormals();

/*======================================================================*/
/*! 
 *   \brief Appends the set of triangle primitives described by this
 *   SurfaceGeometry as STL facets to the given output stream.
 *
 *   The header and footer of the STL file must be pre- and appended by
 *   the user. For the ASCII case this means the stream must start with
 *   'solid <name>' and end with 'endsolid <name>'. It is not possible to
 *   put multiple solids into the same file, therefore write the header,
 *   then all objects and finally the footer. For the binary file format it is
 *   slightly different, the files start with an arbitrary 80 Byte header
 *   which is by default ignored, Bytes 80 - 84 are the number of all
 *   triangles in the file as unsigned int. The binary file needs no footer.
 *
 *   \param os         The stream to write the triangles to
 *   \param writeASCII If writeASCII is true the triangles will be written
 *     in user readable ASCII format, otherwise it will be written in
 *     binary format. In binary format only the raw vertex positions and
 *     normals are stored. The header has to be already written to the
 *     stream beforehand
 */
/*======================================================================*/
    void renderSTL(std::ostream &os, bool writeASCII = true) const;
    
/*======================================================================*/
/*! 
 *   \brief Intersect the SurfaceGeometryObject with the plane defined by the
 *   given parameters and return the resulting set of 2D line end points
 *   forming the intersection polygon.
 *
 *   \param planeNormal  The plane normal. The normal is assumed to be
 *     normalized to unit length, if it is not the behaviour is undefined.
 *   \param planeOffset  The plane offset from the coordinate origin
 *   \param lines        The resulting endpoints of all polygon segments.
 */
/*======================================================================*/
    void planeIntersection(
        blitz::TinyVector<float,3> const &planeNormal, float planeOffset,
        std::vector< blitz::TinyVector<blitz::TinyVector<float,2>,2> > &lines)
        const;

/*======================================================================*/
/*! 
 *   \brief Intersect the SurfaceGeometryObject with the plane defined by the
 *   given parameters and return the resulting set of 2D line end points
 *   forming the intersection polygon.
 *
 *   This implementation is a special case of the method above for axis
 *   aligned planes.
 *
 *   \param direction    The orthogonal dimension to the plane
 *   \param planeOffset  The plane offset from the coordinate origin
 *   \param lines        The resulting endpoints of all polygon segments.
 */
/*======================================================================*/
    void planeIntersection(
        int direction, float planeOffset,
        std::vector< blitz::TinyVector<blitz::TinyVector<float,2>,2> > &lines)
        const;

/*======================================================================*/
/*! 
 *   Get the tight, axis-aligned lower bound of the bounding box for this
 *   surface geometry. Lower and upper bound will be recomputed if a
 *   random-access reference to the vertex vector of this SurfaceGeometry
 *   was requested since the last call to boundingBoxLowerBoundUm() or
 *   boundingBoxUpperBoundUm(). If the user stores a persistent reference
 *   for editing the automatic update fails.
 *
 *   \return The lower bound of the bounding box enclosing this
 *     SurfaceGeometry
 */
/*======================================================================*/
    blitz::TinyVector<double,3> const &boundingBoxLowerBoundUm() const;

/*======================================================================*/
/*! 
 *   Get the tight, axis-aligned upper bound of the bounding box for this
 *   surface geometry. Lower and upper bound will be recomputed if a
 *   random-access reference to the vertex vector of this SurfaceGeometry
 *   was requested since the last call to boundingBoxLowerBoundUm() or
 *   boundingBoxUpperBoundUm(). If the user stores a persistent reference
 *   for editing the automatic update fails.
 *
 *   \return The upper bound of the bounding box enclosing this
 *     SurfaceGeometry
 */
/*======================================================================*/
    blitz::TinyVector<double,3> const &boundingBoxUpperBoundUm() const;

  private:

    void _updateBoundingBox() const;

    std::vector<VertexT> _vertices;
    std::vector<NormalT> _normals;
    std::vector<IndexT> _indices;

    mutable blitz::TinyVector<double,3> _boundingBoxLowerBoundUm,
        _boundingBoxUpperBoundUm;
    mutable bool _boundingBoxUpToDate;
    
  };

}

#endif
