/**************************************************************************
 *
 * This file belongs to the iRoCS Toolbox.
 *
 * Copyright (C) 2015 Thorsten Falk
 *
 *        Image Analysis Lab, University of Freiburg, Germany
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **************************************************************************/

#include "OrthoViewWidget.hh"

#include <QtCore/QSettings>
#include <QtGui/QScrollBar>
#include <QtGui/QBoxLayout>
#include <QtGui/QScrollArea>
#include <QtGui/QSplitter>
#include <QtGui/QComboBox>
#include <QtGui/QToolBar>
#include <QtGui/QLineEdit>
#include <QtGui/QAction>
#include <QtGui/QPainter>
#include <QtXml/QXmlStreamWriter>

#include "MultiChannelModel.hh"
#include "OrthoViewPlane.hh"
#include "AnnotationChannelSpecs.hh"
#include "DataChannelSpecsOrthoViewRenderer.hh"
#include "AnnotationChannelSpecsOrthoViewRenderer.hh"
#include "MarkerOrthoViewRenderer.hh"
#include "Double3ControlElement.hh"
#include "IntControlElement.hh"

OrthoViewWidget::OrthoViewWidget(
    MultiChannelModel* model, QWidget* parent, Qt::WindowFlags f)
        : ViewWidget(model, parent, f), _cachedPosition(-1.0),
          _interpolation(false), _zoom(1.0)
{
  QVBoxLayout* mainLayout = new QVBoxLayout;

  // Create Tool bar
  QToolBar *toolbar = new QToolBar;
  toolbar->setFloatable(false);
  toolbar->setMovable(false);

  p_resetZoomAction = new QAction(
      QIcon::fromTheme(
          "zoom-original", QIcon(":/zoom-original.png")), "", this);
  p_resetZoomAction->setShortcut(Qt::CTRL + Qt::Key_1);
  p_resetZoomAction->setToolTip(tr("Reset zoom to 100%"));
  connect(p_resetZoomAction, SIGNAL(triggered()), SLOT(resetZoom()));
  toolbar->addAction(p_resetZoomAction);

  p_zoomOutAction = new QAction(
      QIcon::fromTheme(
          "zoom-out", QIcon(":/zoom-out.png")), "", this);
  p_zoomOutAction->setShortcut(QKeySequence::ZoomOut);
  p_zoomOutAction->setToolTip(tr("Zoom out by one step"));
  connect(p_zoomOutAction, SIGNAL(triggered()), SLOT(zoomOut()));
  toolbar->addAction(p_zoomOutAction);

  QStringList standardZooms;
  standardZooms << "1" << "2" << "5" << "10" << "25" << "50"
                << "75" << "100" << "150" << "200" << "400" << "1000"
                << "5000" << "10000";
  p_zoomControl = new QComboBox;
  p_zoomControl->addItems(standardZooms);
  p_zoomControl->setCurrentIndex(7);
  p_zoomControl->setEditable(true);
  p_zoomControl->lineEdit()->setValidator(new QIntValidator(1, 10000, NULL));
  connect(p_zoomControl, SIGNAL(currentIndexChanged(int)),
          SLOT(updateZoomFromComboBox()));
  connect(p_zoomControl, SIGNAL(editTextChanged(QString const &)),
          SLOT(updateZoomFromComboBox(QString const &)));
  connect(this, SIGNAL(zoomChanged(double)), SLOT(setZoomInComboBox(double)));
  toolbar->addWidget(p_zoomControl);

  p_zoomInAction = new QAction(
      QIcon::fromTheme(
          "zoom-in", QIcon(":/zoom-in.png")), "", this);
  p_zoomInAction->setShortcut(QKeySequence::ZoomIn);
  p_zoomInAction->setToolTip(tr("Zoom in by one step"));
  connect(p_zoomInAction, SIGNAL(triggered()), SLOT(zoomIn()));
  toolbar->addAction(p_zoomInAction);

  // Position indicator/selector
  p_crosshairPositionControl = new Double3ControlElement(
      tr("Pos:"), blitz::TinyVector<double,3>(0.0));
  setPositionUmControlRangeFromModel();
  connect(p_crosshairPositionControl, SIGNAL(valueChanged()),
          SLOT(updatePositionUmFromControl()));
  connect(p_model, SIGNAL(boundingBoxChanged(
                              blitz::TinyVector<double,3> const &,
                              blitz::TinyVector<double,3> const &)),
          SLOT(setPositionUmControlRangeFromModel()));
  connect(p_model, SIGNAL(boundingBoxChanged(
                              blitz::TinyVector<double,3> const &,
                              blitz::TinyVector<double,3> const &)),
          SLOT(updateShape()));
  connect(p_model, SIGNAL(
              elementSizeChanged(const blitz::TinyVector<double,3>&)),
          SLOT(setPositionUmControlRangeFromModel()));
  connect(this, SIGNAL(positionChanged(const blitz::TinyVector<double,3>&)),
          SLOT(setPositionUmInControl(const blitz::TinyVector<double,3>&)));
  toolbar->addWidget(p_crosshairPositionControl);

  QSettings settings;
  p_fontSizeControl = new IntControlElement(
      "Font size", settings.value("Orthoview/Fontsize", 5).toInt());
  p_fontSizeControl->setRange(1, 1000);
  connect(p_fontSizeControl, SIGNAL(valueChanged()),
          SLOT(updateCacheFontSize()));
  toolbar->addWidget(p_fontSizeControl);

  mainLayout->addWidget(toolbar);

  // Prepare main orthoview
  p_vSplitter = new QSplitter(Qt::Vertical);
  p_uhSplitter = new QSplitter(Qt::Horizontal);
  p_lhSplitter = new QSplitter(Qt::Horizontal);

  p_xyView = new OrthoViewPlane(this, 0);
  p_xyScrollArea = new QScrollArea;
  p_xyScrollArea->setWidget(p_xyView);
  p_uhSplitter->addWidget(p_xyScrollArea);
  connect(this, SIGNAL(zoomChanged(double)), p_xyView, SLOT(adjustSize()));

  p_xzView = new OrthoViewPlane(this, 1);
  p_xzScrollArea = new QScrollArea;
  p_xzScrollArea->setWidget(p_xzView);
  p_lhSplitter->addWidget(p_xzScrollArea);
  connect(this, SIGNAL(zoomChanged(double)), p_xzView, SLOT(adjustSize()));

  p_zyView = new OrthoViewPlane(this, 2);
  p_zyScrollArea = new QScrollArea;
  p_zyScrollArea->setWidget(p_zyView);
  p_uhSplitter->addWidget(p_zyScrollArea);
  connect(this, SIGNAL(zoomChanged(double)), p_zyView, SLOT(adjustSize()));

  QWidget* dummyWidget = new QWidget;
  p_lhSplitter->addWidget(dummyWidget);
  p_vSplitter->addWidget(p_uhSplitter);
  p_vSplitter->addWidget(p_lhSplitter);

  // Load old splitter settings
  p_vSplitter->restoreState(settings.value("VSplitter/state").toByteArray());
  p_uhSplitter->restoreState(settings.value("HSplitter/state").toByteArray());
  p_lhSplitter->restoreState(settings.value("HSplitter/state").toByteArray());

  // Synchronize QSplitters
  connect(p_lhSplitter, SIGNAL(splitterMoved(int, int)),
          SLOT(synchronizeSplitters(int, int)));
  connect(p_uhSplitter, SIGNAL(splitterMoved(int, int)),
          SLOT(synchronizeSplitters(int, int)));

  // Autoscroll scroll bars
  connect(p_xyScrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)),
          p_zyScrollArea->verticalScrollBar(), SLOT(setValue(int)));
  connect(p_xyScrollArea->horizontalScrollBar(), SIGNAL(valueChanged(int)),
          p_xzScrollArea->horizontalScrollBar(), SLOT(setValue(int)));
  connect(p_xzScrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)),
          p_zyScrollArea->horizontalScrollBar(), SLOT(setValue(int)));
  connect(p_xzScrollArea->horizontalScrollBar(), SIGNAL(valueChanged(int)),
          p_xyScrollArea->horizontalScrollBar(), SLOT(setValue(int)));
  connect(p_zyScrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)),
          p_xyScrollArea->verticalScrollBar(), SLOT(setValue(int)));
  connect(p_zyScrollArea->horizontalScrollBar(), SIGNAL(valueChanged(int)),
          p_xzScrollArea->verticalScrollBar(), SLOT(setValue(int)));

  mainLayout->addWidget(p_vSplitter);
  setLayout(mainLayout);

  setPositionUm(
      blitz::TinyVector<float,3>(
          (p_model->lowerBoundUm() + p_model->upperBoundUm()) / 2.0));
  p_xyScrollArea->setAlignment(Qt::AlignCenter);
  p_xzScrollArea->setAlignment(Qt::AlignCenter);
  p_zyScrollArea->setAlignment(Qt::AlignCenter);

  p_model->addView(this);
}

