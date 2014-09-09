#include <fstream>
#include <vector>
#include <blitz/array.h>
// #include <blitz/tinyvec-et.h>

namespace atb
{

  class STLFileWriter
  {
    std::ofstream  _outFile;
    std::string    _objectName;
    float          _tailRadius;
    float          _headRadius;
    int            _nSegments;
    float          _borderRadius;
    int            _nEllipseSegments;

  public:
  
    STLFileWriter(const std::string& fileName,
                  const std::string& objectName="dummy")
            : _outFile(fileName.c_str()),
              _objectName(objectName),
              _tailRadius(0.5),
              _headRadius(2),
              _nSegments(20),
              _borderRadius(0.5),
              _nEllipseSegments(72)
          {
            _outFile << "solid " << _objectName << "\n";
          }
  
    ~STLFileWriter()
          {
            _outFile << "endsolid " << _objectName << "\n";
          }
  
          

    void drawTriangle(const blitz::TinyVector<float,3>& p1,
                      const blitz::TinyVector<float,3>& p2,
                      const blitz::TinyVector<float,3>& p3,
                      const blitz::TinyVector<float,3>& normal =
                      blitz::TinyVector<float,3>(0,0,0))
          {
            _outFile << "facet normal " 
                     << normal(0) << " " << normal(1) << " " << normal(2)
                     << "\n"
                     << "  outer loop\n"
                     << "     vertex " 
                     << p1(0) << " " << p1(1) << " " << p1(2) << "\n"
                     << "     vertex " 
                     << p2(0) << " " << p2(1) << " " << p2(2) << "\n"
                     << "     vertex " 
                     << p3(0) << " " << p3(1) << " " << p3(2) << "\n"
                     << "   endloop\n"
                     << "endfacet\n";
          }
  
    void drawRotationObjectFromContour(
        const blitz::TinyVector<float,3>& origin, 
        const blitz::TinyVector<float,3>& nx,
        const blitz::TinyVector<float,3>& ny,
        const blitz::TinyVector<float,3>& nz,
        const std::vector< blitz::TinyVector<float,2> >& contour,
        const std::vector< blitz::TinyVector<float,2> >& normals,
        const int nAngles)
          {
            // make a rotation object from this contour
            for( int i = 0; i < nAngles; ++i)
            {
              float phi1 = float(i)/nAngles*2*M_PI;
              float phi2 = float(i+1)/nAngles*2*M_PI;
              blitz::TinyVector<float,3> rotNy1 = 
                  sin( phi1) * ny + cos( phi1) * nz;
            
              blitz::TinyVector<float,3> rotNy2 = 
                  sin( phi2) * ny + cos( phi2) * nz;
            
              blitz::TinyVector<float,3> rotNyMean = 
                  (rotNy1 + rotNy2)/2;
              rotNyMean /= sqrt( blitz::dot( rotNyMean, rotNyMean));
            
              for( size_t j = 0; j < contour.size()-1; ++j)
              {
                blitz::TinyVector<float,3> p1 = 
                    origin 
                    + nx *     contour[j](0) 
                    + rotNy1 * contour[j](1);

                blitz::TinyVector<float,3> p2 = 
                    origin 
                    + nx *     contour[j](0) 
                    + rotNy2 * contour[j](1);

                blitz::TinyVector<float,3> p3 = 
                    origin 
                    + nx *     contour[j+1](0) 
                    + rotNy1 * contour[j+1](1);

                blitz::TinyVector<float,3> p4 = 
                    origin 
                    + nx *     contour[j+1](0) 
                    + rotNy2 * contour[j+1](1);

                blitz::TinyVector<float,3> normal = 
                    nx *     normals[j](0) 
                    + rotNyMean * normals[j](1);

                // triangle 1 (p1 -- p2 -- p3)
                if(blitz::any(p1 != p2))
                {
                  drawTriangle( p1, p2, p3, normal);
                }
                
                // triangle 2 (p2 -- p4 -- p3)
                if(blitz::any(p3 != p4))
                {
                  drawTriangle( p2, p4, p3, normal);
                }
              }
              
            }
          }
  
          
    void drawCylinder(const blitz::TinyVector<float,3>& origin, 
                      const blitz::TinyVector<float,3>& dxyz,
                      double radius = -1)
          {
            if (radius <= 0) radius = _tailRadius;
          
            // compute x- and y- unit vectors
            float length = sqrt( blitz::dot( dxyz, dxyz));
          
            blitz::TinyVector<float,3> nx = dxyz / length;

            blitz::TinyVector<float,3> unitCol(0,0,1);
            blitz::TinyVector<float,3> ny;
            blitz::TinyVector<float,3> nz;
          
            ny = blitz::cross( nx, unitCol);
            nz = blitz::cross( nx, ny);
          
            ny /= sqrt( blitz::dot( ny, ny));
            nz /= sqrt( blitz::dot( nz, nz));

            // create the contour of the arrow
            std::vector<blitz::TinyVector<float,2> > contour(4);
            contour[0] = length, 0;
            contour[1] = length, radius;
            contour[2] = 0, radius;
            contour[3] = 0,0;
          
            std::vector<blitz::TinyVector<float,2> > normals(4);
            normals[0] = 1, 0;
            normals[1] = 0, 1;
            normals[2] = 0, 1;
            normals[3] = -1,0;
          
            for( size_t i = 0; i < normals.size(); ++i)
            {
              normals[i] /= sqrt( blitz::dot( normals[i], normals[i]));
            }
          
            // make a rotation object from this contour
            drawRotationObjectFromContour( origin, nx, ny, nz,
                                           contour, normals, _nSegments);
          }
  


