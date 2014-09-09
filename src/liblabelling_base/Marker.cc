/**************************************************************************
 **       Title: Generic Marker
 **    $RCSfile$
 **   $Revision: 3880 $ $Name$
 **       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
 **   Copyright: GPL $Author: tschmidt $
 ** Description:
 **
 **************************************************************************/

#include "Marker.hh"

#include "PointMarker.hh"
#include "SphereMarker.hh"
#include "CylinderMarker.hh"
#include "NucleusMarker.hh"
#include "SurfaceMarker.hh"
#include "CellMarker.hh"
#include "AnnotationChannel.hh"
#include "MultiChannelModel.hh"

// #include <blitz/tinyvec-et.h>

namespace iRoCS
{

  Marker::MarkerType Marker::markerTypeFromString(std::string const &name)
  {
    if (name == "Point") return Point;
    if (name == "Sphere") return Sphere;
    if (name == "Nucleus") return Nucleus;
    if (name == "Cylinder") return Cylinder;
    if (name == "SHSurface") return SHSurface;
    if (name == "Surface") return Surface;
    if (name == "Cell") return Cell;
    std::cerr << __FILE__ << ":" << __LINE__ << ": Missing Implementation for "
              << "marker type " << name << std::endl;
    exit(-1);
  }

  std::string Marker::markerTypeToString(MarkerType m)
  {
    switch(m)
    {
    case Point :
      return "Point";
    case Sphere :
      return "Sphere";
    case Nucleus :
      return "Nucleus";
    case Cylinder :
      return "Cylinder";
    case SHSurface :
      return "SHSurface";
    case Surface :
      return "Surface";
    case Cell :
      return "Cell";
    default:
      std::cerr << __FILE__ << ":" << __LINE__ << ": Missing Implementation"
                << std::endl;
      exit(-1);
    }
  }

  Marker::Marker(blitz::TinyVector<double,3> const &positionUm,
                 AnnotationChannel *channel)
          : p_channel(channel), _id("noId"), _positionUm(positionUm),
            _label(-1), _predictedLabel(-1), _manual(false),
            _needsFeatureUpdate(true), _features(std::vector<double>()),
            _probabilityMap(std::map<int,double>()), _qcDistance(-1),
            _radialDistance(-1), _phi(-1), _selected(false),
            _boundingBoxUpToDate(false), _boundingBoxLowerBoundUm(-1.0),
            _boundingBoxUpperBoundUm(-1.0)
  {}
  
  Marker::Marker(Marker const &marker)
          : p_channel(NULL), _id(marker._id), _positionUm(marker._positionUm),
            _label(marker._label), _predictedLabel(marker._predictedLabel),
            _manual(marker._manual),
            _needsFeatureUpdate(marker._needsFeatureUpdate),
            _features(marker._features),
            _probabilityMap(marker._probabilityMap),
            _qcDistance(marker._qcDistance),
            _radialDistance(marker._radialDistance), _phi(marker._phi),
            _boundingBoxUpToDate(marker._boundingBoxUpToDate),
            _boundingBoxLowerBoundUm(marker._boundingBoxLowerBoundUm),
            _boundingBoxUpperBoundUm(marker._boundingBoxUpperBoundUm)
  {}
  
  Marker::~Marker()
  {}

  Marker &Marker::operator=(Marker const &marker)
  {
    _id = marker._id;
    _positionUm = marker._positionUm;
    _label = marker._label;
    _predictedLabel = marker._predictedLabel;
    _manual = marker._manual;
    _needsFeatureUpdate = marker._needsFeatureUpdate;
    _features = marker._features;
    _probabilityMap = marker._probabilityMap;
    _qcDistance = marker._qcDistance;
    _radialDistance = marker._radialDistance;
    _phi = marker._phi;
    _randomColor = marker._randomColor;
    _boundingBoxUpToDate = marker._boundingBoxUpToDate;
    _boundingBoxLowerBoundUm = marker._boundingBoxLowerBoundUm;
    _boundingBoxUpperBoundUm = marker._boundingBoxUpperBoundUm;
    return *this;
  }

  void Marker::setChannel(AnnotationChannel *channel)
  {
    if (p_channel == channel) return;
    AnnotationChannel oldChannel = p_channel;
    p_channel = channel;
    if (oldChannel != NULL) p_channel->removeMarker(this, false);
    if (p_channel != NULL) p_channel->addMarker(this);
  }

  AnnotationChannelSpecs *Marker::channel() const
  {
    return p_channel;
  }
  
  std::string Marker::id() const
  {
    return _id;
  }
  
