/**************************************************************************
 *
 * This file belongs to the iRoCS Toolbox.
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

#include "Marker.hh"

#include <QtCore/QThread>

#include "MarkerPresetWidget.hh"
#include "MarkerControlWidget.hh"
#include "MarkerRenderer.hh"
#include "PointMarker.hh"
#include "SphereMarker.hh"
#include "CylinderMarker.hh"
#include "NucleusMarker.hh"
#include "SurfaceMarker.hh"
#include "CellMarker.hh"
#include "AnnotationChannelSpecs.hh"
#include "MultiChannelModel.hh"
#include "ViewWidget.hh"
#include "ColorMap.hh"

// #include <blitz/tinyvec-et.h>

#include "../libArrayToolbox/TypeTraits.hh"

QStringList Marker::supportedMarkerTypes()
{
  QStringList res;
  res << markerTypeToString(Point) << markerTypeToString(Sphere)
      << markerTypeToString(Nucleus) << markerTypeToString(Cylinder)
      << markerTypeToString(SHSurface) << markerTypeToString(Surface)
      << markerTypeToString(Cell);
  return res;
}

Marker::MarkerType Marker::markerTypeFromString(const QString& name)
{
  if (name == tr("Point")) return Point;
  if (name == tr("Sphere")) return Sphere;
  if (name == tr("Nucleus")) return Nucleus;
  if (name == tr("Cylinder")) return Cylinder;
  if (name == tr("SHSurface")) return SHSurface;
  if (name == tr("Surface")) return Surface;
  if (name == tr("Cell")) return Cell;
  std::cerr << __FILE__ << ":" << __LINE__ << ": Missing Implementation for "
            << "marker type " << name.toStdString() << std::endl;
  exit(-1);
}

QString Marker::markerTypeToString(MarkerType m)
{
  switch(m)
  {
  case Point :
    return tr("Point");
  case Sphere :
    return tr("Sphere");
  case Nucleus :
    return tr("Nucleus");
  case Cylinder :
    return tr("Cylinder");
  case SHSurface :
    return tr("SHSurface");
  case Surface :
    return tr("Surface");
  case Cell :
    return tr("Cell");
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": Missing Implementation"
              << std::endl;
    exit(-1);
  }
}

Marker::Marker(AnnotationChannelSpecs* channel)
        : QObject(), p_channel(channel), _id("noId"),
          _positionUm(0.0), _label(-1), _predictedLabel(-1),
          _manual(false), _needsFeatureUpdate(true),
          _features(std::vector<double>()),
          _probabilityMap(std::map<int,double>()), _qcDistance(-1),
          _radialDistance(-1), _phi(-1), 
          _randomColor(ColorMap::generateRandomColor()),
          _boundingBoxUpToDate(false),
          _boundingBoxLowerBoundUm(-1.0), _boundingBoxUpperBoundUm(-1.0),
          _updatesEnabled(true)
{
  if (p_channel != NULL) _updatesEnabled = p_channel->updatesEnabled();
}

Marker::Marker(const blitz::TinyVector<double,3>& positionUm,
               AnnotationChannelSpecs* channel)
        : QObject(), p_channel(channel), _id("noId"),
          _positionUm(positionUm), _label(-1), _predictedLabel(-1),
          _manual(false), _needsFeatureUpdate(true),
          _features(std::vector<double>()),
          _probabilityMap(std::map<int,double>()), _qcDistance(-1),
          _radialDistance(-1), _phi(-1), 
          _randomColor(ColorMap::generateRandomColor()),
          _boundingBoxUpToDate(false),
          _boundingBoxLowerBoundUm(-1.0), _boundingBoxUpperBoundUm(-1.0),
          _updatesEnabled(true)
{
  if (p_channel != NULL) _updatesEnabled = p_channel->updatesEnabled();
}

Marker::Marker(const Marker& marker)
        : QObject(), p_channel(NULL),
          _renderers(), _id(marker._id),
          _positionUm(marker._positionUm), _label(marker._label),
          _predictedLabel(marker._predictedLabel), _manual(marker._manual),
          _needsFeatureUpdate(marker._needsFeatureUpdate),
          _features(marker._features),
          _probabilityMap(marker._probabilityMap),
          _qcDistance(marker._qcDistance),
          _radialDistance(marker._radialDistance), _phi(marker._phi),
          _randomColor(marker._randomColor),
          _boundingBoxUpToDate(marker._boundingBoxUpToDate),
          _boundingBoxLowerBoundUm(marker._boundingBoxLowerBoundUm),
          _boundingBoxUpperBoundUm(marker._boundingBoxUpperBoundUm),
          _updatesEnabled(marker._updatesEnabled)
{}

Marker::~Marker()
{
  for (std::map<ViewWidget*, MarkerRenderer*>::iterator it =
           _renderers.begin(); it != _renderers.end(); ++it)
  {
    delete it->second;
    it->first->update();
  }
}

Marker &Marker::operator=(const Marker& marker)
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

void Marker::setChannel(AnnotationChannelSpecs* channel)
{
  if (p_channel == channel) return;
  p_channel = channel;
  if (p_channel != NULL) setUpdatesEnabled(p_channel->updatesEnabled());
}

AnnotationChannelSpecs* Marker::channel() const
{
  return p_channel;
}

void Marker::removeRenderer(ViewWidget* view)
{
  std::map<ViewWidget*, MarkerRenderer*>::iterator it =
      _renderers.find(view);
  if (it != _renderers.end())
  {
    delete it->second;
    _renderers.erase(it);
  }
}

MarkerRenderer* Marker::renderer(ViewWidget* view) const
{
  if (_renderers.find(view) == _renderers.end()) return NULL;
  return _renderers.find(view)->second;
}

const std::map<ViewWidget*, MarkerRenderer*>& Marker::renderers() const
{
  return _renderers;
}

std::string Marker::id() const
{
  return _id;
}

void Marker::setId(const std::string& id)
{
  if (_id == id) return;
  _id = id;
  if (p_channel != NULL)
  {
    if (selected() && p_channel->markerControlWidget() != NULL)
        p_channel->markerControlWidget()->setValues(this);
    if (p_channel->model() != NULL) p_channel->model()->setModified(true);
  }
}

blitz::TinyVector<double,3> Marker::positionUm() const
{
  if (p_channel == NULL) return relativePositionUm();
  if (blitz::all(p_channel->transformation() ==
                 atb::traits< blitz::TinyMatrix<double,4,4> >::one))
      return relativePositionUm();
  return atb::homogeneousToEuclidean(
      atb::mvMult(
          atb::invert(p_channel->transformation()),
          atb::euclideanToHomogeneous(relativePositionUm())));
}

void Marker::setPositionUm(const blitz::TinyVector<double,3>& pos) 
{
  if (p_channel == NULL) setRelativePositionUm(pos);
  else
  {
    if (blitz::all(p_channel->transformation() ==
                   atb::traits< blitz::TinyMatrix<double,4,4> >::one))
        setRelativePositionUm(pos);
    else
        setRelativePositionUm(
            atb::homogeneousToEuclidean(
                atb::mvMult(
                    p_channel->transformation(),
                    atb::euclideanToHomogeneous(pos))));
  }
}

blitz::TinyVector<double,3> Marker::relativePositionUm() const
{
  return _positionUm;
}

void Marker::setRelativePositionUm(const blitz::TinyVector<double,3>& pos) 
{
  if (blitz::all(_positionUm == pos)) return;
  _positionUm = pos;
  _needsFeatureUpdate = true;
  if (p_channel != NULL)
  {
    if (selected() && p_channel->markerControlWidget() != NULL)
        p_channel->markerControlWidget()->setValues(this);
    if (p_channel->model() != NULL)
    {
      p_channel->model()->setModified(true);
      update();
    }
  }
}

bool Marker::manual() const 
{
  return _manual;
}

void Marker::setManual(const bool manual) 
{
  if (_manual == manual) return;
  _manual = manual;
  if (p_channel != NULL)
  {
    if (selected() && p_channel->markerControlWidget() != NULL)
        p_channel->markerControlWidget()->setValues(this);
    if (p_channel->model() != NULL) p_channel->model()->setModified(true);
  }
}

bool Marker::needsFeatureUpdate() const 
{
  return _needsFeatureUpdate;
}

void Marker::setNeedsFeatureUpdate(bool needsFeatureUpdate) 
{
  if (_needsFeatureUpdate == needsFeatureUpdate) return;
  _needsFeatureUpdate = needsFeatureUpdate;
  if (p_channel != NULL)
  {
    if (selected() && p_channel->markerControlWidget() != NULL)
        p_channel->markerControlWidget()->setValues(this);
    if (p_channel->model() != NULL) p_channel->model()->setModified(true);
  }
}

int Marker::label() const 
{
  return _label;
}

void Marker::setLabel(const int label) 
{
  if (_label == label) return;
  _label = label;
  if (p_channel != NULL)
  {
    if (selected() && p_channel->markerControlWidget() != NULL)
        p_channel->markerControlWidget()->setValues(this);
    if (p_channel->model() != NULL)
    {
      p_channel->model()->setModified(true);
      update();
    }
  }
}

int Marker::predictedLabel() const 
{
  return _predictedLabel;
}

void Marker::setPredictedLabel(const int label) 
{
  if (_predictedLabel == label) return;
  _predictedLabel = label;
  if (p_channel != NULL)
  {
    if (selected() && p_channel->markerControlWidget() != NULL)
        p_channel->markerControlWidget()->setValues(this);
    if (p_channel->model() != NULL)
    {
      p_channel->model()->setModified(true);
      update();
    }
  }
}

const std::vector<double>& Marker::features() const
{
  return _features;
}

double& Marker::feature(size_t index)
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
  if (p_channel != NULL)
  {
    if (selected() && p_channel->markerControlWidget() != NULL)
        p_channel->markerControlWidget()->setValues(this);
    if (p_channel->model() != NULL) p_channel->model()->setModified(true);
  }
}
  
double Marker::radialDistanceUm() const
{
  return _radialDistance;
}

void Marker::setRadialDistanceUm(const double radialDistanceUm)
{
  if (_radialDistance == radialDistanceUm) return;
  _radialDistance = radialDistanceUm;
  if (p_channel != NULL)
  {
    if (selected() && p_channel->markerControlWidget() != NULL)
        p_channel->markerControlWidget()->setValues(this);
    if (p_channel->model() != NULL) p_channel->model()->setModified(true);
  }
}
  
double Marker::phi() const
{
  return _phi;
}

void Marker::setPhi(const double phi)
{
  if (_phi == phi) return;
  _phi = phi;
  if (p_channel != NULL)
  {
    if (selected() && p_channel->markerControlWidget() != NULL)
        p_channel->markerControlWidget()->setValues(this);
    if (p_channel->model() != NULL) p_channel->model()->setModified(true);
  }
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
    positions(i) = (*it)->relativePositionUm();
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

blitz::TinyVector<float,3> const &Marker::randomColor() const
{
  return _randomColor;
}

void Marker::load(
    AnnotationChannelSpecs *channel, BlitzH5File const &inFile,
    std::string const &group, iRoCS::ProgressReporter *pr)
{
  if (channel == NULL) return;
  
  int pStart = (pr != NULL) ? pr->taskProgressMin() : 0;
  int pRange = (pr != NULL) ? (pr->taskProgressMax() - pStart) : 1;

  blitz::Array<blitz::TinyVector<double,3>,1> positions;
  inFile.readDataset(positions, group + "/position_um");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.05f * pRange))) return;

  blitz::Array<int,1> labels(positions.shape());
  _loadFixedSize(labels, inFile, group + "/label");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.1f * pRange))) return;

  blitz::Array<int,1> predictedLabels(positions.shape());
  _loadFixedSize(predictedLabels, inFile, group + "/predicted_label");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.15f * pRange))) return;

  blitz::Array<bool,1> manuals(positions.shape());
  _loadFixedSize(manuals, inFile, group + "/manual");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.2f * pRange))) return;

  blitz::Array<bool,1> needsFeatureUpdates(positions.shape());
  _loadFixedSize(needsFeatureUpdates, inFile, group + "/needsFeatureUpdate");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.25f * pRange))) return;

  blitz::Array<double,1> nFeatures(positions.shape());
  _loadFixedSize(nFeatures, inFile, group + "/nFeatures");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.27f * pRange))) return;

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
        tmp = features(blitz::Range(0, tmp.extent(0) - 1), blitz::Range::all());
      }
      features.resize(tmp.extent(0), tmp.extent(1));
      features = tmp;
    }
  }
  catch (BlitzH5Error&)
  {}
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.3f * pRange))) return;

  blitz::Array<int,1> classes;
  blitz::Array<double,2> probabilities;
  try 
  {
    inFile.readDataset(classes, group + "/platt_classes");
    inFile.readDataset(probabilities, group + "platt_probabilities");
    assert(classes.extent(0) == probabilities.extent(1));
    if (probabilities.extent(0) != positions.extent(0)) 
    {
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
  {}
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.35f * pRange))) return;
  
  blitz::Array<double,1> qcdistances(positions.shape());
  _loadFixedSize(qcdistances, inFile, group + "/qcDistance_um");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.4f * pRange))) return;

  blitz::Array<double,1> radialdistances(positions.shape());
  _loadFixedSize(radialdistances, inFile, group + "/radialDistance_um");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.45f * pRange))) return;

  blitz::Array<double,1> phis(positions.shape());
  _loadFixedSize(phis, inFile, group + "/phi");
  if (pr != NULL && !pr->updateProgress(
          pStart + static_cast<int>(0.5f * pRange))) return;

  for (atb::BlitzIndexT i = 0;
       i < static_cast<atb::BlitzIndexT>(positions.size()); ++i) 
  {
    if (pr != NULL && !pr->updateProgress(
            pStart + static_cast<int>(
                (0.5f + 0.5f * static_cast<float>(i) /
                 static_cast<float>(positions.extent(0) - 1)) * pRange)))
        return;
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
      channel->addMarker(m);
      
      std::stringstream idStream;
      idStream.fill('0');
      idStream << "marker_" << std::setw(8) << i;
      channel->markers().back()->setId(idStream.str());
      channel->markers().back()->setRelativePositionUm(positions(i));
      channel->markers().back()->setLabel(labels(i));
      channel->markers().back()->setPredictedLabel(predictedLabels(i));
      channel->markers().back()->setManual(manuals(i));
      channel->markers().back()->setNeedsFeatureUpdate(needsFeatureUpdates(i));
      if (features.size() != 0)
      {
        if (nFeatures(i) == -1)
        {
          std::vector<double> feavec(features.extent(1));
          for (atb::BlitzIndexT j = 0; j < features.extent(1); ++j)
              feavec[j] = features(i, j);
          channel->markers().back()->setFeatures(feavec);
        }
        else
        {
          std::vector<double> feavec(nFeatures(i));
          for (atb::BlitzIndexT j = 0; j < nFeatures(i); ++j)
              feavec[j] = features(i, j);
          channel->markers().back()->setFeatures(feavec);
        }
      }
      if (probabilities.size() != 0)
      {
        std::map<int,double> prob;
        for (atb::BlitzIndexT j = 0; j < probabilities.extent(1); ++j)
        {
          prob[static_cast<int>(classes(j))] = probabilities(i, j);
        }
        channel->markers().back()->setProbabilityMap(prob);
      }
      channel->markers().back()->setQcDistanceUm(qcdistances(i));
      channel->markers().back()->setRadialDistanceUm(radialdistances(i));
      channel->markers().back()->setPhi(phis(i));
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
  out << positionUm()(2) << ";" << positionUm()(1) << ";" << positionUm()(0)
      << ";" << _label << ";" << _predictedLabel << ";" << ((_manual) ? 1 : 0)
      << ";" << _qcDistance << ";" << _radialDistance << ";" << _phi;
}

void Marker::loadFromMap(std::map<std::string,std::string> const &values)
{
  _positionUm =
      atof(values.find("z (micron)")->second.c_str()),
      atof(values.find("y (micron)")->second.c_str()),
      atof(values.find("x (micron)")->second.c_str());
  _positionUm = atb::homogeneousToEuclidean(
      atb::mvMult(
          p_channel->transformation(),
          atb::euclideanToHomogeneous(_positionUm)));
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

void Marker::setSelected(bool selected)
{
  if (p_channel != NULL)
  {
    if (selected) p_channel->selectMarker(this);
    else p_channel->selectMarker(NULL);
  }
}

bool Marker::selected() const
{
  return (p_channel != NULL) ? (p_channel->selectedMarker() == this) : false;
}

blitz::TinyVector<double,3> Marker::boundingBoxLowerBoundUm() const
{
  if (!_boundingBoxUpToDate) _updateBoundingBox();
  return positionUm() + _boundingBoxLowerBoundUm;
}

blitz::TinyVector<double,3> Marker::boundingBoxUpperBoundUm() const
{
  if (!_boundingBoxUpToDate) _updateBoundingBox();
  return positionUm() + _boundingBoxUpperBoundUm;
}

bool Marker::updatesEnabled() const
{
  return _updatesEnabled;
}

void Marker::copyToATBNucleus(atb::Nucleus &nc) const
{
  nc.setId(_id);
  nc.setPositionUm(positionUm());
  nc.setManual(_manual);
  nc.setNeedsFeatureUpdate(_needsFeatureUpdate);
  nc.setLabel(_label);
  nc.setPredictedLabel(_predictedLabel);
  nc.features() = _features;
  nc.probabilityMap() = _probabilityMap;
  nc.setQcDistanceUm(_qcDistance);
  nc.setRadialDistanceUm(_radialDistance);
  nc.setPhi(_phi);
}

void Marker::copyFromATBNucleus(atb::Nucleus const &nc)
{
  _id = nc.id();
  _positionUm = nc.positionUm();
  if (p_channel != NULL)
    _positionUm = atb::homogeneousToEuclidean(
        atb::mvMult(
            p_channel->transformation(),
            atb::euclideanToHomogeneous(_positionUm)));
  _manual = nc.manual();
  _needsFeatureUpdate = nc.needsFeatureUpdate();
  _label = nc.label();
  _predictedLabel = nc.predictedLabel();
  _features = nc.features();
  _probabilityMap = nc.probabilityMap();
  _qcDistance = nc.qcDistanceUm();
  _radialDistance = nc.radialDistanceUm();
  _phi = nc.phi();
}

void Marker::setUpdatesEnabled(bool enable)
{
  _updatesEnabled = enable;
  for (std::map<ViewWidget*, MarkerRenderer*>::iterator it =
           _renderers.begin(); it != _renderers.end(); ++it)
      it->second->setCacheUpdatesEnabled(enable);
}

void Marker::update()
{
  if (!_updatesEnabled) return;
  
  for (std::map<ViewWidget*, MarkerRenderer*>::iterator it =
           _renderers.begin(); it != _renderers.end(); ++it)
  {
    it->second->updateCache();
    it->first->redraw();
  }
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
  for (size_t i = 0; i < buf.size() && i < tmp.size(); ++i)
      buf(i) = (tmp(i) == 1);
  for (size_t i = tmp.size(); i < buf.size(); ++i) buf(i) = false;
}
  
