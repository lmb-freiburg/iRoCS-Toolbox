/**************************************************************************
 *
 * Copyright (C) 2010 Thorsten Falk, Mario Emmenlauer
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

/**************************************************************************
**       Title: fftw Wrapper for fast fftw Calculation using blitz++ Arrays
**    $RCSfile$
**   $Revision: 4730 $$Name$
**       $Date: 2011-07-06 17:43:02 +0200 (Wed, 06 Jul 2011) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**   Wrapper for libfftw and libblitz++, providing arbitrary dimensional
**   fft's
**
**
**************************************************************************/
#ifndef BLITZFFTW_HH
#define BLITZFFTW_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "BlitzFFTWError.hh"
#include <fftw3.h>

#include <blitz/array.h>

#ifdef HAVE_BLITZ_V9
#include <blitz/tinyvec-et.h>
#endif

#include <set>

typedef int BlitzIndexT;

template <typename DataT>
struct BlitzFFTWPlan
{
};
template <>
struct BlitzFFTWPlan<double>
{
public:
  typedef fftw_plan real_fftw_plan;
};
template <>
struct BlitzFFTWPlan<float>
{
public:
  typedef fftwf_plan real_fftw_plan;
};


template<typename DataT>
class BLITZFFTWDLL_API BlitzFFTW
{
public:
  typedef typename BlitzFFTWPlan<DataT>::real_fftw_plan blitz_fftw_plan;

  enum PaddingType { VALUE, REPEATBORDER, MIRRORBORDER, CYCLICBORDER };
  enum DataPreservePolicy { PRESERVE, OVERWRITE };

  /*======================================================================*/
  /*!
   *   Get a handle to the Singleton BlitzFFTW Object. If no instance exists
   *   so far a new one will be created and the User/System Wisdom will be
   *   loaded
   *
   *   \param policy  The value of this enum decides how the library handles
   *                  given Arrays:
   *                    PRESERVE -  The contents of the input Arrays are
   *                                preserved under all circumstances. If
   *                                necessary copies are allocated, leading
   *                                to up to doubled memory consumption.
   *                    OVERWRITE - The library may overwrite given Arrays
   *                                as needed to avoid additional memory
   *                                usage. This means, that plan() above
   *                                ESTIMATE-mode will alter the input and
   *                                output Array. All data will be lost after
   *                                planning. execute() in c2r-Mode (backward
   *                                transform) overwrites the complex input
   *                                Array.
   *
   *   \return A pointer to the Singleton BlitzFFTW Object
   */
  /*======================================================================*/
  static BlitzFFTW* instance();

  /*======================================================================*/
  /*!
   *   Deletes the BlitzFFTW Singleton Object and frees all associated
   *   resources. Before removing the fftw data, the user wisdom will be
   *   updated
   */
  /*======================================================================*/
  static void clear();

  /*======================================================================*/
  /*!
   *   Loads the local user/system wisdom.
   *   This function is called by the Singleton-Constructor. There is
   *   normally no need to call it explicitly.
   */
  /*======================================================================*/
  void loadWisdom();

  /*======================================================================*/
  /*!
   *   Saves the local user wisdom.
   *   If you used hours for plan computation and don't want to waste
   *   that time again, save the plans to the wisdom file using this
   *   method. If you're done with FFT calculation you can call clear()
   *   instead. Additional to saving the wisdom it also frees used fftw
   *   resources.
   */
  /*======================================================================*/
  void saveWisdom() const;

  /*======================================================================*/
  /*!
   *   If you need to know the extents of a padded dataset without
   *   actually padding the data, you can use this lookup function that
   *   returns the shape vector that would result if an Array of the given
   *   shape would be padded using the pad method. This is useful if you
   *   need to plan a new set of fft's using the plan method.
   *
   *   IndexT should be BlitzIndexT. Templatization is only provided for
   *   backward compatibility to the old int indexing
   *
   *   \param minimumExtents  The extents for which an FFTW-optimal pad
   *                          should be computed
   *
   *   \return  The shape of the padded version of an Array of shape
   *            minimumExtents
   */
  /*======================================================================*/
  template<int Dim, typename IndexT>
  blitz::TinyVector<IndexT,Dim>
  getPaddedShape(const blitz::TinyVector<IndexT,Dim>& minimumExtents) const;

