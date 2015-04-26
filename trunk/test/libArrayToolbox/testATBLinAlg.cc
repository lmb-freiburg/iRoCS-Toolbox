#include "lmbunit.hh"

#include <libArrayToolbox/TypeTraits.hh>
#include <libArrayToolbox/ATBLinAlg.hh>

static void testFactorialFraction() 
{
  double res[] = {1.0, 2.0, 6.0, 24.0, 120.0, 720.0, 5040.0, 40320.0,
                  362880.0, 3628800.0, 39916800.0, 479001600.0, 6227020800.0,
                  87178291200.0, 1307674368000.0, 20922789888000.0,
                  355687428096000.0, 6402373705728000.0, 121645100408832000.0,
                  2432902008176640000.0, 51090942171709440000.0,
                  1124000727777607680000.0, 25852016738884976640000.0,
                  620448401733239439360000.0, 15511210043330985984000000.0,
                  403291461126605635584000000.0,
                  10888869450418352160768000000.0,
                  304888344611713860501504000000.0,
                  8841761993739701954543616000000.0,
                  265252859812191058636308480000000.0,
                  8222838654177922817725562880000000.0,
                  263130836933693530167218012160000000.0,
                  8683317618811886495518194401280000000.0,
                  295232799039604140847618609643520000000.0,
                  10333147966386144929666651337523200000000.0,
                  371993326789901217467999448150835200000000.0,
                  13763753091226345046315979581580902400000000.0,
                  523022617466601111760007224100074291200000000.0,
                  20397882081197443358640281739902897356800000000.0,
                  815915283247897734345611269596115894272000000000.0,
                  33452526613163807108170062053440751665152000000000.0,
                  1405006117752879898543142606244511569936384000000000.0,
                  60415263063373835637355132068513997507264512000000000.0,
                  2658271574788448768043625811014615890319638528000000000.0,
                  119622220865480194561963161495657715064383733760000000000.0};

  for (int n = 1; n < 46; ++n) 
  {
    for (int k = 1; k <= n; ++k) 
    {
      LMBUNIT_ASSERT_EQUAL_DELTA(
          atb::factorialFraction(n, k) / res[n - 1] * res[k - 1],
          1.0, 1e-12);
    }
  }

  for (int n = 1; n < 46; ++n) 
  {
    for (int k = 1; k <= n; ++k) 
    {
      LMBUNIT_ASSERT_EQUAL_DELTA(
          atb::factorialFraction(k, n) * res[n - 1] / res[k - 1],
          1.0, 1e-12);
    }
  }
}

static void testModulo()
{
  LMBUNIT_ASSERT_EQUAL(atb::modulo(5, 3), 2);
  LMBUNIT_ASSERT_EQUAL(atb::modulo(2, 3), 2);
  LMBUNIT_ASSERT_EQUAL(atb::modulo(-4, 3), 2);
  
  LMBUNIT_ASSERT_EQUAL(
      atb::modulo(blitz::TinyVector<ptrdiff_t,3>(5, 2, -4),
                  blitz::TinyVector<ptrdiff_t,3>(3, 3, 3))(0), 2);
  LMBUNIT_ASSERT_EQUAL(
      atb::modulo(blitz::TinyVector<ptrdiff_t,3>(5, 2, -4),
                  blitz::TinyVector<ptrdiff_t,3>(3, 3, 3))(1), 2);
  LMBUNIT_ASSERT_EQUAL(
      atb::modulo(blitz::TinyVector<ptrdiff_t,3>(5, 2, -4),
                  blitz::TinyVector<ptrdiff_t,3>(3, 3, 3))(2), 2);
}

template<typename DataT, int Dim>
static void testEuclideanToHomogeneous()
{
  blitz::TinyVector<DataT,Dim> p;
  for (int d = 0; d < Dim; ++d)
      p(d) = static_cast<DataT>(rand()) / static_cast<DataT>(RAND_MAX);
  for (int d = 0; d < Dim; ++d)
      LMBUNIT_ASSERT_EQUAL(atb::euclideanToHomogeneous(p)(d), p(d));
  LMBUNIT_ASSERT_EQUAL(atb::euclideanToHomogeneous(p)(Dim), DataT(1));
}

