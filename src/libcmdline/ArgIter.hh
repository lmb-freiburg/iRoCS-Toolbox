/**************************************************************************
**       Title: basic agument iterator
**    $RCSfile$
**   $Revision: 78 $$Name$
**       $Date: 2002-05-14 10:14:15 +0200 (Tue, 14 May 2002) $
**   Copyright: GPL $Author: pigorsch $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.3  2002/05/14 08:13:28  pigorsch
**  - added documentation
**
**  Revision 1.2  2002/04/04 12:02:38  pigorsch
**  - renamed operator() to fetch()
**  - removed function temporary()
**
**  Revision 1.1  2002/03/26 07:36:28  ronneber
**  restructuring for autoconf
**
**  Revision 1.1.1.1  2002/03/22 13:45:07  pigorsch
**  moved from polsoft repository
**
**
**
**************************************************************************/

#ifndef ARGITER_HH
#define ARGITER_HH

/*======================================================================*/
/*!
 *  \class ArgIter ArgIter.hh
 *  \brief The ArgIter class is an abstract class used by the CmdLine 
 *         class for fetching command line arguments.
 *
 *  The ArgIter abstract class is an interface for all classes that
 *  iterate through the arguments. The main function of this class is
 *  the fetch() function which returns the current argument and advances
 *  to the next one.
 */
/*======================================================================*/
class ArgIter 
{
public:
  /*====================================================================*/
  /*! 
   *   Constructor.
   */
  /*====================================================================*/
  ArgIter()
  {};

  /*====================================================================*/
  /*! 
   *   Destructor.
   */
  /*====================================================================*/
  virtual ~ArgIter()
  {};

  /*====================================================================*/
  /*! 
   *   Returns the current argument, advances to the next argument and
   *   returns NULL is there are no arguments anymore.
   *
   *   \return current argument, NULL if no arguments
   */
  /*====================================================================*/
  virtual const char*
  fetch()=0;

private:
  /*====================================================================*/
  /*! 
   *   We do not allow copy constructor.
   */
  /*====================================================================*/
   ArgIter(const ArgIter&) 
        {};

  /*====================================================================*/
  /*! 
   *   We do not allow operator=.
   */
  /*====================================================================*/
   ArgIter&
   operator=(const ArgIter&)
        {
          return *this;
        };
};

#endif
