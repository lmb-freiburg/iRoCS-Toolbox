#include "lmbunit.hh"

#include <libArrayToolbox/FastNormalizedCorrelationFilter.hh>

template<typename DataT>
static void testFastNormalizedCorrelationFilter1DCyclicBTNoPad() {
  blitz::Array<DataT,1> data(8);
  data = 0, 1, 2, 1, 0, 0, 0, 2;
  blitz::Array<DataT,1> kernel(8);
  kernel = 1, 2, 3, 2, 1, 2, 3, 2;

  blitz::TinyVector<double,1> elSize(1.0);

  blitz::Array<DataT,1> result;
  atb::FastNormalizedCorrelationFilter<DataT,1>::apply(
      data, elSize, result, kernel, atb::CyclicBT);

  LMBUNIT_ASSERT_EQUAL(result.size(), data.size());

  blitz::Array<DataT,1> expectedResult(8);

  kernel = (kernel - blitz::mean(kernel)) / std::sqrt(
      blitz::dot(kernel - blitz::mean(kernel), kernel - blitz::mean(kernel)));
  data = (data - blitz::mean(data)) / std::sqrt(
      blitz::dot(data - blitz::mean(data), data - blitz::mean(data)));

  for (size_t i = 0; i < data.size(); ++i) {
    DataT expectedResult = 0.0;
    for (size_t j = 0; j < kernel.size(); ++j) {
      expectedResult +=
          data((i + (j - kernel.size() / 2) + data.size()) % data.size()) *
          kernel(j);
    }
    LMBUNIT_ASSERT_EQUAL_DELTA(result(i), expectedResult, 1e-5);
  }
}