    void drawArrow(const blitz::TinyVector<float,3>& origin, 
                   const blitz::TinyVector<float,3>& dxyz)
          {
            // compute x- and y- unit vectors
            float length = std::sqrt( blitz::dot( dxyz, dxyz));
          
            blitz::TinyVector<float,3> nx = dxyz / length;

            blitz::TinyVector<float,3> unitCol(0,0,1);
            blitz::TinyVector<float,3> ny;
            blitz::TinyVector<float,3> nz;
          
            ny = blitz::cross( nx, unitCol);
            nz = blitz::cross( nx, ny);
          
            ny /= std::sqrt( blitz::dot( ny, ny));
            nz /= std::sqrt( blitz::dot( nz, nz));

            // create the contour of the arrow
            std::vector< blitz::TinyVector<float,2> > contour(5);
            contour[0] = length, 0;
            contour[1] = length-2*_headRadius, _headRadius;
            contour[2] = length-2*_headRadius, _tailRadius;
            contour[3] = 0, _tailRadius;
            contour[4] = 0,0;
          
            std::vector< blitz::TinyVector<float,2> > normals(5);
            normals[0] = 1, 0;
            normals[1] = 1, 2;
            normals[2] = -1, 0;
            normals[3] = 0, 1;
            normals[4] = -1,0;
          
            for( size_t i = 0; i < normals.size(); ++i)
            {
              normals[i] /= std::sqrt( blitz::dot( normals[i], normals[i]));
            }
          
            // make a rotation object from this contour
            drawRotationObjectFromContour( origin, nx, ny, nz,
                                           contour, normals, _nSegments);
          }
  
  
    void drawEllipse(const blitz::TinyVector<float,3>& origin, 
                     const blitz::TinyVector<float,3>& axis1,
                     const blitz::TinyVector<float,3>& axis2)
          {
            float radius1 = sqrt( blitz::dot( axis1, axis1));
            float radius2 = sqrt( blitz::dot( axis2, axis2));
          
            blitz::TinyVector<float,3> ny = axis1 / radius1;
            blitz::TinyVector<float,3> nz = axis2 / radius2;
            blitz::TinyVector<float,3> nx;
            nx = blitz::cross( ny, nz);
          
            // create the roational contour for the border of the disc
            std::vector<blitz::TinyVector<float,2> > contour(_nSegments);
            std::vector<blitz::TinyVector<float,2> > normals(_nSegments);
            for( int i = 0; i < _nSegments; ++i)
            {
              float phi = float(i)/(_nSegments-1)*2*M_PI;
              contour[i] = 
                  _borderRadius*sin(phi), 
                  _borderRadius*cos(phi);
              //float phiNormal = (float(i)-0.5)/(_nSegments-1)*2*M_PI;
              normals[i] = sin(phi), cos(phi);
            }
            // normalize normals
            for( size_t i = 0; i < normals.size(); ++i)
            {
              normals[i] /= sqrt( blitz::dot( normals[i], normals[i]));
            }
          
            // make a rotation object from this contour
            for( int i = 0; i < _nEllipseSegments; ++i)
            {
              float phi1 = float(i)/_nEllipseSegments*2*M_PI;
              float phi2 = float(i+1)/_nEllipseSegments*2*M_PI;
              blitz::TinyVector<float,3> center1 = 
                  radius1 * sin( phi1) * ny + radius2 * cos( phi1) * nz;
            
              blitz::TinyVector<float,3> rotNy1 = 
                  center1 / sqrt( blitz::dot( center1, center1));
            
              blitz::TinyVector<float,3> center2 = 
                  radius1 * sin( phi2) * ny + radius2 * cos( phi2) * nz;
            
              blitz::TinyVector<float,3> rotNy2 = 
                  center2 / sqrt( blitz::dot( center2, center2));

              blitz::TinyVector<float,3> rotNyMean = 
                  (rotNy1 + rotNy2)/2;
              rotNyMean /= sqrt( blitz::dot( rotNyMean, rotNyMean));
            
              for( size_t j = 0; j < contour.size()-1; ++j)
              {
                blitz::TinyVector<float,3> p1 = 
                    origin 
                    + nx *     contour[j](0) 
                    + center1 + rotNy1 * contour[j](1);

                blitz::TinyVector<float,3> p2 = 
                    origin 
                    + nx *     contour[j](0) 
                    + center2 + rotNy2 * contour[j](1);

                blitz::TinyVector<float,3> p3 = 
                    origin 
                    + nx *     contour[j+1](0) 
                    + center1 + rotNy1 * contour[j+1](1);

                blitz::TinyVector<float,3> p4 = 
                    origin 
                    + nx *     contour[j+1](0) 
                    + center2 + rotNy2 * contour[j+1](1);

                blitz::TinyVector<float,3> normal = 
                    nx *     normals[j](0) 
                    + rotNyMean * normals[j](1);
                
                // triangle 1 (p1 -- p2 -- p3)
                if(blitz::any(p1 != p2))
                {
                  drawTriangle( p1, p2, p3, normal);
                }
                
                // triangle 2 (p2 -- p4 -- p3)
                if(blitz::any(p3 != p4))
                {
                  drawTriangle( p2, p4, p3, normal);
                }

              }
            }
            
              
          }
  
