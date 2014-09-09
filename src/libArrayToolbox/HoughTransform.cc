#include "HoughTransform.hh"

namespace atb
{

  void houghTransform(
      Array<float,3> const &data, Array<float,3> &response,
      Array<float,3> &radiusUm,
      blitz::TinyVector<double,2> const &radiusRangeUm, double radiusStepUm,
      double minMagnitude, bool invertGradients, iRoCS::ProgressReporter *pr)
  {
    double pStart = (pr != NULL) ? pr->taskProgressMin() : 0.0;
    double pScale = (pr != NULL) ? (pr->taskProgressMax() - pStart) : 1.0;
    
    if (pr != NULL) pr->updateProgressMessage("Computing data gradient");

    // Compute the gradient
    if (pr != NULL)
    {
      pr->setTaskProgressMin(pStart);
      pr->setTaskProgressMax(0.05 * pScale + pStart);
    }
    Array<blitz::TinyVector<float,3>,3> ddata;
    CentralGradientFilter<float,3> filter(MirrorBT);
    filter.apply(data, ddata, pr);
    if (pr != NULL && !pr->updateProgress(0.05 * pScale + pStart)) return;

    // Compute the gradient magnitude
    Array<float,3> magnitude(ddata.shape(), ddata.elementSizeUm());
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (size_t i = 0; i < static_cast<size_t>(ddata.size()); ++i)
        magnitude.data()[i] = static_cast<float>(
            std::sqrt(blitz::dot(ddata.data()[i], ddata.data()[i])));
    
    // Compute gradient directions
    if (pr != NULL)
    {
      if (!pr->updateProgress(0.1 * pScale + pStart)) return;
      pr->updateProgressMessage("Computing gradient directions");
    }
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (size_t i = 0; i < static_cast<size_t>(ddata.size()); ++i)
        ddata.data()[i] /= magnitude.data()[i];
    
    // Normalize gradient magnitude
    if (pr != NULL)
    {
      if (!pr->updateProgress(0.15 * pScale + pStart)) return;
      pr->updateProgressMessage(
          "Normalizing gradient magnitude to [0,1] range");
    }
    magnitude /= blitz::max(magnitude);
    if (pr != NULL && pr->isAborted()) return;

    // Prepare output Arrays
    response.resize(data.shape());
    response.setElementSizeUm(data.elementSizeUm());
    response.setTransformation(data.transformation());
    std::memset(response.data(), 0, response.size() * sizeof(float));
    radiusUm.resize(data.shape());
    radiusUm.setElementSizeUm(data.elementSizeUm());
    radiusUm.setTransformation(data.transformation());
  
    // Setup line direction
    float direction = invertGradients ? -1.0f : 1.0f;

    // Do hough voting
    if (pr != NULL && !pr->updateProgressMessage("Hough Transform"))
        return;
    int nRadii = static_cast<int>(
        (radiusRangeUm(1) - radiusRangeUm(0)) / radiusStepUm) + 1;
    int rIndex = 1;
    Array<float,3> accu(data.shape(), data.elementSizeUm());
    for (double r = radiusRangeUm(0); r <= radiusRangeUm(1);
         r += radiusStepUm, rIndex++)
    {
      if (pr != NULL)
      {
        std::stringstream msg;
        msg << "Processing radius " << r << " (" << rIndex << "/" << nRadii - 1
            << ")";
        pr->updateProgressMessage(msg.str());
        if (!pr->updateProgress(
                (0.15 + 0.85 * static_cast<double>(rIndex) /
                 static_cast<double>(nRadii - 1)) * pScale + pStart)) return;
      }
      std::memset(accu.data(), 0, accu.size() * sizeof(float));
      for (BlitzIndexT z = 0; z < data.extent(0); ++z)
      {
        for (BlitzIndexT y = 0; y < data.extent(1); ++y)
        {
          for (BlitzIndexT x = 0; x < data.extent(2); ++x)
          {
            if (pr != NULL && pr->isAborted()) return;
            if (magnitude(z, y, x) > minMagnitude)
            {
              blitz::TinyVector<BlitzIndexT,3> writePos(
                  blitz::TinyVector<float,3>(
                      static_cast<float>(z), static_cast<float>(y),
                      static_cast<float>(x)) +
                  direction * r * ddata(z, y, x) / data.elementSizeUm() + 0.5);
              if (blitz::all(writePos >= 0 && writePos < data.shape()))
                  accu(writePos) += magnitude(z, y, x);
            }
          }
        }
      }

#ifdef _OPENMP
#pragma omp parallel for
#endif
      for (size_t i = 0; i < static_cast<size_t>(data.size()); ++i)
      {
        if (response.data()[i] < accu.data()[i])
        {
          response.data()[i] = accu.data()[i];
          radiusUm.data()[i] = static_cast<float>(r);
        }
      }
    }
  }  

}
