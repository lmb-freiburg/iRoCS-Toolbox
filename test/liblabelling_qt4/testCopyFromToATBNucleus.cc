#include "lmbunit.hh"

#include <liblabelling_qt4/AnnotationChannelSpecs.hh>
#include <liblabelling_qt4/NucleusMarker.hh>

static void testCopyNucleusMarkerFromATBNucleus()
{
  atb::Nucleus nc;
  nc.setId("1");
  nc.setPositionUm(blitz::TinyVector<double,3>(1.0, 2.0, 3.0));
  nc.setLabel(42);
  nc.setPredictedLabel(23);
  nc.setManual(true);
  nc.setNeedsFeatureUpdate(true);
  std::vector<double> features(4);
  features[0] = 1.0;
  features[1] = 2.0;
  features[2] = 3.0;
  features[3] = 4.0;
  nc.features() = features;
  std::map<int,double> probabilities;
  probabilities[0] = 0.7;
  probabilities[1] = 0.2;
  probabilities[2] = 0.02;
  nc.probabilityMap() = probabilities;
  nc.setQcDistanceUm(100.0);
  nc.setRadialDistanceUm(50.0);
  nc.setPhi(1.2);
  nc.setRadiusUm(2.5);
  nc.shCoefficients().resize(5);
  nc.shCoefficients()(0) = std::complex<double>(2.0, 1.0);
  nc.shCoefficients()(1) = std::complex<double>(1.0, 2.0);
  nc.shCoefficients()(2) = std::complex<double>(0.0, 3.0);
  nc.shCoefficients()(3) = std::complex<double>(-1.0, 4.0);
  nc.shCoefficients()(4) = std::complex<double>(-2.0, 5.0);
  nc.setValue(5.0);
  nc.setConfidence(0.99);
  nc.setSphase(0.75);
  nc.setBorderDistanceUm(40.0);
  nc.setVolumeUm3(30000.0);
  nc.setMitotic(true);
  nc.setPredictedMitotic(true);
  nc.setSubtype(3);
  nc.setPredictedSubtype(4);
  nc.setCellfile(12);
  nc.setPredictedCellfile(13);

  NucleusMarker marker;
  marker.setId("2");
  marker.setPositionUm(blitz::TinyVector<double,3>(5.0, 4.0, 2.0));
  marker.setLabel(11);
  marker.setPredictedLabel(97);
  marker.setManual(false);
  marker.setNeedsFeatureUpdate(false);
  marker.setQcDistanceUm(10.0);
  marker.setRadialDistanceUm(72.0);
  marker.setPhi(-0.7);
  marker.setRadiusUm(3.4);
  segmentation::FrequencyArray coeffs(3);
  coeffs(0) = std::complex<double>(20.0, -1.0);
  coeffs(1) = std::complex<double>(-5.0, -2.0);
  coeffs(2) = std::complex<double>(-10.0, -3.0);
  marker.setCoefficients(coeffs);
  marker.setValue(95.0);
  marker.setConfidence(0.1);
  marker.setSPhase(0.4);
  marker.setBorderDistanceUm(22.0);
  marker.setVolumeUm3(45000.0);
  marker.setMitotic(false);
  marker.setPredictedMitotic(false);
  marker.setSubtype(17);
  marker.setPredictedSubtype(18);
  marker.setContinuity(55);
  marker.setPredictedContinuity(56);
  
  marker.copyFromATBNucleus(nc);
  
  LMBUNIT_ASSERT_EQUAL(marker.id(), "1");
  LMBUNIT_ASSERT_EQUAL(nc.id(), marker.id());
  LMBUNIT_ASSERT(blitz::all(marker.positionUm() == nc.positionUm()));
  LMBUNIT_ASSERT_EQUAL(nc.label(), marker.label());
  LMBUNIT_ASSERT_EQUAL(nc.predictedLabel(), marker.predictedLabel());
  LMBUNIT_ASSERT_EQUAL(nc.manual(), marker.manual());
  LMBUNIT_ASSERT_EQUAL(nc.needsFeatureUpdate(), marker.needsFeatureUpdate());
  LMBUNIT_ASSERT_EQUAL(nc.features().size(), marker.features().size());
  for (size_t i = 0; i < nc.features().size(); ++i)
      LMBUNIT_ASSERT_EQUAL(nc.features()[i], marker.features()[i]);
  LMBUNIT_ASSERT_EQUAL(
      nc.probabilityMap().size(), marker.probabilityMap().size());
  std::map<int,double>::const_iterator it1 = nc.probabilityMap().begin();
  std::map<int,double>::const_iterator it2 = marker.probabilityMap().begin();
  for (; it1 != nc.probabilityMap().end(); ++it1, ++it2)
  {
    LMBUNIT_ASSERT_EQUAL(it1->first, it2->first);
    LMBUNIT_ASSERT_EQUAL(it1->second, it2->second);
  }
  LMBUNIT_ASSERT_EQUAL(nc.qcDistanceUm(), marker.qcDistanceUm());
  LMBUNIT_ASSERT_EQUAL(nc.radialDistanceUm(), marker.radialDistanceUm());
  LMBUNIT_ASSERT_EQUAL(nc.phi(), marker.phi());
  LMBUNIT_ASSERT_EQUAL(nc.radiusUm(), marker.radiusUm());
  LMBUNIT_ASSERT_EQUAL(
      nc.shCoefficients().size(), marker.coefficients().size());
  for (size_t i = 0; i < nc.shCoefficients().size(); ++i)
      LMBUNIT_ASSERT_EQUAL(
          nc.shCoefficients()(i), marker.coefficients()(i));
  LMBUNIT_ASSERT_EQUAL(nc.value(), marker.value());
  LMBUNIT_ASSERT_EQUAL(nc.confidence(), marker.confidence());
  LMBUNIT_ASSERT_EQUAL(nc.sphase(), marker.sPhase());
  LMBUNIT_ASSERT_EQUAL(nc.borderDistanceUm(), marker.borderDistanceUm());
  LMBUNIT_ASSERT_EQUAL(nc.volumeUm3(), marker.volumeUm3());
  LMBUNIT_ASSERT_EQUAL(nc.mitotic(), marker.mitotic());
  LMBUNIT_ASSERT_EQUAL(nc.predictedMitotic(), marker.predictedMitotic());
  LMBUNIT_ASSERT_EQUAL(nc.subtype(), marker.subtype());
  LMBUNIT_ASSERT_EQUAL(nc.predictedSubtype(), marker.predictedSubtype());
  LMBUNIT_ASSERT_EQUAL(nc.cellfile(), marker.continuity());
  LMBUNIT_ASSERT_EQUAL(nc.predictedCellfile(), marker.predictedContinuity());
}

