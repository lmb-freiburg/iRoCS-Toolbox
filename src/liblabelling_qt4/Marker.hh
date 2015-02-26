/**************************************************************************
**       Title: Generic Marker
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#ifndef MARKER_HH
#define MARKER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtCore/QObject>

#include <blitz/array.h>
#include <libBlitzHdf5/BlitzHdf5Light.hh>

#include <libArrayToolbox/ATBNucleus.hh>

#include <map>

class MarkerPresetWidget;
class MarkerControlWidget;

class OrthoViewWidget;
class AnnotationChannelSpecs;
class MarkerRenderer;
class ViewWidget;

/*======================================================================*/
/*!
 *  \class Marker Marker.hh "liblabelling_qt4/Marker.hh"
 *  \brief The Marker class specifies the properties of an abstract Marker
 *    that can be contained in an AnnotationChannelSpecs.
 */
/*======================================================================*/
class Marker : public QObject
{
  
  Q_OBJECT

public:
  
/*======================================================================*/
/*!
 *  \enum MarkerType Marker.hh "liblabelling_qt4/Marker.hh"
 *  \brief The MarkerType enumeration enumerates all available marker types.
 *
 *  If you want to add a new marker type you will have to add it to this
 *  enum and change supportedMarkerTypes(), markerTypeFromString() and
 *  markerTypeToString().
 */
/*======================================================================*/
  enum MarkerType {
      Point = 0x0000,
      Sphere = 0x0001,
      Nucleus = 0x0002,
      Cylinder = 0x0003,
      SHSurface = 0x0004,
      Surface = 0x005,
      Cell = 0x006
  };

/*======================================================================*/
/*! 
 *   Get a QStringList containing string representations of all marker types.
 *   When the user selects to add a new Annotation channel, the items of
 *   this list are presented as choices for the marker type of the new
 *   annotation channel. So if you want to add a new marker type that
 *   is only automatically generated or don't want the user to be able to
 *   manually create an annotation channel of that marker type, just do not
 *   add it to the list.
 *
 *   \return All marker types that can be selected when the user decides
 *     to create a new annotation channel
 */
/*======================================================================*/
  static QStringList supportedMarkerTypes();

/*======================================================================*/
/*! 
 *   Get the MarkerType corresponding to the given string representation.
 *   If the string has no corresponding MarkerType the program will report
 *   an error and immediately shut down!
 *
 *   \param name The string representation of a MarkerType
 *
 *   \return The corresponding MarkerType
 */
/*======================================================================*/
  static MarkerType markerTypeFromString(QString const &name);

/*======================================================================*/
/*! 
 *   Get the string representation of the given MarkerType.
 *
 *   \param m The MarkerType
 *
 *   \return The corresponding string representation
 */
/*======================================================================*/
  static QString markerTypeToString(Marker::MarkerType m);

/*======================================================================*/
/*! 
 *   Constructor. Creates a new marker at the specified position and sets
 *   its parent (AnnotationChannelSpecs) to the channel given. If you create
 *   a marker in a currently displayed channel it might be a good idea to
 *   first create it without parent, set all its properties and then add it
 *   to the Channel using setChannel() to avoid view update cycles.
 *
 *   \param positionUm The marker position in micrometers
 *   \param channel    The annotation channel to add this marker to. You
 *     can set this later using setChannel().
 */
/*======================================================================*/
  Marker(blitz::TinyVector<double,3> const &positionUm = 0.0,
         AnnotationChannelSpecs *channel = NULL);

/*======================================================================*/
/*! 
 *   Copy constructor. Creates a new marker that is identical to the old
 *   marker. The new marker will be created without parent channel you have
 *   to manually add it to a channel using setChannel().
 *
 *   \param marker The original marker
 */
/*======================================================================*/
  Marker(Marker const &marker);

/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
  virtual ~Marker();

/*======================================================================*/
/*! 
 *   Copy assignment operator. Copies all attributes of the given Marker
 *   to this marker. Only the parent channel stays untouched.
 *
 *   \param marker The marker to copy all attributes from
 *
 *   \return A reference to this marker for operator chaining
 */
/*======================================================================*/
  Marker &operator=(Marker const &marker);

/*======================================================================*/
/*! 
 *   Get the MarkerType enum value for this marker. This method must be
 *   implemented by all sub-classes.
 *
 *   \return The MarkerType of this marker
 */
/*======================================================================*/
  virtual MarkerType markerType() const = 0;

/*======================================================================*/
/*! 
 *   Checks whether this marker is of the given MarkerType or derives from it.
 *   This method must be implemented by all sub-classes.
 *
 *   \return \c true if this marker is of the given MarkerType or a
 *     descendent, \c false otherwise
 */
/*======================================================================*/
  virtual bool inherits(MarkerType type) const = 0;

/*======================================================================*/
/*! 
 *   Add this marker to the given channel. This implies removing it from
 *   its old channel if there was one. You can pass NULL here if you want
 *   to remove any channel association from this marker.
 *
 *   \param channel The new channel this marker belongs to
 */
/*======================================================================*/
  void setChannel(AnnotationChannelSpecs* channel);

/*======================================================================*/
/*! 
 *   Get the channel this marker belongs to. If the marker has no parent
 *   NULL is returned.
 *
 *   \return A pointer to the channel containing this marker
 */
/*======================================================================*/
  AnnotationChannelSpecs* channel() const;

/*======================================================================*/
/*! 
 *   Add a rendering engine for the given view to this marker. For each
 *   marker type - view type pair such a Renderer must exist, that is
 *   responsible to get an appropriate visualization of the marker in the
 *   corresponding type of view. A marker can be rendered in arbitrarily
 *   many views simulataneously. This method, however, must take care that
 *   the marker is registered to a specific view at most once. Markers
 *   without associated renderers will not be drawn at all.
 *
 *   \param view The ViewWidget to register this marker to for rendering
 *
 *   \return A pointer to the newly created rendering engine for this
 *     marker/view combination
 */
/*======================================================================*/
  virtual MarkerRenderer *addRenderer(ViewWidget *view) = 0;

/*======================================================================*/
/*! 
 *   Remove the renderer for the given view.
 *
 *   \param view The view to remove this Marker from
 */
/*======================================================================*/
  void removeRenderer(ViewWidget *view);

/*======================================================================*/
/*! 
 *   Get the rendering engine for this Marker and the given view.
 *
 *   \param view The view to get the renderer for this marker for
 *
 *   \return The rendering engine for this Marker/view combination
 */
/*======================================================================*/
  MarkerRenderer *renderer(ViewWidget *view) const;

/*======================================================================*/
/*! 
 *   Get a map containing all ViewWidgets this Marker is currently drawn in
 *   and the corresponding MarkerRenderers.
 *
 *   \return The mapping of views to corresponding renderers
 */
/*======================================================================*/
  const std::map<ViewWidget*, MarkerRenderer*> &renderers() const;

/*======================================================================*/
/*! 
 *   Get the marker id. This attribute is never stored and only used for
 *   visualization in the current session. It is not even guaranteed to be
 *   unique. To identify a Marker you should instead use its address.
 *
 *   \todo Remove id or store it, currently it makes no sense.
 *
 *   \return The session id of this marker
 */
/*======================================================================*/
  std::string id() const;

/*======================================================================*/
/*! 
 *   Set the marker id. This attribute is never stored and only used for
 *   visualization in the current session.
 *
 *   \param id The session id of this marker
 */
/*======================================================================*/
  void setId(const std::string& id);

/*======================================================================*/
/*! 
 *   Get the absolute marker world position in micrometers. The result is
 *   just the relative position transformed by the channel transformation
 *   matrix.
 *
 *   \return The absolute world position in micrometers after applying the
 *     channel transformation
 */
/*======================================================================*/
  blitz::TinyVector<double,3> positionUm() const;

/*======================================================================*/
/*! 
 *   Set the absolute marker world position in micrometers. Internally
 *   this alters the relative position by applying the inverse channel
 *   transformation to the given position.
 *
 *   \param positionUm The absolute world position in micrometers
 */
/*======================================================================*/
  void setPositionUm(const blitz::TinyVector<double,3>& positionUm);

/*======================================================================*/
/*! 
 *   Get the marker position relative to the group transformation.
 *
 *   \return The marker position in micrometers in the coordinate system
 *     defined by the channel transformation
 */
/*======================================================================*/
  blitz::TinyVector<double,3> relativePositionUm() const;

/*======================================================================*/
/*! 
 *   Set the marker position relative to the group transformation.
 *
 *   \param positionUm The marker position in micrometers in the coordinate
 *     system defined by the channel transformation
 */
/*======================================================================*/
  void setRelativePositionUm(const blitz::TinyVector<double,3>& positionUm);