  void Marker::setId(const std::string& id)
  {
    if (_id == id) return;
    _id = id;
    if (p_channel != NULL && p_channel->model() != NULL)
        p_channel->model()->setModified(true);
  }

  blitz::TinyVector<double,3> const &Marker::positionUm() const
  {
    return _positionUm;
  }

  void Marker::setPositionUm(blitz::TinyVector<double,3> const &pos) 
  {
    if (blitz::all(_positionUm == pos)) return;
    _positionUm = pos;
    _needsFeatureUpdate = true;
    if (p_channel != NULL && p_channel->model() != NULL)
        p_channel->model()->setModified(true);
  }

  bool Marker::manual() const 
  {
    return _manual;
  }
  
  void Marker::setManual(bool manual) 
  {
    if (_manual == manual) return;
    _manual = manual;
    if (p_channel != NULL && p_channel->model() != NULL)
        p_channel->model()->setModified(true);
  }

  bool Marker::needsFeatureUpdate() const 
  {
    return _needsFeatureUpdate;
  }
  
  void Marker::setNeedsFeatureUpdate(bool needsFeatureUpdate) 
  {
    if (_needsFeatureUpdate == needsFeatureUpdate) return;
    _needsFeatureUpdate = needsFeatureUpdate;
    if (p_channel != NULL && p_channel->model() != NULL)
        p_channel->model()->setModified(true);
  }

  int Marker::label() const 
  {
    return _label;
  }
  
  void Marker::setLabel(const int label) 
  {
    if (_label == label) return;
    _label = label;
    if (p_channel != NULL && p_channel->model() != NULL)
        p_channel->model()->setModified(true);
  }

  int Marker::predictedLabel() const 
  {
    return _predictedLabel;
  }
  
  void Marker::setPredictedLabel(const int label) 
  {
    if (_predictedLabel == label) return;
    _predictedLabel = label;
    if (p_channel != NULL && p_channel->model() != NULL)
        p_channel->model()->setModified(true);
  }

  const std::vector<double>& Marker::features() const
  {
    return _features;
  }
  
  double const &Marker::feature(size_t index) const
  {
    if (index > _features.size())
        throw std::runtime_error("Tried to access out-of-bounds feature index");
    return _features[index];
  }

  double &Marker::feature(size_t index)
  {
    if (index > _features.size())
        throw std::runtime_error("Tried to access out-of-bounds feature index");
    return _features[index];
  }

  std::vector<double>::iterator Marker::featuresBegin()
  {
    return _features.begin();
  }
  
  std::vector<double>::iterator Marker::featuresEnd()
  {
    return _features.end();
  }
  
  const std::map<int,double>& Marker::probabilityMap() const
  {
    return _probabilityMap;
  }
  
  std::map<int,double>::iterator Marker::probabilityMapFind(int key)
  {
    return _probabilityMap.find(key);
  }
  
  std::map<int,double>::iterator Marker::probabilityMapBegin()
  {
    return _probabilityMap.begin();
  }
  
  std::map<int,double>::iterator Marker::probabilityMapEnd()
  {
    return _probabilityMap.end();
  }
  
  double Marker::qcDistanceUm() const
  {
    return _qcDistance;
  }
  
  void Marker::setQcDistanceUm(const double qcDistanceUm)
  {
    if (_qcDistance == qcDistanceUm) return;
    _qcDistance = qcDistanceUm;
    if (p_channel != NULL && p_channel->model() != NULL)
        p_channel->model()->setModified(true);
  }
  
  double Marker::radialDistanceUm() const
  {
    return _radialDistance;
  }
  
  void Marker::setRadialDistanceUm(const double radialDistanceUm)
  {
    if (_radialDistance == radialDistanceUm) return;
    _radialDistance = radialDistanceUm;
    if (p_channel != NULL && p_channel->model() != NULL)
        p_channel->model()->setModified(true);
  }
  
  double Marker::phi() const
  {
    return _phi;
  }
  
  void Marker::setPhi(const double phi)
  {
    if (_phi == phi) return;
    _phi = phi;
    if (p_channel != NULL && p_channel->model() != NULL)
        p_channel->model()->setModified(true);
  }
  
