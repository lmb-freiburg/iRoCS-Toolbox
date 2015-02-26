#ifndef EPIDERMISLABELLINGPARAMETERSDIALOG_HH
#define EPIDERMISLABELLINGPARAMETERSDIALOG_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QDialog>

#include <liblabelling_qt4/ChannelSelectionControlElement.hh>
#include <liblabelling_qt4/FileNameSelectionControlElement.hh>
#include <liblabelling_qt4/BoolControlElement.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>

#include <string>

#include <liblabelling_qt4/ChannelSpecs.hh>
#include <liblabelling_qt4/AnnotationChannelSpecs.hh>

class EpidermisLabellingParametersDialog :
    public QDialog
{

  Q_OBJECT

  public:
  
  EpidermisLabellingParametersDialog(
      LabellingMainWidget* mainWidget,
      QWidget* parent = NULL, Qt::WindowFlags f = NULL);
  ~EpidermisLabellingParametersDialog();

  ChannelSpecs *dataChannel() const;
  void setDataChannel(ChannelSpecs *channel);

  AnnotationChannelSpecs *annotationChannel() const;
  void setAnnotationChannel(AnnotationChannelSpecs *channel);

  std::string cacheFileName() const;
  void setCacheFileName(std::string const &cacheFileName);

  std::string modelFileName() const;
  void setModelFileName(std::string const &modelFileName);

  bool forceFeatureComputation() const;
  void setForceFeatureComputation(bool force);

  std::string check();

public slots:
  
  void checkAndAccept();

private:
  
  LabellingMainWidget* p_mainWidget;

  ChannelSelectionControlElement *p_dataChannelSelector;
  ChannelSelectionControlElement *p_annotationChannelSelector;
  FileNameSelectionControlElement *p_cacheFileControlElement;
  FileNameSelectionControlElement *p_modelFileNameControlElement;
  BoolControlElement *p_forceFeatureComputationControl;

};

#endif