    void drawSphere(const blitz::TinyVector<float,3>& center,
                    const float radius, const int halfCircleSampling = 10)
          { 
            //sample points for one half circle
            const int sampling = halfCircleSampling;
            blitz::TinyVector<float,3> znormal(0,0,1);
	  
            blitz::Array< blitz::TinyVector<float,3>, 1>
                startHalfCircleSamples(sampling);
            startHalfCircleSamples = blitz::TinyVector<float,3>(0,0,0);
	  
            for (int i = 0; i < sampling; i++)
            {
              float factor = ( float(i)/float(sampling-1)) - 0.5;
              float x = radius * cos( factor * M_PI);
              float y = 0;
              float z = radius * sin( factor * M_PI);
              startHalfCircleSamples(i) = blitz::TinyVector<float,3>(x,y,z);
            }
	  
            blitz::Array< blitz::TinyVector<float,3>, 1>
                halfCircleSamples(sampling);
            halfCircleSamples = startHalfCircleSamples;
            blitz::Array< blitz::TinyVector<float,3>, 1>
                nextHalfCircleSamples(sampling);
            nextHalfCircleSamples = blitz::TinyVector<float,3>(0,0,0);
	  
            for (int i = 1; i < 2*sampling; ++i)
            {
              float factor = ( float(i)/float(2*sampling-1));
              float phi = factor * 2*M_PI;
              float cosPhi = cos(phi);
              float sinPhi = sin(phi);
		  
              // counter-clockwise rotation around z-axes
              blitz::Array<float,2> rotMatrix(3,3);
              rotMatrix(0,0) = cosPhi;
              rotMatrix(0,1) = -sinPhi;
              rotMatrix(0,2) = 0;
			  
              rotMatrix(1,0) = sinPhi;
              rotMatrix(1,1) = cosPhi;
              rotMatrix(1,2) = 0;
				  
              rotMatrix(2,0) = 0;
              rotMatrix(2,1) = 0;
              rotMatrix(2,2) = 1;
		  	  
              for( int j = 0; j < startHalfCircleSamples.extent(0); ++j) {
                nextHalfCircleSamples(j) =
                    mvMult(rotMatrix, startHalfCircleSamples(j));
              }
		  
              for ( int k = 0; k < sampling-2; ++k)
              {
                blitz::TinyVector<float,3> p1 = nextHalfCircleSamples(k);
                blitz::TinyVector<float,3> p2 = nextHalfCircleSamples(k+1);
                blitz::TinyVector<float,3> p3 = halfCircleSamples(k+1);
                blitz::TinyVector<float,3> v1 = p2 - p1;
                blitz::TinyVector<float,3> v2 = p3 - p1;
                blitz::TinyVector<float,3> n = blitz::cross(v1,v2);
                n /= sqrt(n(0)*n(0) + n(1)*n(1) + n(2)*n(2));
                p1 += center; p2 += center; p3 += center;
                drawTriangle(p1, p2, p3, n); 
              }
		  
              for ( int l = 1; l < sampling-1; ++l)
              {
                blitz::TinyVector<float,3> p1 = halfCircleSamples(l);
                blitz::TinyVector<float,3> p2 = nextHalfCircleSamples(l);
                blitz::TinyVector<float,3> p3 = halfCircleSamples(l+1);
                blitz::TinyVector<float,3> v1 = p2 - p1;
                blitz::TinyVector<float,3> v2 = p3 - p1;
                blitz::TinyVector<float,3> n = blitz::cross(v1,v2);
                n /= sqrt(n(0)*n(0) + n(1)*n(1) + n(2)*n(2));
                p1 += center; p2 += center; p3 += center;
                drawTriangle(p1, p2, p3, n);   
              }

              halfCircleSamples = nextHalfCircleSamples;
		  
            }
	  
          }
  
