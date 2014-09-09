#include "OrthoViewOverlay.hh"

OrthoViewOverlay::OrthoViewOverlay(OrthoViewWidget* view)
: p_view(view)
{
  
}


OrthoViewWidget* OrthoViewOverlay::view() const
{
  return p_view;
}

void OrthoViewOverlay::setDimensions(blitz::TinyVector< int, 2 >& dimensions)
{
  _dimensions = dimensions;
}
