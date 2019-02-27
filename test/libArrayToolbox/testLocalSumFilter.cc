#include "lmbunit.hh"

#include <libArrayToolbox/TypeTraits.hh>
#include <libArrayToolbox/LocalSumFilter.hh>
#include <libArrayToolbox/ATBTiming.hh>

template<typename DataT, int Dim>
static void testLocalSumFilterValueBTStaticAtbArrayInPlace()
{
  // std::stringstream ofname;
  // ofname << TOP_BUILD_DIR
  //        << "/test/libArrayToolbox/testArray_testLocalSumFilter_";
  // if (std::numeric_limits<DataT>::is_specialized)
  // {
  //   if (std::numeric_limits<DataT>::is_integer)
  //       ofname << (std::numeric_limits<DataT>::is_signed ? "int" : "uint");
  //   else ofname << "float";
  // }
  // else ofname << "unknown";
  // ofname << 8 * sizeof(DataT) << "_" << Dim << ".h5";

  blitz::TinyVector<atb::BlitzIndexT,Dim> dataShape;
  blitz::TinyVector<atb::BlitzIndexT,Dim> filterShapePx;
  blitz::TinyVector<double,Dim> elSize;
  double avgDim = std::pow(100000.0, 1.0 / static_cast<double>(Dim));
  for (int d = 0; d < Dim; ++d)
  {
    dataShape(d) = static_cast<atb::BlitzIndexT>(
        avgDim + 0.5 * avgDim * (static_cast<double>(std::rand()) /
                                 static_cast<double>(RAND_MAX) - 0.5));
    filterShapePx(d) = static_cast<atb::BlitzIndexT>(
        5 + 5 * (static_cast<double>(std::rand()) /
                 static_cast<double>(RAND_MAX) - 0.5));
    if (filterShapePx(d) % 2 == 0) filterShapePx(d)++;
    elSize(d) = 1.0 + (static_cast<double>(std::rand()) /
                       static_cast<double>(RAND_MAX) - 0.5);
  }
  atb::Array<DataT,Dim> data(dataShape, elSize);
  for (size_t i = 0; i < data.size(); ++i)
      data.dataFirst()[i] = atb::traits<DataT>::saturated *
          static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX);
  // data.save(ofname.str(), "/data", 3);

  atb::Profiler timer;
  timer.tic();
  atb::Array<DataT,Dim> expectedResult(dataShape);
  for (size_t i = 0; i < expectedResult.size(); ++i)
  {
    size_t tmp = i;
    blitz::TinyVector<atb::BlitzIndexT,Dim> x;
    for (int d = Dim - 1; d >= 0; --d)
    {
      x(d) = tmp % data.extent(d);
      tmp /= data.extent(d);
    }
    expectedResult(x) = atb::traits<DataT>::zero;
    for (size_t j = 0; j < static_cast<size_t>(blitz::product(filterShapePx));
         ++j)
    {
      tmp = j;
      blitz::TinyVector<atb::BlitzIndexT,Dim> dx;
      for (int d = Dim - 1; d >= 0; --d)
      {
        dx(d) = tmp % filterShapePx(d) - filterShapePx(d) / 2;
        tmp /= filterShapePx(d);
      }
      blitz::TinyVector<atb::BlitzIndexT,Dim> rdPos(x + dx);
      if (blitz::all(rdPos >= 0) && blitz::all(rdPos < dataShape))
          expectedResult(x) += data(rdPos);
    }
  }
  long long elapsed_mus = timer.toc();
  std::cout << "100 Megapixels: t_naive = "
            << atb::MyDateTime::prettyTime(
                elapsed_mus * 100000000 / Dim / blitz::product(dataShape))
            << "... ";

  timer.tic();
  atb::LocalSumFilter<DataT,Dim>::apply(data, data, filterShapePx);
  elapsed_mus = timer.toc();
  std::cout << "t = " << atb::MyDateTime::prettyTime(
      elapsed_mus * 100000000 / Dim / blitz::product(dataShape)) << "... ";
  // data.save(ofname.str(), "/filtered", 3);
  LMBUNIT_DEBUG_STREAM << "expected result = " << expectedResult << std::endl;
  LMBUNIT_DEBUG_STREAM << "got result      = " << data << std::endl;
  double rmse = 0.0;
  for (size_t i = 0; i < data.size(); ++i)
      rmse += blitz::pow2(data.dataFirst()[i] - expectedResult.dataFirst()[i]);
  rmse = std::sqrt(rmse / data.size());
  LMBUNIT_ASSERT_EQUAL_DELTA(
      rmse, 0.0, 1e-5 * (blitz::max(data) - blitz::min(data)));
}

