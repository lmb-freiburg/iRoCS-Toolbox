/**************************************************************************
**       Title: Prototypic nucleus model
**    $RCSfile: ATBNucleus.cc,v $
**   $Revision: 4735 $ $Name:  $
**       $Date: 2011-07-21 14:59:03 +0200 (Thu, 21 Jul 2011) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#include "ATBNucleus.hh"

#include <sstream>

namespace atb
{
  std::ostream &operator<<(std::ostream &os, Nucleus const &nc)
  {
    os << "Nucleus '" << nc.id() << "' { \n"
       << "  Position:              (" << nc.positionUm()(0) << ", "
       << nc.positionUm()(1) << ", " << nc.positionUm()(2) << ") um\n"
       << "  Label:                  " << nc.label() << "\n"
       << "  Predicted Label:        " << nc.predictedLabel() << "\n"
       << "  Manually annotated:     " << ((nc.manual()) ? "yes" : "no") << "\n"
       << "  Needs feature update:   "
       << ((nc.needsFeatureUpdate()) ? "yes" : "no") << "\n"
       << "  Distance to QC:         " << nc.qcDistanceUm() << " um\n"
       << "  Radial Axis Distance:   " << nc.radialDistanceUm() << " um\n"
       << "  Angle around root axis: " << nc.phi() << " radians\n"
       << "  Radius:                 " << nc.radiusUm() << " um\n"
       << "  Value:                  " << nc.value() << "\n"
       << "  Confidence:             " << nc.confidence() << "\n"
       << "  S-Phase probability:    " << nc.sphase() << "\n"
       << "  Distance to Epidermis:  " << nc.borderDistanceUm() << " um\n"
       << "  Nucleolus Volume:       " << nc.volumeUm3() << " um^3 \n\n"
       << "  Mitotic:                " << ((nc.mitotic()) ? "yes" : "no")
       << "\n"
       << "  Predicted Mitotic:      "
       << ((nc.predictedMitotic()) ? "yes" : "no") << "\n"
       << "  Sub-type:               " << nc.subtype() << "\n"
       << "  Predicted sub-type:     " << nc.predictedSubtype() << "\n"
       << "  Cell file:              " << nc.cellfile() << "\n"
       << "  Predicted cell file:    " << nc.predictedCellfile() << "\n"
       << "  Feature vector: ";
    for (size_t i = 0; i < nc.features().size(); ++i) 
    {
      os << nc.features()[i] << " ";
    }
    os << "\n}\n";
    return os;
  }
    
  void Nucleus::_loadFixedSize(
      blitz::Array<bool,1> &buf, BlitzH5File const &inFile,
      std::string const &dsName, bool showWarnings)
  {
    blitz::Array<unsigned char,1> tmp;
    try
    {
      inFile.readDataset(tmp, dsName);
    }
    catch (BlitzH5Error &)
    {
      if (showWarnings)
          std::cerr << "'" << dsName << "' not found. Setting it to false"
                    << std::endl;
    }
    for (BlitzIndexT i = 0; i < static_cast<BlitzIndexT>(buf.size()) &&
             i < static_cast<BlitzIndexT>(tmp.size()); ++i)
        buf(i) = (tmp(i) == 1);
    for (BlitzIndexT i = static_cast<BlitzIndexT>(tmp.size());
         i < static_cast<BlitzIndexT>(buf.size()); ++i) buf(i) = false;
  }

}
