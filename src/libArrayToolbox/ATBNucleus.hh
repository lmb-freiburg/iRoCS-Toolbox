/**************************************************************************
 **       Title: Prototypic nucleus model
 **    $RCSfile: ATBNucleus.hh,v $
 **   $Revision: 4868 $ $Name:  $
 **       $Date: 2011-12-12 15:46:19 +0100 (Mon, 12 Dec 2011) $
 **   Copyright: GPL $Author: tschmidt $
 ** Description:
 **
 **************************************************************************/

/*======================================================================*/
/*!
 *  \file ATBNucleus.hh
 *  \brief Nucleus class containing cell nucelus specific parameters.
 */
/*======================================================================*/

#ifndef ATBNUCLEUS_HH
#define ATBNUCLEUS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <blitz/array.h>

#include <map>
#include <vector>
#include <list>

#include <libBlitzHdf5/BlitzHdf5Light.hh>

#include "TypeTraits.hh"
#include "RuntimeError.hh"

namespace atb
{

/*======================================================================*/
/*!
 *  \class Nucleus ATBNucleus.hh "libArrayToolbox/ATBNucleus.hh"
 *  \brief The Nucleus class contains cell nucleus attributes and provides
 *    methods to load and save single nuclei and sets of nuclei from/to
 *    hdf5 files in a format compatible to labelling.
 */
/*======================================================================*/
  class Nucleus 
  {

    struct NucleusId 
    {
      NucleusId(double r, const bool m)
              : radius(r), mitotic(m)
            {}
      ~NucleusId()
            {}
      
      double radius;
      bool mitotic;
      
      bool operator==(const NucleusId& ncId) const
            {
              if (radius == ncId.radius &&
                  mitotic == ncId.mitotic) return true;
              return false;
            }
      
      bool operator<(const NucleusId& ncId) const
            {
              if (radius < ncId.radius) return true;
              if (radius > ncId.radius) return false;
              if (!mitotic && ncId.mitotic) return true;
              return false;
            }
    };

