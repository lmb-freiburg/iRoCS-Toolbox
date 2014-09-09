/**************************************************************************
**       Title: Widget for entering or selecting a filename
**    $RCSfile$
**   $Revision: 3881 $ $Name$
**       $Date: 2008-01-18 20:53:53 +0100 (Fri, 18 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#ifndef FILENAMESELECTIONCONTROLELEMENT_HH
#define FILENAMESELECTIONCONTROLELEMENT_HH

#include "AbstractControlElement.hh"

class QLineEdit;

class FileNameSelectionControlElement : public AbstractControlElement
{

  Q_OBJECT
  
public:
  
  FileNameSelectionControlElement(
      const QString& label, const std::string& value = std::string(),
      bool onlyExistingFiles = false, const QString& filter = "",
      QWidget* parent = 0);
  ~FileNameSelectionControlElement();

  std::string value() const;
  std::string toStdString() const;

public slots:
  
  void setValue(const std::string& value);

signals:
        
  void valueChanged(QString const &filename);
  void valueChanged(std::string const &filename);

private slots:
  
  void openFileSelector();
  void emitValueChange();

private:
  
  bool _onlyExisting;
  QString _filter;
  QLineEdit* p_lineedit;
  
};

#endif
