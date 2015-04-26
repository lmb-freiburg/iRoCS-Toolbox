/**************************************************************************
**       Title: simplest feature vector class
**    $RCSfile$
**   $Revision: 476 $$Name$
**       $Date: 2004-08-26 10:36:59 +0200 (Thu, 26 Aug 2004) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**  Revision 1.1  2002/05/06 13:47:29  ronneber
**  initial revision
**
**
**
**************************************************************************/

#ifndef MYFEATUREVECTOR_HH
#define MYFEATUREVECTOR_HH

class MyFeatureVector
{
public:
  MyFeatureVector()
        {}
    
  void setXYAndLabel( float x, float y, int label)
        {
          _x     = x;
          _y     = y;
          _label = label;
        }
  
  int getLabel() const 
        {
          return _label;
        }
  
  float x() const { return _x; }
  float y() const { return _y; }


private:

  /*-----------------------------------------------------------------------
   *  inherit copying (to ensure, that libsvmtl doesn't try to make copies)
   *-----------------------------------------------------------------------*/
  MyFeatureVector( const MyFeatureVector& );
  MyFeatureVector& operator=( const MyFeatureVector&);
  
  float _x;
  float _y;
  int   _label;
};


#endif
