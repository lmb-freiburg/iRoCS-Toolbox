/*
 * lblitzDataExplorer.hh
 *
 *  Created on: Nov 7, 2010
 *      Author: liu
 */

#ifndef LBLITZDATAEXPLORER_HH_
#define LBLITZDATAEXPLORER_HH_

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QImage>

#include <libArrayToolbox/Array.hh>

#include <libArrayToolbox/algo/helper.hh>

template<typename DataT>
  QImage
  arrayToColorQImage(const blitz::Array<DataT, 3> &data);

template<typename DataT>
  QImage
  arrayToGrayQImage(const blitz::Array<DataT, 2> &data);

class BlitzDataExplorer
{
public:

template<typename DataT>
  BlitzDataExplorer(blitz::Array<DataT, 4>& data)
  {
    blitz::Range all = blitz::Range::all();
    m_color = true;
    m_dataColor.resize(data.shape());
    m_dataColor_channel0.reference(m_dataColor(all, all, all, 0));
    m_dataColor_channel1.reference(m_dataColor(all, all, all, 1));
    m_dataColor_channel2.reference(m_dataColor(all, all, all, 2));

    dmin = 0;
    dmax = 255;
    m_dataColor = data;
    m_currentPos(0) = data.extent(0) / 2;
    m_currentPos(1) = data.extent(1) / 2;
    m_currentPos(2) = data.extent(2) / 2;
    m_currentRatio = 1;
    m_currentZoom = 0;
    m_size[0] = data.extent(0);
    m_size[1] = data.extent(1);
    m_size[2] = data.extent(2);

    //      m_originalDataColor.resize(data.shape());
    //      m_originalDataColor = blitz::cast<float>(data);
    ST = 1;
  }

template<typename DataT>
  BlitzDataExplorer(blitz::Array<DataT, 3>& data)
  {

    m_color = false;
    //      std::cerr << "resize" << std::endl;
    m_dataGray.resize(data.shape());
    //      std::cerr << "normalize" << std::endl;
    normalize(data, m_dataGray);
    //      std::cerr << "After normalize" << std::endl;
    m_currentPos = data.shape() / 2;
    m_currentRatio = 1;
    m_currentZoom = 0;
    m_size = data.shape();
    ST = 1;
    //      m_originalDataGray.resize(data.shape());
    //      m_originalDataGray = blitz::cast<float>(data);
  }

template<typename DataT>
  void
  updateData(blitz::Array<bool, 3>& mask, blitz::TinyVector<DataT, 3>& color, blitz::RectDomain<3> & domain)
  {
    std::cerr << "update data" << std::endl;
    m_dataColor_channel0(domain) = where(mask, (color(0) - dmin) / double((dmax - dmin)) * 255, m_dataColor_channel0(
        domain));
    m_dataColor_channel1(domain) = where(mask, (color(1) - dmin) / double((dmax - dmin)) * 255, m_dataColor_channel1(
        domain));
    m_dataColor_channel2(domain) = where(mask, (color(2) - dmin) / double((dmax - dmin)) * 255, m_dataColor_channel2(
        domain));
  }

template<typename DataT>
  void
  updateData(blitz::Array<bool, 3>& mask, DataT & grayValue, blitz::RectDomain<3> & domain)
  {
    m_dataGray(domain) = where(mask, (grayValue - dmin) / double((dmax - dmin)) * 255, m_dataGray(domain));
  }

template<typename DataT>
  void
  updateData(blitz::Array<DataT, 3>& image, blitz::RectDomain<3> & domain)
  {
    m_dataGray(domain) = (image - dmin) / double((dmax - dmin)) * 255;
  }

template<typename DataT>
  void
  updateData(blitz::Array<DataT, 4>& image, blitz::RectDomain<4> & domain)
  {
    m_dataColor(domain) = image(domain);
  }

template<typename DataT>
  void
  updateData(blitz::Array<DataT, 4>& data)
  {
    blitz::Range all = blitz::Range::all();
    m_color = true;
    m_dataColor.resize(data.shape());
    m_dataColor_channel0.reference(m_dataColor(all, all, all, 0));
    m_dataColor_channel1.reference(m_dataColor(all, all, all, 1));
    m_dataColor_channel2.reference(m_dataColor(all, all, all, 2));
    normalize(data, m_dataColor);
    m_size[0] = data.extent(0);
    m_size[1] = data.extent(1);
    m_size[2] = data.extent(2);
    //    m_originalDataColor.resize(data.shape());
    //    m_originalDataColor = blitz::cast<float>(data);
  }

template<typename DataT>
  void
  updateData(blitz::Array<DataT, 3>& data)
  {

    m_color = false;
    std::cerr << "resize" << std::endl;
    m_dataGray.resize(data.shape());
    std::cerr << "normalize" << std::endl;
    normalize(data, m_dataGray);
    std::cerr << "After normalize" << std::endl;
    m_size = data.shape();
    //    m_originalDataGray.resize(data.shape());
    //    m_originalDataGray = blitz::cast<float>(data);
  }

template<typename DataT, typename DataT2, int Dim, int Dim2>
  void
  normalize(blitz::Array<DataT, Dim> &data, blitz::Array<DataT2, Dim2> &normdata)
  {
    dmax = blitz::max(data);
    dmin = blitz::min(data);
    normdata = (data - dmin) / double((dmax - dmin)) * 255;
  }

public:
void
move(int x, int y, int z)
{
  m_currentPos = m_currentPos + blitz::TinyVector<int, 3>(z, y, x);
  keepInRange3D(m_currentPos, blitz::TinyVector<int, 3>(0, 0, 0), m_size - 1);
}

void
zoom(double r)
{
  m_currentZoom = m_currentZoom + r;
  m_currentZoom = std::min(std::max(m_currentZoom, -4.0), 4.0);
  m_currentRatio = std::pow(2, m_currentZoom);
}

void
addCursor(QImage& qview, int x, int y)
{
  for (int i = 0; i < qview.width(); i++)
  {
    qview.setPixel(i, y, 0xff0000ff);
  }
  for (int i = 0; i < qview.height(); i++)
  {
    qview.setPixel(x, i, 0xff0000ff);//(qview->pixel(x,i) & 0x0fffffff));
  }
}

QImage
getXYView()
{
  int z = std::floor(m_currentPos(0) + 0.5);

  if (!m_color)
  {
    blitz::Range all = blitz::Range::all();
    QImage qview;
    if (ST > 1)
    {
      LOG << ST << std::endl;
      int lb = lmax(z + 1 - ST, 0);
      int ub = lmin(z - 1 + ST, m_size(0) - 1);
      blitz::Range sliceRange(lb, ub);
      blitz::Array<unsigned char, 3> subview(m_dataGray(sliceRange, all, all));
      blitz::firstIndex i;
      blitz::secondIndex j;
      blitz::thirdIndex k;
      blitz::Array<unsigned char, 2> view(mean(subview(k, i, j), k));
      qview = arrayToGrayQImage(view);
    }
    else
    {
      blitz::Array<unsigned char, 2> view(m_dataGray(z, all, all));
      qview = arrayToGrayQImage(view);
    }
    QSize imageSize = QSize(qview.width() * m_currentRatio, qview.height() * m_currentRatio);
    qview = qview.scaled(imageSize, Qt::KeepAspectRatio);
    addCursor(qview, m_currentPos(2) * m_currentRatio + m_currentRatio / 2, m_currentPos(1) * m_currentRatio
        + m_currentRatio / 2);
    return qview;
  }
  else
  {
    blitz::Range all = blitz::Range::all();
    QImage qview;
    if (ST > 1)
    {
      //          LOG << ST << std::endl;
      int lb = lmax(z + 1 - ST, 0);
      int ub = lmin(z - 1 + ST, m_size(0) - 1);
      blitz::Range sliceRange(lb, ub);
      blitz::Array<unsigned char, 4> subview(m_dataColor(sliceRange, all, all, all));
      blitz::firstIndex i;
      blitz::secondIndex j;
      blitz::thirdIndex k;
      blitz::fourthIndex l;
      blitz::Array<unsigned char, 3> view(blitz::mean(subview(l, i, j, k), l));
      qview = arrayToColorQImage(view);
    }
    else
    {
      blitz::Array<unsigned char, 3> view(m_dataColor(z, all, all, all));
      qview = arrayToColorQImage(view);
    }
    QSize imageSize = QSize(qview.width() * m_currentRatio, qview.height() * m_currentRatio);
    qview = qview.scaled(imageSize, Qt::KeepAspectRatio);
    addCursor(qview, m_currentPos(2) * m_currentRatio + m_currentRatio / 2, m_currentPos(1) * m_currentRatio
        + m_currentRatio / 2);
    return qview;
  }

}

QImage
getXZView()
{
  int y = std::floor(m_currentPos(1) + 0.5);
  if (!m_color)
  {
    blitz::Range all = blitz::Range::all();
    blitz::Array<unsigned char, 2> view = m_dataGray(all, y, all);
    QImage qview = arrayToGrayQImage(view);
    QSize imageSize = QSize(qview.width() * m_currentRatio, qview.height() * m_currentRatio);
    qview = qview.scaled(imageSize, Qt::KeepAspectRatio);
    addCursor(qview, m_currentPos(2) * m_currentRatio + m_currentRatio / 2, m_currentPos(0) * m_currentRatio
        + m_currentRatio / 2);
    return qview;
  }
  else
  {
    blitz::Range all = blitz::Range::all();
    QImage qview;
    if (ST > 1)
    {
      //          LOG << ST << std::endl;
      int lb = lmax(y + 1 - ST, 0);
      int ub = lmin(y - 1 + ST, m_size(1) - 1);
      blitz::Range sliceRange(lb, ub);
      blitz::Array<unsigned char, 4> subview(m_dataColor(all, sliceRange, all, all));
      blitz::firstIndex i;
      blitz::secondIndex j;
      blitz::thirdIndex k;
      blitz::fourthIndex l;
      blitz::Array<unsigned char, 3> view(blitz::mean(subview(i, l, j, k), l));
      qview = arrayToColorQImage(view);
    }
    else
    {
      blitz::Array<unsigned char, 3> view(m_dataColor(all, y, all, all));
      qview = arrayToColorQImage(view);
    }
    QSize imageSize = QSize(qview.width() * m_currentRatio, qview.height() * m_currentRatio);
    qview = qview.scaled(imageSize, Qt::KeepAspectRatio);
    addCursor(qview, m_currentPos(2) * m_currentRatio + m_currentRatio / 2, m_currentPos(0) * m_currentRatio
        + m_currentRatio / 2);
    return qview;
  }
}

QImage
getZYView()
{
  int x = std::floor(m_currentPos(2) + 0.5);
  blitz::Range all = blitz::Range::all();
  if (!m_color)
  {
    blitz::Array<unsigned char, 2> view = m_dataGray(all, all, x);
    view.transposeSelf(1, 0);
    QImage qview = arrayToGrayQImage(view);
    QSize imageSize = QSize(qview.width() * m_currentRatio, qview.height() * m_currentRatio);
    qview = qview.scaled(imageSize, Qt::KeepAspectRatio);
    addCursor(qview, m_currentPos(0) * m_currentRatio + m_currentRatio / 2, m_currentPos(1) * m_currentRatio
        + m_currentRatio / 2);
    return qview;
  }
  else
  {
    QImage qview;
    if (ST > 1)
    {
      //          LOG << ST << std::endl;
      int lb = lmax(x + 1 - ST, 0);
      int ub = lmin(x - 1 + ST, m_size(2) - 1);
      blitz::Range sliceRange(lb, ub);
      blitz::Array<unsigned char, 4> subview(m_dataColor(all, all, sliceRange, all));
      blitz::firstIndex i;
      blitz::secondIndex j;
      blitz::thirdIndex k;
      blitz::fourthIndex l;
      blitz::Array<unsigned char, 3> view(blitz::mean(subview(i, j, l, k), l));
      view.transposeSelf(1, 0, 2);
      qview = arrayToColorQImage(view);
    }
    else
    {
      blitz::Array<unsigned char, 3> view(m_dataColor(all, all, x, all));
      view.transposeSelf(1, 0, 2);
      qview = arrayToColorQImage(view);
    }
    QSize imageSize = QSize(qview.width() * m_currentRatio, qview.height() * m_currentRatio);
    qview = qview.scaled(imageSize, Qt::KeepAspectRatio);
    addCursor(qview, m_currentPos(0) * m_currentRatio + m_currentRatio / 2, m_currentPos(1) * m_currentRatio
        + m_currentRatio / 2);
    return qview;
  }
}

void
setST(int st)
{
  ST = st;
}
//    blitz::TinyVector<DataT, Dim-3> getPointData();

//    void
//    QImageToArray(QImage *image, Array<DataT, Dim> &newArray);

public:
blitz::TinyVector<double, 3>
getCurrentCenter()
{
  return blitz::TinyVector<int, 3>(m_currentPos(2) * m_currentRatio + m_currentRatio / 2, m_currentPos(1)
      * m_currentRatio + m_currentRatio / 2, m_currentPos(0) * m_currentRatio + m_currentRatio / 2);
}
int
getCurrentX()
{
  return m_currentPos(2) * m_currentRatio + m_currentRatio / 2;
}
int
getCurrentY()
{
  return m_currentPos(1) * m_currentRatio + m_currentRatio / 2;
}
int
getCurrentZ()
{
  return m_currentPos(0) * m_currentRatio + m_currentRatio / 2;
}

void
setPos(int x, int y, int z)
{
  m_currentPos(2) = x > 0 ? std::floor(x / m_currentRatio) : m_currentPos(2);
  m_currentPos(1) = y > 0 ? std::floor(y / m_currentRatio) : m_currentPos(1);
  m_currentPos(0) = z > 0 ? std::floor(z / m_currentRatio) : m_currentPos(0);
  keepInRange3D(m_currentPos, blitz::TinyVector<int, 3>(0, 0, 0), m_size - 1);
}

void
setDataPos(int x, int y, int z)
{
  m_currentPos(2) = x;
  m_currentPos(1) = y;
  m_currentPos(0) = z;
  keepInRange3D(m_currentPos, blitz::TinyVector<int, 3>(0, 0, 0), m_size - 1);
}

blitz::TinyVector<double, 3>
getCurrentPos()
{
  return m_currentPos;
}

blitz::TinyVector<double, 6>
getDataOnPoint(int x, int y, int z)
{
  blitz::TinyVector<int, 3> point;
  point(2) = x > 0 ? std::floor(x / m_currentRatio) : m_currentPos(2);
  point(1) = y > 0 ? std::floor(y / m_currentRatio) : m_currentPos(1);
  point(0) = z > 0 ? std::floor(z / m_currentRatio) : m_currentPos(0);
  keepInRange3D(point, blitz::TinyVector<int, 3>(0, 0, 0), m_size - 1);
  int xx = point(2), yy = point(1), zz = point(0);
  if (m_color)
  {
    return blitz::TinyVector<double, 6>(point(2), point(1), point(0), m_dataColor(zz, yy, xx, 0), m_dataColor(zz, yy,
        xx, 1), m_dataColor(zz, yy, xx, 2));
  }
  else
  {
    //              return blitz::TinyVector<double, 6>(m_currentZoom, m_currentRatio, m_size(0),
    //                  m_size(1), m_size(2),
    //                  m_currentPos(0));
    return blitz::TinyVector<double, 6>(point(2), point(1), point(0), m_dataGray(zz, yy, xx), m_dataGray(zz, yy, xx),
        m_dataGray(zz, yy, xx));
  }
}

public:
bool m_color;
blitz::TinyVector<int, 3> m_size;
blitz::Array<float, 3> m_originalDataGray;
blitz::Array<float, 4> m_originalDataColor;
private:
blitz::Array<unsigned char, 4> m_dataColor;
blitz::Array<unsigned char, 3> m_dataColor_channel0;
blitz::Array<unsigned char, 3> m_dataColor_channel1;
blitz::Array<unsigned char, 3> m_dataColor_channel2;
int ST;

blitz::Array<unsigned char, 3> m_dataGray;
blitz::TinyVector<double, 3> m_currentPos;
double m_currentZoom;
double m_currentRatio;
double dmax, dmin;
};

