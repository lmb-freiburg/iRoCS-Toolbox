#ifndef ROOTSEGMENTATIONTHREAD_HH
#define ROOTSEGMENTATIONTHREAD_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtCore/QThread>

#include <liblabelling_qt4/LabellingMainWidget.hh>
#include <liblabelling_qt4/VisualizationChannelSpecs.hh>

#include <libProgressReporter/ProgressReporter.hh>

#include "RootSegmentationParametersDialog.hh"

class RootSegmentationThread : public QThread
{

  Q_OBJECT

  public:
  
  RootSegmentationThread(
      RootSegmentationParametersDialog const &parameters,
      VisualizationChannelSpecs &segmentationChannel,
      iRoCS::ProgressReporter *progress = NULL,
      LabellingMainWidget *mainWidget = NULL, QWidget *parent = NULL);
  ~RootSegmentationThread();

  void run();

signals:
  
  void error(QString const &);

private:

  void _convertChannelData(
      ChannelSpecs const *channel, atb::Array<double,3> &out);

  RootSegmentationParametersDialog const &_parameters;
  VisualizationChannelSpecs &_segmentationChannel;
  iRoCS::ProgressReporter *p_progress;
  LabellingMainWidget* p_mainWidget;

};

#endif
