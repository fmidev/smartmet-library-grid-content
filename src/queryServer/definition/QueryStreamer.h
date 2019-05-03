#pragma once

#include "ServiceInterface.h"
#include "Query.h"
#include <spine/HTTP.h>
#include <boost/shared_ptr.hpp>



namespace SmartMet
{
namespace QueryServer
{

typedef std::shared_ptr<ServiceInterface> QueryServer_sptr;


class QueryStreamer : public Spine::HTTP::ContentStreamer
{
  public:
                        QueryStreamer();
    virtual             ~QueryStreamer();

    void                init(T::SessionId sessionId,QueryServer_sptr queryServerPtr);
    void                addQuery(Query& query);

    virtual std::string getChunk();

  protected:

    T::SessionId        mSessionId;
    Query_vec           mQueryList;
    uint                mQueryPos;
    QueryServer_sptr    mQueryServerPtr;
};


typedef boost::shared_ptr<QueryStreamer> QueryStreamer_sptr;


}  // namespace QueryServer
}  // namespace SmartMet

