#ifndef SLCTDLG_H
#define SLCTDLG_H

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QMainWindow>
#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QScrollArea>
#include <QtGui/QDialog>

#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>

//#include "defaultParameters.h"


class QAction;
class QListWidget;
class QMenu;
class QTextEdit;
class QPushButton;
class QCheckBox;



class SelectDlg : public QDialog
{
  Q_OBJECT

public:
  SelectDlg(QWidget*, std::map<std::string, bool>&, QString = 0);

  void makeOneSelectionDialog();
  void unselectAll();

protected slots:
  virtual void accept();

  void toggleAll();

  void selectOnlyOneVar();

private:
  QPushButton *_okButton;
  QPushButton *_toggleAllButton;

  std::map<std::string, QCheckBox*> _checkBoxes;
  std::map<std::string, bool> *_chooseFields;
};

#endif
