#pragma once

#include <string>

namespace SmartMet
{
namespace DataServer
{

enum Result
{
  OK                                  =  0,
  NOT_IMPLEMENTED                     = -1,
  DATA_NOT_FOUND                      = -2,
  UNEXPECTED_EXCEPTION                = -3,
  SERVICE_DISABLED                    = -20,
  FILE_NOT_FOUND                      = -2000,
  MESSAGE_NOT_FOUND                   = -2010,
  UNKNOW_COORDINATE_TYPE              = -2020,
  INVALID_NUMBER_OF_COORDINATES       = -2021,
  DATA_SETS_NOT_COMPATIBLE            = -2022,
  INVALID_GEOMETRY_ID                 = -2023,
  GEOMETRY_NOT_FOUND                  = -2024,
  INVALID_DIMENSIONS                  = -2025,
  UNSUPPORTED_INTERPOLATION_METHOD    = -2030
};


std::string getResultString(int resultCode);

}
}
