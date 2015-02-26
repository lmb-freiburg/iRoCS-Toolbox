/**************************************************************************
 **       Title: Surface marker for arbitrary surfaces
 **    $RCSfile$
 **   $Revision: 3881 $ $Name$
 **       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
 **   Copyright: GPL $Author: tschmidt $
 ** Description:
 **
 **************************************************************************/

/*======================================================================*/
/*!
 *  \file SurfaceMarker.hh
 *  \brief SurfaceMarker class derived from PointMarker extending it by
 *    a SurfaceGeometry object holding the marker shape.
 */
/*======================================================================*/

#ifndef ATBSURFACEMARKER_HH
#define ATBSURFACEMARKER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "PointMarker.hh"
#include "SurfaceGeometry.hh"

namespace atb
{

/*======================================================================*/
/*!
 *  \class SurfaceMarker SurfaceMarker.hh "atb/SurfaceMarker.hh"
 *  \brief The SurfaceMarker class is an extension of the PointMarker class
 *     providing additional marker geometry information in a SurfaceGeometry
 *     object.
 *
 *  This class belongs to the iRoCS Toolbox framework and contains information
 *  on markers with arbitrary geometry which is given relative to the center
 *  point specified by the coordinates of the underlying PointMarker. Markers
 *  can be associated to AnnotationChannel objects. By this association
 *  ownership and responsibility for removing the marker resources are passed
 *  to the AnnotationChannel. This means, if the AnnotationChannel is deleted
 *  all associated markers will be deleted, too, and you must not delete
 *  markers as long as they are associated to an AnnotationChannel.
 */
/*======================================================================*/
  class SurfaceMarker : public PointMarker
  {
    