static void testCopySHSurfaceMarkerFromATBNucleus()
{
  atb::Nucleus nc;
  nc.setId("1");
  nc.setPositionUm(blitz::TinyVector<double,3>(1.0, 2.0, 3.0));
  nc.setLabel(42);
  nc.setPredictedLabel(23);
  nc.setManual(true);
  nc.setNeedsFeatureUpdate(true);
  std::vector<double> features(4);
  features[0] = 1.0;
  features[1] = 2.0;
  features[2] = 3.0;
  features[3] = 4.0;
  nc.features() = features;
  std::map<int,double> probabilities;
  probabilities[0] = 0.7;
  probabilities[1] = 0.2;
  probabilities[2] = 0.02;
  nc.probabilityMap() = probabilities;
  nc.setQcDistanceUm(100.0);
  nc.setRadialDistanceUm(50.0);
  nc.setPhi(1.2);
  nc.setRadiusUm(2.5);
  nc.shCoefficients().resize(5);
  nc.shCoefficients()(0) = std::complex<double>(2.0, 1.0);
  nc.shCoefficients()(1) = std::complex<double>(1.0, 2.0);
  nc.shCoefficients()(2) = std::complex<double>(0.0, 3.0);
  nc.shCoefficients()(3) = std::complex<double>(-1.0, 4.0);
  nc.shCoefficients()(4) = std::complex<double>(-2.0, 5.0);
  nc.setValue(5.0);
  nc.setConfidence(0.99);
  nc.setSphase(0.75);
  nc.setBorderDistanceUm(40.0);
  nc.setVolumeUm3(30000.0);
  nc.setMitotic(true);
  nc.setPredictedMitotic(true);
  nc.setSubtype(3);
  nc.setPredictedSubtype(4);
  nc.setCellfile(12);
  nc.setPredictedCellfile(13);

  SHSurfaceMarker marker;
  marker.setId("2");
  marker.setPositionUm(blitz::TinyVector<double,3>(5.0, 4.0, 2.0));
  marker.setLabel(11);
  marker.setPredictedLabel(97);
  marker.setManual(false);
  marker.setNeedsFeatureUpdate(false);
  marker.setQcDistanceUm(10.0);
  marker.setRadialDistanceUm(72.0);
  marker.setPhi(-0.7);
  marker.setRadiusUm(3.4);
  segmentation::FrequencyArray coeffs(3);
  coeffs(0) = std::complex<double>(20.0, -1.0);
  coeffs(1) = std::complex<double>(-5.0, -2.0);
  coeffs(2) = std::complex<double>(-10.0, -3.0);
  marker.setCoefficients(coeffs);
  
  marker.copyFromATBNucleus(nc);
  
  LMBUNIT_ASSERT_EQUAL(marker.id(), "1");
  LMBUNIT_ASSERT_EQUAL(nc.id(), marker.id());
  LMBUNIT_ASSERT(blitz::all(marker.positionUm() == nc.positionUm()));
  LMBUNIT_ASSERT_EQUAL(nc.label(), marker.label());
  LMBUNIT_ASSERT_EQUAL(nc.predictedLabel(), marker.predictedLabel());
  LMBUNIT_ASSERT_EQUAL(nc.manual(), marker.manual());
  LMBUNIT_ASSERT_EQUAL(nc.needsFeatureUpdate(), marker.needsFeatureUpdate());
  LMBUNIT_ASSERT_EQUAL(nc.features().size(), marker.features().size());
  for (size_t i = 0; i < nc.features().size(); ++i)
      LMBUNIT_ASSERT_EQUAL(nc.features()[i], marker.features()[i]);
  LMBUNIT_ASSERT_EQUAL(
      nc.probabilityMap().size(), marker.probabilityMap().size());
  std::map<int,double>::const_iterator it1 = nc.probabilityMap().begin();
  std::map<int,double>::const_iterator it2 = marker.probabilityMap().begin();
  for (; it1 != nc.probabilityMap().end(); ++it1, ++it2)
  {
    LMBUNIT_ASSERT_EQUAL(it1->first, it2->first);
    LMBUNIT_ASSERT_EQUAL(it1->second, it2->second);
  }
  LMBUNIT_ASSERT_EQUAL(nc.qcDistanceUm(), marker.qcDistanceUm());
  LMBUNIT_ASSERT_EQUAL(nc.radialDistanceUm(), marker.radialDistanceUm());
  LMBUNIT_ASSERT_EQUAL(nc.phi(), marker.phi());
  LMBUNIT_ASSERT_EQUAL(nc.radiusUm(), marker.radiusUm());
  LMBUNIT_ASSERT_EQUAL(
      nc.shCoefficients().size(), marker.coefficients().size());
  for (size_t i = 0; i < nc.shCoefficients().size(); ++i)
      LMBUNIT_ASSERT_EQUAL(
          nc.shCoefficients()(i), marker.coefficients()(i));
}

