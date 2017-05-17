#pragma once

#include <string>


namespace SmartMet
{
namespace QueryServer
{

enum Result
{
  OK                                          =  0,
  NOT_IMPLEMENTED                             = -1,
  DATA_NOT_FOUND                              = -2,
  UNEXPECTED_EXCEPTION                        = -3
};


std::string getResultString(int resultCode);

}
}
