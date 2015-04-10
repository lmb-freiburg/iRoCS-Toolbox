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

#ifndef ATBDATASYNTHESIS_HH
#define ATBDATASYNTHESIS_HH

#include "Array.hh"

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <cmath>

namespace atb
{
   
  const int DEFAULT                     = 0x00000000;
  const int NORMALIZE                   = 0x00000001;
  const int NORESIZE                    = 0x00000002;
  const int FORCESAMPLING               = 0x00000004;

  enum ErrorFunctionType{ Erf, ErfC, ErfCX };

/*======================================================================*/
/*! 
 *   General evaluation of erf, erfc and erfcx, where erf is the
 *   error function given as:
 *
 *   \f[\mathrm{erf}(x) =
 *      \frac{1}{\sqrt(\pi)} \cdot \int_0^x e^{-t^2} ,\mathrm{d}t\f]
 *
 *   erfc is the complementary error function: 
 *
 *   \f[\mathrm{erfc}(x) = 1 - \mathrm{erf}(x)\f]
 *
 *   and erfcx is the scaled complementary error function
 *
 *   \f[\mathrm{erfcx}(x) = e^{x^2} \cdot \mathrm{erfc}(x)\f]
 *
 *   \param x    The position to evaluate the error function at.
 *   \param type The type of error function to evaulate
 *
 *   \return The function value at the specified position
 */
/*======================================================================*/
  double erfbase(const double x, ErrorFunctionType type = Erf);

/*======================================================================*/
/*! 
 *   Evaluate the Gaussian distribution with mean mu and standard deviation
 *   sigma at position x. If the standard deviation is zero the function
 *   returns 1 if x equals mu, 0 otherwise (use with care!).
 *
 *   \f[\mathcal{G}(x) := \frac{1}{\sqrt{2 \pi} \sigma} e^{
 *       -\frac{(x - \mu)^2}{2 \sigma^2}}\f]
 *
 *   \param x The position to evaluate the Gaussian at
 *   \param mu The mean of the Gaussian
 *   \param sigma The standard deviation of the Gaussian
 *
 *   \return The Gaussian probability for input x
 */
/*======================================================================*/
  inline double gaussian(double x, double mu, double sigma)
  {
    if (sigma == 0.0) return (x == mu) ? 1.0 : 0.0;
    return std::exp(-0.5 * (x - mu) * (x - mu) / (sigma * sigma)) /
        (std::sqrt(2.0 * M_PI) * sigma);
  }

/*======================================================================*/
/*! 
 *   Evaluate the normalized Hermite polynomial \f$\tilde{H}\f$ at position x.
 *   The polynomial is normalized for the generation of Gaussian derivatives
 *   with standard deviation sigma.
 *
 *        \f[\begin{array}{rcl}
 *          \tilde{H}_0(x)     & = & 1 \\
 *          \tilde{H}_1(x)     & = & \frac{1}{\sigma^2}x \\
 *          \tilde{H}_{n+1}(x) & = & \frac{1}{\sigma^2}(x \tilde{H}_n(x) -
 *                                                      n \tilde{H}_{n-1}(x))
 *        \end{array}\f]
 *
 *   \param x     The position to evaluate the polynomial at
 *   \param sigma The standard deviation of the Gaussian
 *   \param d     The degree of the polynomial
 *
 *   \return The value of the normalized Hermite polynomial at position x
 */
/*======================================================================*/ 
  inline double normalizedHermitePolynomial(double x, double sigma, int d) 
  {
    if (d < 0) return 0.0;
    if (d == 0) return 1.0;
    if (d == 1) return 1.0 / (sigma * sigma) * x;
    return (x * normalizedHermitePolynomial(x, sigma, d - 1) -
            (d - 1) * normalizedHermitePolynomial(x, sigma, d - 2)) /
        (sigma * sigma);
  }

/*======================================================================*/
/*! 
 *   Evaluate the Gaussian derivative of degree d with mean mu and standard
 *   deviation sigma at position x.
 *
 *   \f[\frac{\mathrm{d}^d}{\mathrm{d}x^d} \mathcal{G}(x) :=
 *     \frac{1}{\sqrt{2 \pi} \sigma} \mathcal{H}_{d,\sigma}(x - \mu) \cdot
 *     e^{-\frac{(x - \mu)^2}{2 \sigma^2}}\f]
 *
 *   where \f$\mathcal{H}_{d,\sigma}(x)\f$ is the normalized Hermite
 *   polynomial. 
 *
 *   \param x The position to evaluate the Gaussian at
 *   \param mu The mean of the Gaussian
 *   \param sigma The standard deviation of the Gaussian
 *
 *   \return The Gaussian probability for input x
 *   \see normalizedHermitePolynomial()
 */
/*======================================================================*/
  inline double gaussianDerivative(double x, double mu, double sigma, int d)
  {
    return ((d % 2 == 0) ? 1.0 : -1.0) *
        normalizedHermitePolynomial(x - mu, sigma, d) * gaussian(x, mu, sigma);
  }
  
/*======================================================================*/
/*! 
 *   Calculate gaussian with non-uniform standard deviation sigmaUm. If
 *   sigmaUm is below the elementSize, the sampling will be replaced
 *   by actual integration over the pixel area to retain the Integral
 *   which may lead to artifacts when using the gaussian as a filter
 *   kernel. This has to be investigated further. You can enforce sampling
 *   in any case by giving the FORCESAMPLING flag
 *
 *   \param gauss        The output Array to store the gaussian to (if
 *                       noresize is not given, it will be resized
 *                       to fit the gaussian to an extent of 3 * sigmaUm in any
 *                       direction)
 *   \param sigmaUm      Standard deviation of the gaussian in either direction
 *                       in micrometers
 *   \param elementSize  The gaussian parameters are assumed to be in micron
 *                       so to render an appropriate gaussian for your Array
 *                       data provide the element size here
 *   \param flags        The following flags can be given:\n
 *                         NORESIZE  - Preserve the given output Array
 *                            dimensions\n
 *                         NORMALIZE - The sum of the sampled Gaussian will
 *                            be normalized to one before returning it
 */
/*======================================================================*/
  template<typename DataT, int Dim>
  void gaussian(blitz::Array<DataT,Dim>& gauss,
                blitz::TinyVector<double,Dim> const &sigmaUm,
                blitz::TinyVector<double,Dim> const &elementSizeUm,
                int flags = DEFAULT);

/*======================================================================*/
/*! 
 *   Calculate gaussian with non-uniform standard deviation sigmaUm. If
 *   sigmaUm is below the elementSize, the sampling will be replaced
 *   by actual integration over the pixel area to retain the Integral
 *   which may lead to artifacts when using the gaussian as a filter
 *   kernel. This has to be investigated further. You can enforce sampling
 *   in any case by giving the FORCESAMPLING flag
 *
 *   \param gauss        The output Array to store the gaussian to (if
 *                       noresize is not given, it will be resized
 *                       to fit the gaussian to an extent of 3 * sigmaUm in any
 *                       direction)
 *   \param sigmaUm      Standard deviation of the gaussian in either direction
 *                       in micrometers
 *   \param flags        The following flags can be given:\n
 *                         NORESIZE  - Preserve the given output Array
 *                            dimensions\n
 *                         NORMALIZE - The sum of the sampled Gaussian will
 *                            be normalized to one before returning it
 */
/*======================================================================*/
  template<typename DataT, int Dim>
  void gaussian(Array<DataT,Dim>& gauss,
                blitz::TinyVector<double,Dim> const &sigmaUm,
                int flags = DEFAULT);

/*======================================================================*/
/*! 
 *   Compute gaussian partial derivative filter according to specification
 *   given in d
 *
 *   \param gauss        The output Array to store the gaussian to (if
 *                       noresize is not given, it will be resized
 *                       to fit the gaussian to an extent of 3 * sigmaUm in any
 *                       direction)
 *   \param sigmaUm      Standard deviations of the gaussian in micron.
 *                       The resulting filter will be anisotropic but aligned
 *                       to the axes. This is the usual case for anisotropic
 *                       element sizes
 *   \param elementSize  The gaussian parameters are assumed to be in micron
 *                       so to render an appropriate gaussian derivative
 *                       for your Array data provide the element size here
 *   \param d            The partial derivative specification. Each entry in
 *                       this vector indicates the degree of the derivative
 *                       along the corresponding direction.
 *                       Example:
 *                         Dim = 3, d = (2, 1, 0)
 *                         generates a filter corresponding to
 *        \f[\frac{\partial^3}{\partial x_0^2 \partial x_1}
 *           \mathcal{G}_{\mu, \sigma}(\vec{x})\f]
 *   \param flags        The following flags can be given:\n
 *                         NORESIZE  - Preserve the given output Array
 *                            dimensions\n
 */
/*======================================================================*/
  template<typename DataT, int Dim>
  void gaussianDerivative(blitz::Array<DataT,Dim>& gauss,
                          blitz::TinyVector<double,Dim> const &sigmaUm,
                          blitz::TinyVector<double,Dim> const &elementSizeUm,
                          blitz::TinyVector<int,Dim> const &d,
                          int flags = DEFAULT);

/*======================================================================*/
/*! 
 *   Compute gaussian partial derivative filter according to specification
 *   given in d
 *
 *   \param gauss        The output Array to store the gaussian to (if
 *                       noresize is not given, it will be resized
 *                       to fit the gaussian to an extent of 3 * sigmaUm in any
 *                       direction)
 *   \param sigmaUm      Standard deviations of the gaussian in micron.
 *                       The resulting filter will be anisotropic but aligned
 *                       to the axes. This is the usual case for anisotropic
 *                       element sizes
 *   \param d            The partial derivative specification. Each entry in
 *                       this vector indicates the degree of the derivative
 *                       along the corresponding direction.
 *                       Example:
 *                         Dim = 3, d = (2, 1, 0)
 *                         generates a filter corresponding to
 *        \f[\frac{\partial^3}{\partial x_0^2 \partial x_1}
 *           \mathcal{G}_{\mu, \sigma}(\vec{x})\f]
 *   \param flags        The following flags can be given:\n
 *                         NORESIZE  - Preserve the given output Array
 *                            dimensions\n
 */
/*======================================================================*/
  template<typename DataT, int Dim>
  void gaussianDerivative(Array<DataT,Dim>& gauss,
                          blitz::TinyVector<double,Dim> const &sigmaUm,
                          blitz::TinyVector<int,Dim> const &d,
                          int flags = DEFAULT);

}

#include "ATBDataSynthesis.icc"

#endif
