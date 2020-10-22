#include "QueryStreamer.h"

namespace SmartMet
{
namespace QueryServer
{


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





QueryStreamer::~QueryStreamer()
{
}





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





void QueryStreamer::addQuery(Query& query)
{
  try
  {
    mQueryList.push_back(query);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Operation failed!",nullptr);
    throw exception;
  }
}





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

