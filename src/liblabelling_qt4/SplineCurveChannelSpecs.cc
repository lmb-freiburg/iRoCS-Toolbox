/**************************************************************************
**       Title: Spline Curve Channel Specs
**    $RCSfile$
**   $Revision: 3896 $ $Name$
**       $Date: 2008-07-16 15:17:47 +0200 (Wed, 16 Jul 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    Channel Descriptor for a Spline Curve editor channel
**
**************************************************************************/

// Include QFont to avoid usage of incomplete Implementation
#include <QtGui/QFont>

#include "SplineCurveChannelSpecs.hh"

#include <QtGui/QHBoxLayout>
#include <QtGui/QColorDialog>
#include <QtGui/QPushButton>

#include "SplineCurveChannelSpecsOrthoViewRenderer.hh"
#include "SplineCurveChannelSpecsOpenGlRenderingViewRenderer.hh"
#include "MultiChannelModel.hh"
#include "OrthoViewWidget.hh"

#include "Double3ControlElement.hh"

SplineCurveChannelSpecs::SplineCurveChannelSpecs(
    atb::BSpline< blitz::TinyVector<double,3> > *spline,
    MultiChannelModel *model)
        : ChannelSpecs(model), p_spline(spline)
{
  init();
}

SplineCurveChannelSpecs::~SplineCurveChannelSpecs()
{
  delete p_spline;
}

ChannelSpecs::ChannelType SplineCurveChannelSpecs::channelType() const
{
  return ChannelSpecs::SplineCurve;
}

blitz::TinyVector<float,3> SplineCurveChannelSpecs::color() const 
{
  return blitz::TinyVector<float,3>(p_channelColor->value());
}

void SplineCurveChannelSpecs::setColor(
    blitz::TinyVector<float,3> const &color) 
{
  if (blitz::all(blitz::TinyVector<double,3>(color) == p_channelColor->value()))
      return;
  p_channelColor->setValue(blitz::TinyVector<float,3>(color));
  if (p_model != NULL) p_model->setModified(true);
}

float SplineCurveChannelSpecs::valueAt(
    blitz::TinyVector<double,3> const &positionUm) const 
{
  double uOpt;
  atb::distance(*p_spline, positionUm, uOpt);
  return static_cast<float>(uOpt);
}

std::string SplineCurveChannelSpecs::stringValueAt(
    blitz::TinyVector<double,3> const &positionUm) const 
{
  double uOpt;
  double d = atb::distance(*p_spline, positionUm, uOpt);
  std::stringstream os;
  os << "(" << uOpt << "," << d << ")";
  return os.str();
}

blitz::TinyVector<double,3> const &SplineCurveChannelSpecs::lowerBoundUm() const
{
  if (p_model != NULL) return p_model->lowerBoundUm();
  return _lowerBoundUm;
}

blitz::TinyVector<double,3> const &SplineCurveChannelSpecs::upperBoundUm() const
{
  if (p_model != NULL) return p_model->upperBoundUm();
  return _upperBoundUm;
}

blitz::TinyVector<double,3> const
&SplineCurveChannelSpecs::elementSizeUm() const
{
  if (p_model != NULL) return p_model->elementSizeUm();
  return this->_elementSizeUm;
}

ChannelSpecsRenderer *SplineCurveChannelSpecs::addRenderer(ViewWidget *view)
{
  if (_renderers.find(view) != _renderers.end())
  {
    std::cout << "SplineCurveChannelSpecs::addRenderer(): WARNING: The channel "
              << "already has an associated renderer" << std::endl;
    return _renderers[view];
  }
  switch (view->viewType())
  {
  case ViewWidget::OrthoView:
    _renderers[view] = new SplineCurveChannelSpecsOrthoViewRenderer(
        this, static_cast<OrthoViewWidget*>(view));
    break;
  case ViewWidget::OpenGlRenderingView:
    _renderers[view] = new SplineCurveChannelSpecsOpenGlRenderingViewRenderer(
        this, reinterpret_cast<OpenGlRenderingViewWidget*>(view));
    break;
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": Implementation missing"
              << std::endl;
    break;
  }
  return _renderers[view];
}

void SplineCurveChannelSpecs::load(
    std::string const &fileName, std::string const &group, bool throwErrors)
{
  if (p_spline == NULL)
      p_spline = new atb::BSpline< blitz::TinyVector<double,3> >();
  p_spline->load(fileName, group, throwErrors);
  if (p_model != NULL) p_model->setModified(true);
}

void SplineCurveChannelSpecs::save(
    std::string const &fileName, std::string const &group, bool throwErrors)
{
  p_spline->save(fileName, group, throwErrors);
}

atb::BSpline< blitz::TinyVector<double,3> > const
*SplineCurveChannelSpecs::spline() const
{
  return p_spline;
}

void SplineCurveChannelSpecs::updateChannelColor()
{
  QPixmap icon(16, 16);
  icon.fill(QColor(
                static_cast<int>(color()(0) * 255),
                static_cast<int>(color()(1) * 255),
                static_cast<int>(color()(2) * 255)));
  setIcon(QIcon(icon));
  emitUpdateRequest();
}

void SplineCurveChannelSpecs::chooseColor() 
{
  QColor color = QColorDialog::getColor(
      QColor(static_cast<int>(this->color()(0) * 255),
             static_cast<int>(this->color()(1) * 255),
             static_cast<int>(this->color()(2) * 255)),
      p_channelControlWidget);
  if (color.isValid()) 
  {
    setColor(blitz::TinyVector<float,3>(
                 static_cast<float>(color.redF()),
                 static_cast<float>(color.greenF()),
                 static_cast<float>(color.blueF())));
  }
}

void SplineCurveChannelSpecs::init()
{
  QHBoxLayout *colorLayout = new QHBoxLayout;

  p_channelColor = new Double3ControlElement(
      tr("Color"), blitz::TinyVector<double,3>(1.0, 1.0, 1.0));
  p_channelColor->setRange(blitz::TinyVector<double,3>(0.0, 0.0, 0.0),
                           blitz::TinyVector<double,3>(1.0, 1.0, 1.0));
  p_channelColor->setSingleStep(blitz::TinyVector<double,3>(0.01, 0.01, 0.01));
  connect(p_channelColor, SIGNAL(valueChanged()), SLOT(updateChannelColor()));
  colorLayout->addWidget(p_channelColor);
#ifdef DATADIR
  QPushButton *colorChooserButton = new QPushButton(
      QIcon((std::string(DATADIR) + "/palette.png").c_str()), "");
#else
  QPushButton *colorChooserButton = new QPushButton(QIcon(":/palette.png"), "");
#endif
  colorLayout->addWidget(colorChooserButton);
  connect(colorChooserButton, SIGNAL(pressed()), SLOT(chooseColor()));
  p_channelControlLayout->addLayout(colorLayout);

  p_channelControlLayout->addStretch(1);
}

QColor SplineCurveChannelSpecs::qColor()
{
  return QColor(
      static_cast<int>(p_channelColor->value()(0) * 255),
      static_cast<int>(p_channelColor->value()(1) * 255),
      static_cast<int>(p_channelColor->value()(2) * 255),
      static_cast<int>(alpha() * 255));
}
