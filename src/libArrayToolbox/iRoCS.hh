/*======================================================================*/
/*!
 *  \file iRoCS.hh
 *  \brief The intrinsic root coordinate system.
 *
 *    The corresponding class provides functions for registration to
 *    different kinds of root image data and functions for transforming
 *    Euclidean positions into iRoCS and vice versa.
 */
/*======================================================================*/

#ifndef ATBIROCS_HH
#define ATBIROCS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <vector>
#include <blitz/array.h>

#include <libProgressReporter/ProgressReporter.hh>
#include <libBlitzHdf5/BlitzHdf5Light.hh>

#include "ATBCoupledBSplineModel.hh"
#include "ATBNucleus.hh"
#include "Array.hh"
#include "Interpolator.hh"

namespace atb
{
  
/*======================================================================*/
/*!
 *  \class IRoCS iRoCS.hh "libArrayToolbox/Array.hh"
 *  \brief The IRoCS class provides means to attach iRoCS to different
 *    kinds of processed root images.
 *
 *  It contains all functions necessary to transform Euclidean image
 *  coordinates (z,y,x) (in micrometers) to iRoCS coordinates (z,r,phi)
 *  and vice-versa. It also provides functions to load and save the 
 *  coordinate system in a format understood by labelling. You can access
 *  the underlying coupled curves model from within this class and
 *  manipulate it if required.
 */
/*======================================================================*/
  class IRoCS
  {
    