template<typename DataT>
static void testLocalSumFilter1DValueBTEqualShapes(DataT boundaryValue) {
  blitz::Array<DataT,1> data(11);
  data = 0, 1, 2, 1, 0, -1, 2, 0, 0, 1, 2;
  blitz::TinyVector<double,1> elSize(1.0);
  blitz::Array<DataT,1> result;
  atb::LocalSumFilter<DataT,1>::apply(
      data, elSize, result, data.shape(), atb::ValueBT, boundaryValue);
  blitz::Array<DataT,1> expectedResult(data.shape());
  for (int x = 0; x < data.extent(0); ++x) {
    DataT expectedResult = atb::traits<DataT>::zero;
    for (int dx = -data.extent(0) / 2; dx <= data.extent(0) / 2; ++dx) {
      expectedResult += (x + dx < 0 || x + dx >= data.extent(0)) ?
          boundaryValue : data(x + dx);
    }
    LMBUNIT_ASSERT_EQUAL_DELTA(result(x), expectedResult, 1e-7);
  }
}

template<typename DataT>
static void testLocalSumFilter1DValueBTLargeKernel(DataT boundaryValue) {
  blitz::Array<DataT,1> data(5);
  data = 0, 1, -1, 2, 1;
  blitz::TinyVector<double,1> elSize(1.0);
  blitz::Array<DataT,1> result;
  atb::LocalSumFilter<DataT,1>::apply(
      data, elSize, result, blitz::TinyVector<int,1>(7),
      atb::ValueBT, boundaryValue);
  blitz::Array<DataT,1> expectedResult(11);
  for (int x = 0; x < data.extent(0); ++x) {
    DataT expectedResult = atb::traits<DataT>::zero;
    for (int dx = -3; dx <= 3; ++dx) {
      expectedResult += (x + dx < 0 || x + dx >= data.extent(0)) ?
          boundaryValue : data(x + dx);
    }
    LMBUNIT_ASSERT_EQUAL_DELTA(result(x), expectedResult, 1e-7);
  }
}

int main(int, char**)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST(
      (testLocalSumFilterValueBTStaticAtbArrayInPlace<unsigned char,1>()));
  LMBUNIT_RUN_TEST(
      (testLocalSumFilterValueBTStaticAtbArrayInPlace<short,1>()));
  LMBUNIT_RUN_TEST(
      (testLocalSumFilterValueBTStaticAtbArrayInPlace<float,1>()));
  LMBUNIT_RUN_TEST(
      (testLocalSumFilterValueBTStaticAtbArrayInPlace<double,1>()));
  LMBUNIT_RUN_TEST(
      (testLocalSumFilterValueBTStaticAtbArrayInPlace<unsigned char,2>()));
  LMBUNIT_RUN_TEST(
      (testLocalSumFilterValueBTStaticAtbArrayInPlace<short,2>()));
  LMBUNIT_RUN_TEST(
      (testLocalSumFilterValueBTStaticAtbArrayInPlace<float,2>()));
  LMBUNIT_RUN_TEST(
      (testLocalSumFilterValueBTStaticAtbArrayInPlace<double,2>()));
  LMBUNIT_RUN_TEST(
      (testLocalSumFilterValueBTStaticAtbArrayInPlace<unsigned char,3>()));
  LMBUNIT_RUN_TEST(
      (testLocalSumFilterValueBTStaticAtbArrayInPlace<short,3>()));
  LMBUNIT_RUN_TEST(
      (testLocalSumFilterValueBTStaticAtbArrayInPlace<float,3>()));
  LMBUNIT_RUN_TEST(
      (testLocalSumFilterValueBTStaticAtbArrayInPlace<double,3>()));

  LMBUNIT_RUN_TEST(testLocalSumFilter1DValueBTEqualShapes<float>(0.0f));
  LMBUNIT_RUN_TEST(testLocalSumFilter1DValueBTEqualShapes<float>(1.0f));

  LMBUNIT_RUN_TEST(testLocalSumFilter1DValueBTLargeKernel<float>(0.0f));
  LMBUNIT_RUN_TEST(testLocalSumFilter1DValueBTLargeKernel<float>(1.0f));

  LMBUNIT_WRITE_STATISTICS();
  return _nFails;
}
