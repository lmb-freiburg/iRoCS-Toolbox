#include "AttachIRoCSThread.hh"

#include "AttachIRoCSParametersDialog.hh"

#include <liblabelling_qt4/NucleusMarker.hh>

#include <libIRoCS/AttachIRoCSWorker.hh>

AttachIRoCSThread::AttachIRoCSThread(
    AttachIRoCSParametersDialog const &parameters, atb::IRoCS *rct,
    iRoCS::ProgressReporter *progress, LabellingMainWidget *mainWidget,
    QWidget *parent) 
        : QThread(parent), _parameters(parameters), p_rct(rct),
          p_progress(progress), p_mainWidget(mainWidget)
{
  std::cout << "Initializing Attach iRoCS... OK" << std::endl;
}

AttachIRoCSThread::~AttachIRoCSThread()
{
  std::cout << "Cleaning up Attach iRoCS... OK" << std::endl;
}

void AttachIRoCSThread::run()
{
  std::cout << "Running AttachIRoCSThread with parameters: "
            << std::endl;
  std::cout << "  marker Channel = " << _parameters.markerChannel()->name()
            << " (" << _parameters.markerChannel()->markers().size()
            << " markers)" << std::endl;
  std::cout << "  QC Channel = " << _parameters.qcChannel()->name()
            << " (" << _parameters.qcChannel()->markers().size()
            << " markers)" << std::endl;
  std::cout << "  iRoCS Channel = " << _parameters.iRoCSChannelName()
            << std::endl;
  std::cout << "  dataWeight (kappa) = " << _parameters.dataWeight()
            << std::endl;
  std::cout << "  Axis stiffness weight (lambda) = "
            << _parameters.axisStiffnessWeight() << std::endl;
  std::cout << "  Thickness Constancy weight (mu) = "
            << _parameters.thicknessConstancyWeight() << std::endl;
  std::cout << "  nIterations = " << _parameters.nIterations() << std::endl;
  std::cout << "  Time step (tau) = " << _parameters.optimizationTimeStep()
            << std::endl;
  std::cout << "  Search Radius = " << std::flush;
  if (_parameters.searchRadiusUm() <= 0.0)
      std::cout << "<auto>" << std::endl;
  else std::cout << _parameters.searchRadiusUm() << std::endl;
    
  if (p_progress != NULL)
  {
    p_progress->setProgressMin(0);
    p_progress->setProgressMax(110);
    if (!p_progress->updateProgressMessage("Converting Markers...")) return;
  }

  std::vector<atb::Nucleus> nuclei;
  for (std::vector<Marker*>::const_iterator it =
           _parameters.markerChannel()->begin();
       it != _parameters.markerChannel()->end(); ++it)
  {
    atb::Nucleus nc;
    nc.setPositionUm((*it)->positionUm());
    nc.setLabel((*it)->label());
    nuclei.push_back(nc);
  }
  
  iRoCS::attachIRoCS(
      *p_rct, nuclei, _parameters.qcChannel()->markers().front()->positionUm(),
      _parameters.dataWeight(), _parameters.axisStiffnessWeight(),
      _parameters.thicknessConstancyWeight(), _parameters.searchRadiusUm(),
      _parameters.nIterations(), _parameters.optimizationTimeStep(),
      p_progress);
  
  if (p_progress != NULL &&
      !p_progress->updateProgressMessage("Setting nucleus coordinates"))
      return;
  
  // Finally set the intrinsic coordinates for the input markers
  ptrdiff_t nMarkers = static_cast<ptrdiff_t>(
      _parameters.markerChannel()->markers().size());
#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (ptrdiff_t i = 0; i < nMarkers; ++i)
  {
    (*_parameters.markerChannel())[i]->setQcDistanceUm(
        nuclei[i].qcDistanceUm());
    (*_parameters.markerChannel())[i]->setRadialDistanceUm(
        nuclei[i].radialDistanceUm());
    (*_parameters.markerChannel())[i]->setPhi(nuclei[i].phi());
  }
  if (p_progress != NULL && p_progress->isAborted()) return;
}
