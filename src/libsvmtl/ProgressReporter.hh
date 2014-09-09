/**************************************************************************
**       Title: interface for your own progress reporter 
**    $RCSfile$
**   $Revision: 4820 $$Name$
**       $Date: 2011-11-08 10:57:01 +0100 (Tue, 08 Nov 2011) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.3  2004/09/08 14:34:53  ronneber
**  - adapted to new ParamInfo class
**  - added clearScreen()
**  - cosmetic changes
**
**  Revision 1.2  2004/08/26 15:22:01  ronneber
**  - fixed help text
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**  Revision 1.2  2002/10/28 16:00:52  mechnich
**  fixed compatibility problems with gcc3
**
**  Revision 1.1  2002/05/22 16:39:06  ronneber
**  - added progress reporting capabilities
**
**
**
**************************************************************************/

#ifndef PROGRESSREPORTER_HH
#define PROGRESSREPORTER_HH

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "ParamInfo.hh"

namespace svt
{
  const int TASK_LEVEL_GRID_SEARCH = 1;
  const int TASK_LEVEL_CROSS_VAL  = 2;
  const int TASK_LEVEL_MULTICLASS = 3;
  const int TASK_LEVEL_TWOCLASS = 4;
  const int TASK_LEVEL_TRAINING_INFO = 5;
  


  class ProgressReporter
  {
  public:
    ProgressReporter( std::ostream& os = std::cerr)
            :_os(os),
             _verboseLevel(TASK_LEVEL_TWOCLASS), 
             _drawProgressBar( true),
             _cursorPosSaved( false),
             _progressBarLength( 20),
             _rotDashIndex(0),
             _maxTaskLevel(1),
             _dotCounter(0)
          {
            _rotatingDash.push_back("|");
            _rotatingDash.push_back("/");
            _rotatingDash.push_back("-");
            _rotatingDash.push_back("\\");
          }
    

    virtual ~ProgressReporter()
          {}


    virtual void setMaxTaskLevel( int n)
          {
            _maxTaskLevel = n;
          }
    
     /*======================================================================*/
    /*! 
     *   (description)
     *
     *   \param 
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    virtual void clearScreen()
          {
            _os << "\033[2J\033[H";
          }
    


    /*======================================================================*/
    /*! 
     *   This method is called if some progress was made
     *
     *   \param taskLevel     level of current task, e.g. 
     *                        1 for parameter tuning,
     *                        2 for cross validation, 
     *                        3 for multiclass algorithm, 
     *                        4 for twoclass algorithm, etc.
     *
     *   \param taskName      plain test name of current task
     *
     *   \param completenessPercent completeness of current task as
     *                        float between 0.0 and 1.0 (-1 means
     *                        cannot be calculated, -2 means start of
     *                        such a task without percent values)
     *
     *   \param completenessPlainText completeness of current task as
     *                        plain text message, e.g. "3 of 32"
     *
     *   \exception TrainingCanceled could be thrown, when user
     *                        canceled the training process or when a
     *                        timeOut appeared
     *
     */
    /*======================================================================*/
    virtual void reportProgress( int taskLevel,
                                 const std::string& taskName,
                                 float completenessPercent,
                                 const std::string& completenessPlainText)
          {
            if( taskLevel <= _verboseLevel)
            {
              /*------------------------------------------------------------
               *  if progress bar is requested, set cursor to proper line
               *------------------------------------------------------------*/
              if( _drawProgressBar)
              {
                /*-----------------------------------------------------------
                 *  go to upper line
                 *-----------------------------------------------------------*/
                if( _cursorPosSaved == false)
                {
                  // go 15 lines down and up to scroll terminal up
                  for( int i = 0; i < 15; ++i)
                  {
                    _os << "\n";
                  }
                  
                  _os << "\033[" << 15 << "A";
                  
                  // save cursor position
                  _os << "\033[s";
                  _cursorPosSaved = true;
                }

                
                /*-----------------------------------------------------------
                 *  go 'task level'-1 *2 lines down and write 
                 *  plain text message
                 *-----------------------------------------------------------*/
                // go to saved position
                _os << "\033[u\033[s";
                _os << "\n";
                if( taskLevel-_maxTaskLevel > 0)
                {
                  _os << "\033[" << (taskLevel-_maxTaskLevel)*3 << "B";
                }
                _os << taskName << "                           ";
                
                
                /*---------------------------------------------------------
                 *  draw progress bar
                 *--------------------------------------------------------*/
                // go to saved position
                _os << "\033[u\033[s";
                _os << "\n";
                if( taskLevel-_maxTaskLevel+1 > 0)
                {
                  _os << "\033[" << (taskLevel-_maxTaskLevel)*3+1 << "B";
                }
                
                
                if( completenessPercent >= 0)
                {
                  _os << " |";
                  
                  int nHashs = static_cast<int>(
                      static_cast<float>(_progressBarLength) *
                      completenessPercent + 0.5f);
                  
                  for( int i = 0; i < nHashs; ++i)
                  {
                    _os << "#";
                  }
                  for( int i = nHashs; i < _progressBarLength; ++i)
                  {
                    _os << "-";
                  }
                  _os << "| (" << completenessPlainText << ") " 
                      << int(completenessPercent*100) << "%      "<< std::flush;
                  
                  
                }
                else
                {
                  _os << _rotatingDash[_rotDashIndex];
                  _rotDashIndex = (_rotDashIndex + 1) %
                      static_cast<int>(_rotatingDash.size());

                  if( completenessPercent == -2)
                  {
                    _dotCounter = 0;
                  }
                  ++_dotCounter;
                  for( int i = 0; i < _dotCounter; ++i)
                  {
                    _os << ".";
                  }
                  _os << "                            " << std::flush;
                  
                  
                }


                
                /*-----------------------------------------------------------
                 *  go some lines down, that other messages don't
                 *  overwrite progress bars 
                 *-----------------------------------------------------------*/
                  // go to saved position
                  _os << "\033[u\033[s";
                  // go down _maxTaskLevel*2
                  _os << "\033[" << (5-_maxTaskLevel)*3 << "B\n";                
              }
              else
              {
                /*-----------------------------------------------------------
                 *  no progress bar requested, write plaintext messages
                 *-----------------------------------------------------------*/
                if( completenessPercent < 0)
                {
                  // unknown completeness is displayed with dots, or
                  // something else
                  _os << completenessPlainText << std::flush;
                }
                else
                {
                  // indentation according to task level
                  for( int i = 1; i < taskLevel; ++i)
                  {
                  _os << "\t";
                }
                _os << taskName << ": " << int(completenessPercent*100) 
                    << "% (" << completenessPlainText << ")\n";
                }
                
              }
              
            }
          }


