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

#include <blitz/array.h>

#include <libBlitzHdf5/BlitzHdf5Light.hh>

#include <map>

namespace iRoCS
{

  class Marker
  {
    
    public:
  
    enum MarkerType {
        Point = 0x0001,
        Sphere = 0x0002,
        Nucleus = 0x0004,
        Cylinder = 0x0008,
        SHSurface = 0x0010,
        Surface = 0x0020,
        Cell = 0x0040
    };

    static MarkerType markerTypeFromString(std::string const &name);
    static std::string markerTypeToString(Marker::MarkerType m);

    Marker(blitz::TinyVector<double,3> const &positionUm = 0.0,
           AnnotationChannel *channel = NULL);
    Marker(Marker const &marker);
    virtual ~Marker();

    Marker &operator=(Marker const &marker);

    virtual MarkerType type() const = 0;
    virtual bool inherits(MarkerType type) const = 0;

    void setChannel(AnnotationChannelSpecs *channel);
    AnnotationChannelSpecs *channel() const;

    std::string id() const;
    virtual void setId(std::string const &id);

    blitz::TinyVector<double,3> const &positionUm() const;
    virtual void setPositionUm(blitz::TinyVector<double,3> const &positionUm);

    bool manual() const;
    virtual void setManual(bool manual);

    bool needsFeatureUpdate() const;
    virtual void setNeedsFeatureUpdate(bool needsFeatureUpdate);

    int label() const;
    virtual void setLabel(int label);

    int predictedLabel() const;
    virtual void setPredictedLabel(int label);

    std::vector<double> const &features() const;
    double const &feature(size_t index) const;
    double& feature(size_t index);
    std::vector<double>::iterator featuresBegin();
    std::vector<double>::iterator featuresEnd();
  
    std::map<int,double> const &probabilityMap() const;
    std::map<int,double>::iterator probabilityMapFind(int key);
    std::map<int,double>::iterator probabilityMapBegin();
    std::map<int,double>::iterator probabilityMapEnd();

    double qcDistanceUm() const;
    virtual void setQcDistanceUm(double qcDistanceUm);
  
    double radialDistanceUm() const;
    virtual void setRadialDistanceUm(double radialDistanceUm);
  
    double phi() const;
    virtual void setPhi(double phi);
  
    static void save(
        AnnotationChannelSpecs const *channel,
        BlitzH5File &outFile, std::string const &group,
        ProgressReporter *pr = NULL);
    static void load(
        AnnotationChannelSpecs const *channel, BlitzH5File const &inFile,
        std::string const &group, ProgressReporter *pr = NULL);

    virtual void writeCSVHeader(std::ofstream &out) const;
    virtual void saveCSV(std::ofstream &out) const;
    virtual void loadFromMap(std::map<std::string,std::string> const &values);

    virtual bool occupiesPositionUm(
        blitz::TinyVector<double,3> const &positionUm) const = 0;

    blitz::TinyVector<double,3> boundingBoxLowerBoundUm() const;
    blitz::TinyVector<double,3> boundingBoxUpperBoundUm() const;

  protected:

    void setFeatures(std::vector<double> const &features);
    void setProbabilityMap(std::map<int,double> const &probabilityMap);

    virtual void _updateBoundingBox() const = 0;

    static void _loadFixedSize(
        blitz::Array<bool,1> &buf, BlitzH5File const &inFile,
        std::string const &dsName, bool showWarnings = false);
    
    template<typename DataT>
    static void _loadFixedSize(
        blitz::Array<DataT,1> &buf, BlitzH5File const &inFile,
        std::string const &dsName, bool showWarnings = false);
    
    AnnotationChannel *p_channel;

    std::string _id;
    blitz::TinyVector<double,3> _positionUm;
    int _label, _predictedLabel;
    bool _manual, _needsFeatureUpdate;
    std::vector<double> _features;
    std::map<int,double> _probabilityMap;
    double _qcDistance, _radialDistance, _phi;

    mutable bool _boundingBoxUpToDate;
    mutable blitz::TinyVector<double,3> _boundingBoxLowerBoundUm;
    mutable blitz::TinyVector<double,3> _boundingBoxUpperBoundUm;

    friend class AnnotationChannel;

  };

}

#include "Marker.icc"

#endif
