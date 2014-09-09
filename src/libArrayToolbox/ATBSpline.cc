#include "ATBSpline.hh"

namespace atb
{
  
  void fitSplineToSpline(
      BSpline<double> &spline, BSpline<double> const &reference)
  {
#ifdef ATBSPLINE_PROFILE
    BSpline<double>::__profiler[
        "fitSplineToSpline(BSpline<double> &, "
        "BSpline<double> const &)"].start();
#endif
    if (spline.knot(0) != reference.knot(0) ||
        spline.knot(spline.nKnots() - 1) !=
        reference.knot(reference.nKnots() - 1))
    {
      throw RuntimeError(
          "Incompatible spline domains.... splines can only be fitted "
          "against each other, when defined on the same domain.");
    }

    size_t m = reference.nControlPoints();
    // size_t p = reference.degree();
    size_t mp = spline.nControlPoints();
    size_t pp = spline.degree();
    
    blitz::Array<double,2> B(mp, mp);
    B = 0.0;
    for (size_t r = 0; r < mp; ++r)
    {
      for (size_t segment = r; segment <= r + pp; ++segment)
      {
        Polynomial<double> poly =
            (spline.basePolynomial(segment, r) *
             spline.basePolynomial(segment, r)).indefiniteIntegral();
        B(static_cast<BlitzIndexT>(r), static_cast<BlitzIndexT>(r)) +=
            poly(spline.knot(segment + 1)) - poly(spline.knot(segment));
      }
      for (size_t c = r + 1; c < mp && c <= r + pp; ++c)
      {
        Polynomial<double> poly;
        for (size_t segment = r; segment <= r + pp; ++segment)
        {
          poly = (spline.basePolynomial(segment, r) *
                  spline.basePolynomial(segment, c)).indefiniteIntegral();
          B(static_cast<BlitzIndexT>(r), static_cast<BlitzIndexT>(c)) +=
              poly(spline.knot(segment + 1)) - poly(spline.knot(segment));
          B(static_cast<BlitzIndexT>(c), static_cast<BlitzIndexT>(r)) =
              B(static_cast<BlitzIndexT>(r), static_cast<BlitzIndexT>(c));
        }
      }
    }
    B = invert(B);

    // Generate all sub-intervals with corresponding segment indices
    // for piecewise integration
    std::map< std::pair<double, double>, std::pair<size_t,size_t> > intervals;
    double lBound = reference.knot(0);
    size_t j = 1, jp = 1;
    while (j < reference.nKnots() && jp < spline.nKnots())
    {
      if (reference.knot(j) == lBound)
      {
        ++j;
        continue;
      }
      if (spline.knot(jp) == lBound)
      {
        ++jp;
        continue;
      }
      if (reference.knot(j) < spline.knot(jp))
      {
        intervals[std::pair<double,double>(lBound, reference.knot(j))] =
            std::pair<size_t,size_t>(j - 1, jp - 1);
        lBound = reference.knot(j);
        ++j;
      }
      else if (reference.knot(j) > spline.knot(jp))
      {
        intervals[std::pair<double,double>(lBound, spline.knot(jp))] =
            std::pair<size_t,size_t>(j - 1, jp - 1);
        lBound = spline.knot(jp);
        ++jp;
      }
      else
      {
        intervals[std::pair<double,double>(lBound, spline.knot(jp))] =
            std::pair<size_t,size_t>(j - 1, jp - 1);
        lBound = spline.knot(jp);
        ++j;
        ++jp;
      }
    }

    std::vector<double> cp(spline.nControlPoints(), 0.0);

    blitz::Array<double,1> a(mp);
    a = 0.0;
    for (size_t r = 0; r < mp; ++r)
    {
      for (size_t jp = 0; jp < m; ++jp)
      {
        double integral = 0.0;
        std::map< std::pair<double, double>,
            std::pair<size_t,size_t> >::const_iterator it;
        for (it = intervals.begin(); it != intervals.end(); ++it)
        {
          Polynomial<double> poly =
              (reference.basePolynomial(it->second.first, jp) *
               spline.basePolynomial(
                   it->second.second, r)).indefiniteIntegral();
          integral += poly(it->first.second) - poly(it->first.first);
        }
        a(static_cast<BlitzIndexT>(r)) +=
            reference.controlPoint(jp) * integral;
      }
      blitz::Array<double,1> c(mp);
      c = mvMult(B, a);
      for (size_t j = 0; j < spline.nControlPoints(); ++j)
          cp[j] = c(static_cast<BlitzIndexT>(j));
    }
    spline.setControlPoints(cp);
#ifdef ATBSPLINE_PROFILE
    BSpline<double>::__profiler[
        "fitSplineToSpline(BSpline<double> &, "
        "BSpline<double> const &)"].stop();
#endif
  }

