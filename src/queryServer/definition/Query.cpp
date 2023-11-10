#include "Query.h"
#include <grid-files/common/GeneralFunctions.h>
#include <time.h>
#include <boost/functional/hash.hpp>


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
    boost::hash_combine(hash,mSearchType);

    for (auto it = mProducerNameList.begin(); it != mProducerNameList.end(); ++it)
      boost::hash_combine(hash,*it);

    for (auto it = mForecastTimeList.begin(); it != mForecastTimeList.end(); ++it)
      boost::hash_combine(hash,*it);

    boost::hash_combine(hash,mAttributeList.getHash());
    boost::hash_combine(hash,mCoordinateType);

    for (auto coordinates = mAreaCoordinates.begin(); coordinates != mAreaCoordinates.end(); ++coordinates)
    {
      for (auto it = coordinates->begin(); it != coordinates->end(); ++it)
      {
        boost::hash_combine(hash,it->x());
        boost::hash_combine(hash,it->y());
      }
    }

    boost::hash_combine(hash,mRadius);

    for (auto it = mQueryParameterList.begin(); it != mQueryParameterList.end(); ++it)
      boost::hash_combine(hash,it->getHash());

    boost::hash_combine(hash,mTimezone);
    boost::hash_combine(hash,mStartTime);
    boost::hash_combine(hash,mEndTime);
    boost::hash_combine(hash,mTimesteps);
    boost::hash_combine(hash,mTimestepSizeInMinutes);
    boost::hash_combine(hash,mAnalysisTime);

    for (auto it = mGeometryIdList.begin(); it != mGeometryIdList.end(); ++it)
      boost::hash_combine(hash,*it);

    boost::hash_combine(hash,mLanguage);
    boost::hash_combine(hash,mGenerationFlags);
    boost::hash_combine(hash,mFlags);
    boost::hash_combine(hash,mMaxParameterValues);

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





void Query::removeInternalAggregationValues()
{
  try
  {
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
      stream << space(level) << "- mStartTime              = \n";

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

    stream << space(level) << "- mGenerationFlags        = " << mGenerationFlags << "\n";

    stream << space(level) << "- mGeometryIdList         = \n";
    for (auto it = mGeometryIdList.begin(); it != mGeometryIdList.end(); ++it)
      stream << space(level) << "   * " << *it << "\n";

    stream << space(level) << "- mAttributeList          = \n";
    mAttributeList.print(stream,level+2,optionFlags);

    stream << space(level) << "- mForecastTimeList\n";
    for (auto it = mForecastTimeList.begin(); it != mForecastTimeList.end(); ++it)
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