static void testCopySphereMarkerFromATBNucleus()
{
  atb::Nucleus nc;
  nc.setId("1");
  nc.setPositionUm(blitz::TinyVector<double,3>(1.0, 2.0, 3.0));
  nc.setLabel(42);
  nc.setPredictedLabel(23);
  nc.setManual(true);
  nc.setNeedsFeatureUpdate(true);
  std::vector<double> features(4);
  features[0] = 1.0;
  features[1] = 2.0;
  features[2] = 3.0;
  features[3] = 4.0;
  nc.features() = features;
  std::map<int,double> probabilities;
  probabilities[0] = 0.7;
  probabilities[1] = 0.2;
  probabilities[2] = 0.02;
  nc.probabilityMap() = probabilities;
  nc.setQcDistanceUm(100.0);
  nc.setRadialDistanceUm(50.0);
  nc.setPhi(1.2);
  nc.setRadiusUm(2.5);
  nc.shCoefficients().resize(5);
  nc.shCoefficients()(0) = std::complex<double>(2.0, 1.0);
  nc.shCoefficients()(1) = std::complex<double>(1.0, 2.0);
  nc.shCoefficients()(2) = std::complex<double>(0.0, 3.0);
  nc.shCoefficients()(3) = std::complex<double>(-1.0, 4.0);
  nc.shCoefficients()(4) = std::complex<double>(-2.0, 5.0);
  nc.setValue(5.0);
  nc.setConfidence(0.99);
  nc.setSphase(0.75);
  nc.setBorderDistanceUm(40.0);
  nc.setVolumeUm3(30000.0);
  nc.setMitotic(true);
  nc.setPredictedMitotic(true);
  nc.setSubtype(3);
  nc.setPredictedSubtype(4);
  nc.setCellfile(12);
  nc.setPredictedCellfile(13);

  SphereMarker marker;
  marker.setId("2");
  marker.setPositionUm(blitz::TinyVector<double,3>(5.0, 4.0, 2.0));
  marker.setLabel(11);
  marker.setPredictedLabel(97);
  marker.setManual(false);
  marker.setNeedsFeatureUpdate(false);
  marker.setQcDistanceUm(10.0);
  marker.setRadialDistanceUm(72.0);
  marker.setPhi(-0.7);
  marker.setRadiusUm(3.4);
  
  marker.copyFromATBNucleus(nc);
  
  LMBUNIT_ASSERT_EQUAL(marker.id(), "1");
  LMBUNIT_ASSERT_EQUAL(nc.id(), marker.id());
  LMBUNIT_ASSERT(blitz::all(marker.positionUm() == nc.positionUm()));
  LMBUNIT_ASSERT_EQUAL(nc.label(), marker.label());
  LMBUNIT_ASSERT_EQUAL(nc.predictedLabel(), marker.predictedLabel());
  LMBUNIT_ASSERT_EQUAL(nc.manual(), marker.manual());
  LMBUNIT_ASSERT_EQUAL(nc.needsFeatureUpdate(), marker.needsFeatureUpdate());
  LMBUNIT_ASSERT_EQUAL(nc.features().size(), marker.features().size());
  for (size_t i = 0; i < nc.features().size(); ++i)
      LMBUNIT_ASSERT_EQUAL(nc.features()[i], marker.features()[i]);
  LMBUNIT_ASSERT_EQUAL(
      nc.probabilityMap().size(), marker.probabilityMap().size());
  std::map<int,double>::const_iterator it1 = nc.probabilityMap().begin();
  std::map<int,double>::const_iterator it2 = marker.probabilityMap().begin();
  for (; it1 != nc.probabilityMap().end(); ++it1, ++it2)
  {
    LMBUNIT_ASSERT_EQUAL(it1->first, it2->first);
    LMBUNIT_ASSERT_EQUAL(it1->second, it2->second);
  }
  LMBUNIT_ASSERT_EQUAL(nc.qcDistanceUm(), marker.qcDistanceUm());
  LMBUNIT_ASSERT_EQUAL(nc.radialDistanceUm(), marker.radialDistanceUm());
  LMBUNIT_ASSERT_EQUAL(nc.phi(), marker.phi());
  LMBUNIT_ASSERT_EQUAL(nc.radiusUm(), marker.radiusUm());
}

