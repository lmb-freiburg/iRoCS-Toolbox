/**************************************************************************
 *
 * Copyright (C) 2015 Margret Keuper, Thorsten Falk
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

#ifndef LIBSEGMENTATION_GVF_INL_HH
#define LIBSEGMENTATION_GVF_INL_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "gvf.hh"

#include <libArrayToolbox/TypeTraits.hh>

#ifdef HAVE_BLITZ_V9
#include <blitz/tinyvec-et.h>
#endif

namespace segmentation
{

/**
 * Solve Euler-Lagrange equation for gradient vector flow using
 * successive over-relaxation
 * \f[
 * 0 = \mu \Delta u_i - \|\nabla f\|^2 ( u_i - \frac{\partial f}{\partial i} )
 * \f]
 * u : output gradient vector flow
 * f : input vector field
 * */
  template<typename T>
  void gradientVectorFlowSOR(
      blitz::Array<blitz::TinyVector<T, 3>, 3> &gradient,
      blitz::TinyVector<T,3> const &el_size_um, T mu, T nu, int max_iter,
      iRoCS::ProgressReporter *progress)
  {
    blitz::TinyVector<atb::BlitzIndexT,3> shape(gradient.shape());
    // normalize gradient and gradient magnitude
    blitz::Array<T,3> gradient_norm_sq(shape);

    T max_norm_sq = T(0);
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (size_t i = 0; i < gradient.size(); ++i)
    {
      gradient_norm_sq.data()[i] =
          gradient.data()[i](0) * gradient.data()[i](0)
          + gradient.data()[i](1) * gradient.data()[i](1)
          + gradient.data()[i](2) * gradient.data()[i](2);
#ifdef _OPENMP
#pragma omp critical
#endif
      if (gradient_norm_sq.data()[i] > max_norm_sq)
          max_norm_sq = gradient_norm_sq.data()[i];
    }

#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (size_t i = 0; i < gradient.size(); ++i)
    {
      gradient.data()[i] /= std::sqrt(max_norm_sq);
      gradient_norm_sq.data()[i] /= max_norm_sq;
    }

    // normalizer

    T max_el_size = blitz::max(el_size_um);
    blitz::TinyVector<T,3> normalizer(
        (el_size_um / max_el_size) * (el_size_um / max_el_size));
    normalizer = 1;
    std::cout << "GVF - normalizer " << normalizer << std::endl;

    // const part in linear equation
    blitz::Array<blitz::TinyVector<T, 3>,3> b_arr(shape);
    b_arr = gradient_norm_sq * gradient;

    // short notation
    blitz::Array<blitz::TinyVector<T, 3>, 3>& x = gradient;

    for (int iter = 1; iter <= max_iter; ++iter)
    {
      if (progress != NULL && !progress->updateProgress(
              static_cast<double>(iter) / static_cast<double>(max_iter) *
              progress->taskProgressMax() - progress->taskProgressMin()) +
          progress->taskProgressMin()) break;
#ifdef _OPENMP
#pragma omp parallel for
#endif
      for (int lev = 1; lev < shape(0) - 1; ++lev)
      {
        for (int row = 1; row < shape(1) - 1; ++row)
        {
          for (int col = 1; col < shape(2) - 1; ++col)
          {
            // own coefficients
            // laplacian - squared gradient magnitude
            T a_ii = -6 - gradient_norm_sq(lev, row, col)/mu;
            // const part
            const blitz::TinyVector<T, 3>& b =  -b_arr(lev, row, col)/mu;

            // other coefficients * x
            // this is just the finite-difference laplacian
            blitz::TinyVector<T, 3> a_ij_x =
                (x(lev - 1, row, col) +
                 x(lev + 1, row, col)) * normalizer(0) +
                (x(lev, row - 1, col) +
                 x(lev, row + 1, col)) * normalizer(1) +
                (x(lev, row, col - 1) +
                 x(lev, row, col + 1)) * normalizer(2);
            // SOR iteration
            x(lev, row, col) = (1. - nu) * x(lev, row, col) +
                nu/a_ii * (b - a_ij_x);
          }
        }
      }
    } // for maxiter
  }

  template<typename T>
  void msGradientVectorFlow(
      blitz::Array<blitz::TinyVector<T, 3>, 3> &gradient,
      blitz::TinyVector<T,3> const &el_size_um, T mu, T hs, T hr, int max_iter,
      iRoCS::ProgressReporter *progress)
  {
    std::cout << "MSGVF - Initializing" << std::endl;
    T scaling=el_size_um(2)/el_size_um(0);
    T hz=hs*scaling;
    blitz::TinyVector<atb::BlitzIndexT,3> Shape(gradient.shape());
    hr*=hr;
    hz=std::floor(hz);
    atb::BlitzIndexT hz_int = static_cast<atb::BlitzIndexT>(hz);
    atb::BlitzIndexT hs_int = static_cast<atb::BlitzIndexT>(hs);
    blitz::Array<T,3> dx2(
        Shape(0) + 2 * hz_int, Shape(1) + 2 * hs_int, Shape(2) + 2 * hs_int);
    blitz::Array<T,3> dy2(dx2.shape());
    blitz::Array<T,3> dz2(dx2.shape());

    blitz::Array<T,3> gradient_norm(Shape);
    gradient_norm =
        blitz::sqrt(blitz::pow2(gradient[0]) +
                    blitz::pow2(gradient[1]) +
                    blitz::pow2(gradient[2]));
    T maxgrad=blitz::max(gradient_norm);
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(gradient.size()); ++i)
    {
      gradient.data()[i] /= maxgrad;
      gradient_norm.data()[i] /= maxgrad;
    }

    dx2=0;
    dy2=0;
    dz2=0;

    dx2(blitz::Range(hz_int, hz_int + Shape(0) - 1),
        blitz::Range(hs_int, hs_int + Shape(1) - 1),
        blitz::Range(hs_int, hs_int + Shape(2) - 1)) = gradient[0];
    dy2(blitz::Range(hz_int, hz_int + Shape(0) - 1),
        blitz::Range(hs_int, hs_int + Shape(1) - 1),
        blitz::Range(hs_int, hs_int + Shape(2) - 1)) = gradient[1];
    dz2(blitz::Range(hz_int, hz_int + Shape(0) - 1),
        blitz::Range(hs_int, hs_int + Shape(1) - 1),
        blitz::Range(hs_int, hs_int + Shape(2) - 1)) = gradient[2];

    blitz::TinyVector<atb::BlitzIndexT,3> ShapeL(dx2.shape());
    blitz::Array<T,3> b(ShapeL);

    b = dx2 * dx2 + dy2 * dy2 + dz2 * dz2;

    blitz::Array<T,3> c1(ShapeL);
    c1 = b * dx2;
    blitz::Array<T,3> c2(ShapeL);
    c2 = b * dy2;
    blitz::Array<T,3> c3(ShapeL);
    c3 = b * dz2;
    b = static_cast<T>(1) - b;
    blitz::Array<blitz::TinyVector<T,3>,3> ms_f(ShapeL);
    ms_f[0]=0;
    ms_f[1]=0;
    ms_f[2]=0;

    T hssq = hs * hs;
    scaling *= scaling;
    T invscaling = static_cast<T>(1.0) / scaling;

    std::cout << "MSGVF - Running" << std::endl;

    int count;
    /************* Solve GVF = (u,v) iteratively ***************/
    for (count = 1; count <= max_iter; count++)
    {
      if (progress != NULL && !progress->updateProgress(
              static_cast<double>(count) / static_cast<double>(max_iter) *
              (progress->taskProgressMax() - progress->taskProgressMin()) +
              progress->taskProgressMin())) return;

      //    /* IV: Compute Laplace operator using Neuman condition */
      //    blitz::Array<float,2> Lu = laplacian(u);
      //    blitz::Array<float,2> Lv = laplacian(v);
      //
      //    /******** V: Update GVF ************/
      //    u = (1-b) * u + mu * Lu + c1;
      //    v = (1-b) * v + mu * Lv + c2;

#ifdef _OPENMP
#pragma omp parallel for
#endif
      for(atb::BlitzIndexT lev = hz_int; lev < ShapeL(0) - hz_int; ++lev)
      {
        if (progress != NULL && progress->isAborted()) continue;
        for(atb::BlitzIndexT row = hs_int; row < ShapeL(1) - hs_int; ++row)
        {
          for(atb::BlitzIndexT col = hs_int; col < ShapeL(2) - hs_int; ++col)
          {
            ms_f(lev, row, col)[0]=0;
            ms_f(lev, row, col)[1]=0;
            ms_f(lev, row, col)[2]=0;

            T count=0;

            T dx_l=dx2(lev, row, col);
            T dy_l=dy2(lev, row, col);
            T dz_l=dz2(lev, row, col);

            T distv=0;

            //float dweight=1;
            //for spherical kernel
            for(atb::BlitzIndexT lev2 = lev - hz_int;
                lev2 < lev + hz_int + 1; ++lev2)
            {
              for(atb::BlitzIndexT row2 = row - hs_int;
                  row2 < row + hs_int + 1; ++row2)
              {
                for(atb::BlitzIndexT col2 = col - hs_int;
                    col2 < col + hs_int + 1; ++col2)
                {
                  T dx_l2 = dx2(lev2, row2, col2);
                  T dy_l2 = dy2(lev2, row2, col2);
                  T dz_l2 = dz2(lev2, row2, col2);
                  distv = ((dx_l2 - dx_l) * (dx_l2 - dx_l) +
                           (dy_l2 - dy_l) * (dy_l2 - dy_l) +
                           (dz_l2 - dz_l) * (dz_l2 - dz_l)) / hr +
                      ((invscaling * static_cast<T>(lev - lev2) *
                        static_cast<T>(lev - lev2)) +
                       (static_cast<T>(row - row2) *
                        static_cast<T>(row - row2) +
                        static_cast<T>(col - col2) *
                        static_cast<T>(col - col2))) / hssq;

                  if(distv < 1)
                  {
                    //float dweight=1-distv;
                    //count+=dweight;
                    count++;
                    ms_f(lev, row, col)[0] += dx_l2;
                    ms_f(lev, row, col)[1] += dy_l2;
                    ms_f(lev, row, col)[2] += dz_l2;
                  }
                }
              }
            }
            if(count > 0.5)
            {
              ms_f(lev, row, col)[0] /= count;
              ms_f(lev, row, col)[1] /= count;
              ms_f(lev, row, col)[2] /= count;
            }
          }
        }
      }
      if (progress != NULL && progress->isAborted()) return;

      dx2 = ms_f[0];
#ifdef _OPENMP
#pragma omp parallel for
#endif
      for(atb::BlitzIndexT lev = 1; lev < ShapeL(0) - 1; ++lev)
      {
        if (progress != NULL && progress->isAborted()) continue;
        for(atb::BlitzIndexT row = 1; row < ShapeL(1) - 1; ++row)
        {
          for(atb::BlitzIndexT col = 1; col < ShapeL(2) - 1; ++col)
          {
            dx2(lev, row, col) =
                ((b(lev, row, col)) * dx2(lev, row, col) +
                 mu * ((dx2(lev - 1, row, col) +
                        dx2(lev + 1, row, col) -
                        2 * dx2(lev, row, col)) * scaling +
                       dx2(lev, row - 1, col) +
                       dx2(lev, row + 1, col) +
                       dx2(lev, row, col - 1) +
                       dx2(lev, row, col + 1) -
                       4 * dx2(lev, row, col)) + c1(lev, row, col));
          }
        }
      }
      if (progress != NULL && progress->isAborted()) return;

      dy2 = ms_f[1];
#ifdef _OPENMP
#pragma omp parallel for
#endif
      for(atb::BlitzIndexT lev = 1; lev < ShapeL(0) - 1; ++lev)
      {
        if (progress != NULL && progress->isAborted()) continue;
        for(atb::BlitzIndexT row = 1; row < ShapeL(1) - 1; ++row)
        {
          for(atb::BlitzIndexT col = 1; col < ShapeL(2) - 1; ++col)
          {
            dy2(lev, row, col) =
                ((b(lev, row, col)) * dy2(lev, row, col) +
                 mu * ((dy2(lev - 1, row, col) +
                        dy2(lev + 1, row, col) -
                        2 * dy2(lev, row, col)) * scaling +
                       dy2(lev, row - 1, col) +
                       dy2(lev, row + 1, col) +
                       dy2(lev, row, col - 1) +
                       dy2(lev, row, col + 1) -
                       4 * dy2(lev, row, col)) + c2(lev, row, col));
          }
        }
      }
      if (progress != NULL && progress->isAborted()) return;

      dz2 = ms_f[2];
#ifdef _OPENMP
#pragma omp parallel for
#endif
      for(atb::BlitzIndexT lev = 1; lev < ShapeL(0) - 1; ++lev)
      {
        if (progress != NULL && progress->isAborted()) continue;
        for(atb::BlitzIndexT row = 1; row < ShapeL(1) - 1; ++row)
        {
          for(atb::BlitzIndexT col = 1; col < ShapeL(2) - 1; ++col)
          {
            dz2(lev, row, col) =
                ((b(lev, row, col)) * dz2(lev, row, col) +
                 mu * ((dz2(lev - 1, row, col) +
                        dz2(lev + 1, row, col) -
                        2 * dz2(lev, row, col)) * scaling +
                       dz2(lev, row - 1, col) +
                       dz2(lev, row + 1, col) +
                       dz2(lev, row, col - 1) +
                       dz2(lev, row, col + 1) -
                       4 * dz2(lev, row, col)) + c3(lev, row, col));
          }
        }
      }
    }
    if (progress != NULL && progress->isAborted()) return;

    gradient[0] = dx2(
        blitz::Range(hz_int, hz_int + Shape(0) - 1),
        blitz::Range(hs_int, hs_int + Shape(1) - 1),
        blitz::Range(hs_int, hs_int + Shape(2) - 1));
    gradient[1] = dy2(
        blitz::Range(hz_int, hz_int + Shape(0) - 1),
        blitz::Range(hs_int, hs_int + Shape(1) - 1),
        blitz::Range(hs_int, hs_int + Shape(2) - 1));
    gradient[2] = dz2(
        blitz::Range(hz_int, hz_int + Shape(0) - 1),
        blitz::Range(hs_int, hs_int + Shape(1) - 1),
        blitz::Range(hs_int, hs_int + Shape(2) - 1));

    std::cout << "MSGVF done after " << count - 1 << " of " << max_iter
              << " iterations" << std::endl;
  }

} // namespace

#endif //guard
