#include "Query.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace QueryServer
{




Query::Query()
{
  try
  {
    mFlags = 0;
    mGenerationFlags = 0;
    mSearchType = QST_POINT;
    mRadius = 0;
    mDem = 0;
    mCoverType = 0;
    mMaxParameterValues = 1000000;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





Query::Query(Query& query)
{
  try
  {
    mProducerNameList = query.mProducerNameList;
    mStartTime = query.mStartTime;
    mEndTime = query.mEndTime;
    mAnalysisTime = query.mAnalysisTime;
    mForecastTimeList = query.mForecastTimeList;
    mSearchType = query.mSearchType;
    mCoordinateList = query.mCoordinateList;
    mRadius = query.mRadius;
    mDem = query.mDem;
    mCoverType = query.mCoverType;
    mQueryParameterList = query.mQueryParameterList;
    mLanguage = query.mLanguage;
    mGenerationFlags = query.mGenerationFlags;
    mFlags = query.mFlags;
    mMaxParameterValues = query.mMaxParameterValues;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





Query::~Query()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool Query::parameterInQuery(std::string param)
{
  try
  {
    for (auto it = mQueryParameterList.rbegin(); it != mQueryParameterList.rend(); ++it)
    {
      if (it->mParam == param  ||  it->mSymbolicName == param)
        return true;
    }
    return false;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





uint Query::getValuesPerTimeStep()
{
  try
  {
    uint valueCount = 0;
    for (auto param = mQueryParameterList.begin(); param != mQueryParameterList.end(); ++param)
    {
      for (auto vList = param->mValueList.begin(); vList != param->mValueList.end(); ++vList)
      {
        uint vCount = vList->mValueList.getLength();
        if (vCount > valueCount)
          valueCount = vCount;
      }
    }
    return valueCount;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}




QueryParameter* Query::getQueryParameterPtr(std::string param)
{
  try
  {
    for (auto it = mQueryParameterList.rbegin(); it != mQueryParameterList.rend(); ++it)
    {
      if (it->mParam == param  ||  it->mSymbolicName == param)
        return &(*it);
    }
    return nullptr;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





QueryParameter* Query::getQueryParameterPtr(uint id)
{
  try
  {
    for (auto it = mQueryParameterList.rbegin(); it != mQueryParameterList.rend(); ++it)
    {
      if (it->mId == id)
        return &(*it);
    }
    return nullptr;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Query::removeTemporaryParameters()
{
  try
  {
    while (mQueryParameterList.size() > 0)
    {
      if (mQueryParameterList[mQueryParameterList.size()-1].mTemporary)
        mQueryParameterList.pop_back();
      else
        return;
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Query::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "Query\n";

    stream << space(level) << "- mStartTime              = " << mStartTime << "\n";
    stream << space(level) << "- mEndTime                = " << mEndTime << "\n";
    stream << space(level) << "- mAnalysisTime           = " << mAnalysisTime << "\n";
    stream << space(level) << "- mFlags                  = " << mFlags << "\n";
    stream << space(level) << "- mSearchType             = " << (int)mSearchType << "\n";
    stream << space(level) << "- mRadius                 = " << mRadius << "\n";
    stream << space(level) << "- mDem                    = " << mDem << "\n";
    stream << space(level) << "- mCoverType              = " << mCoverType << "\n";
    stream << space(level) << "- mLanguage               = " << mLanguage << "\n";
    stream << space(level) << "- mMaxParameterValues     = " << mMaxParameterValues << "\n";

    stream << space(level) << "- mGenerationFlags        = " << mGenerationFlags << "\n";

    stream << space(level) << "- mForecastTimeList\n";
    for (auto it = mForecastTimeList.begin(); it != mForecastTimeList.end(); ++it)
      stream << space(level) << "   * " << *it << "\n";

    stream << space(level) << "- mProducerNameList       = \n";
    for (auto it = mProducerNameList.begin(); it != mProducerNameList.end(); ++it)
      stream << space(level) << "   * " << *it << "\n";

    stream << space(level) << "- mPolygonPath            = \n";
    for (auto cList = mCoordinateList.begin(); cList != mCoordinateList.end(); ++cList)
    {
      stream << space(level) << "  - mCoordinateList       = \n";
      for (auto it = cList->begin(); it != cList->end(); ++it)
        stream << space(level) << "     * " << it->x() << "," << it->y() << "\n";
    }

    stream << space(level) << "- mQueryParameterListList = \n";
    for (auto it = mQueryParameterList.begin(); it != mQueryParameterList.end(); ++it)
      it->print(stream,level+2,optionFlags);

  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





}  // namespace QueryServer
}  // namespace SmartMet

