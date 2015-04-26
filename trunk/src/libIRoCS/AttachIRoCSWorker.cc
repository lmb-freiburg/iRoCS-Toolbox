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

#include "AttachIRoCSWorker.hh"

namespace iRoCS
{
  
  void attachIRoCS(
      atb::IRoCS &iRoCS, std::vector<atb::Nucleus> &nuclei,
      blitz::TinyVector<double,3> const &qcPositionUm, double kappa,
      double lambda, double mu, double searchRadiusUm, int nIterations,
      double tau, ProgressReporter *pr)
  {
    if (pr != NULL)
    {
      pr->setProgressMin(0);
      pr->setProgressMax(110);
      if (!pr->updateProgress(1)) return;
      pr->updateProgressMessage("Attaching iRoCS...");
    }
    
    iRoCS.fit(qcPositionUm, nuclei, kappa, lambda, mu, nIterations, tau,
              searchRadiusUm);
    
    if (pr != NULL)
    {
      if (!pr->updateProgress(102)) return;
      pr->updateProgressMessage("Setting nucleus coordinates");
    }
    
    // Finally set the intrinsic coordinates for the input markers
    int p = 0;
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(nuclei.size()); ++i)
    {
      if (pr != NULL && !pr->updateProgress(
              static_cast<int>(102 + (7 * p) / nuclei.size()))) continue;
#ifdef _OPENMP
#pragma omp atomic
#endif
      ++p;
      blitz::TinyVector<double,3> p(
          iRoCS.getCoordinates(nuclei[i].positionUm()));
      nuclei[i].setQcDistanceUm(p(0));
      nuclei[i].setRadialDistanceUm(p(1));
      nuclei[i].setPhi(p(2));
    }
    if (pr != NULL) pr->updateProgress(110);    
  }

}
