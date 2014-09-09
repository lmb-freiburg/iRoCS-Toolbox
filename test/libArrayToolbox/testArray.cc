#include "lmbunit.hh"

#include <libArrayToolbox/Array.hh>
#include <libArrayToolbox/TinyMatrixOperators.hh>

template<typename DataT, int Dim>
static void testArrayConstructor()
{
  blitz::TinyVector<atb::BlitzIndexT,Dim> dataShape;
  blitz::TinyVector<double,Dim> elSize;
  blitz::TinyMatrix<double,Dim+1,Dim+1> trafo;
  for (int d = 0; d < Dim; ++d) 
  {
    dataShape(d) = static_cast<atb::BlitzIndexT>(
        10 + std::rand() / (RAND_MAX / 100));
    elSize(d) = 2.0 * static_cast<double>(std::rand()) /
        static_cast<double>(RAND_MAX);
  }
  for (int r = 0; r < Dim + 1; ++r)
      for (int c = 0; c < Dim + 1; ++c)
          trafo(r, c) = static_cast<double>(std::rand()) /
              static_cast<double>(RAND_MAX);
  
  {
    atb::Array<DataT,Dim> data;
    LMBUNIT_ASSERT(
        blitz::all(data.shape() == static_cast<atb::BlitzIndexT>(0)));
    LMBUNIT_ASSERT(blitz::all(data.elementSizeUm() == 1.0));
    LMBUNIT_ASSERT(
        blitz::all(data.transformation() ==
                   atb::traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one));
  }

  {
    atb::Array<DataT,Dim> data(dataShape);
    LMBUNIT_ASSERT(blitz::all(data.shape() == dataShape));
    LMBUNIT_ASSERT(blitz::all(data.elementSizeUm() == 1.0));
    LMBUNIT_ASSERT(
        blitz::all(data.transformation() ==
                   atb::traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one));
  }

  {
    atb::Array<DataT,Dim> data(dataShape, elSize);
    LMBUNIT_ASSERT(blitz::all(data.shape() == dataShape));
    LMBUNIT_ASSERT(blitz::all(data.elementSizeUm() == elSize));
    LMBUNIT_ASSERT(
        blitz::all(data.transformation() ==
                   atb::traits< blitz::TinyMatrix<double,Dim+1,Dim+1> >::one));
  }

  {
    atb::Array<DataT,Dim> data(dataShape, elSize, trafo);
    LMBUNIT_ASSERT(blitz::all(data.shape() == dataShape));
    LMBUNIT_ASSERT(blitz::all(data.elementSizeUm() == elSize));
    LMBUNIT_ASSERT(blitz::all(data.transformation() == trafo));

    atb::Array<DataT,Dim> data2(data);
    LMBUNIT_ASSERT(blitz::all(data2.shape() == dataShape));
    LMBUNIT_ASSERT(blitz::all(data2.elementSizeUm() == elSize));
    LMBUNIT_ASSERT(blitz::all(data2.transformation() == trafo));
    LMBUNIT_ASSERT_EQUAL(data.data(), data2.data());
  }
}

