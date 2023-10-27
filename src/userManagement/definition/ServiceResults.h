#pragma once

#include <string>

namespace SmartMet
{
namespace UserManagement
{

enum Result
{
  OK                                  =  0,
  NOT_IMPLEMENTED                     = -1,
  DATA_NOT_FOUND                      = -2,
  UNEXPECTED_EXCEPTION                = -3,
  INVALID_SESSION                     = -10,
  SERVICE_DISABLED                    = -20,
  USER_ATTRIBUTE_NOT_FOUND            = -31,
};


std::string getResultString(int resultCode);

}
}
