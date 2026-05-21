#pragma once

#include "ServiceInterface.h"
#include "Query.h"
#include <spine/HTTP.h>
#include <memory>



namespace SmartMet
{
namespace QueryServer
{

typedef std::shared_ptr<ServiceInterface> QueryServer_sptr;  //!< Shared pointer to a QueryServer ServiceInterface.

// ====================================================================================
/*! \brief HTTP content streamer that executes a list of queries and delivers results chunk-by-chunk.
 *
 *  Integrates with the Spine HTTP framework's chunked-transfer mechanism.  Each call
 *  to getChunk() executes the next pending Query via the QueryServer and serialises
 *  the results.  Used for large query responses that should not be buffered in memory. */
// ====================================================================================

class QueryStreamer : public Spine::HTTP::ContentStreamer
{
  public:
                        QueryStreamer();
    virtual             ~QueryStreamer();

    void                init(T::SessionId sessionId,QueryServer_sptr queryServerPtr);
    void                addQuery(Query& query);

    virtual std::string getChunk();

  protected:

    T::SessionId        mSessionId;       //!< Session identifier passed to each executeQuery call.
    Query_vec           mQueryList;       //!< Ordered list of queries to be executed and streamed.
    uint                mQueryPos;        //!< Index of the next query to execute in mQueryList.
    QueryServer_sptr    mQueryServerPtr;  //!< QueryServer used to execute each query.
};


typedef std::shared_ptr<QueryStreamer> QueryStreamer_sptr;  //!< Shared pointer to a QueryStreamer.


}  // namespace QueryServer
}  // namespace SmartMet