template<typename DataT>
  QImage
  arrayToGrayQImage(const blitz::Array<DataT, 2> &data)
  {
    blitz::Range all = blitz::Range::all();
    blitz::TinyVector<double, 2> size = data.shape();
    blitz::Array<unsigned char, 3> imageBuffer(size(0), size(1), 4);

    imageBuffer(all, all, 0) = blitz::cast<unsigned char>(data);
    imageBuffer(all, all, 1) = blitz::cast<unsigned char>(data);
    imageBuffer(all, all, 2) = blitz::cast<unsigned char>(data);
    imageBuffer(all, all, 3) = 0xff;
    QImage image(size(1), size(0), QImage::Format_ARGB32);
    memcpy(image.bits(), imageBuffer.data(), size(0) * size(1) * 4);
    return image;
  }

template<typename DataT>
  QImage
  arrayToColorQImage(const blitz::Array<DataT, 3> &data)
  {
    blitz::Range all = blitz::Range::all();
    blitz::TinyVector<double, 3> size = data.shape();
    blitz::Array<unsigned char, 3> imageBuffer(size(0), size(1), 4);
    imageBuffer(all, all, blitz::Range(0, 2)) = blitz::cast<unsigned char>(data);
    imageBuffer(all, all, 3) = 0xff;
    QImage image(size(1), size(0), QImage::Format_ARGB32);
    memcpy(image.bits(), imageBuffer.data(), size(0) * size(1) * 4);
    return image;
  }

#endif /* LBLITZDATAEXPLORER_HH_ */