OrthoViewWidget::~OrthoViewWidget()
{
  // Unregister view from model
  p_model->removeView(this);

  // Save splitter settings
  QSettings settings;
  settings.setValue("VSplitter/state", p_vSplitter->saveState());
  settings.setValue("HSplitter/state", p_uhSplitter->saveState());
  settings.setValue("Orthoview/Fontsize", p_fontSizeControl->value());
}

OrthoViewPlane* OrthoViewWidget::xyView()
{
  return p_xyView;
}

OrthoViewPlane* OrthoViewWidget::xzView()
{
  return p_xzView;
}

OrthoViewPlane* OrthoViewWidget::zyView()
{
  return p_zyView;
}

OrthoViewPlane* OrthoViewWidget::orthoViewPlane(int orthogonalDimension)
{
  if (p_xyView->orthogonalDimension() == orthogonalDimension) return p_xyView;
  if (p_xzView->orthogonalDimension() == orthogonalDimension) return p_xzView;
  if (p_zyView->orthogonalDimension() == orthogonalDimension) return p_zyView;
  std::cerr << __FILE__ << ":" << __LINE__
            << " error: the given orthogonal dimension " << orthogonalDimension
            << " is invalid" << std::endl;
  exit(-1);
  return NULL;
}

ViewWidget::ViewType OrthoViewWidget::viewType() const
{
  return ViewWidget::OrthoView;
}

