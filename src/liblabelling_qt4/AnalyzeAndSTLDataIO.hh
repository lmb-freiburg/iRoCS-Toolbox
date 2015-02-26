#ifndef ANALYZEANDSTLDATAIO_HH
#define ANALYZEANDSTLDATAIO_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QDialog>

class LabellingMainWidget;
class FileNameSelectionControlElement;

class AnalyzeAndSTLDataIO
{
  
  public:

  AnalyzeAndSTLDataIO(LabellingMainWidget *mainWidget);
  ~AnalyzeAndSTLDataIO();
  
  void save(std::string const &fileNameStub) const;
  
  void save();

private:
  
  static std::string _replaceSpecialCharacters(std::string const &s);

  LabellingMainWidget *p_mainWidget;

};

#endif
