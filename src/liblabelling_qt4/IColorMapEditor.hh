#ifndef ICOLORMAPEDITOR_HH
#define ICOLORMAPEDITOR_HH

#include <cstddef>

class ColorMap;

class IColorMapEditor
{

public:
  
  IColorMapEditor(ColorMap *colorMap = NULL);
  virtual ~IColorMapEditor();

  void setColorMap(ColorMap *colorMap);
  ColorMap *colorMap() const;

  virtual void updateIndexRange(bool updatePreview = true) = 0;
  virtual void updateIntermediateColors(bool updatePreview = true) = 0;
  virtual void updateOverrideColors(bool updatePreview = true) = 0;
  virtual void updateMonochromeColor(bool updatePreview = true) = 0;
  virtual void updateRangeIndicator(bool updatePreview = true) = 0;
  virtual void updateGamma(bool updatePreview = true) = 0;
  virtual void updateControls() = 0;

protected:
  
  ColorMap *p_colorMap;

};

#endif