  /*======================================================================*/
  /*!
   *   Pads the given blitz::Array to Array-Extents that are suitable for
   *   fast FFT calculation. The padding strategy can be selected.
   *
   *   IndexT should be BlitzIndexT. Templatization is only provided for
   *   backward compatibility to the old int indexing
   *
   *   \param in              The Array to get a padded version of
   *   \param out             The Array padded data will be returned in.
   *                          You can give the same Array for in and out
   *   \param lb              Returns the upper left corner of the data
   *                          Range in the padded Array
   *   \param ub              Returns the lower right corner of the data
   *                          Range in the padded Array
   *   \param minimumExtents  The Array will be padded to at least these
   *                          extents
   *   \param paddingMode     Switches the padding behavior. At the moment
   *                          four modes are implemented:
   *      \li \c VALUE - Pads the Array with the fixed value provided in
   *         paddingValue
   *      \li \c REPEATBORDER - Fills the Pad with the values of the nearest
   *         data points
   *      \li \c MIRRORBORDER - Fills the Pad with the values of the mirrored
   *         data points
   *      \li \c CYCLICBORDER - Fills the Pad with the values of the cyclicly
   *         continued array
   *   \param paddingValue    In padding mode VALUE, the Array will be
   *                          padded using this fixed value
   */
  /*======================================================================*/
  template<int Dim, typename IndexT>
  void pad(const blitz::Array<DataT,Dim>& in,
           blitz::Array<DataT,Dim>& out,
           blitz::TinyVector<IndexT,Dim>& lb,
           blitz::TinyVector<IndexT,Dim>& ub,
           const blitz::TinyVector<IndexT,Dim>& minimumExtents = IndexT(0),
           const PaddingType paddingMode = VALUE,
           const DataT paddingValue = static_cast<DataT>(0)) const;

  /*======================================================================*/
  /*!
   *   Pads the given complex blitz::Array to Array-Extents that are
   *   suitable for fast FFT calculation. The padding strategy can be
   *   selected.
   *
   *   IndexT should be BlitzIndexT. Templatization is only provided for
   *   backward compatibility to the old int indexing
   *
   *   \param in              The Array to get a padded version of
   *   \param out             The Array padded data will be returned in.
   *                          You can give the same Array for in and out
   *   \param lb              Returns the upper left corner of the data
   *                          Range in the padded Array
   *   \param ub              Returns the lower right corner of the data
   *                          Range in the padded Array
   *   \param minimumExtents  The Array will be padded to at least these
   *                          extents
   *   \param paddingMode     Switches the padding behavior. At the moment
   *                          four modes are implemented:
   *      \li \c VALUE - Pads the Array with the fixed value provided in
   *         paddingValue
   *      \li \c REPEATBORDER - Fills the Pad with the values of the nearest
   *         data points
   *      \li \c MIRRORBORDER - Fills the Pad with the values of the mirrored
   *         data points
   *      \li \c CYCLICBORDER - Fills the Pad with the values of the cyclicly
   *         continued array
   *   \param paddingValue    In padding mode VALUE, the Array will be
   *                          padded using this fixed value
   */
  /*======================================================================*/
  template<int Dim, typename IndexT>
  void pad(const blitz::Array<std::complex<DataT>,Dim>& in,
           blitz::Array<std::complex<DataT>,Dim>& out,
           blitz::TinyVector<IndexT,Dim>& lb,
           blitz::TinyVector<IndexT,Dim>& ub,
           const blitz::TinyVector<IndexT,Dim>& minimumExtents = IndexT(0),
           const PaddingType paddingMode = VALUE,
           const std::complex<DataT> paddingValue =
           static_cast< std::complex<DataT> >(0)) const;

  /*======================================================================*/
  /*!
   *   Unpads the given blitz::Array using the Range corners provided
   *
   *   IndexT should be BlitzIndexT. Templatization is only provided for
   *   backward compatibility to the old int indexing
   *
   *   \param in              The Array to unpad
   *   \param out             The Array unpadded data will be returned in.
   *                          You can give the same Array for in and out
   *   \param lb              The upper left corner of the data Range
   *                          to retrieve
   *   \param ub              The lower right corner of the data Range
   *                          to retrieve
   */
  /*======================================================================*/
  template<int Dim, typename IndexT>
  void unpad(const blitz::Array<DataT,Dim>& in,
             blitz::Array<DataT,Dim>& out,
             const blitz::TinyVector<IndexT,Dim>& lb,
             const blitz::TinyVector<IndexT,Dim>& ub) const;

