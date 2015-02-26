/**************************************************************************
**       Title: Ortho-View Annotation-Channel Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the different View- and Channel
**    types. For each Channel type and each View type one such interface
**    class needs to be implemented. This is the specific implementation
**    for AnnotationChannelSpecs <=> OrthoView
**
**************************************************************************/

#ifndef ANNOTATIONCHANNELSPECSORTHOVIEWRENDERER_HH
#define ANNOTATIONCHANNELSPECSORTHOVIEWRENDERER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ChannelSpecsOrthoViewRenderer.hh"

class AnnotationChannelSpecs;
class OrthoViewWidget;
class Marker;

class AnnotationChannelSpecsOrthoViewRenderer :
    public ChannelSpecsOrthoViewRenderer
{

Q_OBJECT

public:
  
  AnnotationChannelSpecsOrthoViewRenderer(
      AnnotationChannelSpecs* channel, OrthoViewWidget* view);
  ~AnnotationChannelSpecsOrthoViewRenderer();
  
  void userInteractionEvent(UserInteractionEvent* event);
  void render(QPainter* painter = NULL) const;
  void render(
      QXmlStreamWriter &svgStream,
      blitz::TinyVector<double,3> const &shapePx,
      blitz::TinyVector<double,3> const &lowerBoundUm,
      blitz::TinyVector<double,3> const &upperBoundUm,
      std::string const &font, double fontSizePt,
      iRoCS::ProgressReporter *pr = NULL) const;

public slots:

/*======================================================================*/
/*! 
 *   Enables or disables the updates of the rendering cache. This usually
 *   means that changes in the ChannelSpecs or the view position don't
 *   lead to an update of the view. This is useful, if you have to
 *   programmatically perform many small operations on the channel of which
 *   not each should trigger a repaint.
 *
 *   \param enable true to enable, false to disable cache updates
 */
/*======================================================================*/
  virtual void setCacheUpdatesEnabled(bool enable);

/*======================================================================*/
/*! 
 *   Update the rendering cache for the specified orthoview dimension. This
 *   function is called by the updateCache()-Method for every direction.
 *   On crosshair position change the view widget explicitely calls this
 *   function with the dimensions orthogonal to the cross hair position
 *   change. Reimplement this function if you have expensive computations
 *   that should not be done at every repaint. The default implementation only
 *   calls the update() function of the view. You can disable the cacheUpdates
 *   using the setCacheUpdatesEnabled() function. This is important if you
 *   programmatically perform many operations on the ChannelSpecs, of which
 *   each would need a cache update. After the sequence of operations, you
 *   have to call updateCache() explicitely to apply the changes. After cache
 *   update update() of the view is called to visualize the changes. So if
 *   many channels are involved you should call setUpdatesEnabled(false)
 *   to the view before these changes.
 */
/*======================================================================*/
  virtual void updateCache(int direction) const;

private:
  
  Marker *p_marker;

};

#endif