  public:

/*======================================================================*/
/*! 
 *   Default constructor. Creates a new Nucleus object with default values.
 */
/*======================================================================*/
    Nucleus()
            : _id("marker"), _positionUm(-1.0), _label(-1), _predictedLabel(-1),
              _manual(false), _needsFeatureUpdate(true), _features(),
              _probabilityMap(), _qcDistanceUm(-1.0), _radialDistanceUm(-1.0),
              _phi(-1.0), _radiusUm(-1.0), _coefficients(),
              _value(-1.0), _confidence(-1.0), _sphase(-1.0),
              _borderDistanceUm(-1.0), _volumeUm3(-1.0), _mitotic(false),
              _predictedMitotic(false), _subtype(-1), _predictedSubtype(-1),
              _cellfile(-1), _predictedCellfile(-1)
          {}
    
/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    ~Nucleus()
          {}
    
/*======================================================================*/
/*! 
 *   Get the id string for this Nucleus. The id will not be written or
 *   loaded. It exists only for convenience to identify a nucleus. The
 *   user has to ensure that this field has a unique value per nucleus if
 *   required.
 *
 *   \return The id string of this Nucleus
 */
/*======================================================================*/
    std::string const &id() const
          {
            return _id;
          }
    
/*======================================================================*/
/*! 
 *   Set the id string for this Nucleus. The id will not be written or
 *   loaded. It exists only for convenience to identify a nucleus. The
 *   user has to ensure that this field has a unique value per nucleus if
 *   required.
 *
 *   \param id The new id string of this Nucleus
 */
/*======================================================================*/
    void setId(std::string const &id)
          {
            _id = id;
          }
    
/*======================================================================*/
/*! 
 *   Get the Nucleus position in micrometers.
 *
 *   \return The Nucleus position in micrometers
 */
/*======================================================================*/
    blitz::TinyVector<double,3> const &positionUm() const
          {
            return _positionUm;
          }

/*======================================================================*/
/*! 
 *   Set the Nucleus position in micrometers.
 *
 *   \param positionUm The new Nucleus position in micrometers
 */
/*======================================================================*/
    void setPositionUm(blitz::TinyVector<double,3> const &positionUm)
          {
            _positionUm = positionUm;
          }

/*======================================================================*/
/*! 
 *   Get the cell layer label of the nucleus. If the manual flag is set
 *   this is the manually assigned ground truth label which should not
 *   have been touched by automatic classifications.
 *
 *   \param label The label of the nucleus, the following list are the
 *     defaults:\n
 *    -1 : unknown\n
 *     0 : no nucleus\n
 *     1 : Root Cap\n
 *     2 : Epidermis\n
 *     3 : Cortex\n
 *     4 : Endodermis\n
 *     5 : Pericycle\n
 *     6 : Vasculature
 */
/*======================================================================*/
    int label() const
          {
            return _label;
          }

/*======================================================================*/
/*! 
 *   Set the cell layer label of the nucleus. If you additionally set the
 *   manual flag this label is treated as groundtruth label and should not
 *   be altered by automatic classification algorithms.
 *
 *   \param label The label of the nucleus, the following list are the
 *     defaults:\n
 *    -1 : unknown\n
 *     0 : no nucleus\n
 *     1 : Root Cap\n
 *     2 : Epidermis\n
 *     3 : Cortex\n
 *     4 : Endodermis\n
 *     5 : Pericycle\n
 *     6 : Vasculature
 */
/*======================================================================*/
    void setLabel(int label)
          {
            _label = label;
          }

/*======================================================================*/
/*! 
 *   Get the predicted cell layer label of the nucleus. This field should
 *   be set by automatic classifiers to allow comparison to manually assigned
 *   labels.
 *
 *   \param label The predicted label of the nucleus, the following list
 *     are the defaults:\n
 *    -1 : unknown\n
 *     0 : no nucleus\n
 *     1 : Root Cap\n
 *     2 : Epidermis\n
 *     3 : Cortex\n
 *     4 : Endodermis\n
 *     5 : Pericycle\n
 *     6 : Vasculature
 */
/*======================================================================*/
    int predictedLabel() const
          {
            return _predictedLabel;
          }

/*======================================================================*/
/*! 
 *   Set the predicted cell layer label of the nucleus. This field should
 *   be set by automatic classifiers to allow comparison to manually assigned
 *   labels.
 *
 *   \param label The predicted label of the nucleus, the following list
 *     are the defaults:\n
 *    -1 : unknown\n
 *     0 : no nucleus\n
 *     1 : Root Cap\n
 *     2 : Epidermis\n
 *     3 : Cortex\n
 *     4 : Endodermis\n
 *     5 : Pericycle\n
 *     6 : Vasculature
 */
/*======================================================================*/
    void setPredictedLabel(int predictedLabel)
          {
            _predictedLabel = predictedLabel;
          }

/*======================================================================*/
/*! 
 *   Get the manual flag for this Nucleus. This flag should be set for
 *   manual ground truth annotations. The manually assigned label, sub-type
 *   and division status can then be queried using the label(), subType()
 *   and mitotic() methods. If this flag is set automatic classifiers may not
 *   alter the label, sub-type or mitotic state.
 *
 *   \return A bool indicating whether the Nucleus is manually annotated.
 */
/*======================================================================*/
    bool manual() const
          {
            return _manual;
          }

/*======================================================================*/
/*! 
 *   Set the manual flag for this Nucleus. This flag should be set for
 *   manual ground truth annotations. The manually assigned label, sub-type
 *   and division status can then be queried using the label(), subType()
 *   and mitotic() methods. If this flag is set automatic classifiers may not
 *   alter the label, sub-type or mitotic state.
 *
 *   \param manual A bool indicating whether the Nucleus is manually annotated.
 */
/*======================================================================*/
    void setManual(bool manual)
          {
            _manual = manual;
          }

/*======================================================================*/
/*! 
 *   This function should return true if the feature vector needs to be
 *   recomputed prior to a classification, false otherwise.
 *
 *   \return A bool indicating whether a feature recomputation is required
 */
/*======================================================================*/
    bool needsFeatureUpdate() const
          {
            return _needsFeatureUpdate;
          }
  
/*======================================================================*/
/*! 
 *   Set or reset the needsFeatureUpdate flag.
 *
 *   \param needsFeatureUpdate A bool indicating whether a feature
 *     recomputation is required
 */
/*======================================================================*/
    void setNeedsFeatureUpdate(bool needsFeatureUpdate)
          {
            _needsFeatureUpdate = needsFeatureUpdate;
          }

/*======================================================================*/
/*! 
 *   Get a random-access reference to the feature vector of this Nucleus.
 *
 *   \return A reference to the feature vector
 */
/*======================================================================*/
    std::vector<double> &features()
          {
            return _features;
          }

/*======================================================================*/
/*! 
 *   Get a read-only reference to the feature vector of this Nucleus.
 *
 *   \return A reference to the feature vector
 */
/*======================================================================*/
    std::vector<double> const &features() const
          {
            return _features;
          }

/*======================================================================*/
/*! 
 *   Get a random-access reference to the probability map of this Nucleus.
 *   The probability map should be used to store soft class assignments.
 *   For each possible label it should contain the probability that the
 *   Nucleus belongs to that class.
 *
 *   \return A reference to the probability map
 */
/*======================================================================*/
    std::map<int,double> &probabilityMap()
          {
            return _probabilityMap;
          }
  
/*======================================================================*/
/*! 
 *   Get a read-only reference to the probability map of this Nucleus.
 *   The probability map should be used to store soft class assignments.
 *   For each possible label it should contain the probability that the
 *   Nucleus belongs to that class.
 *
 *   \return A reference to the probability map
 */
/*======================================================================*/
    std::map<int,double> const &probabilityMap() const
          {
            return _probabilityMap;
          }

/*======================================================================*/
/*! 
 *   Get the (signed) distance between this nucleus and the root's
 *   quiescent center along the root axis in micrometers. This is the
 *   iRoCS z coordinate.
 *
 *   \return The distance of this nucleus to the quiescent center along
 *     the axis
 */
/*======================================================================*/
    double qcDistanceUm() const
          {
            return _qcDistanceUm;
          }

/*======================================================================*/
/*! 
 *   Set the (signed) distance between this nucleus and the root's
 *   quiescent center along the root axis in micrometers. This is the
 *   iRoCS z coordinate.
 *
 *   \param qcDistanceUm The distance of this nucleus to the quiescent
 *     center along the axis
 */
/*======================================================================*/
    void setQcDistanceUm(double qcDistanceUm)
          {
            _qcDistanceUm = qcDistanceUm;
          }

/*======================================================================*/
/*! 
 *   Get the distance of this nucleus to the root axis in micrometers.
 *   This is the iRoCS r coordinate.
 *
 *   \return The distance of this nucleus to the root axis
 */
/*======================================================================*/
    double radialDistanceUm() const
          {
            return _radialDistanceUm;
          }
  
/*======================================================================*/
/*! 
 *   Set the distance of this nucleus to the root axis in micrometers.
 *   This is the iRoCS r coordinate.
 *
 *   \param radialDistanceUm The distance of this nucleus to the root axis
 */
/*======================================================================*/
    void setRadialDistanceUm(double radialDistanceUm)
          {
            _radialDistanceUm = radialDistanceUm;
          }
      
/*======================================================================*/
/*! 
 *   Get the angle around the root axis of this nucleus in radians.
 *   This is the iRoCS phi coordinate.
 *
 *   \return The angle around the root axis of this nucleus
 */
/*======================================================================*/
    double phi() const
          {
            return _phi;
          }
  
/*======================================================================*/
/*! 
 *   Set the angle around the root axis of this nucleus in radians.
 *   This is the iRoCS phi coordinate.
 *
 *   \return The angle around the root axis of this nucleus
 */
/*======================================================================*/
    void setPhi(const double phi)
          {
            _phi = phi;
          }
    
/*======================================================================*/
/*! 
 *   Get the radius of the Nucleus in micrometers. This should be the
 *   nucleolus radius.
 *
 *   \return The nucleolus radius in micrometers
 */
/*======================================================================*/ 
    double radiusUm() const
          {
            return _radiusUm;
          }

/*======================================================================*/
/*! 
 *   Set the radius of the Nucleus in micrometers. This should be set to
 *   the nucleolus radius if possible.
 *
 *   \param radiusUm The nucleolus radius in micrometers
 */
/*======================================================================*/ 
    void setRadiusUm(const double radiusUm)
          {
            _radiusUm = radiusUm;
          }

/*======================================================================*/
/*! 
 *   Get a read-only reference to the spherical harmonic coefficients
 *   describing the shape of this Nucleus. If the coefficients are empty
 *   the Nucleus is assumed to be perfectly spherical with the stored
 *   radius.
 *
 *   \return A reference to the spherical harmonic coefficient vector
 */
/*======================================================================*/
    blitz::Array<std::complex<double>,1> const &shCoefficients() const
          {
            return _coefficients;
          }

/*======================================================================*/
/*! 
 *   Get a random-access reference to the spherical harmonic coefficients
 *   describing the shape of this Nucleus. If the coefficients are empty
 *   the Nucleus is assumed to be perfectly spherical with the stored
 *   radius.
 *
 *   \return A reference to the spherical harmonic coefficient vector
 */
/*======================================================================*/
    blitz::Array<std::complex<double>,1> &shCoefficients()
          {
            return _coefficients;
          }

/*======================================================================*/
/*! 
 *   Get the content of the value field of this Nucleus. The value field
 *   is a general purpose parameter which is mainly used to sort the Nuclei.
 *   It can be used to store probabilities or the like.
 *
 *   \return The content of the value field
 */
/*======================================================================*/
    double value() const
          {
            return _value;
          }

/*======================================================================*/
/*! 
 *   Set the content of the value field of this Nucleus. The value field
 *   is a general purpose parameter which is mainly used to sort the Nuclei.
 *   It can be used to store probabilities or the like.
 *
 *   \param value The new content of the value field
 */
/*======================================================================*/
    void setValue(double value)
          {
            _value = value;
          }

/*======================================================================*/
/*! 
 *   Get the content of the confidence field of this Nucleus. The confidence
 *   field is a general purpose parameter.
 *   It can be used to store classification confidences or the like.
 *
 *   \return The confidence
 */
/*======================================================================*/
    double confidence() const
          {
            return _confidence;
          }

/*======================================================================*/
/*! 
 *   Set the content of the confidence field of this Nucleus. The confidence
 *   field is a general purpose parameter.
 *   It can be used to store classification confidences or the like.
 *
 *   \param confidence The new confidence
 */
/*======================================================================*/
    void setConfidence(double confidence)
          {
            _confidence = confidence;
          }

/*======================================================================*/
/*! 
 *   Get the S-Phase probability of this Nucleus (if available). S-Phase is
 *   the DNA synthesis phase in the cell cycle.
 *
 *   \return The probability of being in S-Phase
 */
/*======================================================================*/
    double sphase() const
          {
            return _sphase;
          }

/*======================================================================*/
/*! 
 *   Set the S-Phase probability of this Nucleus. S-Phase is the DNA
 *   synthesis phase in the cell cycle.
 *
 *   \param sphase The probability of being in S-Phase
 */
/*======================================================================*/
    void setSphase(double sphase)
          {
            _sphase = sphase;
          }

/*======================================================================*/
/*! 
 *   Get the distance of this nucleus to the outer root boundary in
 *   micrometers. This parameter is more stable than the distance to the
 *   axis, but is hard to get without cell boundary markers. Usually it
 *   will be unused.
 *
 *   \return The distance of this nucleus to the root boundary
 */
/*======================================================================*/
    double borderDistanceUm() const
          {
            return _borderDistanceUm;
          }

/*======================================================================*/
/*! 
 *   Set the distance of this nucleus to the outer root boundary in
 *   micrometers. This parameter is more stable than the distance to the
 *   axis, but is hard to get without cell boundary markers. Usually it
 *   will be unused.
 *
 *   \borderDistanceUm The distance of this nucleus to the root boundary
 */
/*======================================================================*/
    void setBorderDistanceUm(double borderDistanceUm)
          {
            _borderDistanceUm = borderDistanceUm;
          }
    
/*======================================================================*/
/*! 
 *   Get the volume of this nucleus in micrometers cube. A segmentation
 *   should store the segment volume here.
 *
 *   \return The nucleus volume in micrometers
 */
/*======================================================================*/
    double volumeUm3() const
          {
            return _volumeUm3;
          }
  
/*======================================================================*/
/*! 
 *   Set the volume of this nucleus in micrometers cube. A segmentation
 *   should store the segment volume using this method.
 *
 *   \param volumeUm3 The nucleus volume in micrometers
 */
/*======================================================================*/
    void setVolumeUm3(double volumeUm3)
          {
            _volumeUm3 = volumeUm3;
          }

/*======================================================================*/
/*! 
 *   Get the mitotic state (dividing/not dividing) of this Nucleus. The
 *   mitotic flag is specifically designed to store manual annotations that
 *   will not be altered by automatic classifications if the manualFlag for
 *   this Nucleus is set.
 *
 *   \return A bool indicating whether the cell the nucleus belongs to
 *     is currently dividing
 */
/*======================================================================*/
    bool mitotic() const
          {
            return _mitotic;
          }

/*======================================================================*/
/*! 
 *   Set the mitotic state (dividing/not dividing) of this Nucleus. The
 *   mitotic flag is specifically designed to store manual annotations that
 *   will not be altered by automatic classifications if the manualFlag for
 *   this Nucleus is set.
 *
 *   \param mitotic A bool indicating whether the cell the nucleus belongs to
 *     is currently dividing
 */
/*======================================================================*/
    void setMitotic(bool mitotic)
          {
            _mitotic = mitotic;
          }

/*======================================================================*/
/*! 
 *   Get the predicted mitotic state (dividing/not dividing) of this Nucleus.
 *   The predictedMitotic flag is specifically designed to store automatic
 *   classification results.
 *
 *   \return A bool indicating whether the cell the nucleus belongs to
 *     is currently dividing
 */
/*======================================================================*/
    bool predictedMitotic() const
          {
            return _predictedMitotic;
          }

/*======================================================================*/
/*! 
 *   Set the predicted mitotic state (dividing/not dividing) of this Nucleus.
 *   The predictedMitotic flag is specifically designed to store automatic
 *   classification results.
 *
 *   \return A bool indicating whether the cell the nucleus belongs to
 *     is currently dividing
 */
/*======================================================================*/
    void setPredictedMitotic(bool predictedMitotic)
          {
            _predictedMitotic = predictedMitotic;
          }

/*======================================================================*/
/*! 
 *   Get the sub-type of this Nucleus. The subtype can be used to
 *   differentiate between sub-populations within layers (with are
 *   distinguished using the label attribute). E.g. epidermis has two
 *   sub-types: trichoblasts and atrichoblasts. The subtype field is
 *   specifically designed for manual ground-truth annotations and should
 *   not be touched by automatic classification if the manual flag is set.
 *
 *   \return The subtype of this Nucleus
 */
/*======================================================================*/
    int subtype() const
          {
            return _subtype;
          }

/*======================================================================*/
/*! 
 *   Set the sub-type of this Nucleus. The subtype can be used to
 *   differentiate between sub-populations within layers (with are
 *   distinguished using the label attribute). E.g. epidermis has two
 *   sub-types: trichoblasts and atrichoblasts. The subtype field is
 *   specifically designed for manual ground-truth annotations and should
 *   not be touched by automatic classification if the manual flag is set.
 *
 *   \param subType The subtype of this Nucleus
 */
/*======================================================================*/
    void setSubtype(int subType)
          {
            _subtype = subType;
          }

/*======================================================================*/
/*! 
 *   Get the predicted sub-type of this Nucleus. The subtype can be used to
 *   differentiate between sub-populations within layers (with are
 *   distinguished using the label attribute). E.g. epidermis has two
 *   sub-types: trichoblasts and atrichoblasts. The predictedSubtype field is
 *   specifically designed for automatic classifications.
 *
 *   \return The predicted subtype of this Nucleus
 */
/*======================================================================*/
    int predictedSubtype() const
          {
            return _predictedSubtype;
          }

/*======================================================================*/
/*! 
 *   Set the predicted sub-type of this Nucleus. The subtype can be used to
 *   differentiate between sub-populations within layers (with are
 *   distinguished using the label attribute). E.g. epidermis has two
 *   sub-types: trichoblasts and atrichoblasts. The predictedSubtype field is
 *   specifically designed for automatic classifications.
 *
 *   \param subType The predicted subtype of this Nucleus
 */
/*======================================================================*/
    void setPredictedSubtype(int subType)
          {
            _predictedSubtype = subType;
          }

/*======================================================================*/
/*! 
 *   Get the cell file index of this nucleus. A cell file is a linear chain
 *   of cells aligned along the root axis with common lineage. The cellfile
 *   field is specifically designed for manual ground-truth annotations and
 *   should not be touched by automatic classification if the manual flag 
 *   is set.
 *
 *   \return The cell file index of this Nucleus
 */
/*======================================================================*/
    int cellfile() const
          {
            return _cellfile;
          }
  
/*======================================================================*/
/*! 
 *   Set the cell file index of this nucleus. A cell file is a linear chain
 *   of cells aligned along the root axis with common lineage. The cellfile
 *   field is specifically designed for manual ground-truth annotations and
 *   should not be touched by automatic classification if the manual flag 
 *   is set.
 *
 *   \param cellfile The cell file index of this Nucleus
 */
/*======================================================================*/
    void setCellfile(int cellfile)
          {
            _cellfile = cellfile;
          }

/*======================================================================*/
/*! 
 *   Get the predicted cell file index of this nucleus. A cell file is a
 *   linear chain of cells aligned along the root axis with common lineage.
 *   The cellfile field is specifically designed for automatic
 *   classifications.
 *
 *   \return The predicted cell file index of this Nucleus
 */
/*======================================================================*/
    int predictedCellfile() const
          {
            return _predictedCellfile;
          }

/*======================================================================*/
/*! 
 *   Set the predicted cell file index of this nucleus. A cell file is a
 *   linear chain of cells aligned along the root axis with common lineage.
 *   The cellfile field is specifically designed for automatic
 *   classifications.
 *
 *   \param predictedCellfile The predicted cell file index of this Nucleus
 */
/*======================================================================*/
    void setPredictedCellfile(int predictedCellfile)
          {
            _predictedCellfile = predictedCellfile;
          }

/*======================================================================*/
/*! 
 *   Less than operator comparing the value field of this Nucleus and the
 *   given right-hand-side Nucleus.
 *
 *   \param rhs The Nucleus to compare this Nucleus to
 *
 *   \return this->value() < rhs.value()
 */
/*======================================================================*/
    bool operator<(const Nucleus& rhs) const
          {
            return _value < rhs.value();
          }

/*======================================================================*/
/*! 
 *   File IO Function for writing a list of Nuclei.
 *
 *   \param nuclei   A set of nuclei in some stl-sequence like list or
 *                   vector. More specific: the type NucleusSequenceT
 *                   must support stl-like iteration, push_back(), pop_back(),
 *                   back() and empty().
 *   \param outFile  A file handle to the hdf5 I/O file, which must be opened
 *                   for writing
 *   \param group    The group to write the nuclei to.
 *
 *   \exception BlitzH5Error If something goes wrong this error will be
 *                           thrown
 */
/*======================================================================*/
    template<typename NucleusSequenceT>
    static void saveList(
        NucleusSequenceT const &nuclei, BlitzH5File &outFile,
        std::string const &group);
    
/*======================================================================*/
/*! 
 *   File IO Function for writing a list of Nuclei. On error it prints an
 *   error message to std::cerr and the program exits with a status of -1
 *
 *   \param nuclei       A set of nuclei in some stl-sequence like list or
 *                       vector. More specific: the type NucleusSequenceT
 *                       must support stl-like iteration, push_back(),
 *                       pop_back(), back() and empty().
 *   \param outFileName  A file with the given name will be created or
 *                       if existing opened for writing the nuclei to
 *   \param group        The group to write the nuclei to.
 */
/*======================================================================*/
    template<typename NucleusSequenceT>
    static void saveList(
        NucleusSequenceT const &nuclei, std::string const &outFileName,
        std::string const &group);
    
/*======================================================================*/
/*! 
 *   File IO Function for reading a list of Nuclei.
 *
 *   \param nuclei   A set of nuclei in some stl-sequence like list or
 *                   vector. More specific: the type NucleusSequenceT
 *                   must support stl-like iteration, push_back(), pop_back(),
 *                   back() and empty().
 *   \param inFile   A file handle to the hdf5 I/O file
 *   \param group    The group to read the nuclei from.
 *   \param showWarnings If this flag is set to true, missing meta data
 *                       will produce output of warnings. 
 *
 *   \exception BlitzH5Error If something goes wrong this error will be
 *                           thrown
 *   \exception RuntimeError If the read position information is not
 *                              3-D this error is thrown (only version 2)
 */
/*======================================================================*/
    template<typename NucleusSequenceT>
    static void loadList(NucleusSequenceT &nuclei,
                         BlitzH5File const &inFile, std::string const &group,
                         bool showWarnings = false);

/*======================================================================*/
/*! 
 *   File IO Function for reading a list of Nuclei. On error it prints an
 *   error message to std::cerr and the program exits with a status of -1
 *
 *   \param nuclei       A set of nuclei in some stl-sequence like list or
 *                       vector. More specific: the type NucleusSequenceT
 *                       must support stl-like iteration, push_back(),
 *                       pop_back(), back() and empty().
 *   \param inFileName   A file with the given name will be opened for reading
 *                       the nuclei from
 *   \param group        The group to read the nuclei from.
 *   \param showWarnings If this flag is set to true, missing meta data
 *                       will produce warnings.
 */
/*======================================================================*/
    template<typename NucleusSequenceT>
    static void loadList(
        NucleusSequenceT &nuclei, std::string const &inFileName,
        std::string const &group, bool showWarnings = false);