  /*======================================================================*/
  /*!
   *   Unpads the given complex blitz::Array using the Range corners provided
   *
   *   IndexT should be BlitzIndexT. Templatization is only provided for
   *   backward compatibility to the old int indexing
   *
   *   \param in              The Array to unpad
   *   \param out             The Array unpadded data will be returned in.
   *                          You can give the same Array for in and out
   *   \param lb              The upper left corner of the data Range
   *                          to retrieve
   *   \param ub              The lower right corner of the data Range
   *                          to retrieve
   */
  /*======================================================================*/
  template<int Dim, typename IndexT>
  void unpad(const blitz::Array<std::complex<DataT>,Dim>& in,
             blitz::Array<std::complex<DataT>,Dim>& out,
             const blitz::TinyVector<IndexT,Dim>& lb,
             const blitz::TinyVector<IndexT,Dim>& ub) const;

  /*======================================================================*/
  /*!
   *   Use this method to compute a new fftw Plan, for execution of a
   *   transform of a pair of Arrays of the extents provided. If there
   *   already exists a plan (either through former ffts or from a wisdom
   *   file), it will be used, otherwise a new plan will be computed.
   *   This is the version for forward FFT real to complex
   *
   *   \param in         blitz++-Array containing real valued data entries.
   *                     The contents are irrelevant, it only has to be given
   *                     with the correct extents. In fact, the Array should not
   *                     be initialized, because depending on the planning mode
   *                     data will be overwritten!
   *   \param out        A blitz++-Array containing complex-valued data entries.
   *                     It's extents should be equal to the extents
   *                     of the input Array except for the last Dimension,
   *                     which should be out[Dim - 1] = in[Dim - 1] / 2 + 1.
   *   \param plan_flags With these flags you can customize the planning mode
   *                     Usually planning is performed in exhaustive mode
   *                     allowing destruction of the input data array.
   *                     Possible flags are:
   *                       FFTW_ESTIMATE   (fastest, but may yield bad
   *                                        transform performance)
   *                       FFTW_MEASURE    (slower, medium
   *                                        transform performance)
   *                       FFTW_PATIENT    (even slower, slightly better
   *                                        transform performance)
   *                       FFTW_EXHAUSTIVE (slowest, guarantees best
   *                                        transform performance)
   */
  /*======================================================================*/
  template<int Dim>
  void plan_forward(blitz::Array<DataT,Dim>& in,
                    blitz::Array<std::complex<DataT>,Dim>& out,
                    const DataPreservePolicy policy = PRESERVE,
                    const unsigned int plan_flags = FFTW_MEASURE) const;

  /*======================================================================*/
  /*!
   *   Use this method to compute a new fftw Plan, for execution of a
   *   transform of a pair of Arrays of the extents provided. If there
   *   already exists a plan (either through former ffts or from a wisdom
   *   file), it will be used, otherwise a new plan will be computed.
   *   This is the version for backward FFT complex to real
   *
   *   \param in         A blitz++-Array containing complex-valued data entries.
   *                     The contents are irrelevant, it only has to be given
   *                     with the correct extents. In fact, the Array should not
   *                     be initialized, because depending on the planning mode
   *                     data will be overwritten!
   *   \param out        blitz++-Array containing real valued data entries.
   *                     It's extents should be equal to the extents
   *                     of the input Array except for the last Dimension,
   *                     which should be out[Dim - 1] = (in[Dim - 1] - 1) * 2.
   *   \param plan_flags With these flags you can customize the planning mode
   *                     Usually planning is performed in exhaustive mode
   *                     allowing destruction of the input data array.
   *                     Possible flags are:
   *                       FFTW_ESTIMATE   (fastest, but may yield bad
   *                                        transform performance)
   *                       FFTW_MEASURE    (slower, medium
   *                                        transform performance)
   *                       FFTW_PATIENT    (even slower, slightly better
   *                                        transform performance)
   *                       FFTW_EXHAUSTIVE (slowest, guarantees best
   *                                        transform performance)
   */
  /*======================================================================*/
  template<int Dim>
  void plan_backward(blitz::Array<std::complex<DataT>,Dim>& in,
                     blitz::Array<DataT,Dim>& out,
                     const DataPreservePolicy policy = PRESERVE,
                     const unsigned int plan_flags = FFTW_MEASURE) const;


