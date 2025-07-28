#include "Query.h"
#include <grid-files/common/GeneralFunctions.h>
#include <time.h>
#include <macgyver/Hash.h>


namespace SmartMet
{
namespace QueryServer
{




Query::Query()
{
  try
  {
    mSearchType = 0;
    mFlags = 0;
    mGenerationFlags = 0;
    mCoordinateType = T::CoordinateTypeValue::LATLON_COORDINATES;
    mRadius = 0;
    mStartTime = 0;
    mEndTime = 0;
    mTimesteps = 0;
    mTimestepSizeInMinutes = 0;
    mMaxParameterValues = 1000000;
    mProcessingStartTime = time(nullptr);
    mMaxProcessingTimeInSeconds = 300;

  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




/*
Query::Query(const Query& query)
{
  try
  {
    mSearchType = query.mSearchType;
    mProducerNameList = query.mProducerNameList;
    mForecastTimeList = query.mForecastTimeList;
    mAttributeList = query.mAttributeList;
    mCoordinateType = query.mCoordinateType;
    mAreaCoordinates = query.mAreaCoordinates;
    mRadius = query.mRadius;
    mQueryParameterList = query.mQueryParameterList;
    mTimezone = query.mTimezone;
    mStartTime = query.mStartTime;
    mEndTime = query.mEndTime;
    mTimesteps = query.mTimesteps;
    mTimestepSizeInMinutes = query.mTimestepSizeInMinutes;
    mAnalysisTime = query.mAnalysisTime;
    mGeometryIdList = query.mGeometryIdList;
    mLanguage = query.mLanguage;
    mGenerationFlags = query.mGenerationFlags;
    mFlags = query.mFlags;
    mMaxParameterValues = query.mMaxParameterValues;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}
*/




Query::~Query()
{
  try
  {
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





std::size_t Query::getHash()
{
  try
  {
    std::size_t hash = 0;
    Fmi::hash_merge(hash,mSearchType);

    for (auto it = mProducerNameList.begin(); it != mProducerNameList.end(); ++it)
      Fmi::hash_merge(hash,*it);

    for (auto it = mForecastTimeList.begin(); it != mForecastTimeList.end(); ++it)
      Fmi::hash_merge(hash,*it);

    Fmi::hash_merge(hash,mAttributeList.getHash());
    Fmi::hash_merge(hash,mCoordinateType);

    for (auto coordinates = mAreaCoordinates.begin(); coordinates != mAreaCoordinates.end(); ++coordinates)
    {
      for (auto it = coordinates->begin(); it != coordinates->end(); ++it)
      {
        Fmi::hash_merge(hash,it->x());
        Fmi::hash_merge(hash,it->y());
      }
    }

    Fmi::hash_merge(hash,mRadius);

    for (auto it = mQueryParameterList.begin(); it != mQueryParameterList.end(); ++it)
      Fmi::hash_merge(hash,it->getHash());

    Fmi::hash_merge(hash,mTimezone);
    Fmi::hash_merge(hash,mStartTime);
    Fmi::hash_merge(hash,mEndTime);
    Fmi::hash_merge(hash,mTimesteps);
    Fmi::hash_merge(hash,mTimestepSizeInMinutes);
    Fmi::hash_merge(hash,mAnalysisTime);

    for (auto it = mGeometryIdList.begin(); it != mGeometryIdList.end(); ++it)
      Fmi::hash_merge(hash,*it);

    Fmi::hash_merge(hash,mLanguage);
    Fmi::hash_merge(hash,mGenerationFlags);
    Fmi::hash_merge(hash,mFlags);
    Fmi::hash_merge(hash,mMaxParameterValues);

    return hash;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
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
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
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
        uint vCount = (*vList)->mValueList.getLength();
        if (vCount > valueCount)
          valueCount = vCount;
      }
    }
    return valueCount;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
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
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
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
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Query::getResultProducerIdList(std::set<uint>& producerIdList)
{
  try
  {
    for (auto param = mQueryParameterList.begin(); param != mQueryParameterList.end(); ++param)
    {
      for (auto val = param->mValueList.begin(); val != param->mValueList.end(); val++)
      {
        //if (producerIdList.find((*val)->mProducerId) == producerIdList.end())
          producerIdList.insert((*val)->mProducerId);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Query::removeTemporaryParameters()
{
  try
  {
    int s = mQueryParameterList.size();
    while (s > 0)
    {
      if (mQueryParameterList[s-1].mTemporary)
      {
        mQueryParameterList.pop_back();
      }
      s--;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Query::removeAggregationValues()
{
  try
  {
    //return;

    for (auto it = mQueryParameterList.begin(); it != mQueryParameterList.end(); ++it)
    {
      it->removeAggregationValues();
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Query::removeAggregationTimes()
{
  try
  {
    return;
    if (mAggregationTimes.size() == 0)
      return;

    for (auto it = mQueryParameterList.begin(); it != mQueryParameterList.end(); ++it)
    {
      it->removeTimes(mAggregationTimes);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Query::removeInternalAggregationValues()
{
  try
  {
    //return;

    for (auto it = mQueryParameterList.begin(); it != mQueryParameterList.end(); ++it)
    {
      it->removeInternalAggregationValues();
      if (it->mFlags & QueryParameter::Flags::InternalAggregationParameter)
        it->removeAggregationValues();
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Query::updateForecastTimeList()
{
  try
  {
    mForecastTimeList.clear();
    for (auto it = mQueryParameterList.begin(); it != mQueryParameterList.end(); ++it)
    {
      it->updateForecastTimeList(mForecastTimeList);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }

}


void Query::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "Query\n";
    stream << space(level) << "- mSearchType             = " << C_INT(mSearchType) << "\n";
    stream << space(level) << "- mTimezone               = " << mTimezone << "\n";
    if (mStartTime > 0)
      stream << space(level) << "- mStartTime              = " << utcTimeFromTimeT(mStartTime) << "\n";
    else
      stream << space(level) << "- mStartTime              = " << utcTimeFromTimeT(0) << "\n";

    if (mEndTime > 0)
      stream << space(level) << "- mEndTime                = " << utcTimeFromTimeT(mEndTime) << "\n";
    else
      stream << space(level) << "- mEndTime                = \n";

    stream << space(level) << "- mTimesteps              = " << mTimesteps << "\n";
    stream << space(level) << "- mTimestepSizeInMinutes  = " << mTimestepSizeInMinutes << "\n";
    stream << space(level) << "- mAnalysisTime           = " << mAnalysisTime << "\n";
    stream << space(level) << "- mFlags                  = " << mFlags << "\n";
    stream << space(level) << "- mCoordinateType         = " << C_INT(mCoordinateType) << "\n";
    stream << space(level) << "- mRadius                 = " << mRadius << "\n";
    stream << space(level) << "- mLanguage               = " << mLanguage << "\n";
    stream << space(level) << "- mMaxParameterValues     = " << mMaxParameterValues << "\n";
    stream << space(level) << "- mProcessingStartTime (UTC)   = " << utcTimeFromTimeT(mProcessingStartTime) << "\n";
    stream << space(level) << "- mMaxProcessingTimeInSeconds  = " << mMaxProcessingTimeInSeconds << "\n";

    stream << space(level) << "- mGenerationFlags        = " << mGenerationFlags << "\n";

    stream << space(level) << "- mGeometryIdList         = \n";
    for (auto it = mGeometryIdList.begin(); it != mGeometryIdList.end(); ++it)
      stream << space(level) << "   * " << *it << "\n";

    stream << space(level) << "- mAttributeList          = \n";
    mAttributeList.print(stream,level+2,optionFlags);

    stream << space(level) << "- mForecastTimeList\n";
    for (auto it = mForecastTimeList.begin(); it != mForecastTimeList.end(); ++it)
      stream << space(level) << "   * " << utcTimeFromTimeT(*it) << "\n";

    stream << space(level) << "- mAggregationTimes\n";
    for (auto it = mAggregationTimes.begin(); it != mAggregationTimes.end(); ++it)
      stream << space(level) << "   * " << utcTimeFromTimeT(*it) << "\n";

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
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





}  // namespace QueryServer
}  // namespace SmartMet