    /*======================================================================*/
    /*! 
     *   (description)
     *
     *   \param 
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    virtual void additionalInfo( int taskLevel,
                                 const std::string& text)
          {
            if( taskLevel <= _verboseLevel)
            {
              if( _drawProgressBar)
              {
                /*-----------------------------------------------------------
                 *  go some lines down, that other messages don't
                 *  overwrite progress bars 
                 *-----------------------------------------------------------*/
                // go to saved position
                _os << "\033[u\033[s";
                // go down _maxTaskLevel*2
                _os << "\033[" << (5-_maxTaskLevel)*3 << "B\n";
              }
              
              _os << text << std::flush;
            }
          }
    
    
    
    /*======================================================================*/
    /*! 
     *   set the reporting level for reportProgress() method. 0 means
     *   report nothing. Higher numbers mean: report more. 
     *
     *   \param verboseLevel only report progress events equal to or
     *                         below this level 
     *
     */
    /*======================================================================*/
    void setVerboseLevel( int verboseLevel)
          {
            _verboseLevel = verboseLevel;
          }
    
    template<typename STDATA>
    void loadParameters( STDATA& stData)
          {
            stData.getValue( "verbose_level", _verboseLevel);
            stData.getValue( "draw_progress_bar", _drawProgressBar);
          }
    
    template<typename STDATA>
    void saveParameters( STDATA& stData) const
          {
            stData.setValue( "verbose_level", _verboseLevel);
            stData.setValue( "draw_progress_bar", _drawProgressBar);
          }

    /*======================================================================*/
    /*! 
     *   get information about the parameters, that are used in
     *   loadParameters() and saveParameters(). The Infos are appended
     *   to the passed array
     *
     *   \param p  array of ParamInfos. The Infos will be appended to
     *             this array
     */
    /*======================================================================*/
    static void getParamInfos( std::vector<ParamInfo>& p)
          {
            p.push_back( ParamInfo( "verbose_level", "vb"));
            p.back().addAlternative( "0", "report nothing");
            p.back().addAlternative( "1", "report parameter tuning");
            p.back().addAlternative( "2", "report cross validation");
            p.back().addAlternative( "3", "report multi class svm");
            p.back().addAlternative( "4", "report two class svm");
            p.back().addAlternative( "5", "report addtitional training infos");
            
            p.push_back( ParamInfo( "draw_progress_bar", "p"));
            p.back().addAlternative( "0", "no acsii progress bars");
            p.back().addAlternative( "1", 
                                     "draw acsii progress bars (default)");
            
          }

    static const char* name()
          {
            return "ascii_progress_reporter";
          }
    
    static const char* description()
          {
            return "reports progress on ansi ascii terminal";
          }

  private:
    std::ostream& _os;
    int _verboseLevel;
    bool _drawProgressBar;
    bool _cursorPosSaved;
    int _progressBarLength;
    int _rotDashIndex;
    std::vector< std::string> _rotatingDash;
    int _maxTaskLevel;
    int _dotCounter;
    
    
    
    
    
  };
}

#endif
