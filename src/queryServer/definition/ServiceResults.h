#pragma once

#include <string>


namespace SmartMet
{
namespace QueryServer
{

// ====================================================================================
/*! \brief Result codes returned by QueryServer service methods. */
// ====================================================================================

enum Result
{
  OK                                          =  0,    //!< Operation completed successfully.
  NOT_IMPLEMENTED                             = -1,    //!< Method not implemented by this backend.
  DATA_NOT_FOUND                              = -2,    //!< Requested data does not exist.
  UNEXPECTED_EXCEPTION                        = -3,    //!< An unexpected C++ exception was caught.
  SERVICE_DISABLED                            = -20,   //!< Service has been administratively disabled.
  NO_PRODUCERS_FOUND                          = -100   //!< None of the requested producers were found.
};


std::string getResultString(int resultCode);

}
}
