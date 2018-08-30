#pragma once

namespace SmartMet
{

namespace ContentServer
{


class RequestFlags
{
  public:
    static const unsigned int INCLUDE_TIME_BEFORE      = 1 << 0;
    static const unsigned int INCLUDE_TIME_AFTER       = 1 << 1;
};

}
}


