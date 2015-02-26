/**************************************************************************
**       Title: Abstract Marker OpenGlRenderingView Renderer
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class is the interface between the OpenGlRenderingView and
**    different Marker types. For each Marker type one such interface class
**    needs to be implemented.
**
**************************************************************************/

#ifndef MARKEROPENGLRENDERINGVIEWRENDERER_HH
#define MARKEROPENGLRENDERINGVIEWRENDERER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "MarkerRenderer.hh"

#include <blitz/array.h>

class OpenGlRenderingViewWidget;

class MarkerOpenGlRenderingViewRenderer : public MarkerRenderer
{

Q_OBJECT

public:
  
  MarkerOpenGlRenderingViewRenderer(
      Marker* marker, OpenGlRenderingViewWidget* view);
  MarkerOpenGlRenderingViewRenderer(
      MarkerOpenGlRenderingViewRenderer const &renderer);
  virtual ~MarkerOpenGlRenderingViewRenderer();

  virtual MarkerRenderer *clone() const = 0;

protected:
  
/*======================================================================*/
/*! 
 *   Set the pen according to the PresetWidget
 *
 *   \param hilighted  If the marker is selected set this bool to true
 */
/*======================================================================*/
  void _setPenColor(bool hilighted = false) const;

};

#endif