void OrthoViewWidget::addChannel(ChannelSpecs*)
{
  // Dummy, does nothing
}

void OrthoViewWidget::removeChannel(ChannelSpecs*)
{
  // Update plane data
  for (int d = 0; d < 3; ++d) updatePlaneData(NULL, d);
}

bool OrthoViewWidget::interpolation() const
{
  return _interpolation;
}

double OrthoViewWidget::zoom() const
{
  return _zoom;
}

int OrthoViewWidget::fontSize() const
{
  return p_fontSizeControl->value();
}

blitz::TinyVector<float,3> OrthoViewWidget::um2Px(
    blitz::TinyVector<float,3> const &positionUm)
{
  if (p_model->elementSizeUm()(2) == std::numeric_limits<double>::infinity())
      return 0.0f;
  return ((positionUm - p_model->lowerBoundUm()) /
          p_model->elementSizeUm()(2) + 0.5 * p_model->elementSizeUm() /
          p_model->elementSizeUm()(2)) * zoom();
}

double OrthoViewWidget::scaleToPx(double distanceUm)
{
  if (p_model->elementSizeUm()(2) == std::numeric_limits<double>::infinity())
      return 0.0;
  return distanceUm / p_model->elementSizeUm()(2) * zoom();
}

double OrthoViewWidget::um2Px(double positionUm, int dimension)
{
  if (p_model->elementSizeUm()(2) == std::numeric_limits<double>::infinity())
      return 0.0;
  return ((positionUm - p_model->lowerBoundUm()(dimension)) /
          p_model->elementSizeUm()(2) +
          0.5 * p_model->elementSizeUm()(dimension) /
          p_model->elementSizeUm()(2)) * zoom();
}