  /*======================================================================*/
  /*!
   *   Use this method to compute a new fftw Plan, for execution of a
   *   transform of a pair of Arrays of the extents provided. If there
   *   already exists a plan (either through former ffts or from a wisdom
   *   file), it will be used, otherwise a new plan will be computed.
   *   This is the version for backward FFT complex to real
   *
   *   \param in         A blitz++-Array containing complex-valued data entries.
   *                     The contents are irrelevant, it only has to be given
   *                     with the correct extents. In fact, the Array should not
   *                     be initialized, because depending on the planning mode
   *                     data will be overwritten!
   *   \param out        blitz++-Array containing complex-valued data entries.
   *                     It's extents should be equal to the extents
   *                     of the input Array.
   *   \param plan_flags With these flags you can customize the planning mode
   *                     Usually planning is performed in exhaustive mode
   *                     allowing destruction of the input data array.
   *                     Possible flags are:
   *                       FFTW_ESTIMATE   (fastest, but may yield bad
   *                                        transform performance)
   *                       FFTW_MEASURE    (slower, medium
   *                                        transform performance)
   *                       FFTW_PATIENT    (even slower, slightly better
   *                                        transform performance)
   *                       FFTW_EXHAUSTIVE (slowest, guarantees best
   *                                        transform performance)
   */
  /*======================================================================*/
  template<int Dim>
  void plan_forward(blitz::Array<std::complex<DataT>,Dim>& in,
                    blitz::Array<std::complex<DataT>,Dim>& out,
                    const DataPreservePolicy policy = PRESERVE,
                    const unsigned int plan_flags = FFTW_MEASURE) const;

  /*---------------------------------------------------------------------
   * same as plan_forward, but returns plan for further use with
   * execute_plan (guru interface). Useful mainly for multithreading
   *--------------------------------------------------------------------*/
  template<int Dim>
  blitz_fftw_plan
  get_plan_forward(blitz::Array<DataT,Dim>& in,
                   blitz::Array<std::complex<DataT>,Dim>& out,
                   const DataPreservePolicy policy = PRESERVE,
                   const unsigned int plan_flags = FFTW_MEASURE) const;

  template<int Dim>
  blitz_fftw_plan
  get_plan_forward(blitz::Array<std::complex<DataT>,Dim>& in,
                   blitz::Array<std::complex<DataT>,Dim>& out,
                   const DataPreservePolicy policy = PRESERVE,
                   const unsigned int plan_flags = FFTW_MEASURE) const;


  /*======================================================================*/
  /*!
   *   Use this method to compute a new fftw Plan, for execution of a
   *   transform of a pair of Arrays of the extents provided. If there
   *   already exists a plan (either through former ffts or from a wisdom
   *   file), it will be used, otherwise a new plan will be computed.
   *   This is the version for backward FFT complex to real
   *
   *   \param in         A blitz++-Array containing complex-valued data entries.
   *                     The contents are irrelevant, it only has to be given
   *                     with the correct extents. In fact, the Array should not
   *                     be initialized, because depending on the planning mode
   *                     data will be overwritten!
   *   \param out        blitz++-Array containing complex-valued data entries.
   *                     It's extents should be equal to the extents
   *                     of the input Array.
   *   \param plan_flags With these flags you can customize the planning mode
   *                     Usually planning is performed in exhaustive mode
   *                     allowing destruction of the input data array.
   *                     Possible flags are:
   *                       FFTW_ESTIMATE   (fastest, but may yield bad
   *                                        transform performance)
   *                       FFTW_MEASURE    (slower, medium
   *                                        transform performance)
   *                       FFTW_PATIENT    (even slower, slightly better
   *                                        transform performance)
   *                       FFTW_EXHAUSTIVE (slowest, guarantees best
   *                                        transform performance)
   */
  /*======================================================================*/
  template<int Dim>
  void plan_backward(blitz::Array<std::complex<DataT>,Dim>& in,
                     blitz::Array<std::complex<DataT>,Dim>& out,
                     const DataPreservePolicy policy = PRESERVE,
                     const unsigned int plan_flags = FFTW_MEASURE) const;

