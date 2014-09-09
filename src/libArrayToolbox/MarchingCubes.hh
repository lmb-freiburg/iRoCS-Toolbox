/**************************************************************************
 **       Title: The Marching Cubes Algorithm
 **    $RCSfile:$
 **   $Revision:$ $Name:$
 **       $Date:$
 **   Copyright: GPL $Author:$
 ** Description:
 **
 **   Implementation of the Marching Cubes algorithm to convert the zero
 **   levelset of a 3D function into a polygon mesh.
 **
 **************************************************************************/

/*======================================================================*/
/*!
 *  \file atb/MarchingCubes.hh
 *  \brief The marching cubes algorithm to convert the zero
 *    levelset of a 3D function into a polygon mesh.
 */
/*======================================================================*/

#ifndef ATBMARCHINGCUBES_HH
#define ATBMARCHINGCUBES_HH

#include <blitz/array.h>

// #ifdef HAVE_CONFIG_H
// #include "../config.hh"
// #endif

// #ifdef HAVE_BLITZ_V9
// #include <blitz/tinyvec-et.h>
// #endif

#include <vector>
#include <set>

#include "Array.hh"
#include "SurfaceGeometry.hh"
#include "Neighborhood.hh"

namespace atb
{

/*======================================================================*/
/*!
 *  \class MarchingCubes MarchingCubes.hh "atb/MarchingCubes.hh"
 *  \brief The MarchingCubes class provides the Marching Cubes Algorithm
 *     for isosurface tesselation of real scalar volume data.
 *
 *  It is mainly used to convert volumetric masks into surface
 *  representations. For binary input the algorithm produces staircasing
 *  artifacts, that can be avoided by smoothing the transitions
 *  between foreground and background before passing the mask to the Marching
 *  Cubes algorithm.
 */
/*======================================================================*/
  class MarchingCubes
  {

  public:
    
/*======================================================================*/
/*!
 *  A triangle in 3-D space
 */
/*======================================================================*/
    typedef blitz::TinyVector<SurfaceGeometry::VertexT,3> Triangle;

  private:
    
    struct GridCell
    {
      blitz::TinyVector<SurfaceGeometry::VertexT,8> cornerCoordinates;
      blitz::TinyVector<double,8> cornerValues;
    };

    static int edgeTable[256];    
    static int triangleTable[256][16];

    static int computeTrianglesForGridCell(
        GridCell const &gridCell, std::vector<Triangle> &triangles,
        double isoLevel = 0.0);
    
    static void computeTrianglesForGridCell(
        GridCell const &gridCell, blitz::TinyVector<int,12> &cellIndices,
        SurfaceGeometry &surface, double isoLevel = 0.0);

    static SurfaceGeometry::VertexT getInterpolatedVertex(
        double isoLevel, SurfaceGeometry::VertexT const &p1,
        SurfaceGeometry::VertexT const &p2, double val1, double val2);

  public:
    
/*======================================================================*/
/*! 
 *   The triangulate function computes a vertex mesh from a given levelset
 *   Array. The input Array type must be scalar and real. Integer and
 *   boolean Arrays should work as well, for the vertex position computation
 *   the Array values are casted to double, so if your data type supports
 *   double casts properly, it should work.
 *
 *   \param data The Array containing the function to extract a levelset
 *     surface from
 *   \param surface  The SurfaceGeometry object, the surface will be
 *     written to
 *   \param isoLevel The function value to extract the surface for. For
 *     binary input 0.5 is a good choice.
 *   \param simplifyTolerance  If a value greater than 0 is given here
 *     the mesh will be simplified by merging triangles until a triangle
 *     merge exceeds the given tolerance which is the volume difference
 *     before and after the merge
 */
/*======================================================================*/
    template<typename DataT>
    static void triangulate(
        Array<DataT,3> const &data, SurfaceGeometry &surface,
        double isoLevel = 0.0, double simplifyTolerance = 0.0);

/*======================================================================*/
/*! 
 *   The triangulate function computes a vertex mesh from a given levelset
 *   Array. The input Array type must be scalar and real. Integer and
 *   boolean Arrays should work as well, for the vertex position computation
 *   the Array values are casted to double, so if your data type supports
 *   double casts properly, it should work.
 *
 *   \param data The Array containing the function to extract a levelset
 *     surface from
 *   \param elementSizeUm The element size in micrometers
 *   \param surface  The SurfaceGeometry object, the surface will be
 *     written to
 *   \param isoLevel The function value to extract the surface for. For
 *     binary input 0.5 is a good choice.
 *   \param simplifyTolerance  If a value greater than 0 is given here
 *     the mesh will be simplified by merging triangles until a triangle
 *     merge exceeds the given tolerance which is the volume difference
 *     before and after the merge
 */
/*======================================================================*/
    template<typename DataT>
    static void triangulate(
        blitz::Array<DataT,3> const &data,
        blitz::TinyVector<double,3> const &elementSizeUm,
        SurfaceGeometry &surface, double isoLevel = 0.0,
        double simplifyTolerance = 0.0);

/*======================================================================*/
/*! 
 *   Simplify the given mesh by merging vertices as long as the given
 *   tolerance is not exceeded.
 *
 *   \param surface  The SurfaceGeometry to simplify
 *   \param simplifyTolerance  If a value greater than 0 is given here
 *     the mesh will be simplified by merging triangles until a triangle
 *     merge exceeds the given tolerance which is the volume difference
 *     before and after the merge
 */
/*======================================================================*/
    static void simplifyMesh(
        SurfaceGeometry &surface, double simplifyTolerance);

  private:
    
    static double _computeEdgeRemovalCost(
        std::pair<size_t,size_t> const &edge, SurfaceGeometry const &surface,
        std::vector< std::set<size_t> > const &triangles);

    static void _checkAndFixMesh(SurfaceGeometry &surface);

  };

}

#include "MarchingCubes.icc"

#endif