blitz::TinyVector<float,3> OrthoViewWidget::px2Um(
    blitz::TinyVector<float,3> const &positionPx)
{
  if (p_model->elementSizeUm()(2) == std::numeric_limits<double>::infinity())
      return 0.0f;
  return (positionPx / zoom() - 0.5 * p_model->elementSizeUm() /
          p_model->elementSizeUm()(2)) * p_model->elementSizeUm()(2) +
      p_model->lowerBoundUm();
}

double OrthoViewWidget::scaleToUm(double distancePx)
{
  if (p_model->elementSizeUm()(2) == std::numeric_limits<double>::infinity())
      return 0.0;
  return distancePx / zoom() * p_model->elementSizeUm()(2);
}

double OrthoViewWidget::px2Um(double positionPx, int dimension)
{
  if (p_model->elementSizeUm()(2) == std::numeric_limits<double>::infinity())
      return 0.0;
  return (positionPx / zoom() - 0.5 * p_model->elementSizeUm()(dimension) /
          p_model->elementSizeUm()(2)) * p_model->elementSizeUm()(2) +
      p_model->lowerBoundUm()(dimension);
}

void OrthoViewWidget::paint(QPainter &painter)
{
  p_xyView->paint(painter);
  p_xzView->paint(painter);
  p_zyView->paint(painter);
}