  void fitSplineToPointCloud(
      BSpline<double> &spline, std::vector<double> const &u,
      std::vector<double> const &points)
  {
#ifdef ATBSPLINE_PROFILE
    BSpline<double>::__profiler[
        "fitSplineToPointCloud(BSpline<double> &, "
        "std::vector<double> const &, std::vector<double> const &)"].start();
#endif
    // Compute degree-diagonal symmetric Base matrix
    size_t m = spline.nControlPoints();
    size_t n = points.size();
    blitz::Array<double,2> B(m, m);
    B = 0.0;
    for (size_t j = 0; j < m; ++j)
        for (size_t jp = 0; jp < m; ++jp)
            for (size_t i = 0; i < n; ++i)
                B(static_cast<BlitzIndexT>(j), static_cast<BlitzIndexT>(jp)) +=
                    spline.base(u[i], jp) * spline.base(u[i], j);
    
    // Compute righthandside vector and solve the system
    blitz::Array<double,1> a(m);
    a = 0.0;
    for (size_t j = 0; j < m; ++j)
        for (size_t i = 0; i < n; ++i)
            a(static_cast<BlitzIndexT>(j)) += spline.base(u[i], j) * points[i];
    blitz::Array<double,1> x(m);
    x = mvMult(invert(B), a);
    for (size_t j = 0; j < m; ++j)
        spline.setControlPoint(j, x(static_cast<BlitzIndexT>(j)));
#ifdef ATBSPLINE_PROFILE
    BSpline<double>::__profiler[
        "fitSplineToPointCloud(BSpline<double> &, "
        "std::vector<double> const &, std::vector<double> const &)"].stop();
#endif
  }
  
  double distance(BSpline<double> const&spline, double x, double &u)
  {
#ifdef ATBSPLINE_PROFILE
    BSpline<double>::__profiler[
        "distance(BSpline<double> &, double, double)"].start();
#endif
    std::map<double, double> candidates;
    for (size_t segment = spline.degree();
         segment < spline.nControlPoints(); ++segment)
    {
      Polynomial<double> b;
      Polynomial<double> db;
      for (size_t j = segment - spline.degree(); j <= segment; ++j)
      {
        b += spline.controlPoint(j) * spline.basePolynomial(segment, j);
        db += spline.controlPoint(j) * spline.basePolynomial(segment, j, -1);
      }
      b = db * (b - Polynomial<double>(std::vector<double>(1, x)));
      std::vector< std::complex<double> > roots(b.roots());
      for (size_t i = 0; i < roots.size(); ++i)
      {
        if (std::abs(roots[i].imag()) < 1e-10 &&
            roots[i].real() >= spline.knot(segment) &&
            roots[i].real() < spline.knot(segment + 1))
        {
          candidates[std::abs(spline(roots[i].real()) - x)] =
              roots[i].real();
        }
      }
    }

    // Add the spline end points to the candidate set
    candidates[std::abs(spline(0.0) - x)] = 0.0;
    candidates[std::abs(spline(1.0) - x)] = 1.0;    
    
    // Return the candidate with minimum distance
    u = candidates.begin()->second;
#ifdef ATBSPLINE_PROFILE
    BSpline<double>::__profiler[
        "distance(BSpline<double> &, double, double)"].stop();
#endif
    return candidates.begin()->first;
  }
 
}