static void testCopyPointMarkerFromATBNucleus()
{
  atb::Nucleus nc;
  nc.setId("1");
  nc.setPositionUm(blitz::TinyVector<double,3>(1.0, 2.0, 3.0));
  nc.setLabel(42);
  nc.setPredictedLabel(23);
  nc.setManual(true);
  nc.setNeedsFeatureUpdate(true);
  std::vector<double> features(4);
  features[0] = 1.0;
  features[1] = 2.0;
  features[2] = 3.0;
  features[3] = 4.0;
  nc.features() = features;
  std::map<int,double> probabilities;
  probabilities[0] = 0.7;
  probabilities[1] = 0.2;
  probabilities[2] = 0.02;
  nc.probabilityMap() = probabilities;
  nc.setQcDistanceUm(100.0);
  nc.setRadialDistanceUm(50.0);
  nc.setPhi(1.2);
  nc.setRadiusUm(2.5);
  nc.shCoefficients().resize(5);
  nc.shCoefficients()(0) = std::complex<double>(2.0, 1.0);
  nc.shCoefficients()(1) = std::complex<double>(1.0, 2.0);
  nc.shCoefficients()(2) = std::complex<double>(0.0, 3.0);
  nc.shCoefficients()(3) = std::complex<double>(-1.0, 4.0);
  nc.shCoefficients()(4) = std::complex<double>(-2.0, 5.0);
  nc.setValue(5.0);
  nc.setConfidence(0.99);
  nc.setSphase(0.75);
  nc.setBorderDistanceUm(40.0);
  nc.setVolumeUm3(30000.0);
  nc.setMitotic(true);
  nc.setPredictedMitotic(true);
  nc.setSubtype(3);
  nc.setPredictedSubtype(4);
  nc.setCellfile(12);
  nc.setPredictedCellfile(13);

  PointMarker marker;
  marker.setId("2");
  marker.setPositionUm(blitz::TinyVector<double,3>(5.0, 4.0, 2.0));
  marker.setLabel(11);
  marker.setPredictedLabel(97);
  marker.setManual(false);
  marker.setNeedsFeatureUpdate(false);
  marker.setQcDistanceUm(10.0);
  marker.setRadialDistanceUm(72.0);
  marker.setPhi(-0.7);
  
  marker.copyFromATBNucleus(nc);
  
  LMBUNIT_ASSERT_EQUAL(marker.id(), "1");
  LMBUNIT_ASSERT_EQUAL(nc.id(), marker.id());
  LMBUNIT_ASSERT(blitz::all(marker.positionUm() == nc.positionUm()));
  LMBUNIT_ASSERT_EQUAL(nc.label(), marker.label());
  LMBUNIT_ASSERT_EQUAL(nc.predictedLabel(), marker.predictedLabel());
  LMBUNIT_ASSERT_EQUAL(nc.manual(), marker.manual());
  LMBUNIT_ASSERT_EQUAL(nc.needsFeatureUpdate(), marker.needsFeatureUpdate());
  LMBUNIT_ASSERT_EQUAL(nc.features().size(), marker.features().size());
  for (size_t i = 0; i < nc.features().size(); ++i)
      LMBUNIT_ASSERT_EQUAL(nc.features()[i], marker.features()[i]);
  LMBUNIT_ASSERT_EQUAL(
      nc.probabilityMap().size(), marker.probabilityMap().size());
  std::map<int,double>::const_iterator it1 = nc.probabilityMap().begin();
  std::map<int,double>::const_iterator it2 = marker.probabilityMap().begin();
  for (; it1 != nc.probabilityMap().end(); ++it1, ++it2)
  {
    LMBUNIT_ASSERT_EQUAL(it1->first, it2->first);
    LMBUNIT_ASSERT_EQUAL(it1->second, it2->second);
  }
  LMBUNIT_ASSERT_EQUAL(nc.qcDistanceUm(), marker.qcDistanceUm());
  LMBUNIT_ASSERT_EQUAL(nc.radialDistanceUm(), marker.radialDistanceUm());
  LMBUNIT_ASSERT_EQUAL(nc.phi(), marker.phi());
}

