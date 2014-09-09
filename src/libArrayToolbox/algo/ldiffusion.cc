/*
 * ldiffusion.cpp
 *
 *  Created on: Nov 18, 2010
 *      Author: liu
 */

#include "ldiffusion.hh"

int mindex3(int x, int y, int z, int sizx, int sizy)
{
  return z * sizx * sizy + y * sizx + x;
}

double CalculateDiffusionNewGreyValue(
    double *u,double *a,double *b,double *c,double *d,double *e,double *f,
    int x,int y,int z,int nx,int ny,int nz,int px,int py,int pz, double dt,
    int *dimsu)
{
  double A2, A4, A5, A6, A8,B1, B2,B3,B4,B5,B6,B7,B8,B9,C2,C4,C5,C6,C8;
  double u_new;
  double du;
  double di;
  double eps=(double)1e-35;
  int index;

  /* Compute tensor-driven diffusion (as in [1] pp. 80-82) */
  A2 = -f[mindex3(x,y,nz,dimsu[0],dimsu[1])] -
      f[mindex3(x,py,z,dimsu[0],dimsu[1])];
  A4 =  e[mindex3(x,y,nz,dimsu[0],dimsu[1])] +
      e[mindex3(nx,y,z,dimsu[0],dimsu[1])];
  A6 = -e[mindex3(x,y,nz,dimsu[0],dimsu[1])] -
      e[mindex3(px,y,z,dimsu[0],dimsu[1])];
  A8 =  f[mindex3(x,y,nz,dimsu[0],dimsu[1])] +
      f[mindex3(x,ny,z,dimsu[0],dimsu[1])];
  B1 = -d[mindex3(nx,y,z,dimsu[0],dimsu[1])] -
      d[mindex3(x,py,z,dimsu[0],dimsu[1])];
  B3 =  d[mindex3(px,y,z,dimsu[0],dimsu[1])] +
      d[mindex3(x,py,z,dimsu[0],dimsu[1])];
  B7 =  d[mindex3(nx,y,z,dimsu[0],dimsu[1])] +
      d[mindex3(x,ny,z,dimsu[0],dimsu[1])];
  B9 = -d[mindex3(px,y,z,dimsu[0],dimsu[1])] -
      d[mindex3(x,ny,z,dimsu[0],dimsu[1])];
  C2 =  f[mindex3(x,y,pz,dimsu[0],dimsu[1])] +
      f[mindex3(x,py,z,dimsu[0],dimsu[1])];
  C4 = -e[mindex3(x,y,pz,dimsu[0],dimsu[1])] -
      e[mindex3(nx,y,z,dimsu[0],dimsu[1])];
  C6 =  e[mindex3(x,y,pz,dimsu[0],dimsu[1])] +
      e[mindex3(px,y,z,dimsu[0],dimsu[1])];
  C8 = -f[mindex3(x,y,pz,dimsu[0],dimsu[1])] -
      f[mindex3(x,ny,z,dimsu[0],dimsu[1])];

  A5 = c[mindex3(x,y,nz,dimsu[0],dimsu[1])] +
      c[mindex3(x,y,z,dimsu[0],dimsu[1])];
  B2 = b[mindex3(x,py,z,dimsu[0],dimsu[1])] +
      b[mindex3(x,y,z,dimsu[0],dimsu[1])];
  B4 = a[mindex3(nx,y,z,dimsu[0],dimsu[1])] +
      a[mindex3(x,y,z,dimsu[0],dimsu[1])];
  B5 = -(a[mindex3(nx,y,z,dimsu[0],dimsu[1])] +
         2 * a[mindex3(x,y,z,dimsu[0],dimsu[1])] +
         a[mindex3(px,y,z,dimsu[0],dimsu[1])]);
  B5 += -(b[mindex3(x,ny,z,dimsu[0],dimsu[1])] +
          2 * b[mindex3(x,y,z,dimsu[0],dimsu[1])] +
          b[mindex3(x,py,z,dimsu[0],dimsu[1])]);
  B5 += -(c[mindex3(x,y,nz,dimsu[0],dimsu[1])] +
          2 * c[mindex3(x,y,z,dimsu[0],dimsu[1])] +
          c[mindex3(x,y,pz,dimsu[0],dimsu[1])]);
  B6 = a[mindex3(px,y,z,dimsu[0],dimsu[1])] +
      a[mindex3(x,y,z,dimsu[0],dimsu[1])];
  B8 = b[mindex3(x,ny,z,dimsu[0],dimsu[1])] +
      b[mindex3(x,y,z,dimsu[0],dimsu[1])];
  C5 = c[mindex3(x,y,pz,dimsu[0],dimsu[1])] +
      c[mindex3(x,y,z,dimsu[0],dimsu[1])];

  du=0;
  du += A2 * u[mindex3(x,py,nz,dimsu[0],dimsu[1])];
  du += A4 * u[mindex3(nx,y,nz,dimsu[0],dimsu[1])];
  du += A6 * u[mindex3(px,y,nz,dimsu[0],dimsu[1])];
  du += A8 * u[mindex3(x,ny,nz,dimsu[0],dimsu[1])];

  du += B1 * u[mindex3(nx,py,z,dimsu[0],dimsu[1])];
  du += B3 * u[mindex3(px,py,z,dimsu[0],dimsu[1])];
  du += B7 * u[mindex3(nx,ny,z,dimsu[0],dimsu[1])];
  du += B9 * u[mindex3(px,ny,z,dimsu[0],dimsu[1])];

  du += C2 * u[mindex3(x,py,pz,dimsu[0],dimsu[1])];
  du += C4 * u[mindex3(nx,y,pz,dimsu[0],dimsu[1])];
  du += C6 * u[mindex3(px,y,pz,dimsu[0],dimsu[1])];
  du += C8 * u[mindex3(x,ny,pz,dimsu[0],dimsu[1])];

  du *= 0.5;

  du += A5 * u[mindex3(x,y,nz,dimsu[0],dimsu[1])];
  du += B2 * u[mindex3(x,py,z,dimsu[0],dimsu[1])];
  du += B4 * u[mindex3(nx,y,z,dimsu[0],dimsu[1])];
  du += B6 * u[mindex3(px,y,z,dimsu[0],dimsu[1])];
  du += B8 * u[mindex3(x,ny,z,dimsu[0],dimsu[1])];
  du += C5 * u[mindex3(x,y,pz,dimsu[0],dimsu[1])];

  du *=0.5f*dt;

  /* Perform the edge preserving diffusion filtering on the image */
  index = mindex3(x,y,z,dimsu[0],dimsu[1]);
  di=(1-0.5f*dt*B5); if((di<eps)&&(di>-eps)) { di=eps; }
  u_new = (u[index] + du )/di;
//	u_new = (du )/di;
  return u_new;
}