template<typename DataT, int Dim>
static void testHomogeneousToEuclidean()
{
  blitz::TinyVector<DataT,Dim+1> pHom;
  for (int d = 0; d < Dim + 1; ++d)
      pHom(d) = static_cast<DataT>(rand()) / static_cast<DataT>(RAND_MAX);
  for (int d = 0; d < Dim; ++d)
      LMBUNIT_ASSERT_EQUAL_DELTA(
          atb::homogeneousToEuclidean(pHom)(d),
          pHom(d) / pHom(Dim), DataT(1e-10));
}

static void testEuclideanToSpherical()
{
  blitz::TinyVector<double,3> p;
  for (int d = 0; d < 3; ++d)
      p(d) = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
  double length = std::sqrt(blitz::dot(p, p));
  LMBUNIT_ASSERT_EQUAL_DELTA(
      atb::euclideanToSpherical(p)(0), std::acos(p(0) / length), 1e-10);
  LMBUNIT_ASSERT_EQUAL_DELTA(
      atb::euclideanToSpherical(p)(1), std::atan2(p(1), p(2)), 1e-10);
  LMBUNIT_ASSERT_EQUAL_DELTA(
      atb::euclideanToSpherical(p)(2), length, 1e-10);

  // ToDo: Another test for the variant with transformation would be nice
}

static void testSphericalToEuclidean()
{
  blitz::TinyVector<double,3> p;
  p(0) = M_PI *
      (static_cast<double>(rand()) / static_cast<double>(RAND_MAX) - 0.5);
  p(1) =
      2.0 * M_PI * static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
  p(2) = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
  LMBUNIT_ASSERT_EQUAL_DELTA(
      atb::sphericalToEuclidean(p)(0), p(2) * std::cos(p(0)), 1e-10);
  LMBUNIT_ASSERT_EQUAL_DELTA(
      atb::sphericalToEuclidean(p)(1),
      p(2) * std::sin(p(0)) * std::sin(p(1)), 1e-10);
  LMBUNIT_ASSERT_EQUAL_DELTA(
      atb::sphericalToEuclidean(p)(2),
      p(2) * std::sin(p(0)) * std::cos(p(1)), 1e-10);

  // ToDo: Another test for the variant with transformation would be nice
}

static void testRotate() 
{
  blitz::TinyVector<double,3> vec(1.0, 0.0, 0.0);
  blitz::TinyVector<double,3> axis(0.0, 0.0, 1.0);
  blitz::TinyVector<double,3> res;
  res = atb::rotate(vec, axis, M_PI / 2.0);
  LMBUNIT_ASSERT_EQUAL_DELTA(res(0), 0.0, 1e-10);
  LMBUNIT_ASSERT_EQUAL_DELTA(res(1), 1.0, 1e-10);
  LMBUNIT_ASSERT_EQUAL_DELTA(res(2), 0.0, 1e-10);
  axis = 0.0, 1.0, 0.0;
  res = atb::rotate(vec, axis, M_PI / 2.0);
  LMBUNIT_ASSERT_EQUAL_DELTA(res(0), 0.0, 1e-10);
  LMBUNIT_ASSERT_EQUAL_DELTA(res(1), 0.0, 1e-10);
  LMBUNIT_ASSERT_EQUAL_DELTA(res(2), -1.0, 1e-10);
}

static void testDot()
{
  blitz::Array<double,1> v1(3);
  v1 = 1, 2, 3;
  blitz::Array<double,1> v2(3);
  v2 = 4, 5, 6;
  LMBUNIT_ASSERT_EQUAL(atb::dot(v1, v2), 32);
}

