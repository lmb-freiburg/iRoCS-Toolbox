#ifndef MITOSISCLASSIFICATIONPARAMETERSDIALOG_HH
#define MITOSISCLASSIFICATIONPARAMETERSDIALOG_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QDialog>

#include <libIRoCS/MitosisClassificationParameters.hh>

#include <liblabelling_qt4/ChannelSelectionControlElement.hh>
#include <liblabelling_qt4/FileNameSelectionControlElement.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>

#include <string>

class MitosisClassificationParametersDialog :
    public QDialog, public MitosisClassificationParameters
{

  Q_OBJECT

  public:
  
  MitosisClassificationParametersDialog(
      LabellingMainWidget* mainWidget,
      QWidget* parent = NULL, Qt::WindowFlags f = NULL);
  ~MitosisClassificationParametersDialog();

  DataChannelSpecs *dataChannel() const;
  void setDataChannel(DataChannelSpecs *channel);

  AnnotationChannelSpecs *annotationChannel() const;
  void setAnnotationChannel(AnnotationChannelSpecs *channel);

  std::string cacheFileName() const;
  void setCacheFileName(std::string const &cacheFileName);

  std::string modelFileName() const;
  void setModelFileName(std::string const &modelFileName);

public slots:
  
  void checkAndAccept();

private:
  
  LabellingMainWidget* p_mainWidget;

  ChannelSelectionControlElement *p_dataChannelSelector;
  ChannelSelectionControlElement *p_annotationChannelSelector;
  FileNameSelectionControlElement *p_cacheFileControlElement;
  FileNameSelectionControlElement *p_modelFileNameControlElement;

};

#endif
