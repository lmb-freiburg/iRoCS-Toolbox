#include "IColorMapEditor.hh"

#include "ColorMap.hh"

IColorMapEditor::IColorMapEditor(ColorMap* colorMap)
        : p_colorMap(colorMap)
{}

IColorMapEditor::~IColorMapEditor()
{}

void IColorMapEditor::setColorMap(ColorMap *colorMap)
{
  p_colorMap = colorMap;
  updateControls();
}

ColorMap *IColorMapEditor::colorMap() const
{
  return p_colorMap;
}