static void testOuterProduct() 
{
  blitz::TinyVector<float,3> v1(1.2, 2.7, 0.3);
  blitz::TinyVector<float,2> v2(0.5, -1.3);
  blitz::TinyMatrix<float,3,2> res;
  res = atb::outerProduct(v1, v2);
  bool TinyVector3_TinyVector2 = true;
  float eps = 1e-6f;
  for (int i = 0; i < 3; ++i) 
  {
    for (int j = 0; j < 2; ++j) 
    {
      if (res(i, j) < v1(i) * v2(j) - eps ||
          res(i, j) > v1(i) * v2(j) + eps)
          TinyVector3_TinyVector2 = false;
    }
  }
  LMBUNIT_ASSERT(TinyVector3_TinyVector2);
}

static void testTranspose()
{
  blitz::Array<double,2> data(3, 4);
  for (size_t i = 0; i < data.size(); ++i)
      data.data()[i] =
          static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
  blitz::Array<double,2> dataTranspose(4, 3);
  dataTranspose = atb::transpose(data);
  for (atb::BlitzIndexT r = 0; r < data.extent(0); ++r)
      for (atb::BlitzIndexT c = 0; c < data.extent(1); ++c)
          LMBUNIT_ASSERT_EQUAL(data(r, c), dataTranspose(c, r));
}

static void testMvMult() 
{
  blitz::Array<float,2> m(3, 3);
  m = 1, 2, 3, 4, 5, 6, 7, 8, 9;
  blitz::TinyVector<float,3> v(1.0, 2.0, 0.5);
  blitz::TinyVector<float,3> res;
  res = atb::mvMult(m, v);
  LMBUNIT_ASSERT_EQUAL(res(0), 6.5);
  LMBUNIT_ASSERT_EQUAL(res(1), 17.0);
  LMBUNIT_ASSERT_EQUAL(res(2), 27.5);
  
  m.resize(2, 3);
  m = 1, 2, 3, 4, 5, 6;
  blitz::Array<float,1> va(3);
  va = 1.0, 2.0, 0.5;
  blitz::Array<float,1> resa(2);
  resa = atb::mvMult(m, va);
  LMBUNIT_ASSERT_EQUAL(resa(0), 6.5f);
  LMBUNIT_ASSERT_EQUAL(resa(1), 17.0f);
}

static void testMmMult() 
{
  {
    blitz::Array<float,2> m1(2, 3);
    m1 = 1, 2, 3, 4, 5, 6;
    blitz::Array<float,2> m2(3, 1);
    m2 = 1, 2, 3;
    blitz::Array<float,2> m3(2, 1);
    m3 = atb::mmMult(m1, m2);
    LMBUNIT_ASSERT_EQUAL(m3(0, 0), 14.0f);
    LMBUNIT_ASSERT_EQUAL(m3(1, 0), 32.0f);
  }
  {
    blitz::TinyMatrix<float,2,3> m1;
    m1 = 1, 2, 3, 4, 5, 6;
    blitz::TinyMatrix<float,3,1> m2;
    m2 = 1, 2, 3;
    blitz::TinyMatrix<float,2,1> m3;
    m3 = atb::mmMult(m1, m2);
    LMBUNIT_ASSERT_EQUAL(m3(0, 0), 14.0f);
    LMBUNIT_ASSERT_EQUAL(m3(1, 0), 32.0f);
  }
}

