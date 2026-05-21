#pragma once

#include <string>

namespace SmartMet
{
namespace SessionManagement
{

// ====================================================================================
/*! \brief Result codes returned by SessionManagement service methods. */
// ====================================================================================

enum Result
{
  OK                                  =  0,    //!< Operation completed successfully.
  NOT_IMPLEMENTED                     = -1,    //!< Method not implemented by this backend.
  DATA_NOT_FOUND                      = -2,    //!< Requested session or attribute does not exist.
  UNEXPECTED_EXCEPTION                = -3,    //!< An unexpected C++ exception was caught.
  INVALID_SESSION                     = -10,   //!< The provided session identifier is invalid or expired.
  SESSION_ATTRIBUTE_NOT_FOUND         = -11,   //!< The requested session attribute was not found.
  SERVICE_DISABLED                    = -20,   //!< Service has been administratively disabled.
};


std::string getResultString(int resultCode);

}
}