  bool manual() const;
  void setManual(const bool manual);

  bool needsFeatureUpdate() const;
  void setNeedsFeatureUpdate(bool needsFeatureUpdate);

  int label() const;
  void setLabel(const int label);

  int predictedLabel() const;
  void setPredictedLabel(const int label);

  const std::vector<double>& features() const;
  double& feature(size_t index);
  std::vector<double>::iterator featuresBegin();
  std::vector<double>::iterator featuresEnd();
  
  const std::map<int,double>& probabilityMap() const;
  std::map<int,double>::iterator probabilityMapFind(int key);
  std::map<int,double>::iterator probabilityMapBegin();
  std::map<int,double>::iterator probabilityMapEnd();

  double qcDistanceUm() const;
  void setQcDistanceUm(const double qcDistanceUm);
  
  double radialDistanceUm() const;
  void setRadialDistanceUm(const double radialDistanceUm);
  
  double phi() const;
  void setPhi(const double phi);
  
/*======================================================================*/
/*! 
 *   If the view type is set to random use this color to obtain the marker
 *   color to use.
 *
 *   \return The random color associated to this marker
 */
/*======================================================================*/
  blitz::TinyVector<float,3> const &randomColor() const;

  static void save(
      AnnotationChannelSpecs const *channel,
      BlitzH5File &outFile, std::string const &group,
      iRoCS::ProgressReporter *pr = NULL);
  static void load(
      AnnotationChannelSpecs *channel, BlitzH5File const &inFile,
      std::string const &group, iRoCS::ProgressReporter *pr = NULL);