  public:
    
/*======================================================================*/
/*! 
 *   Default constructor.
 *
 *   \param progressReporter If given, fitting progress will be output
 *     through the given iRoCS::ProgressReporter. The user has to ensure,
 *     that the progress reporter is available during the lifetime of the
 *     IRoCS object or until it is detached using setProgressReporter().
 */
/*======================================================================*/
    IRoCS(iRoCS::ProgressReporter *progressReporter = NULL);

/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    ~IRoCS();
    
/*======================================================================*/
/*! 
 *   Attach a iRoCS::ProgressReporter through which progress will be output.
 *   You can pass \c NULL here if you want to disable progress reporting.
 *
 *   \param progressReporter The iRoCS::ProgressReporter through which output
 *     will be passed to the caller
 */
/*======================================================================*/
    void setProgressReporter(iRoCS::ProgressReporter *progressReporter);
    
/*======================================================================*/
/*! 
 *   Fit iRoCS to a given list of nuclei. Only nuclei with label 2 (epidermis)
 *   will be used, all others will not influence the fit. For fitting, the
 *   coupled curves model is applied.
 *
 *   Schmidt, T. et al. Modeling of Sparsely Sampled Tubular Surfaces Using
 *   Coupled Curves. LNCS, 7476, pp. 83-92, 2012
 *
 *   \param qcPositionUm The Euclidean position (z,y,x) of the quiescent
 *     centre (QC) in micrometers. The root axis does not need to pass
 *     through the given point, but its projection onto the iRoCS axis is
 *     used as z-origin.
 *   \param nuclei The nuclei to fit iRoCS to. Only nuclei with label 2 will
 *     be considered.
 *   \param kappa  The data term weight of the coupled curves model
 *   \param lambda The weight for the smoothness regularizer. Since the
 *     underlying splines are intrinsically smooth, this parameter is only
 *     needed if the model starts to ascillate around the real solution.
 *   \param mu     The weight for the thickness constancy regularizer.
 *     Use this parameter if you want to enforce low thickness variations
 *     because you know, that the tube has no such variation. For roots it
 *     should be chosen small to allow the model to adapt to the thickness
 *     variations of the root.
 *   \param nIter  The maximum number of iterations until the model is
 *     reparametrized. Choose this large to avoid the model to end up
 *     in local optima. The default leads to good results in most cases.
 *   \param tau    The initial evolution time step. Bigger values for tau
 *     increase the evolution speed in the beginning, but the risk of
 *     oscillations which again reduce the convergence speed increases. During
 *     the optimization tau will be gradually reduced if no Energy decrease
 *     could be reached with the current step size.
 *   \param searchRadiusUm This parameter is the threshold at which the loss
 *     function psi becomes saturated. Low values reduce the evolution speed
 *     but also reduce the risk of 'jumping' out of the tube into neighboring
 *     structures. When fitting to one single straight root segment the
 *     radius should be chosen large to increase performance and reduce the
 *     risk of early termination due to low nucleus density. If a value less
 *     than or equal to zero is given here, the distance is automatically
 *     determined and set to half the average distance between neighboring
 *     epidermis markers along the point cloud's main axis.
 */
/*======================================================================*/
    void fit(blitz::TinyVector<double,3> const &qcPositionUm,
             std::list<Nucleus> const &nuclei,
             double kappa = 1.0, double lambda = 0.0, double mu = 0.0,
             int nIter = 1000, double tau = 0.1, double searchRadiusUm = 0.0);

/*======================================================================*/
/*! 
 *   Fit iRoCS to a given vector of nuclei. Only nuclei with label 2 (epidermis)
 *   will be used, all others will not influence the fit. For fitting, the
 *   coupled curves model is applied.
 *
 *   Schmidt, T. et al. Modeling of Sparsely Sampled Tubular Surfaces Using
 *   Coupled Curves. LNCS, 7476, pp. 83-92, 2012
 *
 *   \param qcPositionUm The Euclidean position (z,y,x) of the quiescent
 *     centre (QC) in micrometers. The root axis does not need to pass
 *     through the given point, but its projection onto the iRoCS axis is
 *     used as z-origin.
 *   \param nuclei The nuclei to fit iRoCS to. Only nuclei with label 2 will
 *     be considered.
 *   \param kappa  The data term weight of the coupled curves model
 *   \param lambda The weight for the smoothness regularizer. Since the
 *     underlying splines are intrinsically smooth, this parameter is only
 *     needed if the model starts to ascillate around the real solution.
 *   \param mu     The weight for the thickness constancy regularizer.
 *     Use this parameter if you want to enforce low thickness variations
 *     because you know, that the tube has no such variation. For roots it
 *     should be chosen small to allow the model to adapt to the thickness
 *     variations of the root.
 *   \param nIter  The maximum number of iterations until the model is
 *     reparametrized. Choose this large to avoid the model to end up
 *     in local optima. The default leads to good results in most cases.
 *   \param tau    The initial evolution time step. Bigger values for tau
 *     increase the evolution speed in the beginning, but the risk of
 *     oscillations which again reduce the convergence speed increases. During
 *     the optimization tau will be gradually reduced if no Energy decrease
 *     could be reached with the current step size.
 *   \param searchRadiusUm This parameter is the threshold at which the loss
 *     function psi becomes saturated. Low values reduce the evolution speed
 *     but also reduce the risk of 'jumping' out of the tube into neighboring
 *     structures. When fitting to one single straight root segment the
 *     radius should be chosen large to increase performance and reduce the
 *     risk of early termination due to low nucleus density. If a value less
 *     than or equal to zero is given here, the distance is automatically
 *     determined and set to half the average distance between neighboring
 *     epidermis markers along the point cloud's main axis.
 */
/*======================================================================*/
    void fit(blitz::TinyVector<double,3> const &qcPositionUm,
             std::vector<Nucleus> const &nuclei,
             double kappa = 1.0, double lambda = 0.0, double mu = 0.0,
             int nIter = 1000, double tau = 0.1, double searchRadiusUm = 0.0);

/*======================================================================*/
/*! 
 *   Fit iRoCS to a given cellular segmentation. For fitting, the
 *   coupled curves model is applied to the perimeter pixels of the root.
 *
 *   Schmidt, T. et al. Modeling of Sparsely Sampled Tubular Surfaces Using
 *   Coupled Curves. LNCS, 7476, pp. 83-92, 2012
 *
 *   \param qcPositionUm The Euclidean position (z,y,x) of the quiescent
 *     centre (QC) in micrometers. The root axis does not need to pass
 *     through the given point, but its projection onto the iRoCS axis is
 *     used as z-origin.
 *   \param segmentation The Array containing the segmentation masks.
 *     Segments with value below 2 will be ignored, since 0 is the label
 *     for the edges between the segments and 1 the background label.
 *   \param kappa  The data term weight of the coupled curves model
 *   \param lambda The weight for the smoothness regularizer. Since the
 *     underlying splines are intrinsically smooth, this parameter is only
 *     needed if the model starts to ascillate around the real solution.
 *   \param mu     The weight for the thickness constancy regularizer.
 *     Use this parameter if you want to enforce low thickness variations
 *     because you know, that the tube has no such variation. For roots it
 *     should be chosen small to allow the model to adapt to the thickness
 *     variations of the root.
 *   \param nIter  The maximum number of iterations until the model is
 *     reparametrized. Choose this large to avoid the model to end up
 *     in local optima. The default leads to good results in most cases.
 *   \param tau    The initial evolution time step. Bigger values for tau
 *     increase the evolution speed in the beginning, but the risk of
 *     oscillations which again reduce the convergence speed increases. During
 *     the optimization tau will be gradually reduced if no Energy decrease
 *     could be reached with the current step size.
 *   \param searchRadiusUm This parameter is the threshold at which the loss
 *     function psi becomes saturated. Low values resuce the evolution speed
 *     but also reduce the risk of 'jumping' out of the tube into neighboring
 *     structures. When fitting to one single straight root segment the
 *     radius should be chosen large to increase performance. If a value less
 *     than or equal to zero is given here, the distance is automatically
 *     determined and set to half the average distance between neighboring
 *     pixels along the point cloud's main axis. That will be very accurate
 *     but also very slow.
 */
/*======================================================================*/
    void fit(blitz::TinyVector<double,3> const &qcPositionUm,
             Array<int,3> const &segmentation,
             double kappa = 1.0, double lambda = 0.0, double mu = 0.0,
             int nIter = 1000, double tau = 0.1, double searchRadiusUm = 0.0);

/*======================================================================*/
/*! 
 *   Fit iRoCS to a given vector of Euclidean points. For fitting, the
 *   coupled curves model is applied.
 *
 *   Schmidt, T. et al. Modeling of Sparsely Sampled Tubular Surfaces Using
 *   Coupled Curves. LNCS, 7476, pp. 83-92, 2012
 *
 *   \param qcPositionUm The Euclidean position (z,y,x) of the requested
 *     iRoCS origin in micrometers. The root axis does not need to pass
 *     through the given point, but its projection onto the iRoCS axis is
 *     used as z-origin.
 *   \param nuclei The nuclei to fit iRoCS to. Only nuclei with label 2 will
 *     be considered.
 *   \param kappa  The data term weight of the coupled curves model
 *   \param lambda The weight for the smoothness regularizer. Since the
 *     underlying splines are intrinsically smooth, this parameter is only
 *     needed if the model starts to ascillate around the real solution.
 *   \param mu     The weight for the thickness constancy regularizer.
 *     Use this parameter if you want to enforce low thickness variations
 *     because you know, that the tube has no such variation. For roots it
 *     should be chosen small to allow the model to adapt to the thickness
 *     variations of the root.
 *   \param nIter  The maximum number of iterations until the model is
 *     reparametrized. Choose this large to avoid the model to end up
 *     in local optima. The default leads to good results in most cases.
 *   \param tau    The initial evolution time step. Bigger values for tau
 *     increase the evolution speed in the beginning, but the risk of
 *     oscillations which again reduce the convergence speed increases. During
 *     the optimization tau will be gradually reduced if no Energy decrease
 *     could be reached with the current step size.
 *   \param searchRadiusUm This parameter is the threshold at which the loss
 *     function psi becomes saturated. Low values reduce the evolution speed
 *     but also reduce the risk of 'jumping' out of the tube into neighboring
 *     structures. When fitting to one single straight root segment the
 *     radius should be chosen large to increase performance and reduce the
 *     risk of early termination due to low nucleus density. If a value less
 *     than or equal to zero is given here, the distance is automatically
 *     determined and set to half the average distance between neighboring
 *     epidermis markers along the point cloud's main axis.
 */
/*======================================================================*/
    void fit(blitz::TinyVector<double,3> const &qcPositionUm,
             std::vector< blitz::TinyVector<double,3> > &positionsUm,
             double kappa = 1.0, double lambda = 0.0, double mu = 0.0,
             int nIter = 1000, double tau = 0.1, double searchRadiusUm = 0.0);

/*======================================================================*/
/*! 
 *   Get the iRoCS position for the given image position in micrometers.
 *
 *   \param pos The image position (x,y,z) in micrometers to transform into
 *     iRoCS coordinates
 *
 *   \return The iRoCS coordinate vector (z,r,phi) corresponding to the
 *     given Euclidean image position
 */
/*======================================================================*/
    blitz::TinyVector<double,3> getCoordinates(
        blitz::TinyVector<double,3> const &pos) const;

/*======================================================================*/
/*! 
 *   Get the signed euclidean distance of the given point to the tubular
 *   surface. Negative values indicate, that the point is inside the
 *   tubular structure, positive values mean it is outside.
 *
 *   \param pos The point position (z,y,x) in micrometers
 *
 *   \return The distance in micrometers of the given position to the
 *     surface of the tubular structure.
 */
/*======================================================================*/
    double getDistanceToSurface(blitz::TinyVector<double,3> const &pos) const;

/*======================================================================*/
/*! 
 *   Get the euclidean position in micrometers for the given iRoCS coordinate
 *   vector.
 *
 *   \param coordinates The iRoCS coordinates (z,r,phi) to transform into
 *     Euclidean image coordinates
 *
 *   \return The Euclidean coordinates (z,y,x) corresponding to the given
 *     iRoCS coordinate vector
 */
/*======================================================================*/
    blitz::TinyVector<double,3> getEuclideanPositionUm(
        blitz::TinyVector<double,3> const &coordinates) const;

/*======================================================================*/
/*! 
 *   Compute a straightened view of the given dataset according to this
 *   iRoCS instance. The output Array must be resized to its final shape
 *   and its coordinates are (y, x, z), where z is the axis direction and
 *   x and y are the orthogonal plane dimensions.
 *
 *   \param data The original dataset to transform
 *   \param originalElementSizeUm The element size of the original dataset
 *   \param straightened The Array to write the straightened view into
 *   \param straightenedElementSizeUm The output element size
 *   \param originUm The coordinate origin (z = 0, r = 0, phi = 0) will be
 *     mapped to the given micrometer position
 */
/*======================================================================*/
    template<typename DataT>
    void computeStraightenedView(
        blitz::Array<DataT,3> const &data,
        blitz::TinyVector<double,3> const &originalElementSizeUm,
        blitz::Array<DataT,3> &straightened,
        blitz::TinyVector<double,3> const &straightenedElementSizeUm,
        blitz::TinyVector<double,3> const &originUm, double phiOffset) const;

