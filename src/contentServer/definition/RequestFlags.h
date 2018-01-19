#pragma once

namespace SmartMet
{

namespace ContentServer
{


enum class RequestFlags
{
  INCLUDE_TIME_BEFORE      = 1 << 0,
  INCLUDE_TIME_AFTER       = 1 << 1
};

}
}


