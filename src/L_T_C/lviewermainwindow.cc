#include "lviewermainwindow.hh"
#include "lviewer.hh"
#include <iostream>

#include <QtCore/QSettings>
#include <QtGui/QDockWidget>
#include <QtGui/QAction>
#include <QtGui/QFileDialog>
#include <QtCore/QDir>
#include <QtGui/QCloseEvent>
#include <QtGui/QStatusBar>

lViewerMainWindow::lViewerMainWindow()
{
  explorer = NULL;explorerRef= NULL;
  createActions();
//  createMenus();
  createDockWindows();
//  createStatusBar();

  iStatus = 0;

//  setWindowTitle(tr("CS_Viewer"));

  //  readSettings();
  QSettings settings("LMB", "CS_Viewer");
  restoreGeometry(settings.value("geometry").toByteArray());

//  setFocusPolicy(Qt::WheelFocus);
  //  setFocus();
}

lViewerMainWindow::~lViewerMainWindow()
{

}

void
lViewerMainWindow::createMenus()
{
}

void
lViewerMainWindow::createDockWindows()
{
  QDockWidget *dock = new QDockWidget(tr("XY"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::TopDockWidgetArea);
  lvXY = new lViewer();
  dock->setWidget(lvXY);
  addDockWidget(Qt::LeftDockWidgetArea, dock);

  connect(lvXY->imageLabel, SIGNAL(wheel(int)), this, SLOT(receiveWheelXY(int)));
//  connect(lvXY->imageLabel, SIGNAL(zoom(int)), this, SLOT(zoom(int)));
  connect(lvXY->imageLabel, SIGNAL(status(int)), this, SLOT(status(int)));
  connect(lvXY->imageLabel, SIGNAL(move(int, int, int)), this, SLOT(moveXY(int, int, int)));
  connect(lvXY->imageLabel, SIGNAL(point(int, int, int)), this, SLOT(pointXY(int, int, int)));

  dock = new QDockWidget(tr("ZY"), this);
  dock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::TopDockWidgetArea);
  lvZY = new lViewer();
  dock->setWidget(lvZY);
  addDockWidget(Qt::RightDockWidgetArea, dock);

  connect(lvZY->imageLabel, SIGNAL(wheel(int)), this, SLOT(receiveWheelZY(int)));
//  connect(lvZY->imageLabel, SIGNAL(zoom(int)), this, SLOT(zoom(int)));
  connect(lvZY->imageLabel, SIGNAL(status(int)), this, SLOT(status(int)));
  connect(lvZY->imageLabel, SIGNAL(move(int, int, int)), this, SLOT(moveZY(int, int, int)));
  connect(lvZY->imageLabel, SIGNAL(point(int, int, int)), this, SLOT(pointZY(int, int, int)));

  dock = new QDockWidget(tr("XZ"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::BottomDockWidgetArea);
  lvXZ = new lViewer();
  dock->setWidget(lvXZ);
  addDockWidget(Qt::LeftDockWidgetArea, dock);

  connect(lvXZ->imageLabel, SIGNAL(wheel(int)), this, SLOT(receiveWheelXZ(int)));
//  connect(lvXZ->imageLabel, SIGNAL(zoom(int)), this, SLOT(zoom(int)));
  connect(lvXZ->imageLabel, SIGNAL(status(int)), this, SLOT(status(int)));
  connect(lvXZ->imageLabel, SIGNAL(move(int, int, int)), this, SLOT(moveXZ(int, int, int)));
  connect(lvXZ->imageLabel, SIGNAL(point(int, int, int)), this, SLOT(pointXZ(int, int, int)));

  dock = new QDockWidget(tr("Reference"), this);
  dock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
  lvF = new lViewer();
  dock->setWidget(lvF);
  addDockWidget(Qt::RightDockWidgetArea, dock);
  connect(lvF->imageLabel, SIGNAL(wheel(int)), this, SLOT(receiveWheelXZ(int)));
}

void
lViewerMainWindow::createActions()
{
  openAct = new QAction(tr("&Open..."), this);
  openAct->setShortcut(tr("Ctrl+O"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
}

void
lViewerMainWindow::open()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());
  if (!fileName.isEmpty())
    {
    }
}
void
lViewerMainWindow::closeEvent(QCloseEvent * event)
{
  //  writeSettings();
  QSettings settings("LMB", "CTA_QT_Viewer");
  settings.setValue("geometry", saveGeometry());

  event->accept();
  emit close();
}

void
lViewerMainWindow::refreshImage()
{
  //  std::cerr << "refreshImage" << std::endl << std::flush;
  //  std::cerr << qRed((pq->pixel(0,0))) - qGreen(pq->pixel(0,0)) <<":" << qRed((pq->pixel(5,0))) - qGreen(pq->pixel(5,0)) << std::endl;
  lvXY->show((explorer)->getXYView());

  lvZY->show((explorer)->getZYView());
  lvXZ->show((explorer)->getXZView());

  int x = explorer->getCurrentX();
  int y = explorer->getCurrentY();
  int z = explorer->getCurrentZ();

  lvXY->scrollArea->ensureVisible(x, y, 100, 100);
  lvZY->scrollArea->ensureVisible(z, y, 100, 100);
  lvXZ->scrollArea->ensureVisible(x, z, 100, 100);
}

void
lViewerMainWindow::alignRefImage(int dim)
{
  if (explorerRef!= NULL)
  {
    blitz::TinyVector< double, 3 > pos;
    pos = explorer->getCurrentPos();
    explorerRef->setDataPos(pos(2), pos(1), pos(0));
    int x = explorerRef->getCurrentX();
    int y = explorerRef->getCurrentY();
    int z = explorerRef->getCurrentZ();

    switch(dim)
    {
    case 0:
      lvF->show((explorerRef)->getXYView());
      lvF->scrollArea->ensureVisible(x, y, 100, 100);
      break;
    case 2:
      lvF->show((explorerRef)->getZYView());
      lvF->scrollArea->ensureVisible(z, y, 100, 100);
      break;
    case 1:
      lvF->show((explorerRef)->getXZView());
      lvF->scrollArea->ensureVisible(x, z, 100, 100);
      break;
    default:
      break;
    }
  }
  oldDim = dim;
}

void
lViewerMainWindow::receiveWheelXY(int delta)
{
  //  std::cerr << "receive wheel " << delta << std::endl;
  if (delta > 0)
    explorer->move(0, 0, delta / 120);
  else
    explorer->move(0, 0, delta / 120);
  refreshImage();
  alignRefImage(0);
}

void
lViewerMainWindow::receiveWheelXZ(int delta)
{
  //  std::cerr << "receive wheel " << delta << std::endl;
  if (delta > 0)
    explorer->move(0, delta / 120, 0);
  else
    explorer->move(0, delta / 120, 0);
  refreshImage();
  alignRefImage(1);
}

void
lViewerMainWindow::receiveWheelZY(int delta)
{
  //  std::cerr << "receive wheel " << delta << std::endl;
  if (delta > 0)
    explorer->move(delta / 120, 0, 0);
  else
    explorer->move(delta / 120, 0, 0);
  refreshImage();
  alignRefImage(2);
}

void
lViewerMainWindow::zoom(int delta)
{
  if (delta > 0)
    explorer->zoom(0.1 * delta / 100);
  else
    explorer->zoom(0.1 * delta / 100);
  refreshImage();
}

void
lViewerMainWindow::status(int delta)
{
//  iStatus += (delta / 120);
//  statusBar()->showMessage(QString("%1").arg(iStatus));

  emit wheel(delta);
}

void
lViewerMainWindow::showEvent(QShowEvent * /*event*/)
{
  QSettings settings("LMB", "CTA_QT_Viewer");
  restoreGeometry(settings.value("geometry").toByteArray());
}

void
lViewerMainWindow::moveXY(int x, int y, int flag)
{
  blitz::TinyVector<double, 6> data = explorer->getDataOnPoint(x, y, -1);
  if (flag == 0)
    {
      QString s = QString("Pos : %1,%2,%3    Data : %4,%5,%6").arg(data(0)).arg(data(1)).arg(data(2)).arg(data(3)).arg(
          data(4)).arg(data(5));
      statusBar()->showMessage(s);
      emit query(data(0), data(1), data(2), flag);
    }
  else if (flag == 1)
    {
      emit modify(data(0), data(1), data(2), 2);
//      LOG <<"emit modify 2" << std::endl;
    }
}
void
lViewerMainWindow::moveZY(int z, int y, int flag)
{
  blitz::TinyVector<double, 6> data = explorer->getDataOnPoint(-1, y, z);
  if (flag == 0)
    {
      QString s = QString("Pos : %1,%2,%3    Data : %4,%5,%6").arg(data(0)).arg(data(1)).arg(data(2)).arg(data(3)).arg(
          data(4)).arg(data(5));
      statusBar()->showMessage(s);
      emit query(data(0), data(1), data(2), flag);
    }
  else if (flag == 1)
    {
      emit modify(data(0), data(1), data(2), 2);
    }
}
void
lViewerMainWindow::moveXZ(int x, int z, int flag)
{
  blitz::TinyVector<double, 6> data = explorer->getDataOnPoint(x, -1, z);
  if (flag == 0)
    {
      QString s = QString("Pos : %1,%2,%3    Data : %4,%5,%6").arg(data(0)).arg(data(1)).arg(data(2)).arg(data(3)).arg(
          data(4)).arg(data(5));
      statusBar()->showMessage(s);
      emit query(data(0), data(1), data(2), flag);
    }
  else if (flag == 1)
    {
      emit modify(data(0), data(1), data(2), 2);
    }
}

void
lViewerMainWindow::createStatusBar()
{
  statusBar()->showMessage(tr("Ready"));
}

void
lViewerMainWindow::pointXY(int x, int y, int flag)
{
  if (flag == 0)
    {
      explorer->setPos(x, y, -1);
      refreshImage();
      alignRefImage(0);
    }
  else if (flag >= 1)
    {
      blitz::TinyVector<double, 6> data = explorer->getDataOnPoint(x, y, -1);
      emit modify(data(0), data(1), data(2), flag);
    }
}
void
lViewerMainWindow::pointZY(int z, int y, int flag)
{
  if (flag == 0)
    {
      explorer->setPos(-1, y, z);
      refreshImage();
      alignRefImage(2);
    }
  else if (flag >= 1)
    {
      blitz::TinyVector<double, 6> data = explorer->getDataOnPoint(-1, y, z);
      emit modify(data(0), data(1), data(2), flag);

    }
}
void
lViewerMainWindow::pointXZ(int x, int z, int flag)
{
  if (flag == 0)
    {
      explorer->setPos(x, -1, z);
      refreshImage();
      alignRefImage(1);
    }
  else if (flag >= 1)
    {
      blitz::TinyVector<double, 6> data = explorer->getDataOnPoint(x, -1, z);
      emit modify(data(0), data(1), data(2), flag);
    }
}

void
lViewerMainWindow::wheelEvent(QWheelEvent *event)
{

  if (event->modifiers() == Qt::ControlModifier)
    {
      int delta = event->delta();
      //      std::cerr << focusedView << ":" << event->pos ().x() << "," << event->pos().y() << std::endl;
      zoom(delta);
    }
  if (event->modifiers() == Qt::ControlModifier && explorerRef != NULL)
    {
      int delta = event->delta();
      //      std::cerr << focusedView << ":" << event->pos ().x() << "," << event->pos().y() << std::endl;

      explorerRef->zoom(0.1 * delta / 100);
      alignRefImage(oldDim);
    }
  QWidget::wheelEvent(event);
}
