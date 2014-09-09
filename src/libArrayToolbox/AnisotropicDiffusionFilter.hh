/**************************************************************************
 **       Title: Anisotropic diffusion filter
 **    $RCSfile:$
 **   $Revision:$ $Name:$
 **       $Date:$
 **   Copyright: GPL $Author:$
 ** Description:
 **
 **************************************************************************/

/*======================================================================*/
/*!
 *  \file atb/AnisotropicDiffusionFilter.hh
 *  \brief Anisotropic diffusion filtering of n-D Arrays
 */
/*======================================================================*/

#ifndef ATBANISOTROPICDIFFUSIONFILTER_HH
#define ATBANISOTROPICDIFFUSIONFILTER_HH

#include "GaussianFilter.hh"
#include "CentralHessianUTFilter.hh"
#include "ATBLinAlg.hh"

namespace atb
{

/*======================================================================*/
/*!
 *  \class AnisotropicDiffusionFilter AnisotropicDiffusionFilter.hh "libArayToolbox/AnisotropicDiffusionFilter.hh"
 *  \brief The AnisotropicDiffusionFilter class implements the Filter
 *    interface and provides an anisotropic diffusion filter. This filter
 *    non-linearly removes noise and enhances consistent structures.
 */
/*======================================================================*/
  template<typename DataT, int Dim>
  class AnisotropicDiffusionFilter : public Filter<DataT,Dim,DataT>
  {

  public:
    
    typedef DataT ResultT;

/*======================================================================*/
/*! 
 *   Default Constructor. Creates a stub anisotropic diffusion filter.
 *
 *   \param bt             The boundary treatment this filter uses
 *   \param boundaryValue  If bt is ValueBT, this value will be used for
 *     out-of-Array access
 */
/*======================================================================*/ 
    AnisotropicDiffusionFilter(
        BoundaryTreatmentType bt = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero);
    
/*======================================================================*/
/*! 
 *   Constructor. Creates an anisotropic diffusion filter with given
 *   pre-smoothing Gaussian standard deviation and edge weight kappa.
 *
 *   \param sigmaUm The standard deviations in either direction of the
 *     pre-smoothing Gaussian kernel in micrometers
 *   \param kappa The edge factor (ToDo: clarify what it does)
 *   \param tau   The step width of the Jacobian solver
 *   \param zAnisotropyCorrection Values greater than zero will enhance
 *     weaker edges in z direction than in x-y directions. The z-direction
 *     edge weight will be increased by this constant.
 *   \param nIterations The number of iterations to apply
 *   \param hessianUpdateStepWidth The hessian is recomputed every
 *     hessianUpdateStepWidth iterations
 *   \param bt             The boundary treatment this filter uses
 *   \param boundaryValue  If bt is ValueBT, this value will be used for
 *     out-of-Array access
 */
/*======================================================================*/ 
    AnisotropicDiffusionFilter(
        double kappa, double sigmaUm = -1.0,
        double tau = 0.0625, double zAnisotropyCorrection = 0.0,
        int nIterations = 20, int hessianUpdateStepWidth = 4,
        BoundaryTreatmentType bt = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero);
    
/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    virtual ~AnisotropicDiffusionFilter();

/*======================================================================*/
/*! 
 *   Get the edge weight kappa.
 *
 *   \return The edge weight kappa
 */
/*======================================================================*/
    double kappa() const;

/*======================================================================*/
/*! 
 *   Set the edge weight kappa.
 *
 *   \param kappa The edge weight
 */
/*======================================================================*/
    void setKappa(double kappa);

/*======================================================================*/
/*! 
 *   Get the standard deviations of the Gaussian pre-smoothing filter in
 *   micrometers for all dimensions.
 *
 *   \return The standard deviation vector in micrometers
 */
/*======================================================================*/
    double sigmaUm() const;

/*======================================================================*/
/*! 
 *   Set the standard deviation of the Gaussian pre-smoothing filter in
 *   micrometers for all dimensions.
 *
 *   \param sigmaUm The standard deviation vector in micrometers
 */
/*======================================================================*/
    void setSigmaUm(double sigmaUm);

/*======================================================================*/
/*! 
 *   Get the solver time step.
 *
 *   \return The solver time step
 */
/*======================================================================*/
    double tau() const;

/*======================================================================*/
/*! 
 *   Set the solver time step (In case of convergence problems reduce it)
 *
 *   \param tau The solver time step
 */
/*======================================================================*/
    void setTau(double tau);

/*======================================================================*/
/*! 
 *   Get the anisotropy correction constant applied in z direction.
 *
 *   \return The z anisotropy constant
 */
/*======================================================================*/
    double zAnisotropyCorrection() const;

/*======================================================================*/
/*! 
 *   Set the anisotropy correction constant applied in z direction.
 *
 *   \param zAnisotropyCorrection The z anisotropy constant
 */
/*======================================================================*/
    void setZAnisotropyCorrection(double zAnisotropyCorrection);

/*======================================================================*/
/*! 
 *   Get the number of filtering iterations.
 *
 *   \return The number of filtering iterations
 */
/*======================================================================*/
    int nIterations() const;

/*======================================================================*/
/*! 
 *   Set the number of filtering iterations.
 *
 *   \param nIterations The number of filtering iterations
 */
/*======================================================================*/
    void setNIterations(int nIterations);

/*======================================================================*/
/*! 
 *   Get the iteration stride for hessian recomputation.
 *
 *   \return The number of filtering iterations between two hessian updates
 */
/*======================================================================*/
    int hessianUpdateStepWidth() const;

/*======================================================================*/
/*! 
 *   Set the iteration stride for hessian recomputation.
 *
 *   \param hessianUpdateStepWidth The number of filtering iterations
 *     between two hessian updates
 */
/*======================================================================*/
    void setHessianUpdateStepWidth(int hessianUpdateStepWidth);

/*======================================================================*/
/*! 
 *   Apply the filter to the given Array.
 *
 *   \param data          The blitz++ Array to apply the filter to
 *   \param elementSizeUm The element size of the Array
 *   \param filtered      The filtering result
 *   \param pr            Progress will be reported through the
 *     given iRoCS::ProgressReporter
 *
 *   \exception RuntimeError If an error occurs during the filter operation
 *     an exception of this kind is thrown
 */
/*======================================================================*/
    virtual void apply(
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &elementSizeUm,
        blitz::Array<ResultT,Dim> &filtered,
        iRoCS::ProgressReporter *pr = NULL) const;
    