void OrthoViewWidget::paint(
    QXmlStreamWriter &svgStream,
    blitz::TinyVector<double,3> const &shapePx, double marginPx,
    blitz::TinyVector<double,3> const &lowerBoundUm,
    blitz::TinyVector<double,3> const &upperBoundUm,
    double scaleBarLengthUm, std::string const &font, double fontSizePt,
    iRoCS::ProgressReporter *pr)
{
  if (p_model == NULL)
  {
    if (pr != NULL) pr->abort();
    return;
  }
  double um2PxFactor = shapePx(2) / (upperBoundUm(2) - lowerBoundUm(2));
  double pt2PxFactor = 1.25;

  int pMin = (pr != NULL) ? pr->taskProgressMin() : 0;
  int pScale = (pr != NULL) ? pr->taskProgressMax() - pMin : 100;

  if (pr != NULL)
  {
    pr->setTaskProgressMax(static_cast<int>(pMin + 0.33 * pScale));
    if (!pr->updateProgressMessage("Saving xy-view")) return;
  }

  svgStream.writeStartElement("g");
  svgStream.writeAttribute("id", "xy-view");
  p_xyView->paint(
      svgStream, shapePx, lowerBoundUm, upperBoundUm, font, fontSizePt, pr);
  svgStream.writeStartElement("g");
  svgStream.writeAttribute("id", "scale bar");
  svgStream.writeEmptyElement("path");
  svgStream.writeAttribute(
      "d", "M" + QString::number(marginPx) + " " +
      QString::number(shapePx(1) - 2.0 * marginPx) + " h" +
      QString::number(scaleBarLengthUm * um2PxFactor));
  svgStream.writeAttribute("style", "stroke:white;stroke-width:3pt");
  svgStream.writeStartElement("text");
  svgStream.writeAttribute(
      "x", QString::number(0.5 * scaleBarLengthUm * um2PxFactor + marginPx));
  svgStream.writeAttribute(
      "y", QString::number(shapePx(1) - marginPx - fontSizePt * pt2PxFactor));
  svgStream.writeAttribute(
      "style", "font-size:" + QString::number(fontSizePt) + "pt;font-family:" +
      font.c_str() + ";stroke:none;fill:white;text-anchor:middle");
  svgStream.writeCharacters(
      QString::number(scaleBarLengthUm) + " " + QChar(0x03bc) + "m");
  svgStream.writeEndElement(); // text
  svgStream.writeEndElement(); // g id="scale bar"
  svgStream.writeEndElement(); // g id="xy-view"
  if (pr != NULL && pr->isAborted()) return;

  if (pr != NULL)
  {
    pr->setTaskProgressMin(static_cast<int>(pMin + 0.33 * pScale));
    pr->setTaskProgressMax(static_cast<int>(pMin + 0.67 * pScale));
    if (!pr->updateProgressMessage("Saving xz-view")) return;
  }

  svgStream.writeStartElement("g");
  svgStream.writeAttribute("id", "xz-view");
  svgStream.writeAttribute(
      "transform", "translate(0," +
      QString::number(shapePx(1) + marginPx) + ")");
  p_xzView->paint(
      svgStream, shapePx, lowerBoundUm, upperBoundUm, font, fontSizePt, pr);
  svgStream.writeEndElement(); // g id="xz-view"
  if (pr != NULL && pr->isAborted()) return;

  if (pr != NULL)
  {
    pr->setTaskProgressMin(static_cast<int>(pMin + 0.67 * pScale));
    pr->setTaskProgressMax(static_cast<int>(pMin + pScale));
    if (!pr->updateProgressMessage("Saving zy-view")) return;
  }

  svgStream.writeStartElement("g");
  svgStream.writeAttribute("id", "zy-view");
  svgStream.writeAttribute(
      "transform", "translate(" +
      QString::number(shapePx(2) + marginPx) + ",0)");
  p_zyView->paint(
      svgStream, shapePx, lowerBoundUm, upperBoundUm, font, fontSizePt, pr);
  svgStream.writeEndElement(); // g id="zy-view"
  if (pr != NULL && pr->isAborted()) return;

  // Add a clipping polygon
  svgStream.writeEmptyElement("path");
  svgStream.writeAttribute("id", "clipping polygon");
  svgStream.writeAttribute(
      "d", "M0 " + QString::number(shapePx(1)) + " v" +
      QString::number(marginPx) + " h" + QString::number(shapePx(2)) + " v" +
      QString::number(shapePx(0)) + " h" +
      QString::number(marginPx + shapePx(0)) + " v" +
      QString::number(-shapePx(0) - marginPx) + " h" +
      QString::number(-shapePx(0)) + " v" + QString::number(-shapePx(1)) +
      " h" + QString::number(-marginPx) + " v" + QString::number(shapePx(1)) +
      " h" + QString::number(-shapePx(2)));
  svgStream.writeAttribute("style", "stroke:none;fill:#ffffff");
}

void OrthoViewWidget::redraw()
{
#ifdef DEBUG
  std::cerr << "OrthoViewWidget::redraw()" << std::endl;
#endif
  p_xyView->update();
  p_xzView->update();
  p_zyView->update();
}

void OrthoViewWidget::updateShape()
{
  blitz::TinyVector<int,3> crosshairPosPx(
      um2Px(p_crosshairPositionControl->value()));
  p_xyView->adjustSize();
  p_xzView->adjustSize();
  p_zyView->adjustSize();
  p_xyScrollArea->setAlignment(Qt::AlignCenter);
  p_xyScrollArea->ensureVisible(
      crosshairPosPx(p_xyView->dimensions()(0)),
      crosshairPosPx(p_xyView->dimensions()(1)));
  p_xzScrollArea->setAlignment(Qt::AlignCenter);
  p_xzScrollArea->ensureVisible(
      crosshairPosPx(p_xzView->dimensions()(0)),
      crosshairPosPx(p_xzView->dimensions()(1)));
  p_zyScrollArea->setAlignment(Qt::AlignCenter);
  p_zyScrollArea->ensureVisible(
      crosshairPosPx(p_zyView->dimensions()(0)),
      crosshairPosPx(p_zyView->dimensions()(1)));
  p_lhSplitter->setSizes(p_uhSplitter->sizes());
}

void OrthoViewWidget::setInterpolation(bool interpolation)
{
  _interpolation = interpolation;
}

