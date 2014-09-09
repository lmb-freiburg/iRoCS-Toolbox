#ifndef ATBCOUPLEDBSPLINEMODEL_LEGACY_HH
#define ATBCOUPLEDBSPLINEMODEL_LEGACY_HH

#include <libProgressReporter/ProgressReporter.hh>

#include "ATBPolyline.hh"

// #define __SAVEINTERMEDIATERESULTS

#include <fstream>

namespace atb
{

  static bool const dataTermUsePerpendicularPointsOnly = false;
  static bool const controlPointsUpdatePerpendicularToSpline = false;
  static bool const normalizeGradient = true;
  static bool const doLineSearch = false;

// When the optimization stops, regrid the resulting spline to allow
// further refinement
  static bool const regrid = true;

  static bool const weighCurvatureWithSplineLength = true;

  double tikhonovRegularizer(
      double value, std::vector<double> const &parameters);

  double dTikhonovRegularizer(
      double value, std::vector<double> const &parameters);

  double totalVariationRegularizer(
      double value, std::vector<double> const &parameters);

  double dTotalVariationRegularizer(
      double value, std::vector<double> const &parameters);

  double tikhonovCutoffRegularizer(
      double value, std::vector<double> const &parameters);

  double dTikhonovCutoffRegularizer(
      double value, std::vector<double> const &parameters);

  double totalVariationCutoffRegularizer(
      double value, std::vector<double> const &parameters);

  double dTotalVariationCutoffRegularizer(
      double value, std::vector<double> const &parameters);

  template<int Dim>
  class CoupledBSplineModel
  {

  public:
  
    CoupledBSplineModel();
  
    CoupledBSplineModel(CoupledBSplineModel<Dim> const &model);
  
    ~CoupledBSplineModel();
  
    CoupledBSplineModel<Dim> &operator=(CoupledBSplineModel<Dim> const &model);
  
    BSpline< blitz::TinyVector<double,Dim> > const &axis() const;

    BSpline< blitz::TinyVector<double,Dim> > &axis();

    Polyline<Dim> const &axisPolyline() const;

    Polyline<Dim> &axisPolyline();

    BSpline<double> const &thickness() const;

    BSpline<double> &thickness();

    void updateAxisPolyline();

  private:
  
    BSpline< blitz::TinyVector<double,Dim> > _axis;
    Polyline<Dim> _axisPolyline;
    BSpline<double> _thickness;

  };

  struct Energy 
  {
    
    Energy(double kappa = 1.0, double lambda = 0.0, double mu = 0.0);
    
    Energy(Energy const &E);
    
    Energy &operator=(Energy const &E);
    
    double value() const;
    
    double data, curvature, thickness, kappa, lambda, mu;
    
  };

  std::ostream &operator<<(std::ostream &os, Energy const &E);

  /*-----------------------------------------------------------------------
   *  One tube only
   *-----------------------------------------------------------------------*/

  template<int Dim>
  Energy computeCoupledSplineEnergy(
      std::vector< blitz::TinyVector<double,Dim> > const &points,
      CoupledBSplineModel<Dim> &model,
      double kappa, double lambda, double mu,
      double (&regularizer)(double, std::vector<double> const &) =
      tikhonovRegularizer,
      std::vector<double> const &regularizerParameters = std::vector<double>());

  template<int Dim>
  void updateCoupledSplineModel(
      CoupledBSplineModel<Dim> &model,
      std::vector< blitz::TinyVector<double,Dim> > const &dAxisControlPoints,
      std::vector<double> const &dThicknessControlPoints, double tau);

  template<int Dim>
  Energy computeUpdatedCoupledSplineEnergy(
      std::vector< blitz::TinyVector<double,Dim> > const &points,
      CoupledBSplineModel<Dim> model,
      std::vector< blitz::TinyVector<double,Dim> > const &dAxisControlPoints,
      std::vector<double> const &dThicknessControlPoints,
      double tau, double kappa, double lambda, double mu,
      double (&regularizer)(double, std::vector<double> const &),
      std::vector<double> const &regularizerParameters);

