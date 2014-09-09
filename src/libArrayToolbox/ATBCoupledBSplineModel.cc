#include "ATBCoupledBSplineModel.hh"

namespace atb
{

  double tikhonovRegularizer(
      double value, std::vector<double> const &)
  {
    if (value < 0) return 10000.0;
    return value;
  }

  double dTikhonovRegularizer(
      double value, std::vector<double> const &)
  {
    if (value < 0) return 0.0;
    return 1.0;
  }

  double totalVariationRegularizer(
      double value, std::vector<double> const &)
  {
    if (value < 0) return 100.0;
    return std::sqrt(value);
  }

  double dTotalVariationRegularizer(
      double value, std::vector<double> const &)
  {
    if (value < 0) return 0.0;
    return 1.0 / (2.0 * std::sqrt(value));
  }

  double tikhonovCutoffRegularizer(
      double value, std::vector<double> const &parameters)
  {
    if (parameters.size() < 1)
    {
      std::cerr << __FILE__ << ":" << __LINE__
                << ": Too few parameters given to the regularizer"
                << std::endl;
      exit(-1);
    }
    if (value < 0 || std::sqrt(value) >= parameters[0])
        return parameters[0] * parameters[0];
      return value;
  }

  double dTikhonovCutoffRegularizer(
      double value, std::vector<double> const &parameters)
  {
    if (parameters.size() < 1)
    {
      std::cerr << __FILE__ << ":" << __LINE__
                << ": Too few parameters given to the regularizer"
                << std::endl;
      exit(-1);
    }
    if (value < 0 || std::sqrt(value) >= parameters[0]) return 0.0;
    return 1.0;
  }

  double totalVariationCutoffRegularizer(
      double value, std::vector<double> const &parameters)
  {
    if (parameters.size() < 1)
    {
      std::cerr << __FILE__ << ":" << __LINE__
                << ": Too few parameters given to the regularizer"
                << std::endl;
      exit(-1);
    }
    if (value < 0 || std::sqrt(value) >= parameters[0])
        return parameters[0];
    return std::sqrt(value);
  }

  double dTotalVariationCutoffRegularizer(
      double value, std::vector<double> const &parameters)
  {
    if (parameters.size() < 1)
    {
      std::cerr << __FILE__ << ":" << __LINE__
                << ": Too few parameters given to the regularizer"
                << std::endl;
      exit(-1);
    }
    if (value < 0 || std::sqrt(value) >= parameters[0]) return 0.0;
    return 1.0 / (2.0 * std::sqrt(value));
  }

  Energy::Energy(double kappa, double lambda, double mu)
          : data(0.0), curvature(0.0), thickness(0.0),
            kappa(kappa), lambda(lambda), mu(mu)
  {}
  
  Energy::Energy(Energy const &E)
          : data(E.data), curvature(E.curvature), thickness(E.thickness),
            kappa(E.kappa), lambda(E.lambda), mu(E.mu)
  {}
  
  Energy &Energy::operator=(Energy const &E)
  {
    data = E.data;
    curvature = E.curvature;
    thickness = E.thickness;
    kappa = E.kappa;
    lambda = E.lambda;
    mu = E.mu;
    return *this;
  }
  
  double Energy::value() const
  {
    return kappa * data + lambda * curvature + mu * thickness;
  }

  std::ostream &operator<<(std::ostream &os, Energy const &E)
  {
    os << "E = "
       << E.kappa << " * " << E.data << " + "
       << E.lambda << " * " <<  E.curvature << " + "
       << E.mu << " * " << E.thickness << " = " << E.value();
    return os;
  }

}