    blitz::TinyVector<double,3> getAxisPosition(double u) const;
    double thickness(double u) const;

    CoupledBSplineModel<3> const &ccm() const;
    BSpline< blitz::TinyVector<double,3> > const &axisSpline() const;
    BSpline<double> const &thicknessSpline() const;

    double uQC() const;
    blitz::TinyMatrix<double,4,4> const &normalizationTransformation() const;
    blitz::TinyMatrix<double,4,4> const &inverseNormalizationTransformation()
        const;

    blitz::Array<blitz::TinyVector<double,3>,1> &vertices();
    blitz::Array<blitz::TinyVector<ptrdiff_t,3>,1> &indices();
    blitz::Array<blitz::TinyVector<double,3>,1> &normals();

    void save(std::string const &fileName, std::string const &groupName,
              bool throwErrors = false) const;
    void load(std::string const &fileName, std::string const &groupName,
              bool throwErrors = false);
    
    void save(BlitzH5File &outFile, std::string const &groupName) const;
    void load(BlitzH5File const &inFile, std::string const &groupName);
    
  private:
    
    void extractBoundary();
    void computeTransformation();

    void fitEpidermalQuadricRANSAC();
    
    iRoCS::ProgressReporter* p_progress;

    CoupledBSplineModel<3> *p_ccm;
    double _kappa, _lambda, _mu, _tau, _searchRadiusUm;
    int _nIter;

    blitz::TinyMatrix<double,4,4> _trafo, _trafoInv;
    blitz::TinyVector<double,3> _qcPos;
    double _uQC;

    ptrdiff_t _nLatitudes, _nLongitudes;
    blitz::Array<blitz::TinyVector<double,3>,1> _vertices;
    blitz::Array<blitz::TinyVector<ptrdiff_t,3>,1> _indices;
    blitz::Array<blitz::TinyVector<double,3>,1> _normals;

    double* p_curveLengthCache;

  };

}

#include "iRoCS.icc"

#endif