  void Marker::save(
      AnnotationChannelSpecs const *channel, BlitzH5File &outFile,
      std::string const &group, iRoCS::ProgressReporter *pr)
  {
    size_t nMarkers = channel->markers().size();
    if (nMarkers == 0) return;

    int pStart = (pr != NULL) ? pr->taskProgressMin() : 0;
    int pRange = (pr != NULL) ? (pr->taskProgressMax() - pStart) : 1;

    blitz::Array<blitz::TinyVector<double,3>,1> positions(nMarkers);
    blitz::Array<int,1> labels(nMarkers);
    blitz::Array<int,1> predictedLabels(nMarkers);
    blitz::Array<unsigned char,1> manuals(nMarkers);
    blitz::Array<unsigned char,1> needsFeatureUpdates(nMarkers);
    blitz::Array<double,2> features;
    atb::BlitzIndexT feaSize = static_cast<atb::BlitzIndexT>(
        channel->markers().front()->features().size());
    if (feaSize != 0) features.resize(nMarkers, feaSize);
    atb::BlitzIndexT nClasses =
        channel->markers().front()->probabilityMap().size();
    blitz::Array<int,1> plattClasses;
    blitz::Array<double,2> plattProbabilities;
    blitz::Array<double,1> qcdistances(nMarkers);
    blitz::Array<double,1> radialdistances(nMarkers);
    blitz::Array<double,1> phis(nMarkers);
    atb::BlitzIndexT iP;
    std::map<int,double>::iterator itP;
    if (nClasses != 0)
    {
      plattClasses.resize(nClasses);
      iP = 0;
      itP = channel->markers().front()->_probabilityMap.begin();
      for ( ; itP != channel->markers().front()->_probabilityMap.end();
            ++itP, ++iP)
          plattClasses(iP) = itP->first;
      plattProbabilities.resize(nMarkers, nClasses);
    }
    atb::BlitzIndexT i = 0;
    for (std::vector<Marker*>::const_iterator it = channel->markers().begin();
         it != channel->markers().end(); ++it, ++i)
    {
      positions(i) = (*it)->positionUm();
      labels(i) = (*it)->label();
      predictedLabels(i) = (*it)->predictedLabel();
      manuals(i) = ((*it)->manual()) ? 1 : 0;
      needsFeatureUpdates(i) = ((*it)->needsFeatureUpdate()) ? 1 : 0;
      for (size_t j = 0; j < (*it)->features().size() &&
               j < static_cast<size_t>(features.extent(1)); ++j)
          features(i, static_cast<atb::BlitzIndexT>(j)) = (*it)->features()[j];
      if (plattProbabilities.size() != 0) 
      {
        for (itP = (*it)->_probabilityMap.begin(), iP = 0;
             itP != (*it)->_probabilityMap.end(); ++itP, ++iP)
            plattProbabilities(i, iP) = itP->second;
      }
      qcdistances(i) = (*it)->qcDistanceUm();
      radialdistances(i) = (*it)->radialDistanceUm();
      phis(i) = (*it)->phi();
    }
    if (pr != NULL && !pr->updateProgress(
            pStart + static_cast<int>(0.05f * pRange))) return;
    outFile.writeDataset(positions, group + "/position_um");
    if (pr != NULL && !pr->updateProgress(
            pStart + static_cast<int>(0.15f * pRange)))
    {
      outFile.deleteGroup(group);
      return;
    }
    outFile.writeDataset(labels, group + "/label");
    if (pr != NULL && !pr->updateProgress(
            pStart + static_cast<int>(0.25f * pRange)))
    {
      outFile.deleteGroup(group);
      return;
    }
    outFile.writeDataset(predictedLabels, group + "/predicted_label");
    if (pr != NULL && !pr->updateProgress(
            pStart + static_cast<int>(0.35f * pRange)))
    {
      outFile.deleteGroup(group);
      return;
    }
    outFile.writeDataset(manuals, group + "/manual");
    if (pr != NULL && !pr->updateProgress(
            pStart + static_cast<int>(0.45f * pRange)))
    {
      outFile.deleteGroup(group);
      return;
    }
    outFile.writeDataset(
        needsFeatureUpdates, group + "/needsFeatureUpdate");
    if (pr != NULL && !pr->updateProgress(
            pStart + static_cast<int>(0.55f * pRange)))
    {
      outFile.deleteGroup(group);
      return;
    }
    if (feaSize != 0) outFile.writeDataset(features, group + "/features");
    if (pr != NULL && !pr->updateProgress(
            pStart + static_cast<int>(0.65f * pRange)))
    {
      outFile.deleteGroup(group);
      return;
    }
    if (plattClasses.size() != 0) 
    {
      outFile.writeDataset(plattClasses, group + "/platt_classes");
      outFile.writeDataset(
          plattProbabilities, group + "/platt_probabilities");      
    }
    if (pr != NULL && !pr->updateProgress(
            pStart + static_cast<int>(0.75f * pRange)))
    {
      outFile.deleteGroup(group);
      return;
    }
    outFile.writeDataset(qcdistances, group + "/qcDistance_um");
    if (pr != NULL && !pr->updateProgress(
            pStart + static_cast<int>(0.85f * pRange)))
    {
      outFile.deleteGroup(group);
      return;
    }
    outFile.writeDataset(radialdistances, group + "/radialDistance_um");
    outFile.writeDataset(qcdistances, group + "/qcDistance_um");
    if (pr != NULL && !pr->updateProgress(
            pStart + static_cast<int>(0.95f * pRange)))
    {
      outFile.deleteGroup(group);
      return;
    }
    outFile.writeDataset(phis, group + "/phi");
    if (pr != NULL && !pr->updateProgress(pStart + pRange))
        outFile.deleteGroup(group);
  }
  
