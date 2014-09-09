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