  /*---------------------------------------------------------------------
   * same as plan_backward, but returns plan for further use with execute_plan
   * (guru interface)
   * useful mainly for multithreading :-)
   *--------------------------------------------------------------------*/
  template<int Dim>
  blitz_fftw_plan
  get_plan_backward(blitz::Array<std::complex<DataT>,Dim>& in,
                    blitz::Array<DataT,Dim>& out,
                    const DataPreservePolicy policy = PRESERVE,
                    const unsigned int plan_flags = FFTW_MEASURE) const;

  template<int Dim>
  blitz_fftw_plan
  get_plan_backward(blitz::Array<std::complex<DataT>,Dim>& in,
                    blitz::Array<std::complex<DataT>,Dim>& out,
                    const DataPreservePolicy policy = PRESERVE,
                    const unsigned int plan_flags = FFTW_MEASURE) const;


  /*======================================================================*/
  /*!
   *   Use this method for the actual real2complex transform. Before
   *   transformation a plan is selected. If no plan for the current Array-
   *   combination exists it will be computed using the FFTW_ESTIMATE method.
   *   If you want to do many transforms with Arrays of the same shape it
   *   is highly recommended to use fftPlan() once before.
   *   This is the forward transform real to complex.
   *
   *   \param in   The input data Array containing the float data to transform
   *   \param out  The complex output Array, the transform will be written
   *               to
   */
  /*======================================================================*/
  template<int Dim>
  void forward(const blitz::Array<DataT,Dim>& in,
               blitz::Array<std::complex<DataT>,Dim>& out) const;

  /*======================================================================*/
  /*!
   *   Use this method for the actual complex2complex forward transform.
   *   Before transformation a
   *   plan is selected. If no plan for the current Array-combination exists
   *   it will be computed using the FFTW_ESTIMATE method. If you want to do
   *   many transforms with Arrays of the same shape it is highly recommended
   *   to use fftPlan() once before.
   *   This is the forward transform real to complex.
   *
   *   \param in   The input data Array containing the float data to transform
   *   \param out  The complex output Array, the transform will be written
   *               to
   */
  /*======================================================================*/
  template<int Dim>
  void forward(blitz::Array<std::complex<DataT>,Dim>& in,
               blitz::Array<std::complex<DataT>,Dim>& out) const;



  /*======================================================================*/
  /*!
   *   Use this method for the actual complex2real transform. Before
   *   transformation a plan is selected. If no plan for the current Array-
   *   combination exists it will be computed using the FFTW_ESTIMATE method.
   *   If you want to do many transforms with Arrays of the same shape it
   *   is highly recommended to use fftPlan() once before.
   *   This is the backward transform complex to real.
   *
   *   \param in   The complex input Array containing the FFT to transform
   *               back
   *   \param out  The float array to save the back transformed data into
   */
  /*======================================================================*/
  template<int Dim>
  void backward(blitz::Array<std::complex<DataT>,Dim>& in,
                blitz::Array<DataT,Dim>& out,
                const DataPreservePolicy policy = PRESERVE) const;


  /*======================================================================*/
  /*!
   *   Use this method for the actual complex2complex back  transform.
   *   Before transformation a
   *   plan is selected. If no plan for the current Array-combination exists
   *   it will be computed using the FFTW_ESTIMATE method. If you want to do
   *   many transforms with Arrays of the same shape it is highly recommended
   *   to use fftPlan() once before.
   *   This is the backward transform complex to real.
   *
   *   \param in   The complex input Array containing the FFT to transform
   *               back
   *   \param out  The float array to save the back transformed data into
   */
  /*======================================================================*/
  template<int Dim>
  void backward(blitz::Array<std::complex<DataT>,Dim>& in,
                blitz::Array<std::complex<DataT>,Dim>& out,
                const DataPreservePolicy policy = PRESERVE) const;

  /*---------------------------------------------------------------------
   * (guru interface) executes pre_computet plan with different data
   * use get_plan_X to precompute plans.
   * useful mainly for multithreading :-)
   * THREAD SAVE!
   *--------------------------------------------------------------------*/
  template<int Dim>
  void exec_guru_plan(blitz::Array<std::complex<DataT>,Dim>& in,
                      blitz::Array<std::complex<DataT>,Dim>& out,
                      blitz_fftw_plan plan,
                      const DataPreservePolicy policy = PRESERVE) const;


