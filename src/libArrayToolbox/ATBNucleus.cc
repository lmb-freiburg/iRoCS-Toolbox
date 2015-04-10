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