template<typename DataT, int Dim>
static void testArrayLoadSave()
{
  blitz::TinyVector<atb::BlitzIndexT,Dim> dataShape;
  blitz::TinyVector<double,Dim> elSize;
  blitz::TinyMatrix<double,Dim+1,Dim+1> trafo;
  for (int d = 0; d < Dim; ++d) 
  {
    dataShape(d) = static_cast<atb::BlitzIndexT>(
        10 + std::rand() / (RAND_MAX / 100));
    elSize(d) = 2.0 * static_cast<double>(std::rand()) /
        static_cast<double>(RAND_MAX);
  }
  for (int r = 0; r < Dim + 1; ++r)
      for (int c = 0; c < Dim + 1; ++c)
          trafo(r, c) = static_cast<double>(std::rand()) /
              static_cast<double>(RAND_MAX);

  atb::Array<DataT,Dim> data(dataShape, elSize, trafo);
  for (size_t i = 0; i < data.size(); ++i)
      data.data()[i] = typename atb::traits<DataT>::BasicT(i) *
          atb::traits<DataT>::one;

  try
  {
    data.save(TOP_BUILD_DIR "/test/libArrayToolbox/debug.h5", "/data", 3);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE("Could not save atb::Array");
    if (system("rm " TOP_BUILD_DIR "/test/libArrayToolbox/debug.h5") == -1)
        std::cerr << "Warning: Could not remove test file: System call failed!"
                  << std::endl;
    return;
  }
  
  atb::Array<DataT,Dim> data2;
  try
  {
    data2.load(TOP_BUILD_DIR "/test/libArrayToolbox/debug.h5", "/data");
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE("Could not load atb::Array");
    if (system("rm " TOP_BUILD_DIR "/test/libArrayToolbox/debug.h5") == -1)
        std::cerr << "Warning: Could not remove test file: System call failed!"
                  << std::endl;
    return;
  }
  
  LMBUNIT_ASSERT(blitz::all(data.shape() == data2.shape()));
  LMBUNIT_ASSERT(blitz::all(data.elementSizeUm() == data2.elementSizeUm()));
  LMBUNIT_ASSERT(blitz::all(data.transformation() == data2.transformation()));
  LMBUNIT_ASSERT(blitz::all(data == data2));

  if (system("rm " TOP_BUILD_DIR "/test/libArrayToolbox/debug.h5") != 0)
      std::cerr << "Warning: Could not remove test file" << std::endl;
}

template<typename DataT, int Dim>
static void testArrayLoadSaveTinyVector()
{
  blitz::TinyVector<atb::BlitzIndexT,Dim> dataShape;
  blitz::TinyVector<double,Dim> elSize;
  blitz::TinyMatrix<double,Dim+1,Dim+1> trafo;
  for (int d = 0; d < Dim; ++d) 
  {
    dataShape(d) = static_cast<atb::BlitzIndexT>(
        10 + std::rand() / (RAND_MAX / 100));
    elSize(d) = 2.0 * static_cast<double>(std::rand()) /
        static_cast<double>(RAND_MAX);
  }
  for (int r = 0; r < Dim + 1; ++r)
      for (int c = 0; c < Dim + 1; ++c)
          trafo(r, c) = static_cast<double>(std::rand()) /
              static_cast<double>(RAND_MAX);

  atb::Array<blitz::TinyVector<DataT,3>,Dim> data(dataShape, elSize, trafo);
  for (size_t i = 0; i < data.size(); ++i)
      data.data()[i] = static_cast<DataT>(i), static_cast<DataT>(-i),
          static_cast<DataT>(100 * static_cast<double>(rand()) /
                             static_cast<double>(RAND_MAX));

  try
  {
    data.save(TOP_BUILD_DIR "/test/libArrayToolbox/debug.h5", "/data", 3);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE("Could not save atb::Array");
    if (system("rm " TOP_BUILD_DIR "/test/libArrayToolbox/debug.h5") == -1)
        std::cerr << "Warning: Could not remove test file. System call failed!"
                  << std::endl;
    return;
  }
  
  atb::Array<blitz::TinyVector<DataT,3>,Dim> data2;
  try
  {
    data2.load(TOP_BUILD_DIR "/test/libArrayToolbox/debug.h5", "/data");
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE("Could not load atb::Array");
    if (system("rm " TOP_BUILD_DIR "/test/libArrayToolbox/debug.h5") == -1)
        std::cerr << "Warning: Could not remove test file. System call failed!"
                  << std::endl;
    return;
  }
  
  LMBUNIT_ASSERT(blitz::all(data.shape() == data2.shape()));
  LMBUNIT_ASSERT(blitz::all(data.elementSizeUm() == data2.elementSizeUm()));
  LMBUNIT_ASSERT(blitz::all(data.transformation() == data2.transformation()));
  LMBUNIT_ASSERT(blitz::all(data[0] == data2[0]));
  LMBUNIT_ASSERT(blitz::all(data[1] == data2[1]));
  LMBUNIT_ASSERT(blitz::all(data[2] == data2[2]));

  if (system("rm " TOP_BUILD_DIR "/test/libArrayToolbox/debug.h5") != 0)
      std::cerr << "Warning: Could not remove test file" << std::endl;
}

