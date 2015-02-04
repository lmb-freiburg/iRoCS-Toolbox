#include "ATBLinAlg.hh"

namespace atb
{

  double factorialFraction(int n, int k) 
  {
    // Some special cases
    if (k < 0 || n < 0) return 0;
    if (k == n) return 1.0;

    // If denominator is less than numerator return 1.0 / flippedFraction
    if (k > n) return 1.0 / factorialFraction(k, n);

    double res = k + 1;
    for (int i = k + 2; i <= n; ++i) res *= i;
    return res;
  }

  ptrdiff_t modulo(ptrdiff_t a, ptrdiff_t b)
  {
    ptrdiff_t c = a % b;
    return (c < 0) ? (c + b) : c;
  }

  blitz::TinyVector<double,3>
  euclideanToSpherical(
      blitz::TinyVector<double,3> const &eucl,
      blitz::Array<double,2> const &trafo)
  {
    blitz::TinyVector<double,3> pos =
        homogeneousToEuclidean(
            mvMult(trafo, euclideanToHomogeneous(eucl)));
    blitz::TinyVector<double,3> res;
    res(2) = std::sqrt(blitz::dot(pos, pos));
    res(0) = std::acos(pos(0) / res(2));
    res(1) = std::atan2(pos(1), pos(2));
    return res;
  }

  blitz::TinyVector<double,3>
  euclideanToSpherical(blitz::TinyVector<double,3> const &eucl)
  {
    double r = std::sqrt(blitz::dot(eucl, eucl));
    return blitz::TinyVector<double,3>(
        std::acos(eucl(0) / r), std::atan2(eucl(1), eucl(2)), r);
  }

  blitz::TinyVector<double,3>
  sphericalToEuclidean(
      blitz::TinyVector<double,3> const &spherical,
      blitz::Array<double,2> const &trafo)
  {
    blitz::TinyVector<double,3> pos(
        std::cos(spherical(0)),
        std::sin(spherical(0)) * std::sin(spherical(1)),
        std::sin(spherical(0)) * std::cos(spherical(1)));
    pos *= spherical(2);
    return homogeneousToEuclidean(
        mvMult(invert(trafo), euclideanToHomogeneous(pos)));
  }

  blitz::TinyVector<double,3>
  sphericalToEuclidean(blitz::TinyVector<double,3> const &spherical)
  {
    return spherical(2) * blitz::TinyVector<double,3>(
        std::cos(spherical(0)),
        std::sin(spherical(0)) * std::sin(spherical(1)),
        std::sin(spherical(0)) * std::cos(spherical(1)));
  }

  void invert(blitz::Array<double,2> const &A, blitz::Array<double,2> &Ainv)
  {
    blitz::Array<double,2> Acopy(A.shape());
    Acopy = A;
    gsl_matrix* Agsl = GSLWrapper::getGSLView(Acopy);

    Ainv.resize(A.shape());
    gsl_matrix* AinvGsl = GSLWrapper::getGSLView(Ainv);    

    int sign = 0;
    int row_sq = static_cast<int>(A.extent(0));
    gsl_permutation* p = gsl_permutation_calloc(row_sq);
    gsl_linalg_LU_decomp(Agsl, p, &sign);
    gsl_linalg_LU_invert(Agsl, p, AinvGsl);
    gsl_permutation_free(p);
    delete Agsl;
    delete AinvGsl;
  }

  blitz::Array<double,2> invert(blitz::Array<double,2> const &A)
  {
    blitz::Array<double,2> Ainv(A.shape());
    invert(A, Ainv);
    return Ainv;
  }

