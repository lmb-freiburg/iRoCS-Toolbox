#ifndef CSVDATAIO_HH
#define CSVDATAIO_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtCore/QObject>

class LabellingMainWidget;

class CSVDataIO : public QObject
{

public:
  
  static void importAnnotationChannel(LabellingMainWidget* mainWidget);
  static void exportAnnotationChannel(LabellingMainWidget* mainWidget);

};

    
#endif