    void drawStarshapedSurface(
        blitz::TinyVector<double,3> const &center,
        blitz::Array<double,2> const &f)
          { 
            for (ptrdiff_t thetaIdx = 0; thetaIdx < f.extent(0) - 1; ++thetaIdx)
            {
              double theta =
                  static_cast<double>(thetaIdx) /
                  static_cast<double>(f.extent(0) - 1) * M_PI;
              double thetap =
                  static_cast<double>(thetaIdx + 1) /
                  static_cast<double>(f.extent(0) - 1) * M_PI;
              for (ptrdiff_t phiIdx = 0; phiIdx < f.extent(1); ++phiIdx)
              {
                double phi = static_cast<double>(phiIdx) /
                    static_cast<double>(f.extent(1)) * 2.0 * M_PI;
                double phip = (phiIdx == f.extent(1) - 1) ?
                    0.0 : (static_cast<double>(phiIdx + 1) /
                           static_cast<double>(f.extent(1)) * 2.0 * M_PI);

                double r = f(thetaIdx, phiIdx);
                blitz::TinyVector<double,3> v1(
                    r * std::cos(theta),
                    r * std::sin(theta) * std::sin(phi),
                    r * std::sin(theta) * std::cos(phi));
                v1 += center;
              
                r = f(thetaIdx + 1,
                      (phiIdx == f.extent(1) - 1) ? 0 : (phiIdx + 1));
                blitz::TinyVector<double,3> v2(
                    r * std::cos(thetap),
                    r * std::sin(thetap) * std::sin(phip),
                    r * std::sin(thetap) * std::cos(phip));
                v2 += center;
              
                r = f(thetaIdx + 1, phiIdx);
                blitz::TinyVector<double,3> v3(
                    r * std::cos(thetap),
                    r * std::sin(thetap) * std::sin(phi),
                    r * std::sin(thetap) * std::cos(phi));
                v3 += center;
              
                r = f(thetaIdx,
                      (phiIdx == f.extent(1) - 1) ? 0 : (phiIdx + 1));
                blitz::TinyVector<double,3> v4(
                    r * std::cos(theta),
                    r * std::sin(theta) * std::sin(phip),
                    r * std::sin(theta) * std::cos(phip));
                v4 += center;

                blitz::TinyVector<double,3> d21(v2 - v1);
                blitz::TinyVector<double,3> d31(v3 - v1);
                blitz::TinyVector<double,3> d41(v4 - v1);

                blitz::TinyVector<double,3> n(
                    blitz::cross(
                            d21 / std::sqrt(blitz::dot(d21, d21)),
                            d31 / std::sqrt(blitz::dot(d31, d31))));

                drawTriangle(v1, v2, v3, n);

                n = blitz::cross(
                        d41 / std::sqrt(blitz::dot(d41, d41)),
                        d21 / std::sqrt(blitz::dot(d21, d21))));

                drawTriangle(v1, v4, v2, n);
              }
            }
          }

  };

}