  private:
  
  static void _loadFixedSize(
      blitz::Array<bool,1> &buf, BlitzH5File const &inFile,
      std::string const &dsName, bool showWarnings = false);

  template<typename DataT>
  static void _loadFixedSize(
      blitz::Array<DataT,1> &buf, BlitzH5File const &inFile,
      std::string const &dsName, bool showWarnings = false);

    // General marker fields
    std::string _id;
    blitz::TinyVector<double,3> _positionUm;
    int _label, _predictedLabel;
    bool _manual, _needsFeatureUpdate;
    std::vector<double> _features;
    std::map<int,double> _probabilityMap;
    double _qcDistanceUm, _radialDistanceUm, _phi;

    // SphereMarker specific fields
    double _radiusUm;    

    // SHSurface marker specific fields
    blitz::Array<std::complex<double>,1> _coefficients;

    // Nucleaus marker specific fields
    double _value, _confidence, _sphase, _borderDistanceUm, _volumeUm3;
    bool _mitotic, _predictedMitotic;
    int _subtype, _predictedSubtype, _cellfile, _predictedCellfile;

  };
  
/*======================================================================*/
/*! 
 *   Appends a string representation of the given Nucleus to the given
 *   output stream.
 *
 *   \param os The stream to output the nucleus string representation
 *   \param nc The Nucleus object to dump to the stream
 *
 *   \return A reference to the output stream for operator chaining
 */
/*======================================================================*/
  std::ostream &operator<<(std::ostream &os, Nucleus const &nc);
    
}

#include "ATBNucleus.icc"

#endif
