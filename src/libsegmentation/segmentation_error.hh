#ifndef LIBMARGRET_SRC_MARGRETERROR_HH
#define LIBMARGRET_SRC_MARGRETERROR_HH

#include <stdexcept>

namespace segmentation 
{
  
class LabellingError
 : public std::exception
{
    
public:
    
  LabellingError
(const char* message);
  ~LabellingError
() throw();

  const char* what() const throw();

private:
    
  const char* _message;
    
};
 
}

#endif