  void eigenvalueDecompositionRealSymmetric(
      blitz::Array<double,2> const &A,
      blitz::Array<double,2> &U, blitz::Array<double,1> &lambda,
      SortingType sort)
  {
    assert(A.extent(0) == A.extent(1));
    blitz::Array<double,2> Acopy(A.shape());
    Acopy = A;
    gsl_matrix* Agsl = GSLWrapper::getGSLView(Acopy);

    U.resize(A.shape());
    gsl_matrix* Ugsl = GSLWrapper::getGSLView(U);
    
    lambda.resize(A.extent(0));
    gsl_vector* lambdaGsl = GSLWrapper::getGSLView(lambda);
    
    gsl_eigen_symmv_workspace* w = gsl_eigen_symmv_alloc(A.extent(0));
    gsl_eigen_symmv(Agsl, lambdaGsl, Ugsl, w);
    
    switch (sort)
    {
    case Ascending:
      gsl_eigen_gensymmv_sort(lambdaGsl, Ugsl, GSL_EIGEN_SORT_VAL_ASC);
      break;
    case Descending:
      gsl_eigen_gensymmv_sort(lambdaGsl, Ugsl, GSL_EIGEN_SORT_VAL_DESC);
      break;
    case AbsAscending:
      gsl_eigen_gensymmv_sort(lambdaGsl, Ugsl, GSL_EIGEN_SORT_ABS_ASC);
      break;
    case AbsDescending:
      gsl_eigen_gensymmv_sort(lambdaGsl, Ugsl, GSL_EIGEN_SORT_ABS_DESC);
      break;
    default:
      break;
    }
  
    gsl_eigen_symmv_free(w);
    delete Agsl;
    delete Ugsl;
    delete lambdaGsl;
  }

  void eigenvalueDecompositionRealSymmetric(
      blitz::TinyMatrix<double,2,2> const &A,
      blitz::TinyVector<double,2>& lambda,
      SortingType sort)
  {
    double p_2 = 0.5 * (A(0, 0) + A(1, 1));
    double sq = p_2 * p_2 - A(0, 0) * A(1, 1) + A(0, 1) * A(1, 0);

    // Eigenvalues not real
    if (sq < -1.0e-10)
    {
          std::cerr << "Warning: Negative discriminant in eigenvalue "
                    << "decomposition: discriminant = " << sq
                    << " - setting it to zero" << std::endl;
      sq = 0.0;
    }

    // Two equal real eigenvalues ==> return identity matrix
    if (sq < 1.0e-10)
    {
      lambda(0) = p_2;
      lambda(1) = p_2;
      return;
    }
    
    // Two different real eigenvalues
    sq = std::sqrt(sq);
    switch (sort)
    {
    case Ascending:
      lambda(0) = p_2 - sq;
      lambda(1) = p_2 + sq;      
      break;
    case Descending:
      lambda(0) = p_2 + sq;
      lambda(1) = p_2 - sq;
      break;
    case AbsAscending:
      if (std::abs(p_2 - sq) < std::abs(p_2 + sq))
      {
        lambda(0) = p_2 - sq;
        lambda(1) = p_2 + sq;
      }
      else
      {
        lambda(0) = p_2 + sq;
        lambda(1) = p_2 - sq;
      }
      break;
    case AbsDescending:
      if (std::abs(p_2 - sq) > std::abs(p_2 + sq))
      {
        lambda(0) = p_2 - sq;
        lambda(1) = p_2 + sq;
      }
      else
      {
        lambda(0) = p_2 + sq;
        lambda(1) = p_2 - sq;
      }
      break;
    default:
      lambda(0) = p_2 + sq;
      lambda(1) = p_2 - sq;
    }
  }