  virtual void writeCSVHeader(std::ofstream &out) const;
  virtual void saveCSV(std::ofstream &out) const;
  virtual void loadFromMap(std::map<std::string,std::string> const &values);

  void setSelected(bool selected);
  bool selected() const;

/*======================================================================*/
/*! 
 *   This methods checks whether the given micrometer world position is
 *   within the marker volume and is mainly used to test whether it needs to be
 *   drawn by its renderers. It need not be exact, but should at least
 *   respect the bounding box of the Marker.
 *
 *   \param positionUm The world position in micrometers to check for
 *     intersection with this marker
 *
 *   \return \c true if the position is within the marker, \c false otherwise
 */
/*======================================================================*/
  virtual bool occupiesPositionUm(
      blitz::TinyVector<double,3> const &positionUm) const = 0;

/*======================================================================*/
/*! 
 *   Get the lower bound of the marker's bounding box for all dimensions.
 *
 *   \return The bounding box lower bound in micrometers and world coordinates
 */
/*======================================================================*/
  blitz::TinyVector<double,3> boundingBoxLowerBoundUm() const;

/*======================================================================*/
/*! 
 *   Get the upper bound of the marker's bounding box for all dimensions.
 *
 *   \return The bounding box upper bound in micrometers and world coordinates
 */
/*======================================================================*/
  blitz::TinyVector<double,3> boundingBoxUpperBoundUm() const;

/*======================================================================*/
/*! 
 *   Check whether cache updates for this marker are enabled.
 *
 *   \return true if cache updates are enabled, false otherwise
 */
/*======================================================================*/
  bool updatesEnabled() const;

  virtual void copyToATBNucleus(atb::Nucleus &nc) const;
  virtual void copyFromATBNucleus(atb::Nucleus const &nc);

public slots:

/*======================================================================*/
/*! 
 *   Enables/Disables cache updates for this marker. If you programmatically
 *   apply many subsequent changes to a marker disable cache updates,
 *   otherwise each operation triggers a cache update.
 *
 *   \param enable true to enable cache updates,
 *     false to disable cache updates
 */
/*======================================================================*/
  void setUpdatesEnabled(bool enable);

/*======================================================================*/
/*! 
 *   Explicitely trigger a cache update for all associated renderers.
 *   This is only necessary if updates were disabled and after all changes
 *   the cache has to be updated. If updates are disabled a call to this
 *   function is a noop.
 */
/*======================================================================*/
  void update();

protected:

  void setFeatures(const std::vector<double>& features);
  void setProbabilityMap(const std::map<int,double>& probabilityMap);

  static void _loadFixedSize(
      blitz::Array<bool,1> &buf, BlitzH5File const &inFile,
      std::string const &dsName, bool showWarnings = false);

  template<typename DataT>
  static void _loadFixedSize(
      blitz::Array<DataT,1> &buf, BlitzH5File const &inFile,
      std::string const &dsName, bool showWarnings = false)
        {
          size_t targetSize = buf.size();
          try
          {
            inFile.readDataset(buf, dsName);
          }
          catch (BlitzH5Error &)
          {
            buf.resize(0);
            if (showWarnings)
                std::cerr << "'" << dsName << "' not found. Setting it to -1"
                          << std::endl;
          }
          size_t readSize = buf.size();
          buf.resizeAndPreserve(targetSize);
          if (readSize < targetSize)
              buf(blitz::Range(readSize, targetSize - 1)) = DataT(-1);          
        }

 virtual void _updateBoundingBox() const = 0;

  AnnotationChannelSpecs* p_channel;
  std::map<ViewWidget*, MarkerRenderer*> _renderers;

  std::string _id;
  blitz::TinyVector<double,3> _positionUm;
  int _label, _predictedLabel;
  bool _manual, _needsFeatureUpdate;
  std::vector<double> _features;
  std::map<int,double> _probabilityMap;
  double _qcDistance, _radialDistance, _phi;

  blitz::TinyVector<float,3> _randomColor;

/*======================================================================*/
/*! 
 *   If this variable is set to false, the next query for the marker's
 *   bounding box will update it.
 */
/*======================================================================*/
  mutable bool _boundingBoxUpToDate;

/*======================================================================*/
/*! 
 *  This variable contains the lower bound of this marker in micrometers
 *  relative to its world position in micrometers.
 */
/*======================================================================*/
  mutable blitz::TinyVector<double,3> _boundingBoxLowerBoundUm;

/*======================================================================*/
/*! 
 *  This variable contains the lower bound of this marker in micrometers
 *  relative to its world position in micrometers.
 */
/*======================================================================*/
  mutable blitz::TinyVector<double,3> _boundingBoxUpperBoundUm;

  friend class MarkerControlWidget;
  friend class AnnotationChannelSpecs;

private:
  
  bool _updatesEnabled;

};

#endif