static void testCopyNucleusMarkerToATBNucleus()
{
  atb::Nucleus nc;
  nc.setId("1");
  nc.setPositionUm(blitz::TinyVector<double,3>(1.0, 2.0, 3.0));
  nc.setLabel(42);
  nc.setPredictedLabel(23);
  nc.setManual(true);
  nc.setNeedsFeatureUpdate(true);
  std::vector<double> features(4);
  features[0] = 1.0;
  features[1] = 2.0;
  features[2] = 3.0;
  features[3] = 4.0;
  nc.features() = features;
  std::map<int,double> probabilities;
  probabilities[0] = 0.7;
  probabilities[1] = 0.2;
  probabilities[2] = 0.02;
  nc.probabilityMap() = probabilities;
  nc.setQcDistanceUm(100.0);
  nc.setRadialDistanceUm(50.0);
  nc.setPhi(1.2);
  nc.setRadiusUm(2.5);
  nc.shCoefficients().resize(5);
  nc.shCoefficients()(0) = std::complex<double>(2.0, 1.0);
  nc.shCoefficients()(1) = std::complex<double>(1.0, 2.0);
  nc.shCoefficients()(2) = std::complex<double>(0.0, 3.0);
  nc.shCoefficients()(3) = std::complex<double>(-1.0, 4.0);
  nc.shCoefficients()(4) = std::complex<double>(-2.0, 5.0);
  nc.setValue(5.0);
  nc.setConfidence(0.99);
  nc.setSphase(0.75);
  nc.setBorderDistanceUm(40.0);
  nc.setVolumeUm3(30000.0);
  nc.setMitotic(true);
  nc.setPredictedMitotic(true);
  nc.setSubtype(3);
  nc.setPredictedSubtype(4);
  nc.setCellfile(12);
  nc.setPredictedCellfile(13);

  NucleusMarker marker;
  marker.setId("2");
  marker.setPositionUm(blitz::TinyVector<double,3>(5.0, 4.0, 2.0));
  marker.setLabel(11);
  marker.setPredictedLabel(97);
  marker.setManual(false);
  marker.setNeedsFeatureUpdate(false);
  marker.setQcDistanceUm(10.0);
  marker.setRadialDistanceUm(72.0);
  marker.setPhi(-0.7);
  marker.setRadiusUm(3.4);
  segmentation::FrequencyArray coeffs(3);
  coeffs(0) = std::complex<double>(20.0, -1.0);
  coeffs(1) = std::complex<double>(-5.0, -2.0);
  coeffs(2) = std::complex<double>(-10.0, -3.0);
  marker.setCoefficients(coeffs);
  marker.setValue(95.0);
  marker.setConfidence(0.1);
  marker.setSPhase(0.4);
  marker.setBorderDistanceUm(22.0);
  marker.setVolumeUm3(45000.0);
  marker.setMitotic(false);
  marker.setPredictedMitotic(false);
  marker.setSubtype(17);
  marker.setPredictedSubtype(18);
  marker.setContinuity(55);
  marker.setPredictedContinuity(56);
  
  marker.copyToATBNucleus(nc);
  
  LMBUNIT_ASSERT_EQUAL(marker.id(), "2");
  LMBUNIT_ASSERT_EQUAL(nc.id(), marker.id());
  LMBUNIT_ASSERT(blitz::all(marker.positionUm() == nc.positionUm()));
  LMBUNIT_ASSERT_EQUAL(nc.label(), marker.label());
  LMBUNIT_ASSERT_EQUAL(nc.predictedLabel(), marker.predictedLabel());
  LMBUNIT_ASSERT_EQUAL(nc.manual(), marker.manual());
  LMBUNIT_ASSERT_EQUAL(nc.needsFeatureUpdate(), marker.needsFeatureUpdate());
  LMBUNIT_ASSERT_EQUAL(nc.features().size(), marker.features().size());
  for (size_t i = 0; i < nc.features().size(); ++i)
      LMBUNIT_ASSERT_EQUAL(nc.features()[i], marker.features()[i]);
  LMBUNIT_ASSERT_EQUAL(
      nc.probabilityMap().size(), marker.probabilityMap().size());
  std::map<int,double>::const_iterator it1 = nc.probabilityMap().begin();
  std::map<int,double>::const_iterator it2 = marker.probabilityMap().begin();
  for (; it1 != nc.probabilityMap().end(); ++it1, ++it2)
  {
    LMBUNIT_ASSERT_EQUAL(it1->first, it2->first);
    LMBUNIT_ASSERT_EQUAL(it1->second, it2->second);
  }
  LMBUNIT_ASSERT_EQUAL(nc.qcDistanceUm(), marker.qcDistanceUm());
  LMBUNIT_ASSERT_EQUAL(nc.radialDistanceUm(), marker.radialDistanceUm());
  LMBUNIT_ASSERT_EQUAL(nc.phi(), marker.phi());
  LMBUNIT_ASSERT_EQUAL(nc.radiusUm(), marker.radiusUm());
  LMBUNIT_ASSERT_EQUAL(
      nc.shCoefficients().size(), marker.coefficients().size());
  for (size_t i = 0; i < nc.shCoefficients().size(); ++i)
      LMBUNIT_ASSERT_EQUAL(
          nc.shCoefficients()(i), marker.coefficients()(i));
  LMBUNIT_ASSERT_EQUAL(nc.value(), marker.value());
  LMBUNIT_ASSERT_EQUAL(nc.confidence(), marker.confidence());
  LMBUNIT_ASSERT_EQUAL(nc.sphase(), marker.sPhase());
  LMBUNIT_ASSERT_EQUAL(nc.borderDistanceUm(), marker.borderDistanceUm());
  LMBUNIT_ASSERT_EQUAL(nc.volumeUm3(), marker.volumeUm3());
  LMBUNIT_ASSERT_EQUAL(nc.mitotic(), marker.mitotic());
  LMBUNIT_ASSERT_EQUAL(nc.predictedMitotic(), marker.predictedMitotic());
  LMBUNIT_ASSERT_EQUAL(nc.subtype(), marker.subtype());
  LMBUNIT_ASSERT_EQUAL(nc.predictedSubtype(), marker.predictedSubtype());
  LMBUNIT_ASSERT_EQUAL(nc.cellfile(), marker.continuity());
  LMBUNIT_ASSERT_EQUAL(nc.predictedCellfile(), marker.predictedContinuity());
}

