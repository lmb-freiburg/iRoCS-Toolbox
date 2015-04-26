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