  void computeEigenvectors(
      blitz::TinyMatrix<double,2,2> const &A,
      blitz::TinyMatrix<double,2,2> &U,
      blitz::TinyVector<double,2> const &lambda)
  {
    // Two equal real eigenvalues ==> return identity matrix
    if (lambda(0) == lambda(1))
    {
      U = 1.0, 0.0, 0.0, 1.0;
      return;
    }
    
    // Pick orthogonal vector with greatest norm as first eigenvector
    // to ensure numerical stability
    U(0, 0) = A(0, 1);
    U(1, 0) = -(A(0, 0) - lambda(0));
    double sqrNorm1 = U(0, 0) * U(0, 0) + U(1, 0) * U(1, 0);

    U(0, 0) = A(1, 1) - lambda(0);
    U(1, 0) = -A(1, 0);
    double sqrNorm2 = U(0, 0) * U(0, 0) + U(1, 0) * U(1, 0);

    double vecNorm;
    if (sqrNorm1 > sqrNorm2)
    {
      U(0, 0) = A(0, 1);
      U(1, 0) = -(A(0, 0) - lambda(0));
      vecNorm = std::sqrt(sqrNorm1);
    }
    else vecNorm = std::sqrt(sqrNorm2);

    U(0, 0) /= vecNorm;
    U(1, 0) /= vecNorm;
    
    // Generate orthogonal vector to first eigenvector as second eigenvector
    U(0, 1) = U(1, 0);
    U(1, 1) = -U(0, 0);    
  }

  void eigenvalueDecompositionRealSymmetric(
      const blitz::TinyMatrix<double,2,2>& A,
      blitz::TinyMatrix<double,2,2>& U,
      blitz::TinyVector<double,2>& lambda,
      SortingType sort)
  {
    eigenvalueDecompositionRealSymmetric(A, lambda, sort);
    computeEigenvectors(A, U, lambda);
  }

  void eigenvalueDecompositionRealSymmetric(
      blitz::TinyMatrix<double,3,3> const &A,
      blitz::TinyVector<double,3> &lambda,
      SortingType sort)
  {
    double inv3 = 1.0 / 3.0;
    double sqrt3 = std::sqrt(3.0);

    // Setup coefficients of characteristic polynomial
    double a0 = - 2.0 * A(0, 1) * A(1, 2) * A(0, 2) -
        A(0, 0) * A(1, 1) * A(2, 2) +
        A(0, 2) * A(0, 2) * A(1, 1) +
        A(0, 0) * A(1, 2) * A(1, 2) +
        A(0, 1) * A(0, 1) * A(2, 2);
    double a1 = -A(0, 2) * A(0, 2) - A(0, 1) * A(0, 1) - A(1, 2) * A(1, 2) +
        A(0, 0) * A(1, 1) + A(0, 0) * A(2, 2) + A(1, 1) * A(2, 2);
    double a2 = -(A(0, 0) + A(1, 1) + A(2, 2));

    // Solve degree three polynomial (Credits to Kun, who found it in Jama)
    double a2_3 = -inv3 * a2;
    double aDiv3 = std::min(0.0, inv3 * (a1 + a2 * a2_3));
    double mbDiv2 = 0.5 * (-a0 + a2_3 * (2.0 * a2_3 * a2_3 - a1));
    double q = std::min(0.0, mbDiv2 * mbDiv2 + aDiv3 * aDiv3 * aDiv3);
    double magnitude = std::sqrt(-aDiv3);
    double angle = std::atan2(std::sqrt(-q), mbDiv2) * inv3;
    double cs = std::cos(angle);
    double sn = std::sin(angle);
    lambda(0) = a2_3 + 2.0 * magnitude * cs;
    lambda(1) = a2_3 - magnitude * (cs + sqrt3 * sn);
    lambda(2) = a2_3 - magnitude * (cs - sqrt3 * sn);

    // Sort eigenvalues
    sortElements(lambda, sort);
  }

