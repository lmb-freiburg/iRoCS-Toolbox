#ifndef LVIEWER_H
#define LVIEWER_H

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QMainWindow>
#include <QtGui/QPrinter>
#include <QtCore/QEvent>
#include "limagelabel.hh"

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;

class lViewer : public QMainWindow
{
  Q_OBJECT

  public:
  lViewer(QWidget *parent = NULL);
  ~lViewer();
  void show(QImage image);

  bool eventFilter(QObject * /*obj*/, QEvent *event)
  {
    if (event->type() == QEvent::Wheel)
    {
      // ignore the event (this effectively
      // makes it "skip" one object)
      event->ignore();
      return true;
    }
    // return false to continue event propagation
    // for all events
    return false;
  }

  lImageLabel *imageLabel;
  QScrollArea *scrollArea;

  public slots:
      void open();
      void print();
      void zoomIn();
      void zoomOut();
      void normalSize();
      void fitToWindow();
      void about();

  signals:
      void wheel(int delta);

  private:
      void createActions();
      void createMenus();
      void updateActions();
      void scaleImage(double factor);
      void adjustScrollBar(QScrollBar *scrollBar, double factor);


      double scaleFactor;

      QPrinter printer;

      QAction *openAct;
      QAction *printAct;
      QAction *exitAct;
      QAction *zoomInAct;
      QAction *zoomOutAct;
      QAction *normalSizeAct;
      QAction *fitToWindowAct;
      QAction *aboutAct;
      QAction *aboutQtAct;

      QMenu *fileMenu;
      QMenu *viewMenu;
      QMenu *helpMenu;
};

#endif // LVIEWER_H
