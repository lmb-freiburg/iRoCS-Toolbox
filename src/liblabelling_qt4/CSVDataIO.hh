#ifndef CSVDATAIO_HH
#define CSVDATAIO_HH

#include <QtCore/QObject>

class LabellingMainWidget;

class CSVDataIO : public QObject
{

public:
  
  static void importAnnotationChannel(LabellingMainWidget* mainWidget);
  static void exportAnnotationChannel(LabellingMainWidget* mainWidget);

};

    
#endif