  template<int Dim>
  void exec_guru_plan_r2c(blitz::Array<DataT,Dim>& in,
                          blitz::Array<std::complex<DataT>,Dim>& out,
                          blitz_fftw_plan plan,
                          const DataPreservePolicy policy = PRESERVE) const;

  template<int Dim>
  void exec_guru_plan_c2r(blitz::Array<std::complex<DataT>,Dim>& in,
                          blitz::Array<DataT,Dim>& out,
                          blitz_fftw_plan plan,
                          const DataPreservePolicy policy = PRESERVE) const;


  /*======================================================================*/
  /*!
   *   Reorder the Array such that the original order is reconstructed.
   *   This is needed after a fftw forward/backward-transform, because
   *   the ordering is destroyed when computing fftws.
   *
   *   \param in   The input Array containing the FFT to unshuffle
   *   \param out  The float array to save the unshuffled data into
   */
  /*======================================================================*/
  template<int Dim>
  void unShuffle(const blitz::Array<DataT,Dim>& in,
                 blitz::Array<DataT,Dim>& out) const;


  /*======================================================================*/
  /*!
   *   Return the next best size (>= the given) for a FFT.
   *
   *   \param in   The size of the data you want to transform
   *   \param out  The next bigger size that the fftw performs well
   *               for
   */
  /*======================================================================*/
  static size_t nextBestFFTSize(const size_t size);
  static size_t prevBestFFTSize(const size_t size);
  static const std::set<size_t>& nextBestFFTSizes();

  template<int Dim>
  void translate(const blitz::Array<DataT,Dim>& data,
                 blitz::Array<DataT,Dim>& dataTrans,
                 const BlitzIndexT t,
                 const int d) const;

  template<int Dim>
  void translate(const blitz::Array<DataT,Dim>& data,
                 blitz::Array<DataT,Dim>& dataTrans,
                 const blitz::TinyVector<BlitzIndexT,Dim>& t) const;

private:

  BlitzFFTW();
  BlitzFFTW(const BlitzFFTW<DataT>& original);
  ~BlitzFFTW();

  class BlitzFFTWDestructor
  {
  public:
    ~BlitzFFTWDestructor()
          {
            if (BlitzFFTW::p_instance != 0) {
              delete BlitzFFTW::p_instance;
              BlitzFFTW::p_instance = 0;
            }
          }
    void init()
          {}
  };
  friend class BlitzFFTWDestructor;

  static const size_t maxPrepareFFTSize = 65535;
  static void prepareFFTSizes();
  static std::set<size_t> _bestFFTSizes;
  static BlitzFFTW* p_instance;



  // These are the function pointers to the fftw library. They are needed
  // because fftw has name mangeling depending on the datatype:
  // fftwf for loat, fftw for double and fftwl for long double.
  typedef DataT blitz_fftw_complex[2];

  void (*blitz_fftw_execute)(const blitz_fftw_plan p);
  blitz_fftw_plan (*blitz_fftw_plan_dft)(int rank, const int *n, blitz_fftw_complex *in, blitz_fftw_complex *out, int sign, unsigned flags);
  void (*blitz_fftw_execute_dft)(const blitz_fftw_plan p, blitz_fftw_complex *in, blitz_fftw_complex *out);
  blitz_fftw_plan (*blitz_fftw_plan_dft_r2c)(int rank, const int *n, DataT *in, blitz_fftw_complex *out, unsigned flags);
  blitz_fftw_plan (*blitz_fftw_plan_dft_c2r)(int rank, const int *n, blitz_fftw_complex *in, DataT *out, unsigned flags);
  void (*blitz_fftw_execute_dft_r2c)(const blitz_fftw_plan p, DataT *in, blitz_fftw_complex *out);
  void (*blitz_fftw_execute_dft_c2r)(const blitz_fftw_plan p, blitz_fftw_complex *in, DataT *out);
  void (*blitz_fftw_destroy_plan)(blitz_fftw_plan p);
  void (*blitz_fftw_cleanup)(void);
  void *(*blitz_fftw_malloc)(size_t n);

};

// Template specializations

template<>
BlitzFFTW<float>*
BlitzFFTW<float>::instance();

template<>
BlitzFFTW<double>*
BlitzFFTW<double>::instance();

#ifndef BLITZFFTW_HH_NO_INCLUDE_ICC
#include "BlitzFFTW.icc"
#endif

#endif