  void Marker::load(
      AnnotationChannelSpecs *channel, BlitzH5File const &inFile,
      std::string const &group)
  {
    if (channel == NULL) return;
    
    blitz::Array<blitz::TinyVector<double,3>,1> positions;
    inFile.readDataset(positions, group + "/position_um");
    blitz::Array<int,1> labels(positions.size());
    _loadFixedSize(labels, inFile, group + "/label", showWarnings);
    blitz::Array<int,1> predictedLabels(positions.size());
    _loadFixedSize(
        predictedLabels, inFile, group + "/predicted_label", showWarnings);
    blitz::Array<bool,1> manuals(positions.size());
    _loadFixedSize(manuals, inFile, group + "/manual", showWarnings);
    blitz::Array<bool,1> needsFeatureUpdates(positions.size());
    _loadFixedSize(needsFeatureUpdates, inFile, group + "/needsFeatureUpdate",
                   showWarnings);
    blitz::Array<double,1> nFeatures(positions.size());
    _loadFixedSize(nFeatures, inFile, group + "/nFeatures", showWarnings);
    blitz::Array<double,2> features;
    try 
    {
      inFile.readDataset(features, group + "/features");
      if (features.extent(0) != positions.extent(0)) 
      {
        blitz::Array<double,2> tmp(positions.extent(0), features.extent(1));
        if (features.extent(0) < positions.extent(0))
        {
          tmp = -1;
          tmp(blitz::Range(0, positions.extent(0) - 1), blitz::Range::all()) =
              features;
        }
        else
        {
          tmp = features(blitz::Range(0, tmp.extent(0) - 1),
                         blitz::Range::all());
        }
        features.resize(tmp.extent(0), tmp.extent(1));
        features = tmp;
      }
    }
    catch (BlitzH5Error&)
    {}
    blitz::Array<int,1> classes;
    blitz::Array<double,2> probabilities;
    try 
    {
      inFile.readDataset(classes, group + "/platt_classes");
      inFile.readDataset(probabilities, group + "platt_probabilities");
      assert(classes.extent(0) == probabilities.extent(1));
      if (probabilities.extent(0) != positions.extent(0)) 
      {
        std::cerr << "Warning: 'probabilities' size doesn't match the number "
                  << "of nuclei. Resizing and filling with -1 if necessary"
                  << std::endl;
        blitz::Array<double,2> tmp(positions.extent(0),
                                   probabilities.extent(1));
        if (probabilities.extent(0) < positions.extent(0))
        {
          tmp = -1;
          tmp(blitz::Range(0, positions.extent(0) - 1), blitz::Range::all()) =
              probabilities;
        }
        else
        {
          tmp = probabilities(blitz::Range(0, tmp.extent(0) - 1),
                              blitz::Range::all());
        }
        probabilities.resize(tmp.shape());
        probabilities = tmp;
      }
    }
    catch (BlitzH5Error&)
    {
    }
    blitz::Array<double,1> qcDistances(positions.size());
    _loadFixedSize(qcDistances, inFile, group + "/qcDistance_um", showWarnings);
    blitz::Array<double,1> radialDistances(positions.size());
    _loadFixedSize(
        radialDistances, inFile, group + "/radialDistance_um", showWarnings);
    blitz::Array<double,1> phis(positions.size());
    _loadFixedSize(phis, inFile, group + "/phi", showWarnings);
    
    for (atb::BlitzIndexT i = 0; i < positions.extent(0); ++i) 
    {
      Marker* m = NULL;
      switch (channel->markerType()) 
      {
      case Marker::Point :
        m = new PointMarker();
        break;
      case Marker::Sphere :
        m = new SphereMarker();
        break;
      case Marker::Cylinder :
        m = new CylinderMarker();
        break;
      case Marker::SHSurface :
        m = new SHSurfaceMarker();
        break;
      case Marker::Nucleus :
        m = new NucleusMarker();
        break;
      case Marker::Surface :
        m = new SurfaceMarker();
        break;
      case Marker::Cell :
        m = new CellMarker();
        break;
      default:
        std::cerr << __FILE__ << ":" << __LINE__ << ": Missing Implementation"
                  << std::endl;
        exit(-1);  
      }
      if (m != NULL)
      {
        std::stringstream idStream;
        idStream << "marker_" << std::setfill('0') << std::setw(8) << i;
        m->setId(idStream.str());
        m->setPositionUm(positions(i));
        m->setLabel(labels(i));
        m->setPredictedLabel(predictedLabels(i));
        m->setManual((manuals(i) == 1) ? true : false);
        m->setNeedsFeatureUpdate(needsFeatureUpdates(i));
        if (features.size() != 0)
        {
          if (nFeatures(i) == -1)
          {
            m->features().resize(features.extent(1));
            for (BlitzIndexT j = 0; j < features.extent(1); ++j)
                m->features()[j] = features(i, j);
          }
          else
          {
            m->features().resize(nFeatures(i));
            for (int j = 0; j < nFeatures(i); ++j)
                m->features()[j] = features(i, j);
          }
        }    
        if (probabilities.size() != 0)
        {
          for (atb::BlitzIndexT j = 0; j < probabilities.extent(1); ++j)
          {
            m->probabilityMap()[static_cast<int>(classes(j))] =
                probabilities(i, j);
          }
        }
        m->setQcDistanceUm(qcdistances(i));
        m->setRadialDistanceUm(radialdistances(i));
        m->setPhi(phis(i));
        channel->addMarker(m);
      }
    }
  }

