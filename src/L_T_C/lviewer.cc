#include "lviewer.hh"

#include <QtGui/QScrollArea>
#include <QtGui/QFileDialog>
#include <QtCore/QDir>
#include <QtGui/QMessageBox>
#include <QtGui/QAction>
#include <QtGui/QPrintDialog>
#include <QtGui/QPainter>
#include <QtGui/QApplication>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QScrollBar>

#include <iostream>
#include "limagelabel.hh"

lViewer::lViewer(QWidget * /*parent*/)
{
  imageLabel = new lImageLabel();
  imageLabel->setBackgroundRole(QPalette::Base);
  imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  imageLabel->setScaledContents(true);

  scrollArea = new QScrollArea;
  scrollArea->setBackgroundRole(QPalette::Dark);
  scrollArea->setWidget(imageLabel);


  scrollArea->viewport()->installEventFilter(this);
  setCentralWidget(scrollArea);

  scrollArea->setAlignment(Qt::AlignCenter);
//  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//  scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  imageLabel->setAlignment(Qt::AlignCenter);

  //  setFocusPolicy(Qt::StrongFocus);
  //
  //  QSizePolicy sp;
  //  sp.setHeightForWidth(true);
  //  imageLabel->setSizePolicy(sp);
  //  imageLabel->setScaledContents (false);

//  createActions();
//  createMenus();

  setWindowTitle(tr("Image Viewer"));
  resize(500, 400);
}

lViewer::~lViewer()
{

}

void
lViewer::open()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
      QDir::currentPath());
  if (!fileName.isEmpty())
    {
      QImage image(fileName);
      if (image.isNull())
        {
          QMessageBox::information(this, tr("Image Viewer"), tr(
              "Cannot load %1.").arg(fileName));
          return;
        }
      imageLabel->setPixmap(QPixmap::fromImage(image));
      scaleFactor = 1.0;

      printAct->setEnabled(true);
      fitToWindowAct->setEnabled(true);
      updateActions();

      if (!fitToWindowAct->isChecked())
        imageLabel->adjustSize();
    }
}

void
lViewer::show(QImage image)
{

  imageLabel->setPixmap(QPixmap::fromImage(image));
  imageLabel->update();

//  scaleFactor = 1.0;
//
//  printAct->setEnabled(true);
//  //  fitToWindowAct->setEnabled(true);
//  updateActions();
//
//  if (!fitToWindowAct->isChecked())
    imageLabel->adjustSize();

  //  scrollArea->setAlignment(Qt::AlignCenter);
}

void
lViewer::print()
{
  Q_ASSERT(imageLabel->pixmap());
  QPrintDialog dialog(&printer, this);
  if (dialog.exec())
    {
      QPainter painter(&printer);
      QRect rect = painter.viewport();
      QSize size = imageLabel->pixmap()->size();
      size.scale(rect.size(), Qt::KeepAspectRatio);
      painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
      painter.setWindow(imageLabel->pixmap()->rect());
      painter.drawPixmap(0, 0, *imageLabel->pixmap());
    }
}

void
lViewer::zoomIn()
{
  scaleImage(1.25);
}

void
lViewer::zoomOut()
{
  scaleImage(0.8);
}

void
lViewer::normalSize()
{
  imageLabel->adjustSize();
  scaleFactor = 1.0;
}

void
lViewer::fitToWindow()
{
  bool fitToWindow = fitToWindowAct->isChecked();
  scrollArea->setWidgetResizable(fitToWindow);
  if (!fitToWindow)
    {
      normalSize();
    }
  updateActions();
}

void
lViewer::about()
{
  QMessageBox::about(this, tr("About Image Viewer"), tr("Sun Nov 7, 16:06"));
}

void
lViewer::createActions()
{
  openAct = new QAction(tr("&Open..."), this);
  openAct->setShortcut(tr("Ctrl+O"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

  printAct = new QAction(tr("&Print..."), this);
  printAct->setShortcut(tr("Ctrl+P"));
  printAct->setEnabled(false);
  connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

  exitAct = new QAction(tr("E&xit"), this);
  exitAct->setShortcut(tr("Ctrl+Q"));
  connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

  zoomInAct = new QAction(tr("Zoom &In (25%)"), this);
  zoomInAct->setShortcut(tr("Ctrl++"));
  zoomInAct->setEnabled(false);
  connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

  zoomOutAct = new QAction(tr("Zoom &Out (25%)"), this);
  zoomOutAct->setShortcut(tr("Ctrl+-"));
  zoomOutAct->setEnabled(false);
  connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

  normalSizeAct = new QAction(tr("&Normal Size"), this);
  normalSizeAct->setShortcut(tr("Ctrl+S"));
  normalSizeAct->setEnabled(false);
  connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

  fitToWindowAct = new QAction(tr("&Fit to Window"), this);
  fitToWindowAct->setEnabled(false);
  fitToWindowAct->setCheckable(true);
  fitToWindowAct->setShortcut(tr("Ctrl+F"));
  connect(fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));

  aboutAct = new QAction(tr("&About"), this);
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  aboutQtAct = new QAction(tr("About &Qt"), this);
  connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void
lViewer::createMenus()
{
  fileMenu = new QMenu(tr("&File"), this);
  fileMenu->addAction(openAct);
  fileMenu->addAction(printAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

  viewMenu = new QMenu(tr("&View"), this);
  viewMenu->addAction(zoomInAct);
  viewMenu->addAction(zoomOutAct);
  viewMenu->addAction(normalSizeAct);
  viewMenu->addSeparator();
  viewMenu->addAction(fitToWindowAct);

  helpMenu = new QMenu(tr("&Help"), this);
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(aboutQtAct);

  menuBar()->addMenu(fileMenu);
  menuBar()->addMenu(viewMenu);
  menuBar()->addMenu(helpMenu);
}

void
lViewer::updateActions()
{
  zoomInAct->setEnabled(!fitToWindowAct->isChecked());
  zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
  normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}

void
lViewer::scaleImage(double factor)
{
  Q_ASSERT(imageLabel->pixmap());
  scaleFactor *= factor;
  imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

  adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
  adjustScrollBar(scrollArea->verticalScrollBar(), factor);

  zoomInAct->setEnabled(scaleFactor < 10.0);
  zoomOutAct->setEnabled(scaleFactor > 0.1);
}

void
lViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
  scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1)
      * scrollBar->pageStep() / 2)));
}