static void testCopySHSurfaceMarkerToATBNucleus()
{
  atb::Nucleus nc;
  nc.setId("1");
  nc.setPositionUm(blitz::TinyVector<double,3>(1.0, 2.0, 3.0));
  nc.setLabel(42);
  nc.setPredictedLabel(23);
  nc.setManual(true);
  nc.setNeedsFeatureUpdate(true);
  std::vector<double> features(4);
  features[0] = 1.0;
  features[1] = 2.0;
  features[2] = 3.0;
  features[3] = 4.0;
  nc.features() = features;
  std::map<int,double> probabilities;
  probabilities[0] = 0.7;
  probabilities[1] = 0.2;
  probabilities[2] = 0.02;
  nc.probabilityMap() = probabilities;
  nc.setQcDistanceUm(100.0);
  nc.setRadialDistanceUm(50.0);
  nc.setPhi(1.2);
  nc.setRadiusUm(2.5);
  nc.shCoefficients().resize(5);
  nc.shCoefficients()(0) = std::complex<double>(2.0, 1.0);
  nc.shCoefficients()(1) = std::complex<double>(1.0, 2.0);
  nc.shCoefficients()(2) = std::complex<double>(0.0, 3.0);
  nc.shCoefficients()(3) = std::complex<double>(-1.0, 4.0);
  nc.shCoefficients()(4) = std::complex<double>(-2.0, 5.0);
  nc.setValue(5.0);
  nc.setConfidence(0.99);
  nc.setSphase(0.75);
  nc.setBorderDistanceUm(40.0);
  nc.setVolumeUm3(30000.0);
  nc.setMitotic(true);
  nc.setPredictedMitotic(true);
  nc.setSubtype(3);
  nc.setPredictedSubtype(4);
  nc.setCellfile(12);
  nc.setPredictedCellfile(13);

  SHSurfaceMarker marker;
  marker.setId("2");
  marker.setPositionUm(blitz::TinyVector<double,3>(5.0, 4.0, 2.0));
  marker.setLabel(11);
  marker.setPredictedLabel(97);
  marker.setManual(false);
  marker.setNeedsFeatureUpdate(false);
  marker.setQcDistanceUm(10.0);
  marker.setRadialDistanceUm(72.0);
  marker.setPhi(-0.7);
  marker.setRadiusUm(3.4);
  segmentation::FrequencyArray coeffs(3);
  coeffs(0) = std::complex<double>(20.0, -1.0);
  coeffs(1) = std::complex<double>(-5.0, -2.0);
  coeffs(2) = std::complex<double>(-10.0, -3.0);
  marker.setCoefficients(coeffs);
  
  marker.copyToATBNucleus(nc);
  
  LMBUNIT_ASSERT_EQUAL(marker.id(), "2");
  LMBUNIT_ASSERT_EQUAL(nc.id(), marker.id());
  LMBUNIT_ASSERT(blitz::all(marker.positionUm() == nc.positionUm()));
  LMBUNIT_ASSERT_EQUAL(nc.label(), marker.label());
  LMBUNIT_ASSERT_EQUAL(nc.predictedLabel(), marker.predictedLabel());
  LMBUNIT_ASSERT_EQUAL(nc.manual(), marker.manual());
  LMBUNIT_ASSERT_EQUAL(nc.needsFeatureUpdate(), marker.needsFeatureUpdate());
  LMBUNIT_ASSERT_EQUAL(nc.features().size(), marker.features().size());
  for (size_t i = 0; i < nc.features().size(); ++i)
      LMBUNIT_ASSERT_EQUAL(nc.features()[i], marker.features()[i]);
  LMBUNIT_ASSERT_EQUAL(
      nc.probabilityMap().size(), marker.probabilityMap().size());
  std::map<int,double>::const_iterator it1 = nc.probabilityMap().begin();
  std::map<int,double>::const_iterator it2 = marker.probabilityMap().begin();
  for (; it1 != nc.probabilityMap().end(); ++it1, ++it2)
  {
    LMBUNIT_ASSERT_EQUAL(it1->first, it2->first);
    LMBUNIT_ASSERT_EQUAL(it1->second, it2->second);
  }
  LMBUNIT_ASSERT_EQUAL(nc.qcDistanceUm(), marker.qcDistanceUm());
  LMBUNIT_ASSERT_EQUAL(nc.radialDistanceUm(), marker.radialDistanceUm());
  LMBUNIT_ASSERT_EQUAL(nc.phi(), marker.phi());
  LMBUNIT_ASSERT_EQUAL(nc.radiusUm(), marker.radiusUm());
  LMBUNIT_ASSERT_EQUAL(
      nc.shCoefficients().size(), marker.coefficients().size());
  for (size_t i = 0; i < nc.shCoefficients().size(); ++i)
      LMBUNIT_ASSERT_EQUAL(
          nc.shCoefficients()(i), marker.coefficients()(i));
}

