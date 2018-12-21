#include "Query.h"
#include <grid-files/common/GeneralFunctions.h>
#include <time.h>


namespace SmartMet
{
namespace QueryServer
{




Query::Query()
{
  try
  {
    mType = Type::PointValues;
    mSearchType = 0;
    mFlags = 0;
    mGenerationFlags = 0;
    mLocationType = LocationType::Point;
    mCoordinateType = T::CoordinateTypeValue::LATLON_COORDINATES;
    mRadius = 0;
    mMaxParameterValues = 1000000;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





Query::Query(const Query& query)
{
  try
  {
    mType = query.mType;
    mProducerNameList = query.mProducerNameList;
    mTimezone = query.mTimezone;
    mStartTime = query.mStartTime;
    mEndTime = query.mEndTime;
    mAnalysisTime = query.mAnalysisTime;
    mForecastTimeList = query.mForecastTimeList;
    mLocationType = query.mLocationType;
    mCoordinateType = query.mCoordinateType;
    mAttributeList = query.mAttributeList;
    mAreaCoordinates = query.mAreaCoordinates;
    mRadius = query.mRadius;
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





bool Query::parameterInQuery(const std::string& param)
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





QueryParameter* Query::getQueryParameterPtr(const std::string& param)
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
    stream << space(level) << "- mType                   = " << C_INT(mType) << "\n";
    stream << space(level) << "- mSearchType             = " << C_INT(mSearchType) << "\n";
    stream << space(level) << "- mTimezone               = " << mTimezone << "\n";
    stream << space(level) << "- mStartTime              = " << mStartTime << "\n";
    stream << space(level) << "- mEndTime                = " << mEndTime << "\n";
    stream << space(level) << "- mAnalysisTime           = " << mAnalysisTime << "\n";
    stream << space(level) << "- mFlags                  = " << mFlags << "\n";
    stream << space(level) << "- mLocationType           = " << C_INT(mLocationType) << "\n";
    stream << space(level) << "- mCoordinateType         = " << C_INT(mCoordinateType) << "\n";
    stream << space(level) << "- mRadius                 = " << mRadius << "\n";
    stream << space(level) << "- mLanguage               = " << mLanguage << "\n";
    stream << space(level) << "- mMaxParameterValues     = " << mMaxParameterValues << "\n";

    stream << space(level) << "- mGenerationFlags        = " << mGenerationFlags << "\n";
    stream << space(level) << "- mAttributeList          = \n";
    mAttributeList.print(stream,level+2,optionFlags);

    stream << space(level) << "- mForecastTimeList\n";
    for (auto it = mForecastTimeList.begin(); it != mForecastTimeList.end(); ++it)
      stream << space(level) << "   * " << *it << "\n";

    stream << space(level) << "- mProducerNameList       = \n";
    for (auto it = mProducerNameList.begin(); it != mProducerNameList.end(); ++it)
      stream << space(level) << "   * " << *it << "\n";

    stream << space(level) << "- mAreaCoordinates        = \n";
    for (auto cList = mAreaCoordinates.begin(); cList != mAreaCoordinates.end(); ++cList)
    {
      stream << space(level) << "  * mCoordinates (" << cList->size() << " coordinates)\n";
      if (cList->size() < 20)
      {
        for (auto it = cList->begin(); it != cList->end(); ++it)
          stream << space(level) << "     * " << it->x() << "," << it->y() << "\n";
      }
    }

    stream << space(level) << "- mQueryParameterList     = \n";
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

