#include "geodesic.hh"

#include <libArrayToolbox/ATBLinAlg.hh>

namespace segmentation
{

  void generateMask(
      FrequencyArray const &coeffs, blitz::Array<bool,3> &mask,
      blitz::TinyVector<double,3> const &elementSizeUm,
      blitz::TinyVector<double,3> const &centerUm)
  {
    std::cout << "Generating mask... " << std::flush;
    // Transform the coefficients into spatial domain
    int bw = static_cast<int>(std::sqrt(static_cast<double>(coeffs.size())));
    SpatialArray surface(2 * bw, 2 * bw);
    SH_backward::instance(bw).sh_semi_memo_back(
        reinterpret_cast<double const*>(coeffs.data()),
        reinterpret_cast<double*>(surface.data()));
    
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (size_t i = 0; i < static_cast<size_t>(mask.size()); ++i)
    {
      blitz::TinyVector<double,3> posPx;
      size_t tmp = i;
      for (int d = 2; d >= 0; --d)
      {
        posPx(d) = static_cast<double>(tmp % mask.extent(d));
        tmp /= mask.extent(d);
      }
      blitz::TinyVector<double,3> posUm(posPx * elementSizeUm - centerUm);
      blitz::TinyVector<double,3> posSp(atb::euclideanToSpherical(posUm));
      mask.data()[i] =
          (surface(thetaToIndex(posSp(0), bw)), phiToIndex(posSp(1), bw) <
           posSp(2));
    }
    std::cout << "done" << std::endl;
  }


/**
 * calculates radial component of given vector field
 *
 * */
  blitz::Array<float, 3> radial_projection(
      blitz::Array<blitz::TinyVector<float, 3>,3> const &gf,
      blitz::TinyVector<float, 3> const &best_center,
      float scaling)
  {
    blitz::TinyVector<ptrdiff_t,3> Shape = gf.shape();
    blitz::Array<float,3> result(Shape);

    /*********radial gradients*************/
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (int lev = 0; lev < Shape(0); ++lev) {
      for (int row = 0; row < Shape(1); ++row) {
        for (int col = 0; col < Shape(2); ++col) {
          blitz::TinyVector<float,3> pt(
              (best_center(0) - static_cast<float>(lev)) * scaling,
              best_center(1) - static_cast<float>(row),
              best_center(2) - static_cast<float>(col));
          double npt = std::sqrt(blitz::dot(pt, pt));

          if (npt > 0.000001)
              result(lev, row, col) =
                  static_cast<float>(blitz::dot(pt, gf(lev, row, col)) / npt);
          else result(lev, row, col) = 0.0f;
        }
      }
    }
    return result;
  }

// all values in um
  void geodesic_sh(
      blitz::Array<blitz::TinyVector< float, 3>, 3> const &gvf,
      blitz::TinyVector<float, 3> const &elementSizeUm,
      FrequencyArray &coeffs,
      blitz::TinyVector<float, 3> positionUm, float radiusUm,
      int bw, double roundness, int num_iter,
      iRoCS::ProgressReporter *progress)
  {
    double tau = 0.1; // Timestep for update

    // convert to voxel
    double radiusPx = radiusUm / blitz::min(elementSizeUm);
    blitz::TinyVector<double,3> centerPx = positionUm / elementSizeUm;

    blitz::TinyVector<ptrdiff_t,3> Shape(gvf.shape());
    blitz::Array<blitz::TinyVector<float, 3>, 3> rgvf(Shape);

    std::memcpy(rgvf.data(), gvf.data(), gvf.size() * sizeof(float) * 3);

    // simple sanity fix
    if (radiusPx < 0.0f || radiusPx >
        static_cast<float>(blitz::max(Shape)) * 0.5f) {
      radiusPx = static_cast<float>(blitz::max(Shape)) * 0.5f;
      std::cout << "WARNING: invalid radius, using " << radiusPx
                << " pixels" << std::endl;
    }

    std::cout << "Center: " << centerPx << " Radius: " << radiusPx
              << std::endl;
    std::cout << "GVF extent: " << rgvf.shape() << "" << std::endl;
    std::cout << "element size = " << elementSizeUm << std::endl;

    // adapt vector length to voxel size
    blitz::TinyVector<float,3> anisotropyFactor(
        elementSizeUm(2) / elementSizeUm);

#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (size_t i = 0; i < static_cast<size_t>(rgvf.size()); ++i)
        rgvf.data()[i] *= anisotropyFactor;

    blitz::Array<float, 3> radial_comp(Shape);
    radial_comp = radial_projection(
        rgvf, centerPx, elementSizeUm(0) / elementSizeUm(2));

    double *indata = new double[8*bw*bw];
    double *indata_old = new double[8*bw*bw];
    double *outdata = new double[2*bw*bw]; // active surface coordinates
    double *curvature= new double[8*bw*bw];

#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (int i = 0; i < 4*bw*bw; i++)
    {
      indata[2 * i] = static_cast<double>(radiusPx);
      indata[2 * i + 1] = 0.0;
    }

    double *spatialforce = new double[8*bw*bw];
    double *freqforce = new double[2*bw*bw]; // radial forces
    double *curvature_freq= new double[2*bw*bw];
    SH_forward &SH_trans_for = SH_forward::instance(bw);
    SH_backward &SH_trans_back = SH_backward::instance(bw);
    double sf_energy = 0.0;
    double sf_energy_old = 0.0;
    double val_energy = 0.0;
    double val_energy_old = 0.0;
    int conv = 0;

    //evolve snake
    atb::LinearInterpolator<float,3> ip;
    for (int it = 1; it <= num_iter; ++it)
    {
      if (progress != NULL)
      {
        if (!progress->updateProgress(
                static_cast<int>(
                    static_cast<double>(it) / static_cast<double>(num_iter) *
                    (progress->taskProgressMax() -
                     progress->taskProgressMin()) +
                    progress->taskProgressMin())))
        {
          delete[] indata;
          delete[] indata_old;
          delete[] outdata;
          delete[] curvature;
          delete[] spatialforce;
          delete[] freqforce;
          delete[] curvature_freq;
          return;
        }
      }

      sf_energy_old = sf_energy;
      sf_energy = 0.0;
      val_energy_old = val_energy;
      val_energy = 0.0;
      int j = 0;
      blitz::TinyVector<double, 3> pos;
      blitz::TinyVector<ptrdiff_t, 3> pos_low;
      for (int theta = 0; theta < 2 * bw; theta++)
      {
        for (int phi = 0; phi < 2 * bw; phi++)
        {
          double theta_i = M_PI * (2 * theta + 1) / (4 * bw);
          double phi_i = M_PI * phi / (bw);
          pos(0) = centerPx(0) + indata[j] * elementSizeUm(2) /
              elementSizeUm(0) * sin(theta_i) * cos(phi_i);
          pos(1) = centerPx(1) + indata[j] * sin(theta_i) * sin(phi_i);
          pos(2) = centerPx(2) + indata[j] * cos(theta_i);

          // if (pos(0) < 0.0) pos(0) = 0.0;
          // if (pos(1) < 0.0) pos(1) = 0.0;
          // if (pos(2) < 0.0) pos(2) = 0.0;
          // if (pos(0) > static_cast<double>(Shape(0) - 1))
          //     pos(0) = static_cast<double>(Shape(0) - 1);
          // if (pos(1) > static_cast<double>(Shape(1) - 1))
          //     pos(1) = static_cast<double>(Shape(1) - 1);
          // if (pos(2) > static_cast<double>(Shape(2) - 1))
          //     pos(2) = static_cast<double>(Shape(2) - 1);

          double val = ip.get(radial_comp, pos);
          // double val = ATB::interpolate(radial_comp, pos, ATB::NOWRAPAROUND);

          sf_energy += (indata_old[j] - indata[j]) *
              (indata_old[j] - indata[j]);
          val_energy += val * val;

          spatialforce[j++] = val;
          spatialforce[j++] = 0;
        }
      }

      for (int it_indata = 0; it_indata < 8*bw*bw; it_indata++)
          indata_old[it_indata] = indata[it_indata];

      SH_trans_for.sh_semi_memo_for(indata, outdata);

      SH_curvature(outdata, curvature, bw);

      double maxCurvature = 0.0;
      for (int i = 0; i < 4 * bw * bw; ++i)
          if (std::abs(curvature[2 * i]) > maxCurvature)
              maxCurvature = std::abs(curvature[2 * i]);

      if (maxCurvature > 1.0)
      {
#ifdef _OPENMP
#pragma omp parallel for
#endif
          for (int i = 0; i < 4 * bw * bw; ++i)
              curvature[2 * i] /= maxCurvature;
      }

      SH_trans_for.sh_semi_memo_for(spatialforce, freqforce);

      SH_trans_for.sh_semi_memo_for(curvature, curvature_freq);

      /**************gaussian filtering in frequency************/
      int index = 0;
      for (int i = 0; i < bw; i++)
      {
        for (int k = 0; k < bw - i; k++)
        {
          outdata[index+k] = outdata[index+k] - freqforce[index+k]
              + roundness * tau * curvature_freq[index+k];	      
        }
        index = index + (bw - i);
      }
      for (int i = 0; i < bw; i++)
      {
        for (int k = 0; k < i + 1; k++)
        {
          outdata[index+k] = outdata[index+k] - freqforce[index+k]
              + roundness * tau * curvature_freq[index+k];	      
        }
        index = index + i + 1;
      }
      
      SH_trans_back.sh_semi_memo_back(outdata, indata);
      
      if ((sf_energy - sf_energy_old)*(sf_energy - sf_energy_old) < 1e-10)
      {
        if ((val_energy_old - val_energy)*(val_energy_old - val_energy) < 1e-10)
            conv += 1;
      }
      else conv = 0;

      if (conv >= 200) break;
    }

    coeffs.resize(bw * bw);
    int j = 0;
    for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(bw * bw); ++i)
    {
      coeffs(i) = std::complex<double>(outdata[j], outdata[j + 1]);
      j += 2;
    }

