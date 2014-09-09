#ifndef ROOTSEGMENTATIONPARAMETERSDIALOG_HH
#define ROOTSEGMENTATIONPARAMETERSDIALOG_HH

#include <QtGui/QDialog>

#include <liblabelling_qt4/ChannelSpecs.hh>

class QGroupBox;

class ChannelSelectionControlElement;
class FileNameSelectionControlElement;
class StringControlElement;
class DoubleControlElement;
class StringSelectionControlElement;
class IntControlElement;

class LabellingMainWidget;

class RootSegmentationParametersDialog : public QDialog
{

  Q_OBJECT

public:
  
  RootSegmentationParametersDialog(
      LabellingMainWidget* mainWidget,
      QWidget* parent = NULL, Qt::WindowFlags f = NULL);
  ~RootSegmentationParametersDialog();

  ChannelSpecs* dataChannel() const;
  std::string segmentationChannelName() const;
  double processingElementSizeUm() const;

  std::string debugFileName() const;

  // Pre-processing
  int normalizationType() const;
  double gamma() const;
  bool applyMedian() const;
  int medianWidthPx() const;

  // Variance normalization
  bool applyVarianceNormalization() const;
  double varSigmaUm() const;
  double epsilon() const;

  // Anisotropic diffusion
  bool applyDiffusion() const;
  double kappa() const;
  double tau() const;
  double zCompensationFactor() const;
  int nDiffusionIterations() const;

  // Watershed
  double hessianSigmaUm() const;
  double edgeThreshold() const;
  int boundaryThicknessPx() const;
  double minimumCellVolumeUm3() const;

  std::string check();

public slots:

  void setDataChannel(ChannelSpecs *channel);
  void setDataChannel(int channelIndex);
  void setSegmentationChannelName(std::string const &name);
  void setProcessingElementSizeUm(double elementSizeUm);

  void setDebugFileName(std::string const &debugFile);

  // Preprocessing
  void setNormalizationType(int normalizationType);
  void setGamma(double gamma);
  void setApplyMedian(bool applyMedian);
  void setMedianWidthPx(int widthPx);

  // Variance normalization
  void setApplyVarianceNormalization(bool varianceNormalization);
  void setVarSigmaUm(double sigmaUm);
  void setEpsilon(double epsilon);

  // Anisotropic diffusion
  void setApplyDiffusion(bool applyDiffusion);
  void setKappa(double kappa);
  void setTau(double tau);
  void setZCompensationFactor(double zCompensation);
  void setNDiffusionIterations(int nDiffusionIterations);

  // Watershed ---
  void setHessianSigmaUm(double sigmaUm);
  void setEdgeThreshold(double threshold);
  void setBoundaryThicknessPx(int boundaryThicknessPx);
  void setMinimumCellVolumeUm3(double volumeUm3);

  void checkAndAccept();

private:
  
  LabellingMainWidget *p_mainWidget;

  ChannelSelectionControlElement *p_dataChannelSelector;
  StringControlElement *p_segmentationChannelNameControl;
  DoubleControlElement *p_processingElementSizeUmControl;

  FileNameSelectionControlElement *p_debugFileNameControl;
  
  StringSelectionControlElement *p_normalizationTypeControl;
  DoubleControlElement *p_gammaControl;
  IntControlElement *p_medianWidthPxControl;

  QGroupBox *p_varianceNormalizationGroup;
  DoubleControlElement *p_varSigmaUmControl;
  DoubleControlElement *p_epsilonControl;

  QGroupBox *p_diffusionGroup;
  DoubleControlElement *p_kappaControl;
  DoubleControlElement *p_tauControl;
  DoubleControlElement *p_zCompensationFactorControl;
  IntControlElement *p_nDiffusionIterationsControl;

  DoubleControlElement *p_hessianSigmaUmControl;
  DoubleControlElement *p_edgeThresholdControl;
  IntControlElement *p_boundaryThicknessPxControl;
  DoubleControlElement *p_minimumCellVolumeUm3Control;

};

#endif
