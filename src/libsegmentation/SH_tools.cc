/**************************************************************************
 *
 * Copyright (C) 2015 Margret Keuper, Thorsten Falk
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

#include "SH_tools.hh"

#include "SH_forward.hh"
#include "SH_backward.hh"

#include <libArrayToolbox/ATBLinAlg.hh>
#include <libArrayToolbox/Interpolator.hh>

namespace segmentation
{
  
  ptrdiff_t thetaToIndex(double theta, int bw)
  {
    return static_cast<ptrdiff_t>(2.0 * bw * theta / M_PI - 0.5);
  }

  ptrdiff_t phiToIndex(double phi, int bw)
  {
    return static_cast<ptrdiff_t>(phi * bw / M_PI);
  }

  double indexToTheta(ptrdiff_t index, int bw)
  {
    ptrdiff_t absidx = index / (2 * bw);
    if (absidx < 0) absidx = -absidx;
    ptrdiff_t absidxp = (index + 1) / (2 * bw);
    if (absidx < 0) absidxp = -absidxp;
    bool ascending =
        (index >= 0 && absidx % 2 == 0) ||
        (index < 0 && absidxp % 2 == 1);
    ptrdiff_t idx = (ascending) ? atb::modulo(index, 2 * bw) :
        (2 * bw - 1 - atb::modulo(index, 2 * bw));
    return M_PI * (2.0 * static_cast<double>(idx) + 1.0) /
        (4.0 * static_cast<double>(bw));
  }

  double indexToPhi(ptrdiff_t index, int bw)
  {
    return static_cast<double>(atb::modulo(index, 2 * bw)) * M_PI /
        static_cast<double>(bw);
  }

  blitz::TinyVector<double,2> indexToAngles(
      blitz::TinyVector<ptrdiff_t,2> const &index, int bw)
  {
    ptrdiff_t absidx = index(0) / (2 * bw);
    if (absidx < 0) absidx = -absidx;
    double phiOffset = (absidx % 2 == 0) ? 0.0 : M_PI;
    return blitz::TinyVector<double,2>(
        indexToTheta(index(0), bw), indexToPhi(index(1), bw) + phiOffset);
  }

// Compute the linear index in a complex-valued array for the given bandwidth,
// band and order (To get the position in the corresponding interleaved float
// Array, multiply by two)
  ptrdiff_t linearIndex(int bw, int l, int m)
  {
    if (m < 0) return ((bw - 1) * (bw + 2)) / 2 + 1 +
                   (((bw - 1) + m) * (bw + m) / 2) + (l + m);
    else return m * bw - ((m * (m - 1)) / 2) + (l - m);
  }

  void SH_dtheta_up(double const *c, int bw, double *d_theta)
  {
    /*
      transform back with derivaations of spherical harmonics basis functions:
      dY_{lm}/dtheta=w*e^{i*m*phi}*(P_{l,m}(cos(theta))*(-1-l)*cos(theta)+(1-m+l)*P_{l+1,m}(cos(theta)))
      *(-1)/(-1+cos(theta)^2) * sin(theta)
      */
    int bw2=bw*8+1;
    double * c_sweighted= new double[2*bw2*bw2];
    double * c_weighted = new double[2*bw*bw];
    double * d_stheta   = new double[8*bw2*bw2];
    int index=0;
    for(int l=0; l<bw2; l++){
      for(int m=-l; m<0; m++){
        int index2=2*(((bw2-1)*(bw2+2))/2 + 1 + (((bw2-1)+m)*(bw2+m)/2)+(l-abs(m)));//index for l,m
        int sindex=2*(((bw-1)*(bw+2))/2 + 1 + (((bw-1)+m)*(bw+m)/2)+(l-1-abs(m)));//index for l-1,m
        index=2*(((bw-1)*(bw+2))/2 + 1 + (((bw-1)+m)*(bw+m)/2)+(l-abs(m)));//index for l,m
        if((l-1<bw)&&(m<=l-1)){//sindex is valid
          double unweight = atb::factorialFraction(l-m, l+m);
          unweight *= ((2*l+1)/(4*M_PI));
          unweight=std::sqrt(unweight);
          double unweightp=atb::factorialFraction((l-1)-m, (l-1)+m);
          unweightp *= ((2*(l-1)+1)/(4*M_PI));
          unweightp=std::sqrt(unweightp);
          //double unweight=sqrt(((2*(l-1)+1)*(l+m))/((2*l+1)*(l-m)));

          c_sweighted[index2]=c[sindex]*(1-m+(l-1))*unweightp/unweight;//real
          c_sweighted[index2+1]=c[sindex+1]*(1-m+(l-1))*unweightp/unweight;//imaginary
        }
        else{
          c_sweighted[index2]=0;//real
          c_sweighted[index2+1]=0;//imaginary
        }
        if(l<bw){//index is valid
          c_weighted[index]=c[index]*(-1-l);//real
          c_weighted[index+1]=c[index+1]*(-1-l);//imaginary
        }
      }
      for(int m=0; m<=l; m++){ 
        int index2=2*(m*bw2-((m*(m-1))/2)+(l-m));
        index=2*(m*bw-((m*(m-1))/2)+(l-m));//index for l,m
        int sindex=2*(m*bw-((m*(m-1))/2)+(l-1-m));//index for l-1,m
        if((l-1<bw)&&(m<=l-1)){
          double unweight = atb::factorialFraction(l-m, l+m);
          unweight *= ((2*l+1)/(4*M_PI));
          unweight=std::sqrt(unweight);
          double unweightp=atb::factorialFraction((l-1)-m, (l-1)+m);
          unweightp *= ((2*(l-1)+1)/(4*M_PI));
          unweightp=std::sqrt(unweightp);
          //double unweight=sqrt(((2*(l-1)+1)*(l+m))/((2*l+1)*(l-m)));
          c_sweighted[index2]=c[sindex]*(1-m+(l-1))*unweightp/unweight;//real
          c_sweighted[index2+1]=c[sindex+1]*(1-m+(l-1))*unweightp/unweight;//imaginary
        }
        else{
          c_sweighted[index2]=0;//real
          c_sweighted[index2+1]=0;//imaginary
        }
        if(l<bw){
          c_weighted[index]=c[index]*(-1-l);//real
          c_weighted[index+1]=c[index+1]*(-1-l);//imaginary
        }
      }
    }
  
    SH_backward & SH_trans_back_up=SH_backward::instance(bw2);
    SH_backward & SH_trans_back=SH_backward::instance(bw);
    SH_trans_back.sh_semi_memo_back(c_weighted, d_theta);
    SH_trans_back_up.sh_semi_memo_back(c_sweighted, d_stheta);
    delete[] c_weighted;
    delete[] c_sweighted;
    int j=0;
    int j2=0; 
    double theta;//the actual angle theta
    double phi;//the actual angle phi
    double theta2_i;//position of the angle theta in the second expansion (bw2)
    double phi2;//position of the angle phi in the second expansion (bw2)
    double theta2_l=0;
    double phi2_l=0;
    for(int theta_i=0; theta_i<2*bw; theta_i++){
      theta= M_PI*(2*theta_i+1)/(4*bw);//actual angle 
      //position of this angle in the second expansion (bw2)
      theta2_i=2*bw2*theta/M_PI - 1/2;
      theta2_l=floor(theta2_i);
      //weight with (-1)/(-1+cos(theta)^2) * sin(theta) and sum up
      double fac1=1/(std::sin(theta));
      double fac2= (1/std::sin(theta))*std::cos(theta);
      for(int phi_i=0; phi_i<2*bw; phi_i++){
        phi=M_PI*phi_i/bw;//the actual angle phi
        //position of the angle phi in the second expansion (bw2)
        phi2=phi*bw2/M_PI;
        phi2_l=floor(phi2); 
        j2=static_cast<int>(theta2_l)*4*bw2+static_cast<int>(phi2_l)*2;
      
        double d_stheta_j2=d_stheta[j2];
        d_theta[j]=d_theta[j] * fac2 + d_stheta_j2*fac1;//real 
        j++;
        j2++;  
        d_stheta_j2=d_stheta[j2];     
        d_theta[j]=d_theta[j] * fac2 + d_stheta_j2*fac1;//imaginary
        j++;     

      }//for all phi_i
    }//for all theta_i
    delete[] d_stheta;
  }

  void SH_dtheta(FrequencyArray const &c, SpatialArray &d_theta, int degree)
  {
    int bw = static_cast<int>(d_theta.extent(0)) / 2;
    // std::cout << "    SH_dtheta(c[" << c.shape() << "], d_theta["
    //           << d_theta.shape() << "]) ==> bw = " << bw << std::endl;
    assert(d_theta.extent(0) % 2 == 0);
    assert(d_theta.extent(0) == d_theta.extent(1));
    assert(d_theta.size() == 4 * c.size());
    
    std::map< int,std::vector<size_t> > nTerms;
    nTerms[1].push_back(1);
    nTerms[1].push_back(1);
    nTerms[2].push_back(2);
    nTerms[2].push_back(1);
    nTerms[2].push_back(1);
    nTerms[3].push_back(2);
    nTerms[3].push_back(2);
    nTerms[3].push_back(1);
    nTerms[3].push_back(1);
    nTerms[4].push_back(3);
    nTerms[4].push_back(2);
    nTerms[4].push_back(2);
    nTerms[4].push_back(1);
    nTerms[4].push_back(1);

    if (nTerms.find(degree) == nTerms.end())
    {
      std::cerr << "Sorry, d^" << degree << " S / d theta^" << degree
                << " not implemented yet" << std::endl;
      exit(-1);
    }

    // Bandwidth lift
    int bw2 = bw + degree;
    
    std::vector< std::vector<double> > n(degree + 1);
    std::vector< std::vector<double> > ct(degree + 1);
    std::vector< std::vector<SpatialArray*> > d_utheta(degree + 1);
    std::vector< std::vector<FrequencyArray*> > c_weighted(degree + 1);
    
    for (int d = 0; d <= degree; ++d)
    {
      n[d].resize(nTerms[degree][d]);
      ct[d].resize(nTerms[degree][d]);
      d_utheta[d].resize(nTerms[degree][d]);
      c_weighted[d].resize(nTerms[degree][d]);
      for (size_t t = 0; t < nTerms[degree][d]; ++t)
      {
        d_utheta[d][t] = new SpatialArray(2 * bw2, 2 * bw2);
        c_weighted[d][t] = new FrequencyArray(bw2 * bw2);
      }
    }

    for (int l = 0; l < bw2; l++)
    {
      double ld = static_cast<double>(l);
      double ld2 = std::pow(ld, 2.0);

      switch (degree)
      {
      case 1:
      {
        n[0][0] = 1.0 + ld;
        n[1][0] = 1.0;
        break;
      }
      case 2:
      {
        n[0][0] = (1.0 + ld) * (3.0 + ld);
        n[0][1] = (1.0 + ld) * (1.0 + ld);
        n[1][0] = 1.0 + ld;
        n[2][0] = 1.0;
        break;
      }
      case 3:
      {
        n[0][0] = (1.0 + ld) * (11.0 + 8.0 * ld + ld2);
        n[0][1] = std::pow(1.0 + ld, 3.0);
        n[1][0] = 8.0 + 12.0 * ld + 3.0 * ld2;
        n[1][1] = 4.0 + 6.0 * ld + 3.0 * ld2;
        n[2][0] = 1.0 + ld;
        n[3][0] = 1.0;
        break;
      }
      case 4:
      {
        double ld3 = std::pow(ld, 3.0);
        n[0][0] = (1.0 + ld) * (115.0 + 113.0 * ld + 33.0 * ld2 + 3.0 * ld3);
        n[0][1] = (1.0 + ld) * (19.0 + 23.0 * ld + 9.0 * ld2 + ld3);
        n[0][2] = std::pow(1.0 + ld, 4.0);
        n[1][0] = (1.0 + ld) * (10.0 + 8.0 * ld + ld2);
        n[1][1] = (1.0 + ld) * (2.0 + 2.0 * ld + ld2);
        n[2][0] = 7.0 + 12.0 * ld + 3.0 * ld2;
        n[2][1] = 5.0 + 6.0 * ld + 3.0 * ld2;
        n[3][0] = 1.0 + ld;
        n[4][0] = 1.0;
        break;
      }
      default:
        std::cerr << "Never ever that output can happen, that means, that all "
                  << "security mechanisms ensuring that a valid derivative "
                  << "is computed failed..." << std::endl;
        exit(-1);
      }
      
      for (int m = -l; m <= l; m++)
      {
        ptrdiff_t idx2 = linearIndex(bw2, l, m);

        double fac = 1.0;

        double md = static_cast<double>(m);
        double md2 = std::pow(md, 2.0);
        
        for (int d = 0; d <= degree; ++d)
        {
          if (d > 0) fac *= -std::sqrt(std::pow(ld - d + 1.0, 2.0) - md2);
          if ((l - d < bw) && (std::abs(m) <= l - d))
          {
            ptrdiff_t idx = linearIndex(bw, l - d, m);
            for (size_t t = 0; t < nTerms[degree][d]; ++t)
                (*c_weighted[d][t])(idx2) = c(idx) * n[d][t] *
                    std::sqrt((2.0 * (ld - d) + 1.0) / (2.0 * ld + 1.0)) * fac;
          }
        }
      }
    }

    for (int d = 0; d <= degree; ++d)
    {
      for (size_t t = 0; t < nTerms[degree][d]; ++t)
      {
        SH_backward::instance(bw2).sh_semi_memo_back(
            reinterpret_cast<double*>((*c_weighted[d][t]).data()),
            reinterpret_cast<double*>((*d_utheta[d][t]).data()));
      }
    }
    
    for (int lat = 0; lat < 2 * bw2; ++lat)
    {
      double theta = M_PI * (2.0 * lat + 1.0) / (4.0 * bw2);
      
      double factor = ((degree % 2 == 0) ? 1.0 : -1.0) /
          std::pow(std::sin(theta), static_cast<double>(degree));

      switch (degree)
      {
      case 1:
      {
        ct[0][0] = std::cos(theta);
        ct[1][0] = 1.0;
        break;
      }
      case 2:
      {
        ct[0][0] = 0.5;
        ct[0][1] = 0.5 * std::cos(2.0 * theta);
        ct[1][0] = 2.0 * std::cos(theta);
        ct[2][0] = 1.0;
        break;
      }
      case 3:
      {
        ct[0][0] = 0.5 * std::cos(theta);
        ct[0][1] = 0.5 * std::cos(theta) * std::cos(2.0 * theta);
        ct[1][0] = 0.5;
        ct[1][1] = 0.5 * std::cos(2.0 * theta);
        ct[2][0] = 3.0 * std::cos(theta);
        ct[3][0] = 1.0;
        break;
      }
      case 4:
      {
        ct[0][0] = 0.125;
        ct[0][1] = 0.5 * std::cos(2.0 * theta);
        ct[0][2] = 0.125 * std::cos(4.0 * theta);
        ct[1][0] = 2.0 * std::cos(theta);
        ct[1][1] = 2.0 * std::cos(theta) * std::cos(2.0 * theta);
        ct[2][0] = 1.0;
        ct[2][1] = std::cos(2.0 * theta);
        ct[3][0] = 4.0 * std::cos(theta);
        ct[4][0] = 1.0;
        break;
      }
      default:
        std::cerr << "Never ever that output can happen, that means, that all "
                  << "security mechanisms ensuring that a valid derivative "
                  << "is computed failed..." << std::endl;
        exit(-1);
      }

      (*d_utheta[0][0])(lat, blitz::Range::all()) *= ct[0][0];
      for (int d = 0; d <= degree; ++d)
      {
        for (size_t t = 0; t < nTerms[degree][d]; ++t)
        {
          if (d != 0 || t != 0)
          {
            (*d_utheta[0][0])(lat, blitz::Range::all()) +=
                ct[d][t] * (*d_utheta[d][t])(lat, blitz::Range::all());
          }
        }
      }
      (*d_utheta[0][0])(lat, blitz::Range::all()) *= factor;
    }

    atb::LinearInterpolator<std::complex<double>,2> ip;
    for (int lat = 0; lat < 2 * bw; ++lat)
    {
      for(int lon = 0; lon < 2 * bw; ++lon)
      {
        blitz::TinyVector<double,2> pos(
            (lat + 0.5) * static_cast<double>(bw2) /
            static_cast<double>(bw) - 0.5,
            static_cast<double>(lon * bw2) / static_cast<double>(bw));
        d_theta(lat, lon) = ip.get(*d_utheta[0][0], pos);
      }
    }    
    
    for (int d = 0; d <= degree; ++d)
    {
      for (size_t t = 0; t < nTerms[degree][d]; ++t)
      {
        delete d_utheta[d][t];
        delete c_weighted[d][t];
      }
    }
  }

  void SH_dtheta(double const *c, int bw, double *d_theta, int degree)
  {
    SpatialArray dThetaArray(reinterpret_cast<std::complex<double>*>(d_theta), 
                             blitz::TinyVector<ptrdiff_t,2>(2 * bw, 2 * bw),
                             blitz::neverDeleteData);
    // Dieser const_cast is das pure böse!!!
    SH_dtheta(FrequencyArray(
                  const_cast<std::complex<double> *>(
                      reinterpret_cast<std::complex<double> const*>(c)), 
                  blitz::TinyVector<ptrdiff_t,1>(bw * bw),
                  blitz::neverDeleteData), dThetaArray, degree);
  }

  void SH_dpsi(double const *c, int bw, double *d_psi)
  {
    double * c_weighted= new double[2*bw*bw];
    int index=0;
    for(int l=0; l<bw; l++){
      for(int m=-l; m<0; m++){
        index=2*(((bw-1)*(bw+2))/2 + 1 + (((bw-1)+m)*(bw+m)/2)+(l-abs(m)));//index for l,m
        c_weighted[index]=c[index+1]*(-m);//c_{l,m}*i*m
        c_weighted[index+1]=c[index]*(m);
      }
      for(int m=0; m<=l; m++){
        index=2*(m*bw-((m*(m-1))/2)+(l-m));//index for l,m
        c_weighted[index]=c[index+1]*(-m);
        c_weighted[index+1]=c[index]*(m);
      }
    }
    SH_backward & SH_trans_back=SH_backward::instance(bw);
    SH_trans_back.sh_semi_memo_back(c_weighted, d_psi);
    delete[] c_weighted;
  } 

  void SH_normal(double const *cx, double const *cy ,double const *cz,
                 double *nx, double *ny, double *nz, int bw)
  {

    double* d_psix= new double[8*bw*bw];
    double* d_psiy= new double[8*bw*bw];
    double* d_psiz= new double[8*bw*bw];
    double* d_thetax= new double[8*bw*bw];
    double* d_thetay= new double[8*bw*bw];
    double* d_thetaz= new double[8*bw*bw];
    SH_dpsi(cx, bw, d_psix);
    SH_dpsi(cy, bw, d_psiy); 
    SH_dpsi(cz, bw, d_psiz);
    SH_dtheta_up(cx, bw, d_thetax);
    SH_dtheta_up(cy, bw, d_thetay); 
    SH_dtheta_up(cz, bw, d_thetaz);
    // for (int i = 0; i < bw * bw; ++i){
    //  std::cout << "c_" << i << " = (" << cx[2*i] << " + i " << cx[2*i+1] << ", " << cy[2*i] << " + i " << cy[2*i+1] << ", " << cz[2*i] << " + i " << cz[2*i+1] << ") ... " << std::flush;
    //}
    for(int it_indata=0;it_indata<8*bw*bw; it_indata++){
      nx[it_indata]=d_thetay[it_indata]*d_psiz[it_indata] - d_thetaz[it_indata]*d_psiy[it_indata];
      ny[it_indata]=d_thetaz[it_indata]*d_psix[it_indata] - d_thetax[it_indata]*d_psiz[it_indata];
      nz[it_indata]=d_thetax[it_indata]*d_psiy[it_indata] - d_thetay[it_indata]*d_psix[it_indata];



      double norm_n=std::sqrt(nx[it_indata]*nx[it_indata]+
                              ny[it_indata]*ny[it_indata]+
                              nz[it_indata]*nz[it_indata]);
      if(norm_n >0){
        nx[it_indata]=nx[it_indata]/norm_n;
        ny[it_indata]=ny[it_indata]/norm_n;
        nz[it_indata]=nz[it_indata]/norm_n;
      }
    }
    delete[] d_psix;
    delete[] d_psiy;
    delete[] d_psiz;
    delete[] d_thetax;
    delete[] d_thetay;
    delete[] d_thetaz;
  }

  void SH_normal(double const *cx, double *nx, double *ny, double *nz,  int bw)
  {
    double* d_phi= new double[8*bw*bw];
    double* d_theta= new double[8*bw*bw];
    SH_dpsi(cx, bw, d_phi);
    SH_dtheta(cx, bw, d_theta,1);
    int j=0;
    for(int theta_i=0; theta_i<2*bw; theta_i++){
      for(int phi_i=0; phi_i<2*bw; phi_i++){
        //normal on the plane
        nx[j]=-d_theta[j];
        ny[j]=-d_phi[j];
        nz[j]=1;

        //now, transform back to sphere
        double theta= M_PI*(2*(theta_i)+1+nx[j])/(4*bw);
        double phi=M_PI*2*(phi_i+ny[j])/(2*bw);

        nx[j]=nz[j]*sin(theta)*cos(phi);
        ny[j]=nz[j]*sin(theta)*sin(phi);
        nz[j]=nz[j]*cos(theta);
      
        double norm_n=std::sqrt(nx[j]*nx[j]+
                                ny[j]*ny[j]+
                                nz[j]*nz[j]);
        if(norm_n >0){
          nx[j]/=norm_n;
          ny[j]/=norm_n;
          nz[j]/=norm_n;
        }
        j++;
        nx[j]=0;
        ny[j]=0;
        nz[j]=0;
        j++;
      }
    }
    delete[] d_phi;
    delete[] d_theta;
  }

  void SH_curvature(double const *cx, double *curvature, int bw)
  {
    //local mean curvature
    double *nx= new double[8*bw*bw];
    double *ny= new double[8*bw*bw];
    double *nz= new double[8*bw*bw];
    double *nx_theta= new double[8*bw*bw];
    double *ny_theta= new double[8*bw*bw];
    double *nz_theta= new double[8*bw*bw];
    double *nx_phi= new double[8*bw*bw];
    double *ny_phi= new double[8*bw*bw];
    double *nz_phi= new double[8*bw*bw];
    double* d_psi= new double[8*bw*bw];
    double* d_theta= new double[8*bw*bw];;   
    double *cnx = new double[2*bw*bw]; //active surface coordinates
    double *cny = new double[2*bw*bw];
    double *cnz = new double[2*bw*bw];
    SH_forward & SH_trans_for=SH_forward::instance(bw);
    SH_dpsi(cx, bw, d_psi);
    SH_dtheta(cx, bw, d_theta);
    int j=0;
    for(int theta_i=0; theta_i<2*bw; theta_i++){
      for(int phi_i=0; phi_i<2*bw; phi_i++){
        //normal on the plane
        nx[j]=-d_theta[j];
        ny[j]=-d_psi[j];
        nz[j]=1;
      
        double norm_n=std::sqrt(nx[j]*nx[j]+
                                ny[j]*ny[j]+
                                nz[j]*nz[j]);
        if(norm_n >0){
          nx[j]/=norm_n;
          ny[j]/=norm_n;
          nz[j]/=norm_n;
        }
        j++;
        nx[j]=0;
        ny[j]=0;
        nz[j]=0;
        j++;
      }
    }


    SH_trans_for.sh_semi_memo_for(nx, cnx);
    SH_trans_for.sh_semi_memo_for(ny, cny);
    SH_trans_for.sh_semi_memo_for(nz, cnz);

    SH_dpsi(cnx, bw,nx_phi);//d(n)/dphi)
    SH_dpsi(cny, bw,ny_phi); 
    SH_dpsi(cnz, bw,nz_phi);
    SH_dtheta(cnx, bw, nx_theta);//d(n)/dtheta)
    SH_dtheta(cny, bw, ny_theta); 
    SH_dtheta(cnz, bw, nz_theta);


    double maxv=0;
    for(int it_indata=0;it_indata<8*bw*bw; it_indata++){
      double ffe=(1+d_theta[it_indata]*d_theta[it_indata]);//dot product : d_theta*d_theta) 
      double fff=(d_theta[it_indata]*d_psi[it_indata]);//dot product : d_theta*d_psi)
      double ffg=(1+d_psi[it_indata]*d_psi[it_indata]);//dot product : d_psi*d_psi)  
      double ffl=(-1)*(1*nx_theta[it_indata]+//dot product : d_phi*d(n)/dphi)
                       0*ny_theta[it_indata]+
                       d_theta[it_indata]*nz_theta[it_indata]);
      double ffn=(-1)*(0*nx_phi[it_indata]+//dot product : d_phi*d(n)/dphi)
                       1*ny_phi[it_indata]+
                       d_psi[it_indata]*nz_phi[it_indata]);
      double ffm=(1/2)*((0*nx_theta[it_indata]+//dot product : d_phi*d(n)/dphi)
                         1*ny_theta[it_indata]+
                         d_psi[it_indata]*nz_theta[it_indata])+
                        1*nx_phi[it_indata]+//dot product : d_phi*d(n)/dphi)
                        0*ny_phi[it_indata]+
                        d_theta[it_indata]*nz_phi[it_indata]);

      curvature[it_indata] = ffe * ffn + ffg * ffl - 2.0 * fff * ffm;
      double curvature_denom = 2.0 * (ffe * ffg - fff * fff);
      if(curvature_denom > 0)
          curvature[it_indata] = curvature[it_indata] / curvature_denom;

      if(std::abs(curvature[it_indata]) > maxv)
          maxv = std::abs(curvature[it_indata]);

    }


    delete[] nx;
    delete[] ny;
    delete[] nz;
    delete[] nx_theta;
    delete[] ny_theta;
    delete[] nz_theta;
    delete[] nx_phi;
    delete[] ny_phi;
    delete[] nz_phi;
    delete[] cnx;
    delete[] cny;
    delete[] cnz;
    delete[] d_psi;
    delete[] d_theta;
  }

  float SH_volume(double const *cx, int bw)
  {
    //numerische integration fehlt hier!!!!
    double *nx= new double[8*bw*bw];
    double *ny= new double[8*bw*bw];
    double *nz= new double[8*bw*bw];
    double* d_psi= new double[8*bw*bw];
    double* d_theta= new double[8*bw*bw];
    SH_backward & SH_trans_back=SH_backward::instance(bw);
    double* indata= new double[8*bw*bw];
    SH_trans_back.sh_semi_memo_back(cx, indata);
    SH_dpsi(cx, bw, d_psi);
    SH_dtheta(cx, bw, d_theta);
    float volume=0;
    int j=0;

    for(int theta_i=0; theta_i<2*bw; theta_i++){
      for(int phi_i=0; phi_i<2*bw; phi_i++){
        //normal on the plane
        nx[j]=-d_theta[j];
        ny[j]=-d_psi[j];
        nz[j]=1;
        //     std::cout<<d_theta[j]<<" "<<d_phi[j]<<std::endl;

        //now, transform back to sphere
        //float theta= M_PI*(2*(theta_i)+1+nx[j])/(4*bw);
        //float phi=M_PI*2*(phi_i+ny[j])/(2*bw);
        float theta = static_cast<float>(M_PI * (2 * theta_i + 1) / (4 * bw));
        float phi = static_cast<float>(M_PI * 2 * phi_i / (2 * bw));
        //nx[j]=nz[j]*sin(theta)*cos(phi);
        //ny[j]=nz[j]*sin(theta)*sin(phi);
        //nz[j]=nz[j]*cos(theta);
        //std::cout<<nx[j]<<" "<<ny[j]<<" "<<nz[j]<< std::endl;
        //w=gsl_sf_legendre(grad,cos(theta))
        volume += static_cast<float>(
            nx[j] * theta + ny[j] * phi + nz[j] * indata[j]);
        double norm_n=std::sqrt(nx[j]*nx[j]+
                                ny[j]*ny[j]+
                                nz[j]*nz[j]);
        if(norm_n >0){
          nx[j]/=norm_n;
          ny[j]/=norm_n;
          nz[j]/=norm_n;
        }
        j++;
        nx[j]=0;
        ny[j]=0;
        nz[j]=0;
        j++;
 
      }
    }
    volume /= 3.0f;

    delete[] indata;
    delete[] d_theta;
    delete[] d_psi;
    delete[] nx;
    delete[] ny;
    delete[] nz;
    return volume;
  }

  void SH_curvature(double const *cx, double const *cy ,double const *cz,
                    double *curvature, int bw)
  {
    //local mean curvature
    double *nx= new double[8*bw*bw];
    double *ny= new double[8*bw*bw];
    double *nz= new double[8*bw*bw];
    double *nx_theta= new double[8*bw*bw];
    double *ny_theta= new double[8*bw*bw];
    double *nz_theta= new double[8*bw*bw];
    double *nx_phi= new double[8*bw*bw];
    double *ny_phi= new double[8*bw*bw];
    double *nz_phi= new double[8*bw*bw];  
    double* d_psix= new double[8*bw*bw];
    double* d_psiy= new double[8*bw*bw];
    double* d_psiz= new double[8*bw*bw];
    double* d_thetax= new double[8*bw*bw];
    double* d_thetay= new double[8*bw*bw];
    double* d_thetaz= new double[8*bw*bw];   
    double *cnx = new double[2*bw*bw]; //active surface coordinates
    double *cny = new double[2*bw*bw];
    double *cnz = new double[2*bw*bw];
    SH_forward & SH_trans_for=SH_forward::instance(bw);
    SH_dpsi(cx, bw, d_psix);
    SH_dpsi(cy, bw, d_psiy); 
    SH_dpsi(cz, bw, d_psiz);
    SH_dtheta_up(cx, bw, d_thetax);
    SH_dtheta_up(cy, bw, d_thetay); 
    SH_dtheta_up(cz, bw, d_thetaz);
    for(int it_indata=0;it_indata<8*bw*bw; it_indata++){
      nx[it_indata]=d_thetay[it_indata]*d_psiz[it_indata] - d_thetaz[it_indata]*d_psiy[it_indata];
      ny[it_indata]=d_thetaz[it_indata]*d_psix[it_indata] - d_thetax[it_indata]*d_psiz[it_indata];
      nz[it_indata]=d_thetax[it_indata]*d_psiy[it_indata] - d_thetay[it_indata]*d_psix[it_indata];

      double norm_n=std::sqrt(nx[it_indata]*nx[it_indata]+
                              ny[it_indata]*ny[it_indata]+
                              nz[it_indata]*nz[it_indata]);
      if(norm_n >0){
        nx[it_indata]=nx[it_indata]/norm_n;
        ny[it_indata]=ny[it_indata]/norm_n;
        nz[it_indata]=nz[it_indata]/norm_n;
      }
    }
    SH_trans_for.sh_semi_memo_for(nx, cnx);
    SH_trans_for.sh_semi_memo_for(ny, cny);
    SH_trans_for.sh_semi_memo_for(nz, cnz);

    SH_dpsi(cnx, bw,nx_phi);//d(n)/dphi)
    SH_dpsi(cny, bw,ny_phi); 
    SH_dpsi(cnz, bw,nz_phi);
    SH_dtheta_up(cnx, bw, nx_theta);//d(n)/dtheta)
    SH_dtheta_up(cny, bw, ny_theta); 
    SH_dtheta_up(cnz, bw, nz_theta);
    for(int it_indata=0;it_indata<8*bw*bw; it_indata++){
      double ffe=(d_thetax[it_indata]*d_thetax[it_indata]//dot product : d_theta*d_theta)
                  +d_thetay[it_indata]*d_thetay[it_indata]
                  +d_thetaz[it_indata]*d_thetaz[it_indata]); 
      double fff=(d_thetax[it_indata]*d_psix[it_indata]//dot product : d_theta*d_psi)
                  +d_thetay[it_indata]*d_psiy[it_indata]
                  +d_thetaz[it_indata]*d_psiz[it_indata]);
      double ffg=(d_psix[it_indata]*d_psix[it_indata]//dot product : d_psi*d_psi)
                  +d_psiy[it_indata]*d_psiy[it_indata]
                  +d_psiz[it_indata]*d_psiz[it_indata]);   
      double ffl=(-1)*(d_thetax[it_indata]*nx_theta[it_indata]+//dot product : d_phi*d(n)/dphi)
                       d_thetay[it_indata]*ny_theta[it_indata]+
                       d_thetaz[it_indata]*nz_theta[it_indata]);
      double ffn=(-1)*(d_psix[it_indata]*nx_phi[it_indata]+//dot product : d_phi*d(n)/dphi)
                       d_psiy[it_indata]*ny_phi[it_indata]+
                       d_psiz[it_indata]*nz_phi[it_indata]);
      double ffm=(1/2)*((d_psix[it_indata]*nx_theta[it_indata]+//dot product : d_phi*d(n)/dphi)
                         d_psiy[it_indata]*ny_theta[it_indata]+
                         d_psiz[it_indata]*nz_theta[it_indata])+
                        d_thetax[it_indata]*nx_phi[it_indata]+//dot product : d_phi*d(n)/dphi)
                        d_thetay[it_indata]*ny_phi[it_indata]+
                        d_thetaz[it_indata]*nz_phi[it_indata]);

      curvature[it_indata]=(ffe*ffn+ffg*ffl-2*fff*ffm);
      double curvature_denom= (2*(ffe*ffg-fff*fff));
      if( curvature_denom>0){
        curvature[it_indata]=curvature[it_indata]/curvature_denom;
      }
    }

    delete[] nx;
    delete[] ny;
    delete[] nz;
    delete[] nx_theta;
    delete[] ny_theta;
    delete[] nz_theta;
    delete[] nx_phi;
    delete[] ny_phi;
    delete[] nz_phi;
    delete[] cnx;
    delete[] cny;
    delete[] cnz;
    delete[] d_psix;
    delete[] d_psiy;
    delete[] d_psiz;
    delete[] d_thetax;
    delete[] d_thetay;
    delete[] d_thetaz;
  }

  void SH_curvature_gaussian(double const *cx, double const *cy,
                             double const *cz, double *curvature, int bw)
  {
    //gaussian curvature (LN-M^2)/(EG-F^2)
    double *nx= new double[8*bw*bw];
    double *ny= new double[8*bw*bw];
    double *nz= new double[8*bw*bw];
    double *nx_theta= new double[8*bw*bw];
    double *ny_theta= new double[8*bw*bw];
    double *nz_theta= new double[8*bw*bw];
    double *nx_phi= new double[8*bw*bw];
    double *ny_phi= new double[8*bw*bw];
    double *nz_phi= new double[8*bw*bw];  
    double* d_psix= new double[8*bw*bw];
    double* d_psiy= new double[8*bw*bw];
    double* d_psiz= new double[8*bw*bw];
    double* d_thetax= new double[8*bw*bw];
    double* d_thetay= new double[8*bw*bw];
    double* d_thetaz= new double[8*bw*bw];   
    double *cnx = new double[2*bw*bw]; //active surface coordinates
    double *cny = new double[2*bw*bw];
    double *cnz = new double[2*bw*bw];
    SH_forward & SH_trans_for=SH_forward::instance(bw);

    SH_dpsi(cx, bw, d_psix);
    SH_dpsi(cy, bw, d_psiy); 
    SH_dpsi(cz, bw, d_psiz);
    SH_dtheta_up(cx, bw, d_thetax);
    SH_dtheta_up(cy, bw, d_thetay); 
    SH_dtheta_up(cz, bw, d_thetaz);
    for(int it_indata=0;it_indata<8*bw*bw; it_indata++){
      nx[it_indata]=d_thetay[it_indata]*d_psiz[it_indata] - d_thetaz[it_indata]*d_psiy[it_indata];
      ny[it_indata]=d_thetaz[it_indata]*d_psix[it_indata] - d_thetax[it_indata]*d_psiz[it_indata];
      nz[it_indata]=d_thetax[it_indata]*d_psiy[it_indata] - d_thetay[it_indata]*d_psix[it_indata];

      double norm_n=std::sqrt(nx[it_indata]*nx[it_indata]+
                              ny[it_indata]*ny[it_indata]+
                              nz[it_indata]*nz[it_indata]);
      if(norm_n >0){
        nx[it_indata]=nx[it_indata]/norm_n;
        ny[it_indata]=ny[it_indata]/norm_n;
        nz[it_indata]=nz[it_indata]/norm_n;
      }
    }
    SH_trans_for.sh_semi_memo_for(nx, cnx);
    SH_trans_for.sh_semi_memo_for(ny, cny);
    SH_trans_for.sh_semi_memo_for(nz, cnz);

    SH_dpsi(cnx, bw,nx_phi);//d(n)/dphi)
    SH_dpsi(cny, bw,ny_phi); 
    SH_dpsi(cnz, bw,nz_phi);
    SH_dtheta_up(cnx, bw, nx_theta);//d(n)/dtheta)
    SH_dtheta_up(cny, bw, ny_theta); 
    SH_dtheta_up(cnz, bw, nz_theta);
    for(int it_indata=0;it_indata<8*bw*bw; it_indata++){
      double ffe=(d_thetax[it_indata]*d_thetax[it_indata]//dot product : d_theta*d_theta)
                  +d_thetay[it_indata]*d_thetay[it_indata]
                  +d_thetaz[it_indata]*d_thetaz[it_indata]); 
      double fff=(d_thetax[it_indata]*d_psix[it_indata]//dot product : d_theta*d_psi)
                  +d_thetay[it_indata]*d_psiy[it_indata]
                  +d_thetaz[it_indata]*d_psiz[it_indata]);
      double ffg=(d_psix[it_indata]*d_psix[it_indata]//dot product : d_psi*d_psi)
                  +d_psiy[it_indata]*d_psiy[it_indata]
                  +d_psiz[it_indata]*d_psiz[it_indata]);   
      double ffl=(-1)*(d_thetax[it_indata]*nx_theta[it_indata]+//dot product : d_phi*d(n)/dphi)
                       d_thetay[it_indata]*ny_theta[it_indata]+
                       d_thetaz[it_indata]*nz_theta[it_indata]);
      double ffn=(-1)*(d_psix[it_indata]*nx_phi[it_indata]+//dot product : d_phi*d(n)/dphi)
                       d_psiy[it_indata]*ny_phi[it_indata]+
                       d_psiz[it_indata]*nz_phi[it_indata]);
      double ffm=(1/2)*((d_psix[it_indata]*nx_theta[it_indata]+//dot product : d_phi*d(n)/dphi)
                         d_psiy[it_indata]*ny_theta[it_indata]+
                         d_psiz[it_indata]*nz_theta[it_indata])+
                        d_thetax[it_indata]*nx_phi[it_indata]+//dot product : d_phi*d(n)/dphi)
                        d_thetay[it_indata]*ny_phi[it_indata]+
                        d_thetaz[it_indata]*nz_phi[it_indata]);

      curvature[it_indata]=(ffl*ffn-ffm*ffm);
      double curvature_denom= (ffe*ffg-fff*fff);
      if( curvature_denom>0){
        curvature[it_indata]=curvature[it_indata]/curvature_denom;
      }
    }

    delete[] nx;
    delete[] ny;
    delete[] nz;
    delete[] nx_theta;
    delete[] ny_theta;
    delete[] nz_theta;
    delete[] nx_phi;
    delete[] ny_phi;
    delete[] nz_phi;
    delete[] cnx;
    delete[] cny;
    delete[] cnz;
    delete[] d_psix;
    delete[] d_psiy;
    delete[] d_psiz;
    delete[] d_thetax;
    delete[] d_thetay;
    delete[] d_thetaz;
  }

  void SH_curvature_hesse(double const *cx, double const *cy, double const *cz,
                          double *curvature, int bw)
  {
    //normalization with area: factor 1/(EG-F^2)
    //does not work properly
    double* d_psix= new double[8*bw*bw];
    double* d_psiy= new double[8*bw*bw];
    double* d_psiz= new double[8*bw*bw];
    double* d_thetax= new double[8*bw*bw];
    double* d_thetay= new double[8*bw*bw];
    double* d_thetaz= new double[8*bw*bw];
    double *cd_psix = new double[2*bw*bw]; //active surface coordinates
    double *cd_psiy = new double[2*bw*bw];
    double *cd_psiz = new double[2*bw*bw];
    double *cd_thetax = new double[2*bw*bw]; //active surface coordinates
    double *cd_thetay = new double[2*bw*bw];
    double *cd_thetaz = new double[2*bw*bw];
  
    SH_forward & SH_trans_for=SH_forward::instance(bw);
    SH_dpsi(cx, bw, d_psix);
    SH_dpsi(cy, bw, d_psiy); 
    SH_dpsi(cz, bw, d_psiz);
    SH_dtheta_up(cx, bw, d_thetax);
    SH_dtheta_up(cy, bw, d_thetay); 
    SH_dtheta_up(cz, bw, d_thetaz);  
    for(int it_indata=0;it_indata<8*bw*bw; it_indata++){
      double ffe=(d_thetax[it_indata]*d_thetax[it_indata]//dot product : d_theta*d_theta)
                  +d_thetay[it_indata]*d_thetay[it_indata]
                  +d_thetaz[it_indata]*d_thetaz[it_indata]); 
      double fff=(d_thetax[it_indata]*d_psix[it_indata]//dot product : d_theta*d_psi)
                  +d_thetay[it_indata]*d_psiy[it_indata]
                  +d_thetaz[it_indata]*d_psiz[it_indata]);
      double ffg=(d_psix[it_indata]*d_psix[it_indata]//dot product : d_psi*d_psi)
                  +d_psiy[it_indata]*d_psiy[it_indata]
                  +d_psiz[it_indata]*d_psiz[it_indata]);
      double curvature_denom= (ffe*ffg-fff*fff);
      if( curvature_denom>0) curvature[it_indata]=1/(2*curvature_denom);
      else curvature[it_indata]=0;
    }
    SH_trans_for.sh_semi_memo_for(d_psix, cd_psix);
    SH_trans_for.sh_semi_memo_for(d_psiy, cd_psiy);  
    SH_trans_for.sh_semi_memo_for(d_psiz, cd_psiz);
    SH_trans_for.sh_semi_memo_for(d_thetaz, cd_thetaz);
    SH_trans_for.sh_semi_memo_for(d_thetax, cd_thetax);
    SH_trans_for.sh_semi_memo_for(d_thetay, cd_thetay);
    SH_dpsi(cd_psix, bw, d_psix);
    SH_dpsi(cd_psiy, bw, d_psiy); 
    SH_dpsi(cd_psiz, bw, d_psiz);
    SH_dtheta_up(cd_thetax, bw, d_thetax);
    SH_dtheta_up(cd_thetay, bw, d_thetay); 
    SH_dtheta_up(cd_thetaz, bw, d_thetaz);
  
    for(int it_indata=0;it_indata<8*bw*bw; it_indata++){
      curvature[it_indata]*=(d_thetax[it_indata]*d_thetax[it_indata]+
                             d_thetay[it_indata]*d_thetay[it_indata]+
                             d_thetaz[it_indata]*d_thetaz[it_indata]+
                             d_psix[it_indata]*d_psix[it_indata]+     
                             d_psiy[it_indata]*d_psiy[it_indata]+     
                             d_psiz[it_indata]*d_psiz[it_indata]);
    }  
    delete[] d_psix;
    delete[] d_psiy;
    delete[] d_psiz;
    delete[] d_thetax;
    delete[] d_thetay;
    delete[] d_thetaz;
    delete[] cd_psix;
    delete[] cd_psiy;
    delete[] cd_psiz;
    delete[] cd_thetax;
    delete[] cd_thetay;
    delete[] cd_thetaz;
  }

}