    delete[] indata;
    delete[] indata_old;
    delete[] outdata;
    delete[] curvature;
    delete[] spatialforce;
    delete[] freqforce;
    delete[] curvature_freq;
  }

// all values in um
  void geodesic_sh(
      blitz::Array<blitz::TinyVector< float, 3>, 3> const &gvf,
      blitz::TinyVector<float, 3> const &el_size_um,
      blitz::TinyVector<float, 3> best_center,
      float best_radius, int bandwidth, double roundness,
      int num_iter, blitz::Array<bool, 3> &mask,
      blitz::Array<blitz::TinyVector< float, 3  >, 3> &normals,
      iRoCS::ProgressReporter *progress)
  {
    int bw = bandwidth;//no oversampling
    // double sigma = 3;
    double tau = 0.1; // Timestep for update

    // convert to voxel
    best_radius /= blitz::min(el_size_um);
    best_center /= el_size_um;

   // gauss for each band
   // double *gauss_weight = new double[2*bw*bw];
   // double *weight_n = new double[2*bw];

   // for (int i = 0; i < 2*bw; i++) {
   //   weight_n[i] = exp(-(1 / 2) * blitz::pow2(i) / sigma);
   // }

   // int index_weights;
   // int index = 0;
   // for (int i = 0; i < bw; i++) {
   //   index_weights = i;
   //   for (int k = 0;k < bw - i; k++) {
   //     gauss_weight[index+k] = weight_n[index_weights];
   //     index_weights++;
   //   }
   //   index = index + (bw - i);
   // }

   // for (int i = 0; i < bw; i++) {
   //   index_weights = bw - i - 1;
   //   for (int k = 0;k < i + 1; k++) {
   //     gauss_weight[index+k] = weight_n[index_weights];
   //     index_weights++;
   //   }
   //   index_weights = bw - i - 1;
   //   index = index + i + 1;
   // }

    blitz::Array<blitz::TinyVector<float, 3>, 3> rgvf(gvf.shape());
    rgvf = gvf;
    blitz::TinyVector<int, 3> Shape;
    Shape = gvf.shape();
    //inFile.readAttribute(best_center, "best_center",invar.value());
    //inFile.readAttribute(best_radius, "best_radius",invar.value());
    // simple sanity fix
    if (best_radius < 0.0f || best_radius >
        static_cast<float>(blitz::max(Shape)) * 0.5f) {
      best_radius = static_cast<float>(blitz::max(Shape)) * 0.5f;
      std::cout << "WARNING: invalid best_radius, using " << best_radius
                << std::endl;
    }

    std::cout << "Center: " << best_center << " Radius: " << best_radius
              << std::endl;
    std::cout << "GVF extent: " << rgvf.shape() << "" << std::endl;
    std::cout << "element size = " << el_size_um << std::endl;

    //adapt vector length to voxel size
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(rgvf.size()); ++i)
        rgvf.data()[i] *= el_size_um(2) / el_size_um;

    blitz::Array<float, 3> radial_comp(Shape);
    radial_comp = radial_projection(
        rgvf, best_center, el_size_um(0) / el_size_um(2));

    // manu: removed radius rescaling
    //best_radius=1.1*best_radius/(1.5*el_size_um(2));

    double *indata = new double[8*bw*bw];
    double *indata_old = new double[8*bw*bw];
    double *outdata = new double[2*bw*bw]; //active surface coordinates
    double *curvature= new double[8*bw*bw];

    int j = 0;
    for (int i = 0; i < 4*bw*bw; i++) {
      // manu: removed radius rescaling
      indata[j] = static_cast<double>(best_radius);//*1.5;
      j++;
      indata[j] = 0;
      j++;
    }

    double *spatialforce = new double[8*bw*bw];
    double *freqforce = new double[2*bw*bw]; //radial forces
    double *curvature_freq= new double[2*bw*bw];
    SH_forward & SH_trans_for = SH_forward::instance(bw);
    SH_backward & SH_trans_back = SH_backward::instance(bw);
    double sf_energy = 0;
    double sf_energy_old = 0;
    double val_energy = 0;
    double val_energy_old = 0;
    int conv = 0;
    //evolve snake
    atb::LinearInterpolator<float,3> ip;
    for (int it = 1; it <= num_iter; ++it)
    {
      if (progress != NULL)
      {
        if (!progress->updateProgress(
                static_cast<int>(
                    static_cast<double>(it) / static_cast<double>(num_iter) *
                    (progress->taskProgressMax() -
                     progress->taskProgressMin()) +
                    progress->taskProgressMin())))
        {
          delete[] indata;
          delete[] indata_old;
          delete[] outdata;
          delete[] curvature;
          delete[] spatialforce;
          delete[] freqforce;
          delete[] curvature_freq;
          return;
        }
      }

      //std::cout << it + 1 << " " << std::flush;
      //if ((it + 1) % 20 == 0) std::cout << "\n";
      sf_energy_old = sf_energy;
      sf_energy = 0;
      val_energy_old = val_energy;
      val_energy = 0;
      j = 0;
      blitz::TinyVector<double, 3> pos;
      blitz::TinyVector<int, 3> pos_low;
      for (int theta = 0; theta < 2*bw; theta++) {
        for (int phi = 0; phi < 2*bw; phi++) {
          double theta_i = M_PI * (2 * theta + 1) / (4 * bw);
          double phi_i = M_PI * phi / (bw);
          pos(0) = best_center(0) + indata[j] * el_size_um(2) / el_size_um(0) * sin(theta_i) * cos(phi_i);
          pos(1) = best_center(1) + indata[j] * sin(theta_i) * sin(phi_i);
          pos(2) = best_center(2) + indata[j] * cos(theta_i);
          //pos *= el_size_um/el_size_um(2);
          if (pos(0) < 0) pos(0) = 0;
          if (pos(1) < 0) pos(1) = 0;
          if (pos(2) < 0) pos(2) = 0;
          if (pos(0) > Shape(0) - 1) pos(0) = Shape(0) - 1;
          if (pos(1) > Shape(1) - 1) pos(1) = Shape(1) - 1;
          if (pos(2) > Shape(2) - 1) pos(2) = Shape(2) - 1;
          double val = ip.get(radial_comp, pos);
          // double val = ATB::interpolate(radial_comp, pos, ATB::NOWRAPAROUND);
          //sf_pos+=(indata_old[it_indata]-indata[ it_indata])*(indata_old[it_indata]-indata[ it_indata]);
          sf_energy += (indata_old[j] - indata[j]) * (indata_old[j] - indata[ j]);
          val_energy += val * val;
          spatialforce[j] = val;
          j++;
          spatialforce[j] = 0;
          j++;
        }
      }
      for (int it_indata = 0;it_indata < 8*bw*bw; it_indata++) {
        indata_old[it_indata] = indata[ it_indata];
      }

      SH_trans_for.sh_semi_memo_for(indata, outdata);

      SH_curvature(outdata, curvature, bw);
      double maxCurvature = 0.0;
      for (ptrdiff_t i = 0; i < 4 * bw * bw; ++i)
      {
        if (std::abs(curvature[2 * i]) > maxCurvature)
            maxCurvature = std::abs(curvature[2 * i]);
      }
      if (maxCurvature > 1.0)
      {
#ifdef _OPENMP
#pragma omp parallel for
#endif
          for (ptrdiff_t i = 0; i < 4 * bw * bw; ++i)
              curvature[2 * i] /= maxCurvature;
      }

      SH_trans_for.sh_semi_memo_for(spatialforce, freqforce);

      SH_trans_for.sh_semi_memo_for(curvature, curvature_freq);

      /**************gaussian filtering in frequency************/
      int index = 0;
      for (int i = 0; i < bw; i++) {
        for (int k = 0;k < bw - i; k++) {
          outdata[index+k] = outdata[index+k] - freqforce[index+k]
              + roundness * tau * curvature_freq[index+k];	      
        }
        index = index + (bw - i);
      }
      for (int i = 0; i < bw; i++) {
        for (int k = 0;k < i + 1; k++) {
          outdata[index+k] = outdata[index+k] - freqforce[index+k]
              + roundness * tau * curvature_freq[index+k];	      
        }
        index = index + i + 1;
      }

      SH_trans_back.sh_semi_memo_back(outdata, indata);

      if ((sf_energy - sf_energy_old)*(sf_energy - sf_energy_old) < 1e-10) {
        if ((val_energy_old - val_energy)*(val_energy_old - val_energy) < 1e-10)
            conv += 1;
      } else
          conv = 0;
      if (conv >= 200)
          break;

      //std::cout << it << std::endl;
      //std::cout <<" "<< conv<<" " << std::flush;
    }

    double *nx = new double[8*bw*bw];
    double *ny = new double[8*bw*bw];
    double *nz = new double[8*bw*bw];
    SH_normal(outdata, nx, ny, nz, bw);
    normals.resize(Shape);
    normals[0] = 0;
    normals[1] = 0;
    normals[2] = 0;
    j = 0;
    blitz::TinyVector<double, 3> pos;
    blitz::TinyVector<int, 3> pos_round;
    for (int theta = 0; theta < 2*bw; theta++) {
      for (int phi = 0; phi < 2*bw; phi++) {
        double theta_i = M_PI * (2 * theta + 1) / (4 * bw);
        double phi_i = M_PI * phi / (bw);
        pos(0) = best_center(0) +
            indata[j] * el_size_um(2) / el_size_um(0) * sin(theta_i) * cos(phi_i);
        pos(1) = best_center(1) + indata[j] * sin(theta_i) * sin(phi_i);
        pos(2) = best_center(2) + indata[j] * cos(theta_i);
        pos_round(0) = static_cast<int>(std::floor(pos(0) + 0.5));
        pos_round(1) = static_cast<int>(std::floor(pos(1) + 0.5));
        pos_round(2) = static_cast<int>(std::floor(pos(2) + 0.5));
        if ((pos_round(0) > 0) && (pos_round(1) > 0) && (pos_round(2) > 0) &&
            (pos_round(0) <= Shape(0) - 1) && (pos_round(1) <= Shape(1) - 1) &&
            (pos_round(2) <= Shape(2) - 1)) {
          normals[0](pos_round) = static_cast<float>(nx[j] * el_size_um(2) / el_size_um(0));
          normals[1](pos_round) = static_cast<float>(ny[j]);
          normals[2](pos_round) = static_cast<float>(nz[j]);
        }
        j++;
        j++;
      }
    }
    delete[] nx;
    delete[] ny;
    delete[] nz;
    /*********************************************
     **************drawing the boundary************
     **********************************************/

    mask.resize(Shape);
    mask = false;
    j = 0;
    blitz::TinyVector<int, 3> pos_low;

    for (int d = 0; d < 3; d++) {
      for (int it_i = 0;it_i < Shape(0); it_i += (d == 0) ? Shape(0) - 1 : 1) {//only first and last slice
        for (int it_j = 0; it_j < Shape(1); it_j += (d == 1) ? Shape(1) - 1 : 1) {
          for (int it_k = 0; it_k < Shape(2); it_k += (d == 2) ? Shape(2) - 1 : 1) {
            blitz::TinyVector<float, 3> center_vec;
            center_vec(0) = -(el_size_um(0) / el_size_um(2)) *
                (best_center(0) - static_cast<float>(it_i));
            center_vec(1) = -(best_center(1) - static_cast<float>(it_j));
            center_vec(2) = -(best_center(2) - static_cast<float>(it_k));
            double theta_mask = std::acos(
                center_vec(2) / std::sqrt(blitz::dot(center_vec, center_vec)));
            double phi_mask = 1;
            if ((std::abs(center_vec(0)) > 10e-15) ||
                (std::abs(center_vec(1)) > 10e-15)) {
              if (center_vec(1) < 0) 
                  phi_mask = 2 * M_PI - std::acos(
                      center_vec(0) / std::sqrt(blitz::pow2(center_vec(0)) +
                                                blitz::pow2(center_vec(1))));
              else
                  phi_mask = std::acos(
                      center_vec(0) / std::sqrt(blitz::pow2(center_vec(0)) +
                                                blitz::pow2(center_vec(1))));

              double theta = 4 * bw * theta_mask / (M_PI * 2) - 1 / 2;
			  int theta_low = static_cast<int>(std::floor(theta));
              double phi = bw * phi_mask / (M_PI);
              float radius_theta_phi;
			  int phi_low = static_cast<int>(std::floor(phi));
              if (phi_low < 0) phi_low = 0;
              if (theta_low < 0) theta_low = 0;
              int index1 = 2 * (theta_low * 2 * bw + phi_low);
              int index2 = 0;
              int index3 = 0;
              int index4 = 0;
              if (theta_low < 2*bw - 1 && phi_low < 2*bw - 1) {
                index2 = 2 * ((theta_low + 1) * 2 * bw + phi_low);
                index3 = 2 * (theta_low * 2 * bw + (phi_low + 1));
                index4 = 2 * ((theta_low + 1) * 2 * bw + (phi_low + 1));
              } else {
                if (theta_low < 2*bw - 1) {//phi_low==2*bw-1
                  index2 = 2 * ((theta_low + 1) * 2 * bw + phi_low);
                  index3 = 2 * (theta_low * 2 * bw);
                  index4 = 2 * ((theta_low + 1) * 2 * bw);
                }
                if (phi_low < 2*bw - 1) {//theta_low==2*bw-1
                  index2 = 2 * (theta_low * 2 * bw + (bw + phi_low) % (2 * bw));
                  index3 = 2 * (theta_low * 2 * bw + (phi_low + 1));
                  index4 = 2 * (theta_low * 2 * bw + (bw + phi_low + 1) % (2 * bw));
                }
                if ((theta_low == 2*bw - 1) && (phi_low == 2*bw - 1)) {
                  index2 = 2 * (theta_low * 2 * bw + (bw + phi_low) % (2 * bw)) ;
                  index3 = 2 * (theta_low * 2 * bw);
                  index4 = 2 * (theta_low * 2 * bw + bw);
                }
              }

              radius_theta_phi = static_cast<float>(
                  (1.f - theta + theta_low) * (1.f - phi + phi_low) * indata[index1] +
                  (theta - theta_low) * (1.f - phi + phi_low) * indata[index2] +
                  (1.f - theta + theta_low) * (phi - phi_low) * indata[index3] +
                  (theta - theta_low) * (phi - phi_low) * indata[index4]);

              double centerVecNorm =
                  std::sqrt(blitz::dot(center_vec, center_vec));
              if (centerVecNorm > 0)
              {
                center_vec = center_vec / centerVecNorm;
                
                pos = best_center + radius_theta_phi /
                    (el_size_um / el_size_um(2)) * center_vec;

                for (int dim3d = 0; dim3d < 3; ++dim3d) {
                  pos_round(dim3d) =
                      static_cast<int>(std::floor(pos(dim3d) + 0.5));
                  if (pos_round(dim3d) < 0) pos_round(dim3d) = 0;
                  if (pos_round(dim3d) >= Shape(dim3d))
                      pos_round(dim3d) = Shape(dim3d) - 1;
                }
                mask(pos_round) = true;
              }
            }
          }
        }
      }
    }
    delete[] indata;
    delete[] indata_old;
    delete[] outdata;
    delete[] spatialforce;
    delete[] freqforce;
  }

}


