#include "QueryStreamer.h"

namespace SmartMet
{
namespace QueryServer
{


/*! \brief Query Server: Constructor. */

QueryStreamer::QueryStreamer()
{
  try
  {
    mQueryPos = 0;
    mSessionId = 0;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Operation failed!",nullptr);
    throw exception;
  }
}





/*! \brief Query Server: Destructor. */

QueryStreamer::~QueryStreamer()
{
}





/*! \brief Query Server: Init. */

void QueryStreamer::init(T::SessionId sessionId,QueryServer_sptr queryServerPtr)
{
  try
  {
    mSessionId = sessionId;
    mQueryServerPtr = queryServerPtr;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Operation failed!",nullptr);
    throw exception;
  }
}





/*! \brief Query Server: Add query. */

void QueryStreamer::addQuery(Query& query)
{
  try
  {
    mQueryList.emplace_back(query);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Operation failed!",nullptr);
    throw exception;
  }
}





/*! \brief Query Server: Get chunk. */

std::string QueryStreamer::getChunk()
{
  try
  {
    if (mQueryPos < mQueryList.size())
    {
      Query query = mQueryList[mQueryPos];
      mQueryPos++;

      int result = mQueryServerPtr->executeQuery(mSessionId,query);
      if (result != 0)
      {
        setStatus(Spine::HTTP::ContentStreamer::StreamerStatus::EXIT_ERROR);
        return "";
      }

      for (auto it = query.mQueryParameterList.begin(); it != query.mQueryParameterList.end(); ++it)
      {
        for (auto v = it->mValueList.begin(); v != it->mValueList.end(); ++v)
        {
          uint alen = (*v)->mValueData.size();
          for (uint a=0; a<alen; a++)
          {
            uint len = (*v)->mValueData[a].size();
            if (len > 0)
            {
              std::string str((*v)->mValueData[a].begin(), (*v)->mValueData[a].end());
              setStatus(Spine::HTTP::ContentStreamer::StreamerStatus::OK);
              return str;
            }
          }
        }
      }
      setStatus(Spine::HTTP::ContentStreamer::StreamerStatus::OK);
      return "";
    }
    else
    {
      setStatus(Spine::HTTP::ContentStreamer::StreamerStatus::EXIT_OK);
      return "";
    }
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Operation failed!",nullptr);
    throw exception;
  }
}



}  // namespace QueryServer
}  // namespace SmartMet