template<typename DataT, int Dim>
static void testFilterDeriche()
{
  blitz::TinyVector<atb::BlitzIndexT,Dim> dataShape;
  blitz::TinyVector<double,Dim> elSize;
  for (int d = 0; d < Dim; ++d) 
  {
    dataShape(d) = static_cast<atb::BlitzIndexT>(
        10 + std::rand() / (RAND_MAX / 100));
    elSize(d) = 2.0 * static_cast<double>(std::rand()) /
        static_cast<double>(RAND_MAX);
  }
  
  atb::Array<DataT,Dim> data(dataShape, elSize);
  std::memset(data.data(), 0, data.size() * sizeof(DataT));
  data(blitz::TinyVector<atb::BlitzIndexT,Dim>(dataShape / 2)) =
      static_cast<DataT>(100) * atb::traits<DataT>::one;
  
  data.filterDeriche(3.0, 0);
  LMBUNIT_ASSERT(blitz::all(blitz::maxIndex(data) == dataShape / 2));
}

template<typename DataT, int Dim>
static void testFilterDericheTinyVector()
{
  blitz::TinyVector<atb::BlitzIndexT,Dim> dataShape;
  blitz::TinyVector<double,Dim> elSize;
  for (int d = 0; d < Dim; ++d) 
  {
    dataShape(d) = static_cast<atb::BlitzIndexT>(
        10 + std::rand() / (RAND_MAX / 100));
    elSize(d) = 2.0 * static_cast<double>(std::rand()) /
        static_cast<double>(RAND_MAX);
  }
  
  atb::Array<blitz::TinyVector<DataT,3>,Dim> data(dataShape, elSize);
  std::memset(data.data(), 0, data.size() * sizeof(DataT));
  data(blitz::TinyVector<atb::BlitzIndexT,Dim>(dataShape / 2)) =
      blitz::TinyVector<DataT,3>(
          static_cast<DataT>(100) *
          atb::traits< blitz::TinyVector<DataT,3> >::one);
  
  data.filterDeriche(3.0, 0);
  LMBUNIT_ASSERT(blitz::all(blitz::maxIndex(data[0]) == dataShape / 2));
  LMBUNIT_ASSERT(blitz::all(blitz::maxIndex(data[1]) == dataShape / 2));
  LMBUNIT_ASSERT(blitz::all(blitz::maxIndex(data[2]) == dataShape / 2));
}

template<int Dim>
static void testLowerUpperBound()
{
  blitz::TinyVector<atb::BlitzIndexT,Dim> dataShape;
  blitz::TinyVector<double,Dim> elSize;
  blitz::TinyMatrix<double,Dim+1,Dim+1> trafo;
  for (int d = 0; d < Dim; ++d) 
  {
    dataShape(d) = static_cast<atb::BlitzIndexT>(
        10 + std::rand() / (RAND_MAX / 100));
    elSize(d) = 2.0 * static_cast<double>(std::rand()) /
        static_cast<double>(RAND_MAX);
  }
  for (int r = 0; r < Dim + 1; ++r)
      for (int c = 0; c < Dim + 1; ++c)
          trafo(r, c) = (static_cast<double>(std::rand()) /
                         static_cast<double>(RAND_MAX)) - 0.5;

  atb::Array<unsigned char,Dim> data(dataShape, elSize, trafo);

  blitz::TinyVector<double,Dim> lb(
      atb::homogeneousToEuclidean(
          atb::invert(data.transformation()) *
          atb::euclideanToHomogeneous(blitz::TinyVector<double,Dim>(0.0))));
  blitz::TinyVector<double,Dim> ub(lb);
  for (size_t i = 1; i < (1 << Dim); ++i)
  {
    int tmp = static_cast<int>(i);
    blitz::TinyVector<double,Dim> cornerPos;
    for (int d = Dim - 1; d >= 0; --d)
    {
      cornerPos(d) = ((tmp % 2) * data.extent(d)) * data.elementSizeUm()(d);
      tmp /= 2;
    }
    cornerPos = atb::homogeneousToEuclidean(
        atb::invert(data.transformation()) *
        atb::euclideanToHomogeneous(cornerPos));
    for (int d = 0; d < Dim; ++d)
    {
      if (cornerPos(d) < lb(d)) lb(d) = cornerPos(d);
      if (cornerPos(d) > ub(d)) ub(d) = cornerPos(d);
    }
  }
  LMBUNIT_ASSERT_EQUAL_DELTA(
      std::sqrt(blitz::dot(lb - data.lowerBoundUm(),
                           lb - data.lowerBoundUm())), 0.0, 1e-10);
  LMBUNIT_ASSERT_EQUAL_DELTA(
      std::sqrt(blitz::dot(ub - data.upperBoundUm(),
                           ub - data.upperBoundUm())), 0.0, 1e-10);
}

