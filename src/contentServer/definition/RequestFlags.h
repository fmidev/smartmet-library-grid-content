#pragma once

namespace SmartMet
{

namespace ContentServer
{

// ====================================================================================
/*! \brief Bitmask constants that control time-range boundary behaviour in content queries.
 *
 *  These flags are passed to content-list retrieval methods to indicate whether the
 *  nearest available field before and/or after the requested time window should be
 *  included in the result set. */
// ====================================================================================

class RequestFlags
{
  public:
    static const unsigned int INCLUDE_TIME_BEFORE      = 1 << 0; //!< Include the closest field with a valid time before the requested range.
    static const unsigned int INCLUDE_TIME_AFTER       = 1 << 1; //!< Include the closest field with a valid time after the requested range.
};

}
}


