/**************************************************************************
**       Title: Generic Channel Specs
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    Channel Descriptor for one channel of multi-channel data
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2008/01/17 17:05:50  tschmidt
**  - Initial revision
**
**
**************************************************************************/

#ifndef CHANNELSPECS_HH
#define CHANNELSPECS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtCore/QObject>
#include <QtGui/QIcon>
#include <QtGui/QToolButton>

#include <string>
#include <vector>
#include <map>

#include <blitz/array.h>

class QVBoxLayout;

class StringControlElement;
class DoubleControlElement;
class BoolControlElement;
class Double4x4ControlElement;
class OrthoViewWidget;
class ChannelSpecsRenderer;
class ViewWidget;
class MultiChannelModel;

class ChannelSpecs : public QObject
{

  Q_OBJECT

public:

  enum ChannelType {
      Data = 0x0001,
      Visualization = 0x0002,
      RGB = 0x0004,
      Annotation = 0x0008,
      SplineCurve = 0x0010,
      IRoCS = 0x0020,
      IRoCSSCT = 0x0040
  };
  Q_DECLARE_FLAGS(ChannelTypes, ChannelType)
  static ChannelTypes const Any;
  
  ChannelSpecs(MultiChannelModel *model = NULL);
  virtual ~ChannelSpecs();
  
  virtual ChannelType channelType() const = 0;

  void setModel(MultiChannelModel *model);
  MultiChannelModel *model() const;

  std::string name() const;

  QIcon icon() const;

  float alpha() const;
  
  bool visible() const;

  bool hasNewData() const;

  virtual blitz::TinyVector<double,3> const &lowerBoundUm() const;
  virtual blitz::TinyVector<double,3> const &upperBoundUm() const;
  virtual blitz::TinyVector<double,3> const &elementSizeUm() const;
  virtual blitz::TinyMatrix<double,4,4> const &transformation() const;

  QWidget *channelControlWidget() const;

  virtual float valueAt(
      blitz::TinyVector<double,3> const &positionUm) const = 0;

  virtual std::string stringValueAt(
      blitz::TinyVector<double,3> const &positionUm) const = 0;

  virtual ChannelSpecsRenderer *addRenderer(ViewWidget *view) = 0;
  void removeRenderer(ViewWidget *view);
  ChannelSpecsRenderer *renderer(ViewWidget *view) const;
  std::map<ViewWidget*,ChannelSpecsRenderer*> const &renderers() const;
  std::map<ViewWidget*,ChannelSpecsRenderer*> &renderers();

/*======================================================================*/
/*! 
 *   Check whether cache updates for this channel are enabled.
 *
 *   \return true if cache updates are enabled, false otherwise
 */
/*======================================================================*/
  bool updatesEnabled() const;

public slots:
  
  void setName(std::string const &name);
  void setIcon(QIcon const &icon);
  void setAlpha(float alpha);
  void setVisible(bool visible);
  void setHasNewData(bool hasNewData);
  virtual void setTransformation(
      blitz::TinyMatrix<double,4,4> const &transformation);

/*======================================================================*/
/*! 
 *   Enables/Disables cache updates for this channel. If you programmatically
 *   apply many subsequent changes to a channel like automatic marker
 *   insertion or property changes, disable cache updates, otherwise each
 *   operation triggers a cache update.
 *
 *   \param enable true to enable cache updates,
 *     false to disable cache updates
 */
/*======================================================================*/
  void setUpdatesEnabled(bool enable);

/*======================================================================*/
/*! 
 *   Explicitely trigger a cache update for all associated renderers.
 *   This is only necessary if updates were disabled and after all changes
 *   the cache has to be updated. If updates are disabled a call to this
 *   function is a noop.
 */
/*======================================================================*/
  void update();

/*======================================================================*/
/*! 
 *   Reset the channel shape, element size, transformation and bounding box
 *   from the underlying data. This method must be called whenever the
 *   underlying data were programmatically changed using the direct data
 *   access methods. If updates are enabled it will also do cache updates.
 */
/*======================================================================*/
  virtual void revalidate();

signals:

  void boundingBoxChanged(
      blitz::TinyVector<double,3> const &lowerBoundUm,
      blitz::TinyVector<double,3> const &upperBoundUm);
  void iconChanged();
  void nameChanged();
  void transformationChanged();

/*======================================================================*/
/*! 
 *   This signal is emitted whenever the contents or visualization of the
 *   channel are altered and an update of the views is required to reflect
 *   these changes. The MultiChannelModel catches this SIGNAL, calls
 *   updateChache() and redraw() for all views and clears the hasNewData
 *   flag for all channels when the redraw has finished.
 */
/*======================================================================*/
  void viewChanged();

protected slots:

/*======================================================================*/
/*! 
 *   This function sets the hasNewData flag for this channel to true, to
 *   inform the renderers that this channel has new data and possibly cached
 *   values are invalid. It also saves the current visibility state to
 *   _oldVisibilityState and emits a viewChanged() SIGNAL.
 */
/*======================================================================*/
  void emitUpdateRequest();

private slots:

  void toggleAdvanced(bool toggle);
  void setTransformationFromControl();

protected:

  MultiChannelModel *p_model;
  std::map<ViewWidget*,ChannelSpecsRenderer*> _renderers;
  QWidget *p_channelControlWidget;
  QVBoxLayout *p_channelControlLayout;
  DoubleControlElement *p_alphaControl;
  BoolControlElement *p_visibleControl;

  QToolButton *p_toggleAdvancedButton;
  QWidget *p_advancedControlWidget;
  QVBoxLayout *p_advancedControlLayout;

  Double4x4ControlElement *p_transformationControl;

  bool _oldVisibilityState;
  bool _hasNewData;

  blitz::TinyVector<double,3> _lowerBoundUm, _upperBoundUm, _elementSizeUm;
  blitz::TinyMatrix<double,4,4> _originalTransformation, _transformation;

private:

  bool _updatesEnabled;
  std::string _name;
  QIcon _icon;
  
};
    
Q_DECLARE_OPERATORS_FOR_FLAGS(ChannelSpecs::ChannelTypes)

#endif
