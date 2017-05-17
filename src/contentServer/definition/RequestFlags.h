#pragma once

namespace SmartMet
{

namespace ContentServer
{


enum class RequestFlags
{
  INCLUDE_TIME_BEFORE      = 1 << 0,
  INCLUDE_TIME_AFTER       = 1 << 1,

  INCLUDE_LEVEL_BEFORE     = 1 << 8,
  INCLUDE_LEVEL_AFTER      = 1 << 9
};

}
}