static void testCopySphereMarkerToATBNucleus()
{
  atb::Nucleus nc;
  nc.setId("1");
  nc.setPositionUm(blitz::TinyVector<double,3>(1.0, 2.0, 3.0));
  nc.setLabel(42);
  nc.setPredictedLabel(23);
  nc.setManual(true);
  nc.setNeedsFeatureUpdate(true);
  std::vector<double> features(4);
  features[0] = 1.0;
  features[1] = 2.0;
  features[2] = 3.0;
  features[3] = 4.0;
  nc.features() = features;
  std::map<int,double> probabilities;
  probabilities[0] = 0.7;
  probabilities[1] = 0.2;
  probabilities[2] = 0.02;
  nc.probabilityMap() = probabilities;
  nc.setQcDistanceUm(100.0);
  nc.setRadialDistanceUm(50.0);
  nc.setPhi(1.2);
  nc.setRadiusUm(2.5);
  nc.shCoefficients().resize(5);
  nc.shCoefficients()(0) = std::complex<double>(2.0, 1.0);
  nc.shCoefficients()(1) = std::complex<double>(1.0, 2.0);
  nc.shCoefficients()(2) = std::complex<double>(0.0, 3.0);
  nc.shCoefficients()(3) = std::complex<double>(-1.0, 4.0);
  nc.shCoefficients()(4) = std::complex<double>(-2.0, 5.0);
  nc.setValue(5.0);
  nc.setConfidence(0.99);
  nc.setSphase(0.75);
  nc.setBorderDistanceUm(40.0);
  nc.setVolumeUm3(30000.0);
  nc.setMitotic(true);
  nc.setPredictedMitotic(true);
  nc.setSubtype(3);
  nc.setPredictedSubtype(4);
  nc.setCellfile(12);
  nc.setPredictedCellfile(13);

  SphereMarker marker;
  marker.setId("2");
  marker.setPositionUm(blitz::TinyVector<double,3>(5.0, 4.0, 2.0));
  marker.setLabel(11);
  marker.setPredictedLabel(97);
  marker.setManual(false);
  marker.setNeedsFeatureUpdate(false);
  marker.setQcDistanceUm(10.0);
  marker.setRadialDistanceUm(72.0);
  marker.setPhi(-0.7);
  marker.setRadiusUm(3.4);
  
  marker.copyToATBNucleus(nc);
  
  LMBUNIT_ASSERT_EQUAL(marker.id(), "2");
  LMBUNIT_ASSERT_EQUAL(nc.id(), marker.id());
  LMBUNIT_ASSERT(blitz::all(marker.positionUm() == nc.positionUm()));
  LMBUNIT_ASSERT_EQUAL(nc.label(), marker.label());
  LMBUNIT_ASSERT_EQUAL(nc.predictedLabel(), marker.predictedLabel());
  LMBUNIT_ASSERT_EQUAL(nc.manual(), marker.manual());
  LMBUNIT_ASSERT_EQUAL(nc.needsFeatureUpdate(), marker.needsFeatureUpdate());
  LMBUNIT_ASSERT_EQUAL(nc.features().size(), marker.features().size());
  for (size_t i = 0; i < nc.features().size(); ++i)
      LMBUNIT_ASSERT_EQUAL(nc.features()[i], marker.features()[i]);
  LMBUNIT_ASSERT_EQUAL(
      nc.probabilityMap().size(), marker.probabilityMap().size());
  std::map<int,double>::const_iterator it1 = nc.probabilityMap().begin();
  std::map<int,double>::const_iterator it2 = marker.probabilityMap().begin();
  for (; it1 != nc.probabilityMap().end(); ++it1, ++it2)
  {
    LMBUNIT_ASSERT_EQUAL(it1->first, it2->first);
    LMBUNIT_ASSERT_EQUAL(it1->second, it2->second);
  }
  LMBUNIT_ASSERT_EQUAL(nc.qcDistanceUm(), marker.qcDistanceUm());
  LMBUNIT_ASSERT_EQUAL(nc.radialDistanceUm(), marker.radialDistanceUm());
  LMBUNIT_ASSERT_EQUAL(nc.phi(), marker.phi());
  LMBUNIT_ASSERT_EQUAL(nc.radiusUm(), marker.radiusUm());
}

