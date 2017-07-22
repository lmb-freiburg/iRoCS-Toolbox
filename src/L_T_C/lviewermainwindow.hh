#ifndef LVIEWERMAINWINDOW_H
#define LVIEWERMAINWINDOW_H

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QMainWindow>
#include <QtGui/QScrollArea>

#include "lviewer.hh"

#include "lblitzDataExplorer.hh"

class QAction;
class QListWidget;
class QMenu;
class QTextEdit;

class lViewerMainWindow : public QMainWindow
{
Q_OBJECT

public:
lViewerMainWindow();
~lViewerMainWindow();

template<typename DataT, int Dim>
  void
  setData(blitz::Array<DataT, Dim>& data)
  {
    if (explorer != NULL)
    {
      delete explorer;
    }
    explorer = new BlitzDataExplorer(data);
    refreshImage();
  }

template<typename DataT, int Dim>
  void
  setRefData(blitz::Array<DataT, Dim>& data)
  {
    if (explorer != NULL)
    {
      if (explorerRef != NULL)
      {
        delete explorerRef;
      }
      explorerRef = new BlitzDataExplorer(data);
      blitz::TinyVector< double, 3 > pos;
      pos = explorer->getCurrentPos();
      explorerRef->setDataPos(pos(2), pos(1), pos(0));
      lvF->show((explorerRef)->getXYView());
      int x = explorerRef->getCurrentX();
      int y = explorerRef->getCurrentY();
      lvF->scrollArea->ensureVisible(x, y, 100, 100);
    }
  }

template<typename DataT, int Dim>
  void
  updateData(blitz::Array<DataT, Dim>& data)
  {
    explorer->updateData(data);
    refreshImage();
  }

template<typename DataT, int Dim>
  void
  updateData(blitz::Array<bool, Dim>& mask, blitz::TinyVector<DataT, 3>& color, blitz::RectDomain<Dim> domain)
  {
    explorer->updateData(mask, color, domain);
    refreshImage();
  }

template<typename DataT, int Dim>
  void
  updateData(blitz::Array<DataT, Dim>& image, blitz::RectDomain<Dim> domain)
  {
    explorer->updateData(image, domain);
    refreshImage();
  }

void
refreshImage();

void
alignRefImage(int dim);

void
setST(int s)
{
  if (explorer != NULL)
  explorer->setST(s);
  if (explorerRef != NULL)
  explorerRef->setST(s);
  //      LOG << s << std::endl;
}

public slots:
void
receiveWheelXY(int delta);
void
receiveWheelZY(int delta);
void
receiveWheelXZ(int delta);
void
zoom(int delta);
void
moveXY(int x, int y, int flag);
void
moveZY(int z, int y, int flag);
void
moveXZ(int x, int z, int flag);

void
pointXY(int x, int y, int flag);
void
pointZY(int z, int y, int flag);
void
pointXZ(int x, int z, int flag);

void
status(int delta);

void
open();
signals:
void
modify(int x, int y, int z, int flag);
void
query(int x, int y, int z, int flag);
void
close();
void
wheel(int);

protected:
void
showEvent(QShowEvent * event);

//        void keyPressEvent(QKeyEvent *event);
//        void mouseMoveEvent(QTimerEvent *event);
void
wheelEvent(QWheelEvent *event);
//        void mouseReleaseEvent ( QMouseEvent * event ) ;

private:

void
createActions();
void
createMenus();
void
createStatusBar();
void
createDockWindows();
void
closeEvent(QCloseEvent * event);
public:
BlitzDataExplorer * explorer;
BlitzDataExplorer * explorerRef;
public:
lViewer* lvXY;
lViewer* lvZY;
lViewer* lvXZ;
lViewer* lvF;
int oldDim;
private:
QMenu *fileMenu;
QMenu *editMenu;
QMenu *viewMenu;
QMenu *helpMenu;

QAction *newAct;
QAction *saveAct;
QAction *printAct;
QAction *undoAct;
QAction *quitAct;

QAction *openAct;

int iStatus;
};

#endif // LVIEWERMAINWINDOW_H