template<typename DataT,int Dim>
static void testValueAt()
{
  blitz::TinyVector<atb::BlitzIndexT,Dim> dataShape;
  blitz::TinyVector<double,Dim> elSize;
  blitz::TinyMatrix<double,Dim+1,Dim+1> trafo;
  for (int d = 0; d < Dim; ++d) 
  {
    dataShape(d) = static_cast<atb::BlitzIndexT>(
        10 + std::rand() / (RAND_MAX / 100));
    elSize(d) = 2.0 * static_cast<double>(std::rand()) /
        static_cast<double>(RAND_MAX);
  }
  for (int r = 0; r < Dim + 1; ++r)
      for (int c = 0; c < Dim + 1; ++c)
          trafo(r, c) = (static_cast<double>(std::rand()) /
                         static_cast<double>(RAND_MAX)) - 0.5;
  
  atb::Array<DataT,Dim> data(dataShape, elSize, trafo);
  for (size_t i = 0; i < data.size(); ++i)
      data.data()[i] = static_cast<DataT>(i);
 
  blitz::TinyVector<double,Dim> peekPos(
      data.lowerBoundUm() + (data.upperBoundUm() - data.lowerBoundUm()) / 2.0);

  blitz::TinyVector<double,Dim> peekPosPx(
      atb::homogeneousToEuclidean(
          data.transformation() * atb::euclideanToHomogeneous(peekPos)) /
      data.elementSizeUm());
  LMBUNIT_ASSERT_EQUAL_DELTA(
      data.interpolator().get(data, peekPosPx), data.valueAt(peekPos), 1e-10);
}

template<typename DataT,int Dim>
static void testValueAtTinyVector()
{
  blitz::TinyVector<atb::BlitzIndexT,Dim> dataShape;
  blitz::TinyVector<double,Dim> elSize;
  blitz::TinyMatrix<double,Dim+1,Dim+1> trafo;
  for (int d = 0; d < Dim; ++d) 
  {
    dataShape(d) = static_cast<atb::BlitzIndexT>(
        10 + std::rand() / (RAND_MAX / 100));
    elSize(d) = 2.0 * static_cast<double>(std::rand()) /
        static_cast<double>(RAND_MAX);
  }
  for (int r = 0; r < Dim + 1; ++r)
      for (int c = 0; c < Dim + 1; ++c)
          trafo(r, c) = (static_cast<double>(std::rand()) /
                         static_cast<double>(RAND_MAX)) - 0.5;
  
  atb::Array<blitz::TinyVector<DataT,3>,Dim> data(dataShape, elSize, trafo);
  for (size_t i = 0; i < data.size(); ++i)
      data.data()[i] = static_cast<DataT>(i), static_cast<DataT>(-i),
          static_cast<DataT>(100 * static_cast<double>(rand()) /
                             static_cast<double>(RAND_MAX));

  blitz::TinyVector<double,Dim> peekPos(
      data.lowerBoundUm() + (data.upperBoundUm() - data.lowerBoundUm()) / 2.0);

  blitz::TinyVector<double,Dim> peekPosPx(
      atb::homogeneousToEuclidean(
          data.transformation() * atb::euclideanToHomogeneous(peekPos)) /
      data.elementSizeUm());
  for (int d = 0; d < Dim; ++d)
      LMBUNIT_ASSERT_EQUAL_DELTA(
          data.interpolator().get(data, peekPosPx)(d),
          data.valueAt(peekPos)(d), 1e-10);
}