static void testInvert()
{
  {
    blitz::Array<double,2> A(3, 3);
    A = 0.5, 0, 0, 0, 0.5, 0, 0, 0, 0.5;
    blitz::Array<double,2> B;
    atb::invert(A, B);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(0, 0), 2.0, 1e-12);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(0, 1), 0.0, 1e-12);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(0, 2), 0.0, 1e-12);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(1, 0), 0.0, 1e-12);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(1, 1), 2.0, 1e-12);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(1, 2), 0.0, 1e-12);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(2, 0), 0.0, 1e-12);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(2, 1), 0.0, 1e-12);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(2, 2), 2.0, 1e-12);
  }
  {
    blitz::Array<double,2> A(3, 3);
    A = 0.5, 0, 0, 0, 0.5, 0, 0, 0, 0.5;
    blitz::Array<double,2> B(3, 3);
    B = atb::invert(A);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(0, 0), 2.0, 1e-12);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(0, 1), 0.0, 1e-12);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(0, 2), 0.0, 1e-12);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(1, 0), 0.0, 1e-12);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(1, 1), 2.0, 1e-12);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(1, 2), 0.0, 1e-12);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(2, 0), 0.0, 1e-12);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(2, 1), 0.0, 1e-12);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(2, 2), 2.0, 1e-12);
  }
  {
    blitz::TinyMatrix<double,3,3> A;
    A = 0.5, 0, 0, 0, 0.5, 0, 0, 0, 0.5;
    blitz::TinyMatrix<double,3,3> B;
    B = atb::invert(A);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(0, 0), 2.0, 1e-12);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(0, 1), 0.0, 1e-12);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(0, 2), 0.0, 1e-12);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(1, 0), 0.0, 1e-12);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(1, 1), 2.0, 1e-12);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(1, 2), 0.0, 1e-12);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(2, 0), 0.0, 1e-12);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(2, 1), 0.0, 1e-12);
    LMBUNIT_ASSERT_EQUAL_DELTA(B(2, 2), 2.0, 1e-12);
  }
}

static void testDeterminant()
{
  blitz::TinyMatrix<double,3,3> A;
  A = 1, 2, 3,
      6, 5, 4,
      9, 7, 8;
  LMBUNIT_ASSERT_EQUAL_DELTA(atb::determinant(A), -21.0, 1.0e-12);
}

static void testEigenvalueDecompositionRealSymmetric()
{
  blitz::Array<double,2> A(3, 3);
  A = 1, 2, 3,
      2, 5, 6,
      3, 6, 9;
  blitz::Array<double,2> EVec;
  blitz::Array<double,1> EVal;
  atb::eigenvalueDecompositionRealSymmetric(A, EVec, EVal);
  LMBUNIT_ASSERT_EQUAL_DELTA(EVal(2), 0.0, 1e-12);

  blitz::TinyMatrix<double,3,3> B;
  B = 1, 2, 3,
      2, 5, 6,
      3, 6, 9;
  blitz::TinyMatrix<double,3,3> EVecVec;
  blitz::TinyVector<double,3> EValVec;
  atb::eigenvalueDecompositionRealSymmetric(B, EVecVec, EValVec);
  LMBUNIT_ASSERT_EQUAL_DELTA(EValVec(2), 0.0, 1e-12);
}

int main(int, char**)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST(testFactorialFraction());

  LMBUNIT_RUN_TEST(testModulo());

  LMBUNIT_RUN_TEST((testEuclideanToHomogeneous<float,2>()));
  LMBUNIT_RUN_TEST((testEuclideanToHomogeneous<float,3>()));
  LMBUNIT_RUN_TEST((testEuclideanToHomogeneous<double,2>()));
  LMBUNIT_RUN_TEST((testEuclideanToHomogeneous<double,3>()));

  LMBUNIT_RUN_TEST((testHomogeneousToEuclidean<float,2>()));
  LMBUNIT_RUN_TEST((testHomogeneousToEuclidean<float,3>()));
  LMBUNIT_RUN_TEST((testHomogeneousToEuclidean<double,2>()));
  LMBUNIT_RUN_TEST((testHomogeneousToEuclidean<double,3>()));

  LMBUNIT_RUN_TEST(testEuclideanToSpherical());
  LMBUNIT_RUN_TEST(testSphericalToEuclidean());
  LMBUNIT_RUN_TEST(testRotate());

  LMBUNIT_RUN_TEST(testDot());
  LMBUNIT_RUN_TEST(testOuterProduct());
  LMBUNIT_RUN_TEST(testTranspose());
  LMBUNIT_RUN_TEST(testMvMult());
  LMBUNIT_RUN_TEST(testMmMult());
  LMBUNIT_RUN_TEST(testInvert());
  LMBUNIT_RUN_TEST(testDeterminant());
  LMBUNIT_RUN_TEST(testEigenvalueDecompositionRealSymmetric());

  LMBUNIT_WRITE_STATISTICS();
  return _nFails;
}