template<typename DataT>
static void testFastNormalizedCorrelationFilter1DValueBT(DataT boundaryValue) {
  blitz::Array<DataT,1> data(10);
  data = 0, 1, 2, 1, 0, 0, 0, 2, 4, 2;
  blitz::Array<DataT,1> kernel(5);
  kernel = 1, 2, 3, 2, 1;

  blitz::TinyVector<double,1> elSize(1.0);

  blitz::Array<DataT,1> result;
  atb::FastNormalizedCorrelationFilter<DataT,1>::apply(
      data, elSize, result, kernel, atb::ValueBT, boundaryValue);

  LMBUNIT_ASSERT_EQUAL(result.size(), data.size());

  blitz::Array<DataT,1> expectedResult(10);

  DataT kernelMean = blitz::mean(kernel);
  DataT kernelNorm = std::sqrt(
      blitz::dot(kernel - kernelMean, kernel - kernelMean));

  // Left border
  DataT dataMean = (2 * boundaryValue + data(0) + data(1) + data(2)) / 5.0;
  DataT dataNorm = std::sqrt(
      2 * (boundaryValue - dataMean) * (boundaryValue - dataMean) +
      (data(0) - dataMean) * (data(0) - dataMean) +
      (data(1) - dataMean) * (data(1) - dataMean) +
      (data(2) - dataMean) * (data(2) - dataMean));
  expectedResult(0) = (
      (boundaryValue - dataMean) * (kernel(0) - kernelMean) +
      (boundaryValue - dataMean) * (kernel(1) - kernelMean) +
      (data(0) - dataMean) * (kernel(2) - kernelMean) +
      (data(1) - dataMean) * (kernel(3) - kernelMean) +
      (data(2) - dataMean) * (kernel(4) - kernelMean)) /
      (dataNorm * kernelNorm);
  LMBUNIT_ASSERT_EQUAL_DELTA(result(0), expectedResult(0), 1e-5);

  dataMean = (boundaryValue + data(0) + data(1) + data(2) + data(3)) / 5.0;
  dataNorm = std::sqrt(
      (boundaryValue - dataMean) * (boundaryValue - dataMean) +
      (data(0) - dataMean) * (data(0) - dataMean) +
      (data(1) - dataMean) * (data(1) - dataMean) +
      (data(2) - dataMean) * (data(2) - dataMean) +
      (data(3) - dataMean) * (data(3) - dataMean));
  expectedResult(1) = (
      (boundaryValue - dataMean) * (kernel(0) - kernelMean) +
      (data(0) - dataMean) * (kernel(1) - kernelMean) +
      (data(1) - dataMean) * (kernel(2) - kernelMean) +
      (data(2) - dataMean) * (kernel(3) - kernelMean) +
      (data(3) - dataMean) * (kernel(4) - kernelMean)) /
      (dataNorm * kernelNorm);
  LMBUNIT_ASSERT_EQUAL_DELTA(result(1), expectedResult(1), 1e-5);

  // Inner part
  for (int i = 2; i <= 7; ++i) {
    dataMean = (data(i-2) + data(i-1) + data(i) + data(i+1) + data(i+2)) / 5.0;
    dataNorm = std::sqrt(
        (data(i-2) - dataMean) * (data(i-2) - dataMean) +
        (data(i-1) - dataMean) * (data(i-1) - dataMean) +
        (data(i) - dataMean) * (data(i) - dataMean) +
        (data(i+1) - dataMean) * (data(i+1) - dataMean) +
        (data(i+2) - dataMean) * (data(i+2) - dataMean));
    expectedResult(i) = (
        (data(i-2) - dataMean) * (kernel(0) - kernelMean) +
        (data(i-1) - dataMean) * (kernel(1) - kernelMean) +
        (data(i) - dataMean) * (kernel(2) - kernelMean) +
        (data(i+1) - dataMean) * (kernel(3) - kernelMean) +
        (data(i+2) - dataMean) * (kernel(4) - kernelMean)) /
        (dataNorm * kernelNorm);
    LMBUNIT_ASSERT_EQUAL_DELTA(result(i), expectedResult(i), 1e-5);
  }

  // Right border
  dataMean = (data(6) + data(7) + data(8) + data(9) + boundaryValue) / 5.0;
  dataNorm = std::sqrt(
      (data(6) - dataMean) * (data(6) - dataMean) +
      (data(7) - dataMean) * (data(7) - dataMean) +
      (data(8) - dataMean) * (data(8) - dataMean) +
      (data(9) - dataMean) * (data(9) - dataMean) +
      (boundaryValue - dataMean) * (boundaryValue - dataMean));
  expectedResult(8) = (
      (data(6) - dataMean) * (kernel(0) - kernelMean) +
      (data(7) - dataMean) * (kernel(1) - kernelMean) +
      (data(8) - dataMean) * (kernel(2) - kernelMean) +
      (data(9) - dataMean) * (kernel(3) - kernelMean) +
      (boundaryValue - dataMean) * (kernel(4) - kernelMean)) /
      (dataNorm * kernelNorm);
  LMBUNIT_ASSERT_EQUAL_DELTA(result(8), expectedResult(8), 1e-5);

  dataMean = (data(7) + data(8) + data(9) + 2 * boundaryValue) / 5.0;
  dataNorm = std::sqrt(
      (data(7) - dataMean) * (data(7) - dataMean) +
      (data(8) - dataMean) * (data(8) - dataMean) +
      (data(9) - dataMean) * (data(9) - dataMean) +
      2 * (boundaryValue - dataMean) * (boundaryValue - dataMean));
  expectedResult(9) = (
      (data(7) - dataMean) * (kernel(0) - kernelMean) +
      (data(8) - dataMean) * (kernel(1) - kernelMean) +
      (data(9) - dataMean) * (kernel(2) - kernelMean) +
      (boundaryValue - dataMean) * (kernel(3) - kernelMean) +
      (boundaryValue - dataMean) * (kernel(4) - kernelMean)) /
      (dataNorm * kernelNorm);
  LMBUNIT_ASSERT_EQUAL_DELTA(result(9), expectedResult(9), 1e-5);
}

