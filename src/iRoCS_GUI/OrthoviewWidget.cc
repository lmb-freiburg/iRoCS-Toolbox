/**************************************************************************
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

#include "OrthoviewWidget.hh"
#include "OrthoviewPlane.hh"
#include "OrthoviewRenderer.hh"
#include <libMultiChannelModel/MultichannelModel.hh>

#include <QtGui/QBoxLayout>
#include <QtGui/QSplitter>
#include <QtGui/QToolBar>
#include <QtGui/QComboBox>
#include <QtGui/QAction>
#include <QtGui/QLineEdit>
#include <QtCore/QSettings>

#ifdef HAVE_BLITZ_V9
#include <blitz/tinyvec-et.h>
#endif

namespace iRoCS {

#ifdef DEBUG_VERBOSE_XML
  std::string OrthoviewWidget::printState() const {
    std::stringstream stateStream;
    stateStream << "_model=\"" << &_model << "\" _renderers=\"";
    if (_renderers.size() != 0) {
      std::set<OrthoviewRenderer*>::const_iterator it = _renderers.begin();
      stateStream << *it;
      ++it;
      for (; it != _renderers.end(); ++it) stateStream << "," << *it;
    }
    stateStream << "\"";
    return stateStream.str();
  }
#endif

  OrthoviewWidget::OrthoviewWidget(
    MultichannelModel &model, QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f), MultichannelModelObserver(model),
    _centerPositionUm(0.0), _pixelSizeUm(1.0), _crossPositionUm(0.0),
    p_vSplitter(NULL), p_hTopSplitter(NULL), p_hBottomSplitter(NULL),
    _magnification(1.0), p_magnificationComboBox(NULL),
    p_showScalebarAction(NULL), _renderers() {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<OrthoviewWidget@" << this
      << "::OrthoviewWidget model=\"" << &model << "\" parent=\""
      << parent << "\" " << printState() << ">" << std::endl;
#endif
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Create toolbar
    QToolBar *toolbar = new QToolBar;
    mainLayout->addWidget(toolbar);

    QAction *resetMagnificationAction = new QAction(
      QIcon::fromTheme(
        "zoom-original", QIcon(":/zoom-original.png")), tr("Reset Zoom"),
      this);
    resetMagnificationAction->setShortcut(Qt::CTRL + Qt::Key_1);
    resetMagnificationAction->setToolTip(tr("Reset magnification to 100%"));
    connect(resetMagnificationAction, SIGNAL(triggered()),
      SLOT(resetMagnification()));
    toolbar->addAction(resetMagnificationAction);

    QAction *magnifyOutAction = new QAction(
      QIcon::fromTheme(
        "zoom-out", QIcon(":/zoom-out.png")), tr("Zoom Out"), this);
    magnifyOutAction->setShortcut(QKeySequence::ZoomOut);
    magnifyOutAction->setToolTip(tr("Zoom out by one step"));
    connect(magnifyOutAction, SIGNAL(triggered()), SLOT(magnifyOut()));
    toolbar->addAction(magnifyOutAction);

    QStringList standardZooms;
    standardZooms << "1" << "2" << "5" << "10" << "25" << "33" << "50"
      << "75" << "100" << "150" << "200" << "400" << "1000"
      << "5000" << "10000";
    p_magnificationComboBox = new QComboBox;
    p_magnificationComboBox->addItems(standardZooms);
    p_magnificationComboBox->setCurrentIndex(8);
    p_magnificationComboBox->setEditable(true);
    p_magnificationComboBox->lineEdit()->setValidator(
      new QIntValidator(1, 10000, NULL));
    connect(p_magnificationComboBox, SIGNAL(currentIndexChanged(int)),
      SLOT(updateMagnification()));
    connect(p_magnificationComboBox, SIGNAL(editTextChanged(QString const &)),
      SLOT(updateMagnification()));
    connect(this, SIGNAL(magnificationChanged(double)),
      SLOT(setMagnificationInComboBox(double)));
    toolbar->addWidget(p_magnificationComboBox);

    QAction *magnifyInAction = new QAction(
      QIcon::fromTheme(
        "zoom-in", QIcon(":/zoom-in.png")), tr("Zoom in"), this);
    magnifyInAction->setShortcut(QKeySequence::ZoomIn);
    magnifyInAction->setToolTip(tr("Zoom in by one step"));
    connect(magnifyInAction, SIGNAL(triggered()), SLOT(magnifyIn()));
    toolbar->addAction(magnifyInAction);

    p_showScalebarAction = new QAction(
      QIcon(":/show-scalebar.svg"), tr("Show scale bar"), this);
    p_showScalebarAction->setToolTip(tr("Show a scale bar"));
    p_showScalebarAction->setCheckable(true);
    p_showScalebarAction->setChecked(false);
    connect(p_showScalebarAction, SIGNAL(toggled(bool)),
      SLOT(setScalebarVisible(bool)));
    toolbar->addAction(p_showScalebarAction);

    p_enableInterpolationAction = new QAction(
      tr("Linear Interpolation"), this);
    p_enableInterpolationAction->setToolTip(
      tr("Toggle between nearest and linear interpolation"));
    p_enableInterpolationAction->setCheckable(true);
    p_enableInterpolationAction->setChecked(false);
    connect(p_enableInterpolationAction, SIGNAL(toggled(bool)),
      SLOT(setInterpolationEnabled(bool)));
    toolbar->addAction(p_enableInterpolationAction);

    // Create orthoview
    p_vSplitter = new QSplitter(Qt::Vertical);
    mainLayout->addWidget(p_vSplitter);
    p_hTopSplitter = new QSplitter;
    p_hBottomSplitter = new QSplitter;
    p_vSplitter->addWidget(p_hTopSplitter);
    p_vSplitter->addWidget(p_hBottomSplitter);

    connect(p_hTopSplitter, SIGNAL(splitterMoved(int, int)),
      SLOT(synchronizeBottomSplitter()));
    connect(p_hBottomSplitter, SIGNAL(splitterMoved(int, int)),
      SLOT(synchronizeTopSplitter()));

    QFrame *ovFrames[3];
    for (int dim = 0; dim < 3; ++dim) {
      p_ovPlanes[dim] = new OrthoviewPlane(dim, this);
      connect(
        p_ovPlanes[dim], SIGNAL(mousePositionChanged(int, QPoint const &)),
        SLOT(emitMousePositionChanged(int, QPoint const &)));
      ovFrames[dim] = new QFrame;
      ovFrames[dim]->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
      ovFrames[dim]->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
      QHBoxLayout *layout = new QHBoxLayout(ovFrames[dim]);
      layout->setContentsMargins(0, 0, 0, 0);
      layout->addWidget(p_ovPlanes[dim]);
    }
    QWidget *dummy = new QWidget;
    dummy->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    p_hTopSplitter->addWidget(ovFrames[0]);
    p_hTopSplitter->addWidget(ovFrames[2]);
    p_hBottomSplitter->addWidget(ovFrames[1]);
    p_hBottomSplitter->addWidget(dummy);

    QSettings settings;
    p_vSplitter->restoreState(
      settings.value("Orthoview/vSplitter").toByteArray());
    p_hTopSplitter->restoreState(
      settings.value("Orthoview/hSplitter").toByteArray());

    synchronizeBottomSplitter();

    updateModel();

#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</OrthoviewWidget@" << this << "::OrthoviewWidget>"
      << std::endl;
#endif
  }

  OrthoviewWidget::~OrthoviewWidget() {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<OrthoviewWidget@" << this << "::~OrthoviewWidget "
      << printState() << ">" << std::endl;
#endif
    QSettings settings;
    settings.setValue("Orthoview/vSplitter", p_vSplitter->saveState());
    settings.setValue("Orthoview/hSplitter", p_hTopSplitter->saveState());
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</OrthoviewWidget@" << this << "::~OrthoviewWidget>"
      << std::endl;
#endif
  }

  void OrthoviewWidget::updateAddChannel(Channel *channel) {
    if (channel == NULL) return;
    for (std::set<OrthoviewRenderer*>::iterator it =
      _renderers.begin(); it != _renderers.end(); ++it)
      if (&(*it)->observable() == channel) return;

#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<OrthoviewWidget@" << this
      << "::updateAddChannel channel=\"" << channel << "\" "
      << printState() << ">" << std::endl;
#endif

    OrthoviewRenderer *renderer = OrthoviewRenderer::instance(*this, *channel);
    if (_model.nChannels(
      Channel::Float | Channel::Indexed | Channel::RGB) == 1 &&
      renderer->rendersImageData()) {
      _pixelSizeUm = blitz::min(channel->elementSizeUm());
      _centerPositionUm = (channel->boundingBoxLowerBoundUm() +
        channel->boundingBoxUpperBoundUm()) / 2;
      _crossPositionUm = (channel->boundingBoxLowerBoundUm() +
        channel->boundingBoxUpperBoundUm()) / 2;
      update();
    }
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</OrthoviewWidget@" << this << "::updateAddChannel>"
      << std::endl;
#endif
  }

  void OrthoviewWidget::updateRemoveChannel(Channel *channel) {
    if (channel == NULL) return;
    std::set<OrthoviewRenderer*>::iterator it = _renderers.begin();
    for (; it != _renderers.end(); ++it)
      if (&(*it)->observable() == channel) break;
    if (it == _renderers.end()) return;

#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<OrthoviewWidget@" << this
      << "::updateRemoveChannel channel=\"" << channel << "\" "
      << printState() << ">" << std::endl;
#endif
    delete *it;
    if (_model.nChannels() == 0) {
      _pixelSizeUm = blitz::TinyVector<double, 3>(1.0);
      _centerPositionUm = blitz::TinyVector<double, 3>(0.0);
      _crossPositionUm = blitz::TinyVector<double, 3>(0.0);
      update();
    }
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</OrthoviewWidget@" << this << "::updateRemoveChannel>"
      << std::endl;
#endif
  }

  void OrthoviewWidget::updateModified(bool /*modified*/) {}

  void OrthoviewWidget::updateModel() {
    while (_renderers.size() != 0) delete *_renderers.begin();
    for (MultichannelModel::const_iterator it = _model.begin();
      it != _model.end(); ++it) updateAddChannel(*it);
  }

  blitz::TinyVector<double, 3> const &OrthoviewWidget::crossPositionUm() const {
    return _crossPositionUm;
  }

  blitz::TinyVector<double, 3> const &OrthoviewWidget::centerPositionUm() const {
    return _centerPositionUm;
  }

  blitz::TinyVector<double, 3> const &OrthoviewWidget::pixelSizeUm() const {
    return _pixelSizeUm;
  }

  OrthoviewPlane *OrthoviewWidget::plane(int orthogonalDimension) const {
    return (orthogonalDimension < 0 || orthogonalDimension >= 3) ? NULL :
      p_ovPlanes[orthogonalDimension];
  }

  double OrthoviewWidget::magnification() const {
    return _magnification;
  }

  bool OrthoviewWidget::scalebarVisible() const {
    return p_showScalebarAction->isChecked();
  }

  bool OrthoviewWidget::interpolationEnabled() const {
    return p_enableInterpolationAction->isChecked();
  }

  std::set<OrthoviewRenderer*> const &OrthoviewWidget::renderers() const {
    return _renderers;
  }

  void OrthoviewWidget::setScalebarVisible(bool show) {
    if (p_showScalebarAction->isChecked() != show) {
      p_showScalebarAction->setChecked(show);
      return;
    }
    p_ovPlanes[0]->update(
      10, p_ovPlanes[0]->height() - 25, p_ovPlanes[0]->width() / 2, 18);
  }

  void OrthoviewWidget::setInterpolationEnabled(bool enable) {
    if (p_enableInterpolationAction->isChecked() != enable) {
      p_enableInterpolationAction->setChecked(enable);
      return;
    }
    update();
  }

  void OrthoviewWidget::setCrossPositionUm(
    blitz::TinyVector<double, 3> const &positionUm) {
    if (blitz::all(_crossPositionUm == positionUm)) return;
    blitz::TinyVector<double, 3> oldPositionUm(_crossPositionUm);
    _crossPositionUm = positionUm;

    blitz::TinyVector<bool, 3> dimChanged(oldPositionUm != positionUm);
    for (int d = 0; d < 3; ++d) {
      if (dimChanged(d)) p_ovPlanes[d]->update();
      else {
        QPointF oldPositionPx(p_ovPlanes[d]->absoluteUmToPx(oldPositionUm));
        QPointF newPositionPx(p_ovPlanes[d]->absoluteUmToPx(positionUm));
        QRegion region;
        if (dimChanged(p_ovPlanes[d]->planeDimensions()(0))) {
          region += QRect(0, static_cast<int>(std::floor(oldPositionPx.y())),
            p_ovPlanes[d]->width(), 1);
          region += QRect(0, static_cast<int>(std::floor(newPositionPx.y())),
            p_ovPlanes[d]->width(), 1);
        }
        if (dimChanged(p_ovPlanes[d]->planeDimensions()(1))) {
          region += QRect(static_cast<int>(std::floor(oldPositionPx.x())), 0,
            1, p_ovPlanes[d]->height());
          region += QRect(static_cast<int>(std::floor(newPositionPx.x())), 0,
            1, p_ovPlanes[d]->height());
        }
        p_ovPlanes[d]->update(region);
      }
    }
  }

  void OrthoviewWidget::setCenterPositionUm(
    blitz::TinyVector<double, 3> const &positionUm) {
    if (blitz::all(_centerPositionUm == positionUm)) return;
    blitz::TinyVector<double, 3> oldPositionUm(_centerPositionUm);
    _centerPositionUm = positionUm;

    for (int d = 0; d < 3; ++d) {
      QPointF oldPositionPx(p_ovPlanes[d]->absoluteUmToPx(oldPositionUm));
      QPointF newPositionPx(p_ovPlanes[d]->absoluteUmToPx(positionUm));
      if (oldPositionPx != newPositionPx) p_ovPlanes[d]->update();
    }
  }

  void OrthoviewWidget::setPixelSizeUm(
    blitz::TinyVector<double, 3> const &pixelSizeUm) {
    if (blitz::all(_pixelSizeUm == pixelSizeUm)) return;
    _pixelSizeUm = pixelSizeUm;
    update();
  }

  void OrthoviewWidget::setMagnification(double magnification) {
    if (magnification == _magnification) return;
    _magnification = magnification;
    emit magnificationChanged(magnification);
    update();
  }

  void OrthoviewWidget::synchronizeTopSplitter() {
    p_hTopSplitter->setSizes(p_hBottomSplitter->sizes());
  }

  void OrthoviewWidget::synchronizeBottomSplitter() {
    p_hBottomSplitter->setSizes(p_hTopSplitter->sizes());
  }

  void OrthoviewWidget::resetMagnification() {
    setMagnification(1.0);
  }

  void OrthoviewWidget::magnifyIn() {
    double targetMagnification = 0.0;
    int i = 0;
    while (i < p_magnificationComboBox->count() &&
      targetMagnification <= magnification()) {
      targetMagnification =
        p_magnificationComboBox->itemText(i).toDouble() / 100.0;
      i++;
    }
    if (i < p_magnificationComboBox->count())
      setMagnification(targetMagnification);
  }

  void OrthoviewWidget::magnifyOut() {
    double targetMagnification = std::numeric_limits<double>::infinity();
    int i = p_magnificationComboBox->count() - 1;
    while (i >= 0 && targetMagnification >= magnification()) {
      targetMagnification =
        p_magnificationComboBox->itemText(i).toDouble() / 100.0;
      i--;
    }
    if (i >= 0) setMagnification(targetMagnification);
  }

  void OrthoviewWidget::updateMagnification() {
    double requestedMagnification =
      p_magnificationComboBox->currentText().toDouble() / 100.0;
    if (_magnification == requestedMagnification) return;
    setMagnification(requestedMagnification);
  }

  void OrthoviewWidget::setMagnificationInComboBox(double magnification) {
    QString requestedText;
    requestedText.setNum(static_cast<int>(100.0 * magnification));
    if (requestedText == p_magnificationComboBox->currentText()) return;
    p_magnificationComboBox->setEditText(requestedText);
  }

  void OrthoviewWidget::emitMousePositionChanged(
    int orthogonalDimension, QPoint const &mousePositionPx) {
    blitz::TinyVector<int, 2> const &dims =
      p_ovPlanes[orthogonalDimension]->planeDimensions();
    blitz::TinyVector<double, 3> mousePositionUm;
    mousePositionUm(orthogonalDimension) =
      _crossPositionUm(orthogonalDimension);
    mousePositionUm(dims(0)) =
      (static_cast<double>(mousePositionPx.y()) -
        static_cast<double>(p_ovPlanes[orthogonalDimension]->height()) / 2.0) *
      _pixelSizeUm(dims(0)) / magnification() + _centerPositionUm(dims(0));
    mousePositionUm(dims(1)) =
      (static_cast<double>(mousePositionPx.x()) -
        static_cast<double>(p_ovPlanes[orthogonalDimension]->width()) / 2.0) *
      _pixelSizeUm(dims(1)) / magnification() + _centerPositionUm(dims(1));
    emit mousePositionChanged(mousePositionUm);
  }

}
