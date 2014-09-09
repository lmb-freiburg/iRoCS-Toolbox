#ifndef TRAINDETECTORPARAMETERSDIALOG_HH
#define TRAINDETECTORPARAMETERSDIALOG_HH

#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QTreeWidget>

#include <libIRoCS/TrainingParameters.hh>

#include <liblabelling_qt4/FileNameSelectionControlElement.hh>
#include <liblabelling_qt4/HDF5SelectionControlElement.hh>
#include <liblabelling_qt4/StringControlElement.hh>
#include <liblabelling_qt4/StringSelectionControlElement.hh>
#include <liblabelling_qt4/IntControlElement.hh>
#include <liblabelling_qt4/DoubleControlElement.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>

class TrainfileParameters;

class TrainDetectorParametersDialog
    : public QDialog, public TrainingParameters
{

  Q_OBJECT

public:
  
  TrainDetectorParametersDialog(
      LabellingMainWidget* mainWidget,
      QWidget* parent = 0, Qt::WindowFlags f = 0);
  ~TrainDetectorParametersDialog();

  std::vector<TrainfileParameters*> trainFiles() const;

  void setFeatureGroup(std::string const &group);
  std::string featureGroup() const;

  void setGenerateRandomSamples(bool generate);
  bool generateRandomSamples() const;

  void setNInRootSamples(int samples);
  int nInRootSamples() const;

  void setNOutRootSamples(int samples);
  int nOutRootSamples() const;

  void setModelFileName(std::string const &name);
  std::string modelFileName() const;

  void setSdFeatureNormalization(iRoCS::Features::NormalizationType normType);
  iRoCS::Features::NormalizationType sdFeatureNormalization() const;

  void setHoughFeatureNormalization(
      iRoCS::Features::NormalizationType normType);
  iRoCS::Features::NormalizationType houghFeatureNormalization() const;

  void setCost(double cost);
  double cost() const;

  void setGamma(double gamma);
  double gamma() const;

private slots:
  
  void checkAndAccept();
  void addTrainFile();

private:

  LabellingMainWidget* p_mainWidget;

  QTreeWidget *p_trainFileList;
  QPushButton *p_addTrainFileButton;
  StringControlElement *p_featureGroupControlElement;
  QGroupBox* p_randomSamplesGroup;
  IntControlElement* p_nInRootSamplesControlElement;
  IntControlElement* p_nOutRootSamplesControlElement;
  FileNameSelectionControlElement* p_modelFileNameControlElement;
  StringSelectionControlElement *p_sdNormalizationControl;
  StringSelectionControlElement *p_houghNormalizationControl;
  DoubleControlElement* p_costControlElement;
  DoubleControlElement* p_gammaControlElement;
  
};

#endif