template<typename DataT>
static void testFastNormalizedCorrelationFilter1DCropBT() {
  blitz::Array<DataT,1> data(10);
  data = 0, 1, 2, 1, 0, 0, 0, 2, 4, 2;
  blitz::Array<DataT,1> kernel(5);
  kernel = 1, 2, 3, 2, 1;

  blitz::TinyVector<double,1> elSize(1.0);

  blitz::Array<DataT,1> result;
  atb::FastNormalizedCorrelationFilter<DataT,1>::apply(
      data, elSize, result, kernel, atb::CropBT);

  LMBUNIT_ASSERT_EQUAL(result.size(), data.size());

  blitz::Array<DataT,1> expectedResult(10);

  // Left border
  DataT dataMean = (data(0) + data(1) + data(2)) / 3.0;
  DataT dataNorm = std::sqrt(
      (data(0) - dataMean) * (data(0) - dataMean) +
      (data(1) - dataMean) * (data(1) - dataMean) +
      (data(2) - dataMean) * (data(2) - dataMean));
  DataT kernelMean = (kernel(2) + kernel(3) + kernel(4)) / 3.0;
  DataT kernelNorm = std::sqrt(
      (kernel(2) - kernelMean) * (kernel(2) - kernelMean) +
      (kernel(3) - kernelMean) * (kernel(3) - kernelMean) +
      (kernel(4) - kernelMean) * (kernel(4) - kernelMean));
  expectedResult(0) = (
      (data(0) - dataMean) * (kernel(2) - kernelMean) +
      (data(1) - dataMean) * (kernel(3) - kernelMean) +
      (data(2) - dataMean) * (kernel(4) - kernelMean)) /
      (dataNorm * kernelNorm);
  LMBUNIT_ASSERT_EQUAL_DELTA(result(0), expectedResult(0), 1e-5);

  dataMean = (data(0) + data(1) + data(2) + data(3)) / 4.0;
  dataNorm = std::sqrt(
      (data(0) - dataMean) * (data(0) - dataMean) +
      (data(1) - dataMean) * (data(1) - dataMean) +
      (data(2) - dataMean) * (data(2) - dataMean) +
      (data(3) - dataMean) * (data(3) - dataMean));
  kernelMean = (kernel(1) + kernel(2) + kernel(3) + kernel(4)) / 4.0;
  kernelNorm = std::sqrt(
      (kernel(1) - kernelMean) * (kernel(1) - kernelMean) +
      (kernel(2) - kernelMean) * (kernel(2) - kernelMean) +
      (kernel(3) - kernelMean) * (kernel(3) - kernelMean) +
      (kernel(4) - kernelMean) * (kernel(4) - kernelMean));
  expectedResult(1) = (
      (data(0) - dataMean) * (kernel(1) - kernelMean) +
      (data(1) - dataMean) * (kernel(2) - kernelMean) +
      (data(2) - dataMean) * (kernel(3) - kernelMean) +
      (data(3) - dataMean) * (kernel(4) - kernelMean)) /
      (dataNorm * kernelNorm);
  LMBUNIT_ASSERT_EQUAL_DELTA(result(1), expectedResult(1), 1e-5);

  // Inner part
  kernelMean = blitz::mean(kernel);
  kernelNorm = std::sqrt(blitz::dot(kernel - kernelMean, kernel - kernelMean));
  for (int i = 2; i <= 7; ++i) {
    dataMean = (data(i-2) + data(i-1) + data(i) + data(i+1) + data(i+2)) / 5.0;
    dataNorm = std::sqrt(
        (data(i-2) - dataMean) * (data(i-2) - dataMean) +
        (data(i-1) - dataMean) * (data(i-1) - dataMean) +
        (data(i) - dataMean) * (data(i) - dataMean) +
        (data(i+1) - dataMean) * (data(i+1) - dataMean) +
        (data(i+2) - dataMean) * (data(i+2) - dataMean));
    expectedResult(i) = (
        (data(i-2) - dataMean) * (kernel(0) - kernelMean) +
        (data(i-1) - dataMean) * (kernel(1) - kernelMean) +
        (data(i) - dataMean) * (kernel(2) - kernelMean) +
        (data(i+1) - dataMean) * (kernel(3) - kernelMean) +
        (data(i+2) - dataMean) * (kernel(4) - kernelMean)) /
        (dataNorm * kernelNorm);
    LMBUNIT_ASSERT_EQUAL_DELTA(result(i), expectedResult(i), 1e-5);
  }

  // Right border
  dataMean = (data(6) + data(7) + data(8) + data(9)) / 4.0;
  dataNorm = std::sqrt(
      (data(6) - dataMean) * (data(6) - dataMean) +
      (data(7) - dataMean) * (data(7) - dataMean) +
      (data(8) - dataMean) * (data(8) - dataMean) +
      (data(9) - dataMean) * (data(9) - dataMean));
  kernelMean = (kernel(0) + kernel(1) + kernel(2) + kernel(3)) / 4.0;
  kernelNorm = std::sqrt(
      (kernel(0) - kernelMean) * (kernel(0) - kernelMean) +
      (kernel(1) - kernelMean) * (kernel(1) - kernelMean) +
      (kernel(2) - kernelMean) * (kernel(2) - kernelMean) +
      (kernel(3) - kernelMean) * (kernel(3) - kernelMean));
  expectedResult(8) = (
      (data(6) - dataMean) * (kernel(0) - kernelMean) +
      (data(7) - dataMean) * (kernel(1) - kernelMean) +
      (data(8) - dataMean) * (kernel(2) - kernelMean) +
      (data(9) - dataMean) * (kernel(3) - kernelMean)) /
      (dataNorm * kernelNorm);
  LMBUNIT_ASSERT_EQUAL_DELTA(result(8), expectedResult(8), 1e-5);

  dataMean = (data(7) + data(8) + data(9)) / 3.0;
  dataNorm = std::sqrt(
      (data(7) - dataMean) * (data(7) - dataMean) +
      (data(8) - dataMean) * (data(8) - dataMean) +
      (data(9) - dataMean) * (data(9) - dataMean));
  kernelMean = (kernel(0) + kernel(1) + kernel(2)) / 3.0;
  kernelNorm = std::sqrt(
      (kernel(0) - kernelMean) * (kernel(0) - kernelMean) +
      (kernel(1) - kernelMean) * (kernel(1) - kernelMean) +
      (kernel(2) - kernelMean) * (kernel(2) - kernelMean));
  expectedResult(9) = (
      (data(7) - dataMean) * (kernel(0) - kernelMean) +
      (data(8) - dataMean) * (kernel(1) - kernelMean) +
      (data(9) - dataMean) * (kernel(2) - kernelMean)) /
      (dataNorm * kernelNorm);
  LMBUNIT_ASSERT_EQUAL_DELTA(result(9), expectedResult(9), 1e-5);
}