static void testCopyPointMarkerToATBNucleus()
{
  atb::Nucleus nc;
  nc.setId("1");
  nc.setPositionUm(blitz::TinyVector<double,3>(1.0, 2.0, 3.0));
  nc.setLabel(42);
  nc.setPredictedLabel(23);
  nc.setManual(true);
  nc.setNeedsFeatureUpdate(true);
  std::vector<double> features(4);
  features[0] = 1.0;
  features[1] = 2.0;
  features[2] = 3.0;
  features[3] = 4.0;
  nc.features() = features;
  std::map<int,double> probabilities;
  probabilities[0] = 0.7;
  probabilities[1] = 0.2;
  probabilities[2] = 0.02;
  nc.probabilityMap() = probabilities;
  nc.setQcDistanceUm(100.0);
  nc.setRadialDistanceUm(50.0);
  nc.setPhi(1.2);
  nc.setRadiusUm(2.5);
  nc.shCoefficients().resize(5);
  nc.shCoefficients()(0) = std::complex<double>(2.0, 1.0);
  nc.shCoefficients()(1) = std::complex<double>(1.0, 2.0);
  nc.shCoefficients()(2) = std::complex<double>(0.0, 3.0);
  nc.shCoefficients()(3) = std::complex<double>(-1.0, 4.0);
  nc.shCoefficients()(4) = std::complex<double>(-2.0, 5.0);
  nc.setValue(5.0);
  nc.setConfidence(0.99);
  nc.setSphase(0.75);
  nc.setBorderDistanceUm(40.0);
  nc.setVolumeUm3(30000.0);
  nc.setMitotic(true);
  nc.setPredictedMitotic(true);
  nc.setSubtype(3);
  nc.setPredictedSubtype(4);
  nc.setCellfile(12);
  nc.setPredictedCellfile(13);

  PointMarker marker;
  marker.setId("2");
  marker.setPositionUm(blitz::TinyVector<double,3>(5.0, 4.0, 2.0));
  marker.setLabel(11);
  marker.setPredictedLabel(97);
  marker.setManual(false);
  marker.setNeedsFeatureUpdate(false);
  marker.setQcDistanceUm(10.0);
  marker.setRadialDistanceUm(72.0);
  marker.setPhi(-0.7);
  
  marker.copyToATBNucleus(nc);
  
  LMBUNIT_ASSERT_EQUAL(marker.id(), "2");
  LMBUNIT_ASSERT_EQUAL(nc.id(), marker.id());
  LMBUNIT_ASSERT(blitz::all(marker.positionUm() == nc.positionUm()));
  LMBUNIT_ASSERT_EQUAL(nc.label(), marker.label());
  LMBUNIT_ASSERT_EQUAL(nc.predictedLabel(), marker.predictedLabel());
  LMBUNIT_ASSERT_EQUAL(nc.manual(), marker.manual());
  LMBUNIT_ASSERT_EQUAL(nc.needsFeatureUpdate(), marker.needsFeatureUpdate());
  LMBUNIT_ASSERT_EQUAL(nc.features().size(), marker.features().size());
  for (size_t i = 0; i < nc.features().size(); ++i)
      LMBUNIT_ASSERT_EQUAL(nc.features()[i], marker.features()[i]);
  LMBUNIT_ASSERT_EQUAL(
      nc.probabilityMap().size(), marker.probabilityMap().size());
  std::map<int,double>::const_iterator it1 = nc.probabilityMap().begin();
  std::map<int,double>::const_iterator it2 = marker.probabilityMap().begin();
  for (; it1 != nc.probabilityMap().end(); ++it1, ++it2)
  {
    LMBUNIT_ASSERT_EQUAL(it1->first, it2->first);
    LMBUNIT_ASSERT_EQUAL(it1->second, it2->second);
  }
  LMBUNIT_ASSERT_EQUAL(nc.qcDistanceUm(), marker.qcDistanceUm());
  LMBUNIT_ASSERT_EQUAL(nc.radialDistanceUm(), marker.radialDistanceUm());
  LMBUNIT_ASSERT_EQUAL(nc.phi(), marker.phi());
}

int main(int , char **)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST(testCopyNucleusMarkerFromATBNucleus());
  LMBUNIT_RUN_TEST(testCopySHSurfaceMarkerFromATBNucleus());
  LMBUNIT_RUN_TEST(testCopySphereMarkerFromATBNucleus());
  LMBUNIT_RUN_TEST(testCopyPointMarkerFromATBNucleus());

  LMBUNIT_RUN_TEST(testCopyNucleusMarkerToATBNucleus());
  LMBUNIT_RUN_TEST(testCopySHSurfaceMarkerToATBNucleus());
  LMBUNIT_RUN_TEST(testCopySphereMarkerToATBNucleus());
  LMBUNIT_RUN_TEST(testCopyPointMarkerToATBNucleus());

  LMBUNIT_WRITE_STATISTICS();
  return _nFails;
}
