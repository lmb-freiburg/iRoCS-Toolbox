#ifndef DETECTNUCLEIPARAMETERSDIALOG_HH
#define DETECTNUCLEIPARAMETERSDIALOG_HH

#include <QtGui/QDialog>

#include <liblabelling_qt4/ChannelSelectionControlElement.hh>
#include <liblabelling_qt4/FileNameSelectionControlElement.hh>
#include <liblabelling_qt4/StringControlElement.hh>
#include <liblabelling_qt4/LabellingMainWidget.hh>

#include <string>

class DetectNucleiParametersDialog : public QDialog
{

  Q_OBJECT

  public:
  
  DetectNucleiParametersDialog(
      LabellingMainWidget* mainWidget,
      QWidget* parent = NULL, Qt::WindowFlags f = NULL);
  ~DetectNucleiParametersDialog();

  ChannelSpecs* dataChannel() const;
  void setDataChannel(ChannelSpecs *channel);

  std::string annotationChannelName() const;
  void setAnnotationChannelName(std::string const &name);

  std::string cacheFileName() const;
  void setCacheFileName(std::string const &cacheFileName);

  std::string modelFileName() const;
  void setModelFileName(std::string const &modelFileName);

  ptrdiff_t memoryLimit() const;
  void setMemoryLimit(ptrdiff_t memoryLimit);

  std::string check();

public slots:
  
  void checkAndAccept();

private:
  
  LabellingMainWidget* p_mainWidget;

  ChannelSelectionControlElement* p_dataChannelSelector;
  StringControlElement* p_annotationChannelControlElement;
  FileNameSelectionControlElement* p_cacheFileControlElement;
  FileNameSelectionControlElement* p_modelFileNameControlElement;

  ptrdiff_t _memoryLimit;

};

#endif