  template<int Dim>
  Energy refineModel(
      std::vector< blitz::TinyVector<double,Dim> > const &points,
      CoupledBSplineModel<Dim> &model,
      double kappa, double lambda, double mu, int nIter, double tau,
      double (&regularizer)(double, std::vector<double> const &) =
      tikhonovRegularizer,
      double (&dRegularizer)(double, std::vector<double> const &) =
      dTikhonovRegularizer,
      std::vector<double> const &regularizerParameters = std::vector<double>(),
      iRoCS::ProgressReporter *pr = NULL,
      std::string const &evolutionFileName = "",
      std::string const &evolutionGroup = "");
  
  template<int Dim>
  bool regridModel(CoupledBSplineModel<Dim> &model, double minSegmentLength);

  template<int Dim>
  Energy fitCoupledSplines(
      std::vector< blitz::TinyVector<double,Dim> > const &points,
      CoupledBSplineModel<Dim> &model,
      double kappa, double lambda, double mu, int nIter, double tau,
      double regridLength,
      double (&regularizer)(double, std::vector<double> const &) =
      tikhonovRegularizer,
      double (&dRegularizer)(double, std::vector<double> const &) =
      dTikhonovRegularizer,
      std::vector<double> const &regularizerParameters = std::vector<double>(),
      iRoCS::ProgressReporter *pr = NULL,
      std::string const &evolutionFileName = "");

  template<int Dim>
  Energy extendCoupledBSplineModel(
      std::vector<blitz::TinyVector<double,Dim> > const &points,
      CoupledBSplineModel<Dim> const &model,
      CoupledBSplineModel<Dim> &extendedModel,
      double length, size_t direction,
      std::vector< blitz::TinyVector<double,3> > const &searchDirections,
      bool rightEnd, double kappa, double lambda, double mu, size_t nIter,
      double tau, double regridLength,
      double (&regularizer)(double, std::vector<double> const &),
      double (&dRegularizer)(double, std::vector<double> const &),
      std::vector<double> const &regularizerParameters);

  /*-----------------------------------------------------------------------
   *  Same for multiple tubes
   *-----------------------------------------------------------------------*/

  template<int Dim>
  Energy computeCoupledSplineEnergy(
      std::vector< blitz::TinyVector<double,Dim> > const &points,
      std::vector< CoupledBSplineModel<Dim> > &model,
      double kappa, double lambda, double mu,
      double (&regularizer)(double, std::vector<double> const &) =
      tikhonovRegularizer,
      std::vector<double> const &regularizerParameters = std::vector<double>());

  template<int Dim>
  Energy computeUpdatedCoupledSplineEnergy(
      std::vector< blitz::TinyVector<double,Dim> > const &points,
      std::vector< CoupledBSplineModel<Dim> > model,
      std::vector< std::vector< blitz::TinyVector<double,Dim> > >
      const &dAxisControlPoints,
      std::vector< std::vector<double> > const &dThicknessControlPoints,
      double tau, double kappa, double lambda, double mu,
      double (&regularizer)(double, std::vector<double> const &),
      std::vector<double> const &regularizerParameters);

  template<int Dim>
  Energy refineModel(
      std::vector< blitz::TinyVector<double,Dim> > const &points,
      std::vector< CoupledBSplineModel<Dim> > &model,
      double kappa, double lambda, double mu, int nIter, double tau,
      double (&regularizer)(double, std::vector<double> const &) =
      tikhonovRegularizer,
      double (&dRegularizer)(double, std::vector<double> const &) =
      dTikhonovRegularizer,
      std::vector<double> const &regularizerParameters = std::vector<double>());
  
  template<int Dim>
  Energy fitCoupledSplines(
      std::vector< blitz::TinyVector<double,Dim> > const &points,
      std::vector< CoupledBSplineModel<Dim> > &model,
      double kappa, double lambda, double mu, int nIter, double tau,
      double regridLength,
      double (&regularizer)(double, std::vector<double> const &) =
      tikhonovRegularizer,
      double (&dRegularizer)(double, std::vector<double> const &) =
      dTikhonovRegularizer,
      std::vector<double> const &regularizerParameters = std::vector<double>());

#ifdef __SAVEINTERMEDIATERESULTS
  std::string __intermediateResultFileName;
  std::string __intermediateBaseGroup;
  std::string __outerIterGroup;
#endif

}

#include "ATBCoupledBSplineModel.icc"

#endif