  public:
    
/*======================================================================*/
/*! 
 *   Constructor. Creates a new SurfaceMarker with given base position,
 *   geometry and optionally a channel it will be associcated with.
 *
 *   \param positionUm The marker base position. It will be passed to the
 *     PointMarker constructor. The geometry information of this marker
 *     is relative to this position.
 *   \param surface    The SurfaceGeometry object containing vertices,
 *     normals and indices describing the marker shape.
 *   \param channel    The AnnotationChannel this marker will be associated
 *     with. Inserts this marker into the channel after construction.
 *     Ownership of the marker is passed to the AnnotationChannel.
 */
/*======================================================================*/
    SurfaceMarker(
        blitz::TinyVector<double,3> const &positionUm = 0.0,
        SurfaceGeometry const * const surface = NULL,
        AnnotationChannel *channel = NULL);

/*======================================================================*/
/*! 
 *   Copy Constructor. Creates a copy of the given marker. On construction
 *   the new marker will not be associated to any channel independent of
 *   the association of the original marker.
 *
 *   \param marker The marker to copy
 */
/*======================================================================*/
    SurfaceMarker(SurfaceMarker const &marker);

/*======================================================================*/
/*! 
 *   Destructor. If the marker is associated to an AnnotationChannel at
 *   deletion, it will be removed from it and the AnnotationChannel's
 *   modified flag will be set!
 */
/*======================================================================*/
    virtual ~SurfaceMarker();
    
/*======================================================================*/
/*! 
 *   Copy assignment operator. Copies the state of the given marker into
 *   this marker. The AnnotationChannel association will not be changed.
 *   Using this method will set the modified-flag of the associated
 *   AnnotationChannel!
 *
 *   \param marker The marker to copy
 */
/*======================================================================*/
    SurfaceMarker &operator=(SurfaceMarker const &marker);
    
/*======================================================================*/
/*! 
 *   Get the MarkerType of this marker. This allows runtime-type checking.
 *
 *   \return \c Marker::Surface
 */
/*======================================================================*/
    virtual Marker::MarkerType type() const;

/*======================================================================*/
/*! 
 *   Returns whether the marker is of the class associated to the given
 *   MarkerType or one of its ancestors. This queries the isA relation
 *   of object oriented programming.
 *
 *   \param type The query MarkerType
 *
 *   \return \c true if this marker is of or inherits from the given
 *     MarkerType, \c false otherwise
 */
/*======================================================================*/
    virtual bool isA(Marker::MarkerType type) const;
    
/*======================================================================*/
/*! 
 *   Get a read only reference onto the SurfaceGeometry of this marker.
 *
 *   \return The marker's surface geometry object
 */
/*======================================================================*/
    SurfaceGeometry const &surface() const;
    
/*======================================================================*/
/*! 
 *   Get a random-access reference onto the SurfaceGeometry object of
 *   this marker. Using this method will set the modified-flag of the
 *   associated AnnotationChannel!
 *
 *   \return The marker's surface geometry object
 */
/*======================================================================*/
    SurfaceGeometry &surface();  
    
/*======================================================================*/
/*! 
 *   Save an AnnotationChannel containing markers of this type to an HDF5
 *   group.
 *
 *   \param channel The AnnotationChannel to save
 *   \param outFile The BlitzH5File to write the AnnotationChannel to
 *   \param group   The HDF5 group to save the AnnotationChannel to
 *
 *   \exception BlitzH5Error If the AnnotationChannel could not be saved
 *     this exception is thrown.
 */
/*======================================================================*/
    static void save(AnnotationChannel const *channel,
                     BlitzH5File &outFile, std::string const &group);
    
/*======================================================================*/
/*! 
 *   Load an AnnotationChannel containing markers of this type from an HDF5
 *   group. If the markers to load do not contain surface geometry
 *   information the markers will still be loaded but do not contain
 *   geometry information. On successful read and insert this method will
 *   set the modified-flag of the AnnotationChannel!
 *
 *   \param channel The AnnotationChannel to load the markers to. If the
 *     channel already contains markers the loaded markers will be added to
 *     the existing ones.
 *   \param inFile  The BlitzH5File to read the markers from
 *   \param group   The HDF5 group containing the markers
 *
 *   \exception BlitzH5Error If the markers could not be loaded this
 *     exception is thrown.
 */
/*======================================================================*/
    static void load(AnnotationChannel *channel,
                     const BlitzH5File &inFile, std::string const &group);
    
/*======================================================================*/
/*! 
 *   Write the CSV header for SurfaceMarkers to the given output stream.
 *   This function is independent of the actual marker, so it in principle
 *   should be static, but to allow polymorphism it is a normal const member.
 *   The CSV separator is the semicolon.
 *
 *   \param out The output stream to write the header to.
 *
 *   \exception std::exception If the header could not be appended to the
 *     output stream this exception is thrown
 */
/*======================================================================*/
    virtual void writeCSVHeader(std::ostream &out) const;
    
/*======================================================================*/
/*! 
 *   Write this SurfaceMarker to the given output stream as semicolon
 *   separated values. To avoid extensive CSV file sizes, the
 *   SurfaceGeomerty is not actually written to the file, but empty fields
 *   are inserted instead.
 *
 *   \param out The output stream to write the SurfaceMarker to.
 *
 *   \exception std::exception If the marker could not be appended to the
 *     output stream this exception is thrown
 */
/*======================================================================*/
    virtual void saveCSV(std::ostream &out) const;

/*======================================================================*/
/*! 
 *   Load the marker properties from the given attribute-value-map.
 *   The attribute names are the ones used in the CSV header. On
 *   successfully read this method will set the modified-flag of the
 *   associated AnnotationChannel!
 *
 *   \param values The attribute-value map containing the SurfaceMarker
 *     attributes
 */
/*======================================================================*/
    virtual void loadFromMap(std::map<std::string,std::string> const &values);
    
/*======================================================================*/
/*! 
 *   Query whether the given position is inside the SurfaceMarker. This
 *   function only approximately estimates, whether the given point is
 *   within the marker volume. It is only intended for visualization
 *   purposes, not for exact math.
 *
 *   \param positionUm The query position
 *
 *   \return \c true if the position is inside the marker volume, \c false
 *     otherwise
 */
/*======================================================================*/
    virtual bool occupiesPositionUm(
        blitz::TinyVector<double,3> const &positionUm) const;
    
  private:
    
    virtual void _updateBoundingBox() const;
    
    SurfaceGeometry _surface;
    
  };

}

#endif
