/**************************************************************************
 **       Title: Neighborhoods for local operators
 **    $RCSfile:$
 **   $Revision:$ $Name:$
 **       $Date:$
 **   Copyright: GPL $Author:$
 ** Description:
 **
 **   Neighborhood definitions for local operators
 **
 **************************************************************************/

/*======================================================================*/
/*!
 *  \file atb/Neighborhood.hh
 *  \brief Neighborhoods for local operators.
 */
/*======================================================================*/

#ifndef ATBNEIGHBORHOOD_HH
#define ATBNEIGHBORHOOD_HH

#include <blitz/array.h>
#include <set>
#include <vector>

namespace atb
{
  
/*======================================================================*/
/*! 
 *   \struct TinyVectorLessThan Neighborhood.hh "atb/Neighborhood.hh"
 *   \brief The TinyVectorLessThan struct provides an operator
 *     for comparing TinyVectors defining a strict weak ordering between them.
 *
 *   This is especially useful for the STL containers set and map and their
 *   multi-variants, which require a strict weak ordering on their stored keys.
*/
/*======================================================================*/
  template<typename DataT, int Dim>
  struct TinyVectorLessThan
  {
    
/*======================================================================*/
/*! 
 *   Strict weak ordering function for TinyVectors.
 *
 *   \param v1 The first vector
 *   \param v2 The second vector
 *
 *   \return \f$\exists D \in \left\{ 1, \ldots, \mathrm{Dim} \right\},
 \forall d < D: v_1(d) = v_2(d) \wedge v_1(D) < v_2(D)\f$
*/
/*======================================================================*/
    bool operator()(
      blitz::TinyVector<DataT,Dim> const &v1,
      blitz::TinyVector<DataT,Dim> const &v2) const;

  };
  
/*======================================================================*/
/*!
 *  \class Neighborhood Neighborhood.hh "atb/Neighborhood.hh"
 *  \brief The Neighborhood class specifies local pixel/voxel neighborhoods.
 *
 *  These neighborhoods are needed in different local filter operations or
 *  algorithms that operate on local neighborhoods, like i.e. the Dijkstra
 *  algorithm. A neighborhood specifies a stencil containing neighbor
 *  locations relative to the central pixel, so e.g. a 2D 4-neighborhood
 *  consists of the points \f$(-1,0), (1, 0), (0, -1), (0, 1)\f$.
 *
 *  \see Dijkstra
 */
/*======================================================================*/
  template<int Dim>
  class Neighborhood
  {
    
  public:
    
/*======================================================================*/
/*!
 *  \enum Type Neighborhood.hh "atb/Neighborhood.hh"
 *  \brief The Type enum contains the different Neighborhood type names.
 *
 *  \li \c Simple Only pixels connected by faces of dimension Dim - 1 are
 *    included into the neighborhood (i.e. 2D = 4-neighborhood,
 *    3D = 6-neighborhood)
 *  \li \c Complex All voxels that touch the central pixel in at least one
 *    point are included in the neighborhood (i.e. 2D = 8-neighborhood,
 *    3D = 26-neighborhood)
 *  \li \c User All other neighborhoods are user defined neighborhoods.
 *    This is the case for all neighborhoods created using the constructor
 *    taking point vectors, if the points are set using the setNeighbors
 *    function or if the neighborhood is altered using clear, push_back or
 *    erase.
 */
/*======================================================================*/
    enum Type { Simple, Complex, User };
  
    typedef std::set<
      blitz::TinyVector<BlitzIndexT,Dim>, TinyVectorLessThan<BlitzIndexT,Dim> >
    NeighborSet;
    
