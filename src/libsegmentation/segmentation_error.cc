#include "segmentation_error.hh"
namespace segmentation{
LabellingError::LabellingError(const char* message)
    : _message(message)
{}

LabellingError
::~LabellingError
() throw()
{}

const char* LabellingError
::what() const throw()
{
  return _message;
}
}

