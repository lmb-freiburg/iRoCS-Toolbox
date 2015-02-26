#ifndef ORTHOVIEWOVERLAY_HH
#define ORTHOVIEWOVERLAY_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <blitz/array.h>

class QPainter;

class OrthoViewWidget;

class OrthoViewOverlay
{
public:
  OrthoViewOverlay(OrthoViewWidget* view);
  OrthoViewWidget* view() const;
  virtual void render(QPainter* painter = NULL) = 0;
  void setDimensions(blitz::TinyVector<int,2>& dimensions);

protected:
  OrthoViewWidget* p_view;
  blitz::TinyVector<int,2> _dimensions;
};

#endif // ORTHOVIEWOVERLAY_HH