void OrthoViewWidget::setZoom(double zoom)
{
  if (zoom == _zoom) return;
  _zoom = zoom;
  emit zoomChanged(zoom);
}

void OrthoViewWidget::updatePlaneData(
    ChannelSpecsOrthoViewRenderer const *, int direction)
{
  orthoViewPlane(direction)->updateData();
}

void OrthoViewWidget::resetZoom()
{
  setZoom(1.0);
}

void OrthoViewWidget::zoomIn()
{
  double targetZoom = 0.0;
  int i = 0;
  while (i < p_zoomControl->count() && targetZoom <= zoom())
  {
    targetZoom = p_zoomControl->itemText(i).toDouble() / 100.0;
    i++;
  }
  if (i < p_zoomControl->count()) setZoom(targetZoom);
}

void OrthoViewWidget::zoomOut()
{
  double targetZoom = std::numeric_limits<double>::infinity();
  int i = p_zoomControl->count() - 1;
  while (i >= 0 && targetZoom >= zoom())
  {
    targetZoom = p_zoomControl->itemText(i).toDouble() / 100.0;
    i--;
  }
  if (i >= 0) setZoom(targetZoom);
}

void OrthoViewWidget::updateZoomFromComboBox()
{
  double requestedZoom = p_zoomControl->currentText().toDouble() / 100.0;
  if (_zoom == requestedZoom) return;
  setZoom(requestedZoom);
}

void OrthoViewWidget::updateZoomFromComboBox(QString const &str)
{
  double requestedZoom = str.toDouble() / 100.0;
  if (_zoom == requestedZoom) return;
  setZoom(requestedZoom);
}

void OrthoViewWidget::setZoomInComboBox(double zoom)
{
  QString requestedText;
  requestedText.setNum(static_cast<int>(100.0 * zoom));
  if (requestedText == p_zoomControl->currentText()) return;
  p_zoomControl->setEditText(requestedText);
}

void OrthoViewWidget::updatePositionUmFromControl()
{
  if (blitz::all(positionUm() == p_crosshairPositionControl->value())) return;
  setPositionUm(p_crosshairPositionControl->value());
}

void OrthoViewWidget::setPositionUmControlRangeFromModel()
{
  p_crosshairPositionControl->setRange(
      p_model->lowerBoundUm(), p_model->upperBoundUm());
  if (blitz::all(p_model->elementSizeUm() > 0.0))
      p_crosshairPositionControl->setSingleStep(p_model->elementSizeUm());
  else
      p_crosshairPositionControl->setSingleStep(
          blitz::TinyVector<double,3>(1.0));
}

void OrthoViewWidget::setPositionUmInControl(
    blitz::TinyVector<double,3> const &positionUm)
{
  if (blitz::all(positionUm == p_crosshairPositionControl->value())) return;
  p_crosshairPositionControl->setValue(positionUm);
}

void OrthoViewWidget::synchronizeSplitters(int pos, int)
{
  int maxSpace = p_lhSplitter->sizes()[0] + p_lhSplitter->sizes()[1];
  QList<int> sizes;
  sizes.push_back(pos);
  sizes.push_back(maxSpace - pos);
  p_lhSplitter->setSizes(sizes);
  p_uhSplitter->setSizes(sizes);
}

void OrthoViewWidget::updateCacheFontSize()
{
  for (std::vector<ChannelSpecs*>::iterator it = p_model->begin();
       it != p_model->end(); ++it)
  {
    if ((*it)->channelType() == ChannelSpecs::Annotation)
    {
      AnnotationChannelSpecs *channel =
          static_cast<AnnotationChannelSpecs*>(*it);
      for (std::vector<Marker*>::iterator it2 = channel->begin();
           it2 != channel->end(); ++it2)
      {
        static_cast<MarkerOrthoViewRenderer*>(
            (*it2)->renderer(this))->setFontSize(p_fontSizeControl->value());
      }
    }
  }
  redraw();
}