  void Marker::writeCSVHeader(std::ofstream &out) const
  {
    out << "x (micron);y (micron);z (micron);label;predicted label;manual;"
        << "distance from QC (z) (micron);radial distance (r) (micron);"
        << "phi (radians)";
  }

  void Marker::saveCSV(std::ofstream &out) const
  {
    out << _positionUm(2) << ";" << _positionUm(1) << ";" << _positionUm(0)
        << ";" << _label << ";" << _predictedLabel << ";" << ((_manual) ? 1 : 0)
        << ";" << _qcDistance << ";" << _radialDistance << ";" << _phi;
  }

  void Marker::loadFromMap(std::map<std::string,std::string> const &values)
  {
    _positionUm =
        atof(values.find("z (micron)")->second.c_str()),
        atof(values.find("y (micron)")->second.c_str()),
        atof(values.find("x (micron)")->second.c_str());
    _label = atoi(values.find("label")->second.c_str());
    _predictedLabel = atoi(values.find("predictedLabel")->second.c_str());
    _manual = (values.find("manual")->second != "0");
    _qcDistance = 
        atof(values.find("distance from QC (z) (micron)")->second.c_str());
    _radialDistance = atof(
        values.find("radial distance (r) (micron)")->second.c_str());
    _phi = atof(values.find("phi (radians)")->second.c_str());
  }

  void Marker::setFeatures(const std::vector<double>& features) 
  {
    if (_features == features) return;
    _features = features;
    if (p_channel != NULL && p_channel->model() != NULL)
        p_channel->model()->setModified(true);
  }

  void Marker::setProbabilityMap(const std::map<int,double>& probabilityMap)
  {
    if (_probabilityMap == probabilityMap) return;
    _probabilityMap = probabilityMap;
    if (p_channel != NULL && p_channel->model() != NULL)
        p_channel->model()->setModified(true);
  }

  blitz::TinyVector<double,3> Marker::boundingBoxLowerBoundUm() const
  {
    if (!_boundingBoxUpToDate) _updateBoundingBox();
    return _positionUm + _boundingBoxLowerBoundUm;
  }

  blitz::TinyVector<double,3> Marker::boundingBoxUpperBoundUm() const
  {
    if (!_boundingBoxUpToDate) _updateBoundingBox();
    return _positionUm + _boundingBoxUpperBoundUm;
  }

  void Marker::_loadFixedSize(
      blitz::Array<bool,1> &buf, BlitzH5File const &inFile,
      std::string const &dsName, bool showWarnings)
  {
    blitz::Array<unsigned char,1> tmp;
    try
    {
      inFile.readDataset(tmp, dsName);
    }
    catch (BlitzH5Error &)
    {
      if (showWarnings)
          std::cerr << "'" << dsName << "' not found. Setting it to false"
                    << std::endl;
    }
    for (BlitzIndexT i = 0; i < buf.size() && i < tmp.size(); ++i)
        buf(i) = (tmp(i) == 1);
    for (BlitzIndexT i = tmp.size(); i < buf.size(); ++i)
        buf(i) = false;
  }

}