template<typename DataT>
static void testFastNormalizedCorrelationFilter1DCropBTLargeKernel() {
  blitz::Array<DataT,1> data(5);
  data = 1, 2, 3, 2, 1;
  blitz::Array<DataT,1> kernel(7);
  kernel = 0, 1, 2, 1, 0, 0, 2;

  blitz::TinyVector<double,1> elSize(1.0);

  blitz::Array<DataT,1> result;
  atb::FastNormalizedCorrelationFilter<DataT,1>::apply(
      data, elSize, result, kernel, atb::CropBT);

  LMBUNIT_ASSERT_EQUAL(result.size(), data.size());

  DataT dataMean = (data(0) + data(1) + data(2) + data(3)) / 4.0;
  DataT dataNorm = std::sqrt(
      (data(0) - dataMean) * (data(0) - dataMean) +
      (data(1) - dataMean) * (data(1) - dataMean) +
      (data(2) - dataMean) * (data(2) - dataMean) +
      (data(3) - dataMean) * (data(3) - dataMean));
  DataT kernelMean = (kernel(3) + kernel(4) + kernel(5) + kernel(6)) / 4.0;
  DataT kernelNorm = std::sqrt(
      (kernel(3) - kernelMean) * (kernel(3) - kernelMean) +
      (kernel(4) - kernelMean) * (kernel(4) - kernelMean) +
      (kernel(5) - kernelMean) * (kernel(5) - kernelMean) +
      (kernel(6) - kernelMean) * (kernel(6) - kernelMean));
  DataT expectedResult = (
      (data(0) - dataMean) * (kernel(3) - kernelMean) +
      (data(1) - dataMean) * (kernel(4) - kernelMean) +
      (data(2) - dataMean) * (kernel(5) - kernelMean) +
      (data(3) - dataMean) * (kernel(6) - kernelMean)) /
      (dataNorm * kernelNorm);
  LMBUNIT_ASSERT_EQUAL_DELTA(result(0), expectedResult, 1e-5);

  dataMean = blitz::mean(data);
  dataNorm = std::sqrt(blitz::dot(data - dataMean, data - dataMean));
  kernelMean =
      (kernel(2) + kernel(3) + kernel(4) + kernel(5) + kernel(6)) / 5.0;
  kernelNorm = std::sqrt(
      (kernel(2) - kernelMean) * (kernel(2) - kernelMean) +
      (kernel(3) - kernelMean) * (kernel(3) - kernelMean) +
      (kernel(4) - kernelMean) * (kernel(4) - kernelMean) +
      (kernel(5) - kernelMean) * (kernel(5) - kernelMean) +
      (kernel(6) - kernelMean) * (kernel(6) - kernelMean));
  expectedResult = (
      (data(0) - dataMean) * (kernel(2) - kernelMean) +
      (data(1) - dataMean) * (kernel(3) - kernelMean) +
      (data(2) - dataMean) * (kernel(4) - kernelMean) +
      (data(3) - dataMean) * (kernel(5) - kernelMean) +
      (data(4) - dataMean) * (kernel(6) - kernelMean)) /
      (dataNorm * kernelNorm);
  LMBUNIT_ASSERT_EQUAL_DELTA(result(1), expectedResult, 1e-5);

  kernelMean =
      (kernel(1) + kernel(2) + kernel(3) + kernel(4) + kernel(5)) / 5.0;
  kernelNorm = std::sqrt(
      (kernel(1) - kernelMean) * (kernel(1) - kernelMean) +
      (kernel(2) - kernelMean) * (kernel(2) - kernelMean) +
      (kernel(3) - kernelMean) * (kernel(3) - kernelMean) +
      (kernel(4) - kernelMean) * (kernel(4) - kernelMean) +
      (kernel(5) - kernelMean) * (kernel(5) - kernelMean));
  expectedResult = (
      (data(0) - dataMean) * (kernel(1) - kernelMean) +
      (data(1) - dataMean) * (kernel(2) - kernelMean) +
      (data(2) - dataMean) * (kernel(3) - kernelMean) +
      (data(3) - dataMean) * (kernel(4) - kernelMean) +
      (data(4) - dataMean) * (kernel(5) - kernelMean)) /
      (dataNorm * kernelNorm);
  LMBUNIT_ASSERT_EQUAL_DELTA(result(2), expectedResult, 1e-5);

  kernelMean =
      (kernel(0) + kernel(1) + kernel(2) + kernel(3) + kernel(4)) / 5.0;
  kernelNorm = std::sqrt(
      (kernel(0) - kernelMean) * (kernel(0) - kernelMean) +
      (kernel(1) - kernelMean) * (kernel(1) - kernelMean) +
      (kernel(2) - kernelMean) * (kernel(2) - kernelMean) +
      (kernel(3) - kernelMean) * (kernel(3) - kernelMean) +
      (kernel(4) - kernelMean) * (kernel(4) - kernelMean));
  expectedResult = (
      (data(0) - dataMean) * (kernel(0) - kernelMean) +
      (data(1) - dataMean) * (kernel(1) - kernelMean) +
      (data(2) - dataMean) * (kernel(2) - kernelMean) +
      (data(3) - dataMean) * (kernel(3) - kernelMean) +
      (data(4) - dataMean) * (kernel(4) - kernelMean)) /
      (dataNorm * kernelNorm);
  LMBUNIT_ASSERT_EQUAL_DELTA(result(3), expectedResult, 1e-5);

  dataMean = (data(1) + data(2) + data(3) + data(4)) / 4.0;
  dataNorm = std::sqrt(
      (data(1) - dataMean) * (data(1) - dataMean) +
      (data(2) - dataMean) * (data(2) - dataMean) +
      (data(3) - dataMean) * (data(3) - dataMean) +
      (data(4) - dataMean) * (data(4) - dataMean));
  kernelMean = (kernel(0) + kernel(1) + kernel(2) + kernel(3)) / 4.0;
  kernelNorm = std::sqrt(
      (kernel(0) - kernelMean) * (kernel(0) - kernelMean) +
      (kernel(1) - kernelMean) * (kernel(1) - kernelMean) +
      (kernel(2) - kernelMean) * (kernel(2) - kernelMean) +
      (kernel(3) - kernelMean) * (kernel(3) - kernelMean));
  expectedResult = (
      (data(1) - dataMean) * (kernel(0) - kernelMean) +
      (data(2) - dataMean) * (kernel(1) - kernelMean) +
      (data(3) - dataMean) * (kernel(2) - kernelMean) +
      (data(4) - dataMean) * (kernel(3) - kernelMean)) /
      (dataNorm * kernelNorm);
  LMBUNIT_ASSERT_EQUAL_DELTA(result(4), expectedResult, 1e-5);
}