  void computeEigenvectors(
      blitz::TinyMatrix<double,3,3> const &A,
      blitz::TinyMatrix<double,3,3> &U,
      blitz::TinyVector<double,3> const &lambda)
  {
    // Generate orthogonal vectors to the rows of the matrix
    // (A - \lambda_0 I) as candidates for the first eigenvector
    blitz::TinyVector<double,3> e1(1.0, 0.0, 0.0);
    double maxSqrNorm = 0.0;
    blitz::TinyVector<double,3> candidate = blitz::cross(
        blitz::TinyVector<double,3>(A(0, 0) - lambda(0), A(0, 1), A(0, 2)),
        blitz::TinyVector<double,3>(A(1, 0), A(1, 1) - lambda(0), A(1, 2)));
    double sqrNorm = blitz::dot(candidate, candidate);
    if (sqrNorm > maxSqrNorm)
    {
      maxSqrNorm = sqrNorm;
      e1 = candidate / std::sqrt(maxSqrNorm);
    }
    candidate = blitz::cross(
        blitz::TinyVector<double,3>(A(0, 0) - lambda(0), A(0, 1), A(0, 2)),
        blitz::TinyVector<double,3>(A(2, 0), A(2, 1), A(2, 2) - lambda(0)));
    sqrNorm = blitz::dot(candidate, candidate);
    if (sqrNorm > maxSqrNorm)
    {
      maxSqrNorm = sqrNorm;
      e1 = candidate / std::sqrt(maxSqrNorm);
    }
    candidate = blitz::cross(
        blitz::TinyVector<double,3>(A(1, 0), A(1, 1) - lambda(0), A(1, 2)),
        blitz::TinyVector<double,3>(A(2, 0), A(2, 1), A(2, 2) - lambda(0)));
    sqrNorm = blitz::dot(candidate, candidate);
    if (sqrNorm > maxSqrNorm)
    {
      maxSqrNorm = sqrNorm;
      e1 = candidate / std::sqrt(maxSqrNorm);
    }
    U(0, 0) = e1(0);
    U(1, 0) = e1(1);
    U(2, 0) = e1(2);
    
    blitz::TinyVector<double,3> e2(0.0, 1.0, 0.0);
    maxSqrNorm = 0.0;
    candidate = blitz::cross(
        blitz::TinyVector<double,3>(A(0, 0) - lambda(1), A(0, 1), A(0, 2)),
        blitz::TinyVector<double,3>(A(1, 0), A(1, 1) - lambda(1), A(1, 2)));
    sqrNorm = blitz::dot(candidate, candidate);
    if (sqrNorm > maxSqrNorm)
    {
      maxSqrNorm = sqrNorm;
      e2 = candidate / std::sqrt(maxSqrNorm);
    }
    candidate = blitz::cross(
        blitz::TinyVector<double,3>(A(0, 0) - lambda(1), A(0, 1), A(0, 2)),
        blitz::TinyVector<double,3>(A(2, 0), A(2, 1), A(2, 2) - lambda(1)));
    sqrNorm = blitz::dot(candidate, candidate);
    if (sqrNorm > maxSqrNorm)
    {
      maxSqrNorm = sqrNorm;
      e2 = candidate / std::sqrt(maxSqrNorm);
    }
    candidate = blitz::cross(
        blitz::TinyVector<double,3>(A(1, 0), A(1, 1) - lambda(1), A(1, 2)),
        blitz::TinyVector<double,3>(A(2, 0), A(2, 1), A(2, 2) - lambda(1)));
    sqrNorm = blitz::dot(candidate, candidate);
    if (sqrNorm > maxSqrNorm)
    {
      maxSqrNorm = sqrNorm;
      e2 = candidate / std::sqrt(maxSqrNorm);
    }
    U(0, 1) = e2(0);
    U(1, 1) = e2(1);
    U(2, 1) = e2(2);
    
    blitz::TinyVector<double,3> e3(blitz::cross(e1, e2));
    U(0, 2) = e3(0);
    U(1, 2) = e3(1);
    U(2, 2) = e3(2);
  }
  
  void eigenvalueDecompositionRealSymmetric(
      const blitz::TinyMatrix<double,3,3>& A,
      blitz::TinyMatrix<double,3,3>& U,
      blitz::TinyVector<double,3>& lambda,
      SortingType sort)
  {
    eigenvalueDecompositionRealSymmetric(A, lambda, sort);
    computeEigenvectors(A, U, lambda);
  }

}