    typedef typename NeighborSet::const_iterator const_iterator;
  
    
/*======================================================================*/
/*! 
 *   Default constructor.
 *
 *   \param nhoodType The type of the neighborhood to create. If you pass
 *     \c User an empty Neighborhood is created.
 */
/*======================================================================*/
    Neighborhood(Type nhoodType = Simple);

/*======================================================================*/
/*! 
 *   Creation of a user-defined neighborhood with the given neighbors.
 *
 *   \param neighbors The positions of the neighbors of the current pixel.
 *     They need not be adjacent to the central pixel and they need not
 *     describe a compact region.
 */
/*======================================================================*/
    Neighborhood(NeighborSet const &neighbors);

/*======================================================================*/
/*! 
 *   Creation of a user-defined neighborhood with the given neighbors.
 *   If neighbors appear multiple times, the duplicates will be removed.
 *
 *   \param neighbors The positions of the neighbors of the current pixel.
 *     They need not be adjacent to the central pixel and they need not
 *     describe a compact region.
 */
/*======================================================================*/
    Neighborhood(
        std::vector< blitz::TinyVector<BlitzIndexT,Dim> > const &neighbors);

/*======================================================================*/
/*! 
 *   Copy constructor.
 *
 *   \param nhood The Neighborhood to copy.
 */
/*======================================================================*/
    Neighborhood(Neighborhood<Dim> const &nhood);

/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    ~Neighborhood();

/*======================================================================*/
/*! 
 *   Copy assignment operator. Sets the neighborhood to the neighborhood
 *   given.
 *
 *   \param nhood The neighborhood to copy.
 *
 *   \return A reference to this Neighborhood for operator chaining
 */
/*======================================================================*/
    Neighborhood<Dim> &operator=(Neighborhood<Dim> const &nhood);

/*======================================================================*/
/*! 
 *   Get the type of this Neighborhood. Any modification of the Neighborhood
 *   after creation will set the NeighborhoodType to \c User, even if the
 *   changes are undone again.
 *
 *   \return The NeighborhoodType enum value corresponding to this
 *     Neighborhood
 */
/*======================================================================*/
    Type type() const;
    
/*======================================================================*/
/*! 
 *   Get the number of neighbors this Neighborhood contains.
 *
 *   \return The number of neighbors in the Neighborhood
 */
/*======================================================================*/
    size_t size() const;
    
/*======================================================================*/
/*! 
 *   Get read-only access to the neighbor set.
 *
 *   \return The neighbor set of this Neighborhood.
 */
/*======================================================================*/
    NeighborSet const &neighbors() const;

/*======================================================================*/
/*! 
 *   Remove all neighbors from this Neighborhood.
 */
/*======================================================================*/
    void clear();

/*======================================================================*/
/*! 
 *   Insert a new neighbor into this Neighborhood. If the neighbor is
 *   already contained in the Neighborhood it won't be changed.
 *
 *   \param neighbor The neighbor position to insert.
 */
/*======================================================================*/
    void insert(blitz::TinyVector<BlitzIndexT,Dim> const &neighbor);

/*======================================================================*/
/*! 
 *   Erase the given neighbor from the Neighborhood. If the neighbor is not
 *   contained in the Neighborhood it is unaltered.
 *
 *   \param neighbor The neighbor position to remove.
 */
/*======================================================================*/
    void erase(blitz::TinyVector<BlitzIndexT,Dim> const &neighbor);

/*======================================================================*/
/*! 
 *   Set the neighbors within this Neighborhood to the ones provided.
 *
 *   \param nhood The neighbor position vector.
 */
/*======================================================================*/
    void setNeighbors(NeighborSet const &nhood);

/*======================================================================*/
/*! 
 *   Set the neighbors within this Neighborhood to the ones provided. If
 *   neighbors appear multiple times the duplicates will be removed.
 *
 *   \param nhood The neighbor position vector.
 */
/*======================================================================*/
    void setNeighbors(
        std::vector< blitz::TinyVector<BlitzIndexT,Dim> > const &nhood);

/*======================================================================*/
/*! 
 *   Get an iterator to the beginning of the Neighborhood.
 *
 *   \return A NeighborhoodSet iterator pointing to the first neighbor of
 *     this Neighborhood
 */
/*======================================================================*/
    const_iterator begin() const;
    
/*======================================================================*/
/*! 
 *   Get an iterator one element past the end of this Neighborhood.
 *
 *   \return A NeighborhoodSet iterator pointing behind the last neighbor
 *     of this Neighborhood
 */
/*======================================================================*/
    const_iterator end() const;

  private:
    
    Type _type;
    NeighborSet _neighbors;

  };

}

#include "Neighborhood.icc"

#endif