int main(int, char**) {
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST(
      (testFastNormalizedCorrelationFilter1DCyclicBTNoPad<float>()));
  LMBUNIT_RUN_TEST(
      (testFastNormalizedCorrelationFilter1DCyclicBTNoPad<double>()));

  LMBUNIT_RUN_TEST(
      (testFastNormalizedCorrelationFilter1DValueBT<float>(0.0f)));
  LMBUNIT_RUN_TEST(
      (testFastNormalizedCorrelationFilter1DValueBT<double>(0.0)));

  LMBUNIT_RUN_TEST(
      (testFastNormalizedCorrelationFilter1DValueBT<float>(1.0f)));
  LMBUNIT_RUN_TEST(
      (testFastNormalizedCorrelationFilter1DValueBT<double>(1.0)));

  LMBUNIT_RUN_TEST(
      (testFastNormalizedCorrelationFilter1DCropBT<float>()));
  LMBUNIT_RUN_TEST(
      (testFastNormalizedCorrelationFilter1DCropBT<double>()));

  LMBUNIT_RUN_TEST(
      (testFastNormalizedCorrelationFilter1DCropBTLargeKernel<float>()));
  LMBUNIT_RUN_TEST(
      (testFastNormalizedCorrelationFilter1DCropBTLargeKernel<double>()));

  LMBUNIT_WRITE_STATISTICS();
  return _nFails;
}