    // Explicitly force the name mangler to also consider the base class
    // implementation
    using atb::Filter<DataT,Dim,ResultT>::apply;

/*======================================================================*/
/*! 
 *   Apply the filter to the given Array.
 *
 *   \param data          The blitz++ Array to apply the filter to
 *   \param elementSizeUm The element size of the Array
 *   \param filtered      The filtering result
 *   \param sigmaUm       The Gaussian pre-smoothing standard deviation vector
 *     (in micrometers)
 *   \param kappa         The edge weight
 *   \param tau           The step width of the Jacobian solver
 *   \param zAnisotropyCorrection Values greater than zero will enhance
 *     weaker edges in z direction than in x-y directions. The z-direction
 *     edge weight will be increased by this constant.
 *   \param nIterations   The number of iterations to apply
 *   \param hessianUpdateStepWidth The hessian is recomputed every
 *     hessianUpdateStepWidth iterations
 *   \param btType        The boundary treatment to apply
 *   \param boundaryValue The out-of-Array value that should be used if
 *     btType == \c ValueBT
 *   \param pr            Progress will be reported through the
 *     given iRoCS::ProgressReporter
 *
 *   \exception RuntimeError If an error occurs during the filter operation
 *     an exception of this kind is thrown
 */
/*======================================================================*/
    static void apply(
        blitz::Array<DataT,Dim> const &data,
        blitz::TinyVector<double,Dim> const &elementSizeUm,
        blitz::Array<ResultT,Dim> &filtered, double kappa,
        double sigmaUm = -1.0, double tau = 0.0625,
        double zAnisotropyCorrection = 0.0, int nIterations = 20,
        int hessianUpdateStepWidth = 4, BoundaryTreatmentType btType = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero,
        iRoCS::ProgressReporter *pr = NULL);

/*======================================================================*/
/*! 
 *   Apply the filter to the given Array.
 *
 *   \param data          The blitz++ Array to apply the filter to
 *   \param filtered      The filtering result
 *   \param sigmaUm       The Gaussian pre-smoothing standard deviation vector
 *     (in micrometers)
 *   \param kappa         The edge weight
 *   \param tau           The step width of the Jacobian solver
 *   \param zAnisotropyCorrection Values greater than zero will enhance
 *     weaker edges in z direction than in x-y directions. The z-direction
 *     edge weight will be increased by this constant.
 *   \param nIterations   The number of iterations to apply
 *   \param hessianUpdateStepWidth The hessian is recomputed every
 *     hessianUpdateStepWidth iterations
 *   \param btType        The boundary treatment to apply
 *   \param boundaryValue The out-of-Array value that should be used if
 *     btType == \c ValueBT
 *   \param pr            Progress will be reported through the
 *     given iRoCS::ProgressReporter
 *
 *   \exception RuntimeError If an error occurs during the filter operation
 *     an exception of this kind is thrown
 */
/*======================================================================*/
    static void apply(
        Array<DataT,Dim> const &data, Array<ResultT,Dim> &filtered,
        double kappa, double sigmaUm = -1.0, double tau = 0.0625,
        double zAnisotropyCorrection = 0.0, int nIterations = 20,
        int hessianUpdateStepWidth = 4, BoundaryTreatmentType btType = ValueBT,
        DataT const &boundaryValue = traits<DataT>::zero,
        iRoCS::ProgressReporter *pr = NULL);

  private:
    
    double _kappa, _sigmaUm, _tau, _zAnisotropyCorrection;
    int _nIterations, _hessianUpdateStepWidth;

  };

}

#include "AnisotropicDiffusionFilter.icc"

#endif