int main(int, char**)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST((testArrayConstructor<unsigned char,1>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<char,1>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<unsigned short,1>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<short,1>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<unsigned int,1>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<int,1>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<unsigned long,1>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<long,1>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<unsigned long long,1>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<long long,1>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<float,1>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<double,1>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<unsigned char,2>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<char,2>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<unsigned short,2>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<short,2>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<unsigned int,2>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<int,2>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<unsigned long,2>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<long,2>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<unsigned long long,2>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<long long,2>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<float,2>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<double,2>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<unsigned char,3>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<char,3>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<unsigned short,3>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<short,3>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<unsigned int,3>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<int,3>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<unsigned long,3>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<long,3>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<unsigned long long,3>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<long long,3>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<float,3>()));
  LMBUNIT_RUN_TEST((testArrayConstructor<double,3>()));
  LMBUNIT_RUN_TEST(
      (testArrayConstructor<blitz::TinyVector<unsigned char,3>,3>()));
  LMBUNIT_RUN_TEST(
      (testArrayConstructor<blitz::TinyVector<unsigned short,3>,3>()));
  LMBUNIT_RUN_TEST(
      (testArrayConstructor<blitz::TinyVector<float,3>,3>()));
  LMBUNIT_RUN_TEST(
      (testArrayConstructor<blitz::TinyVector<double,3>,3>()));

  LMBUNIT_RUN_TEST((testArrayLoadSave<unsigned char,1>()));
  LMBUNIT_RUN_TEST((testArrayLoadSave<unsigned short,1>()));
  LMBUNIT_RUN_TEST((testArrayLoadSave<int,1>()));
  LMBUNIT_RUN_TEST((testArrayLoadSave<float,1>()));
  LMBUNIT_RUN_TEST((testArrayLoadSave<double,1>()));
  LMBUNIT_RUN_TEST((testArrayLoadSave<unsigned char,2>()));
  LMBUNIT_RUN_TEST((testArrayLoadSave<unsigned short,2>()));
  LMBUNIT_RUN_TEST((testArrayLoadSave<int,2>()));
  LMBUNIT_RUN_TEST((testArrayLoadSave<float,2>()));
  LMBUNIT_RUN_TEST((testArrayLoadSave<double,2>()));
  LMBUNIT_RUN_TEST((testArrayLoadSave<unsigned char,3>()));
  LMBUNIT_RUN_TEST((testArrayLoadSave<unsigned short,3>()));
  LMBUNIT_RUN_TEST((testArrayLoadSave<int,3>()));
  LMBUNIT_RUN_TEST((testArrayLoadSave<float,3>()));
  LMBUNIT_RUN_TEST((testArrayLoadSave<double,3>()));

  LMBUNIT_RUN_TEST((testArrayLoadSaveTinyVector<unsigned char,1>()));
  LMBUNIT_RUN_TEST((testArrayLoadSaveTinyVector<float,1>()));
  LMBUNIT_RUN_TEST((testArrayLoadSaveTinyVector<double,1>()));
  LMBUNIT_RUN_TEST((testArrayLoadSaveTinyVector<unsigned char,2>()));
  LMBUNIT_RUN_TEST((testArrayLoadSaveTinyVector<float,2>()));
  LMBUNIT_RUN_TEST((testArrayLoadSaveTinyVector<double,2>()));
  LMBUNIT_RUN_TEST((testArrayLoadSaveTinyVector<unsigned char,3>()));
  LMBUNIT_RUN_TEST((testArrayLoadSaveTinyVector<float,3>()));
  LMBUNIT_RUN_TEST((testArrayLoadSaveTinyVector<double,3>()));

  LMBUNIT_RUN_TEST((testFilterDeriche<unsigned char,1>()));
  LMBUNIT_RUN_TEST((testFilterDeriche<unsigned short,1>()));
  LMBUNIT_RUN_TEST((testFilterDeriche<int,1>()));
  LMBUNIT_RUN_TEST((testFilterDeriche<float,1>()));
  LMBUNIT_RUN_TEST((testFilterDeriche<double,1>()));
  LMBUNIT_RUN_TEST((testFilterDeriche<unsigned char,2>()));
  LMBUNIT_RUN_TEST((testFilterDeriche<unsigned short,2>()));
  LMBUNIT_RUN_TEST((testFilterDeriche<int,2>()));
  LMBUNIT_RUN_TEST((testFilterDeriche<float,2>()));
  LMBUNIT_RUN_TEST((testFilterDeriche<double,2>()));
  LMBUNIT_RUN_TEST((testFilterDeriche<unsigned char,3>()));
  LMBUNIT_RUN_TEST((testFilterDeriche<unsigned short,3>()));
  LMBUNIT_RUN_TEST((testFilterDeriche<int,3>()));
  LMBUNIT_RUN_TEST((testFilterDeriche<float,3>()));
  LMBUNIT_RUN_TEST((testFilterDeriche<double,3>()));

  LMBUNIT_RUN_TEST((testFilterDericheTinyVector<unsigned char,1>()));
  LMBUNIT_RUN_TEST((testFilterDericheTinyVector<float,1>()));
  LMBUNIT_RUN_TEST((testFilterDericheTinyVector<double,1>()));
  LMBUNIT_RUN_TEST((testFilterDericheTinyVector<unsigned char,2>()));
  LMBUNIT_RUN_TEST((testFilterDericheTinyVector<float,2>()));
  LMBUNIT_RUN_TEST((testFilterDericheTinyVector<double,2>()));
  LMBUNIT_RUN_TEST((testFilterDericheTinyVector<unsigned char,3>()));
  LMBUNIT_RUN_TEST((testFilterDericheTinyVector<float,3>()));
  LMBUNIT_RUN_TEST((testFilterDericheTinyVector<double,3>()));

  LMBUNIT_RUN_TEST((testLowerUpperBound<1>()));
  LMBUNIT_RUN_TEST((testLowerUpperBound<2>()));
  LMBUNIT_RUN_TEST((testLowerUpperBound<3>()));

  LMBUNIT_RUN_TEST((testValueAt<unsigned char,1>()));
  LMBUNIT_RUN_TEST((testValueAt<unsigned short,1>()));
  LMBUNIT_RUN_TEST((testValueAt<int,1>()));
  LMBUNIT_RUN_TEST((testValueAt<float,1>()));
  LMBUNIT_RUN_TEST((testValueAt<double,1>()));
  LMBUNIT_RUN_TEST((testValueAt<unsigned char,2>()));
  LMBUNIT_RUN_TEST((testValueAt<unsigned short,2>()));
  LMBUNIT_RUN_TEST((testValueAt<int,2>()));
  LMBUNIT_RUN_TEST((testValueAt<float,2>()));
  LMBUNIT_RUN_TEST((testValueAt<double,2>()));
  LMBUNIT_RUN_TEST((testValueAt<unsigned char,3>()));
  LMBUNIT_RUN_TEST((testValueAt<unsigned short,3>()));
  LMBUNIT_RUN_TEST((testValueAt<int,3>()));
  LMBUNIT_RUN_TEST((testValueAt<float,3>()));
  LMBUNIT_RUN_TEST((testValueAt<double,3>()));
  
  LMBUNIT_RUN_TEST((testValueAtTinyVector<unsigned char,1>()));
  LMBUNIT_RUN_TEST((testValueAtTinyVector<double,1>()));
  LMBUNIT_RUN_TEST((testValueAtTinyVector<unsigned char,2>()));
  LMBUNIT_RUN_TEST((testValueAtTinyVector<double,2>()));
  LMBUNIT_RUN_TEST((testValueAtTinyVector<unsigned char,3>()));
  LMBUNIT_RUN_TEST((testValueAtTinyVector<double,3>()));
  
  LMBUNIT_WRITE_STATISTICS();
  return _nFails;
}
