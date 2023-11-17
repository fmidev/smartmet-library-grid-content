#include "QueryParameter.h"
#include <grid-files/common/GeneralFunctions.h>
#include <boost/functional/hash.hpp>


namespace SmartMet
{
namespace QueryServer
{


QueryParameter::QueryParameter()
{
  try
  {
    mId = 0;
    mAlternativeParamId = 0;
    mType = 0;
    mLocationType = 0;
    mParameterKeyType = T::ParamKeyTypeValue::UNKNOWN;
    mParameterLevelId = 0;
    mParameterLevel = -1;
    mForecastType = -1;
    mForecastNumber = -1;
    mAreaInterpolationMethod = T::AreaInterpolationMethod::Undefined;
    mTimeInterpolationMethod = T::TimeInterpolationMethod::Undefined;
    mLevelInterpolationMethod = T::LevelInterpolationMethod::Undefined;
    mProducerId = 0;
    mGenerationFlags = 0;
    mGeometryId = -1;
    mPrecision = -1;
    mTimestepsBefore = 0;
    mTimestepsAfter = 0;
    mTimestepSizeInMinutes = 0;
    mTemporary = false;
    mFlags = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




/*
QueryParameter::QueryParameter(const QueryParameter& queryParameter)
{
  try
  {
    mId = queryParameter.mId;
    mAlternativeParamId = queryParameter.mAlternativeParamId;
    mType = queryParameter.mType;
    mLocationType = queryParameter.mLocationType;
    mParam = queryParameter.mParam;
    mOrigParam = queryParameter.mOrigParam;
    mSymbolicName = queryParameter.mSymbolicName;
    mParameterKeyType = queryParameter.mParameterKeyType;
    mParameterKey = queryParameter.mParameterKey;
    mProducerName = queryParameter.mProducerName;
    mGeometryId = queryParameter.mGeometryId;
    mParameterLevelId = queryParameter.mParameterLevelId;
    mParameterLevel = queryParameter.mParameterLevel;
    mForecastType = queryParameter.mForecastType;
    mForecastNumber = queryParameter.mForecastNumber;
    mAreaInterpolationMethod = queryParameter.mAreaInterpolationMethod;
    mTimeInterpolationMethod = queryParameter.mTimeInterpolationMethod;
    mLevelInterpolationMethod = queryParameter.mLevelInterpolationMethod;
    mContourLowValues = queryParameter.mContourLowValues;
    mContourHighValues = queryParameter.mContourHighValues;
    mContourColors = queryParameter.mContourColors;
    mProducerId = queryParameter.mProducerId;
    mGenerationFlags = queryParameter.mGenerationFlags;
    mTimestepsBefore = queryParameter.mTimestepsBefore;
    mTimestepsAfter = queryParameter.mTimestepsAfter;
    mTimestepSizeInMinutes = queryParameter.mTimestepSizeInMinutes;
    mValueList = queryParameter.mValueList;
    mPrecision = queryParameter.mPrecision;
    mTemporary = queryParameter.mTemporary;
    mFunction = queryParameter.mFunction;
    mFunctionParams = queryParameter.mFunctionParams;
    mFlags = queryParameter.mFlags;
    mCoordinates = queryParameter.mCoordinates;
    mAttributeList = queryParameter.mAttributeList;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}
*/




QueryParameter::~QueryParameter()
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





std::size_t QueryParameter::getHash()
{
  try
  {
    std::size_t hash = 0;
    boost::hash_combine(hash,mId);
    boost::hash_combine(hash,mAlternativeParamId);
    boost::hash_combine(hash,mType);
    boost::hash_combine(hash,mLocationType);
    boost::hash_combine(hash,mParam);
    boost::hash_combine(hash,mOrigParam);
    boost::hash_combine(hash,mSymbolicName);
    boost::hash_combine(hash,mParameterKeyType);
    boost::hash_combine(hash,mParameterKey);
    boost::hash_combine(hash,mProducerName);
    boost::hash_combine(hash,mGeometryId);
    boost::hash_combine(hash,mParameterLevelId);
    boost::hash_combine(hash,mParameterLevel);
    boost::hash_combine(hash,mForecastType );
    boost::hash_combine(hash,mForecastNumber);
    boost::hash_combine(hash,mAreaInterpolationMethod);
    boost::hash_combine(hash,mTimeInterpolationMethod);
    boost::hash_combine(hash,mLevelInterpolationMethod);

    for (auto it = mContourLowValues.begin(); it != mContourLowValues.end(); ++it)
      boost::hash_combine(hash,*it);

    for (auto it = mContourHighValues.begin(); it != mContourHighValues.end(); ++it)
      boost::hash_combine(hash,*it);

    for (auto it = mContourColors.begin(); it != mContourColors.end(); ++it)
      boost::hash_combine(hash,*it);

    boost::hash_combine(hash,mProducerId);
    boost::hash_combine(hash,mGenerationFlags);
    boost::hash_combine(hash,mTimestepsBefore);
    boost::hash_combine(hash,mTimestepsAfter);
    boost::hash_combine(hash,mTimestepSizeInMinutes);

    //mValueList = queryParameter.mValueList;

    boost::hash_combine(hash,mPrecision);
    //boost::hash_combine(hash,mTemporary);
    //boost::hash_combine(hash,mFunction);
    //mFunctionParams = queryParameter.mFunctionParams;
    //boost::hash_combine(hash,mFlags);

    //mCoordinates = queryParameter.mCoordinates;

    boost::hash_combine(hash,mAttributeList.getHash());

    return hash;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




void QueryParameter::getValueListSize(int& columns,int& rows)
{
  try
  {
    columns = 0;
    rows = 0;
    if (mValueList.size() == 0)
      return;

    rows = mValueList.size();
    for (auto it = mValueList.begin(); it != mValueList.end(); ++it)
    {
      int len = (*it)->mValueList.getLength();
      if (len > columns)
        columns = len;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





T::ParamValue QueryParameter::getValueListValue(int col,int row)
{
  try
  {
    if (row < 0 || row >= (int)mValueList.size())
      return ParamValueMissing;

    if (col < 0 || col >= (int)mValueList[row]->mValueList.getLength())
      return ParamValueMissing;


    auto rec = mValueList[row]->mValueList.getGridValuePtrByIndex(col);
    if (!rec)
      return ParamValueMissing;

    return rec->mValue;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void QueryParameter::getValueListValuesByColumnRange(int startCol,int endCol,int row,std::vector<T::ParamValue>& values)
{
  try
  {
    for (int c = startCol; c<= endCol; c++)
    {
      values.emplace_back(getValueListValue(c,row));
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void QueryParameter::getValueListValuesByRowRange(int col,int startRow,int endRow,std::vector<T::ParamValue>& values)
{
  try
  {
    for (int r = startRow; r<= endRow; r++)
    {
      values.emplace_back(getValueListValue(col,r));
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void QueryParameter::getValueVectorsByRowRange(int startRow,int endRow,std::vector<std::vector<T::ParamValue>>& valueVectors)
{
  try
  {
    if (startRow < 0 || endRow >= (int)mValueList.size())
      return;

    for (int r = startRow; r<= endRow; r++)
    {
      valueVectors.emplace_back(mValueList[r]->mValueVector);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




void QueryParameter::removeTimes(std::set<time_t>& times)
{
  try
  {
    ParameterValues_sptr_vec valueList = mValueList;
    mValueList.clear();

    for (auto it=valueList.begin(); it != valueList.end(); ++it)
    {
      if (times.find((*it)->mForecastTimeUTC) == times.end())
        mValueList.emplace_back(*it);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void QueryParameter::removeAggregationValues()
{
  try
  {
    ParameterValues_sptr_vec valueList = mValueList;
    mValueList.clear();

    for (auto it=valueList.begin(); it != valueList.end(); ++it)
    {
      if (((*it)->mFlags & ParameterValues::Flags::AggregationValue) == 0)
        mValueList.emplace_back(*it);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void QueryParameter::removeInternalAggregationValues()
{
  try
  {
    ParameterValues_sptr_vec valueList = mValueList;
    mValueList.clear();

    for (auto it=valueList.begin(); it != valueList.end(); ++it)
    {
      if (((*it)->mFlags & ParameterValues::Flags::InternalAggregationValue) == 0)
        mValueList.emplace_back(*it);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void QueryParameter::updateForecastTimeList(std::set<time_t>& mForecastTimeList)
{
  try
  {
    for (auto it=mValueList.begin(); it != mValueList.end(); ++it)
    {
      mForecastTimeList.insert((*it)->mForecastTimeUTC);
    }

  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





T::GridValue* QueryParameter::getValueListRecord(int col,int row)
{
  try
  {
    if (row < 0 || row >= (int)mValueList.size())
      return nullptr;

    if (col < 0 || col >= (int)mValueList[row]->mValueList.getLength())
      return nullptr;

    return mValueList[row]->mValueList.getGridValuePtrByIndex(col);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




QueryParameter* QueryParameter::duplicate()
{
  try
  {
    return new QueryParameter(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void QueryParameter::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "QueryParameter\n";
    stream << space(level) << "- mId                       = " << mId << "\n";
    stream << space(level) << "- mAlternativeParamId       = " << mAlternativeParamId << "\n";
    stream << space(level) << "- mType                     = " << C_INT(mType)<< "\n";
    stream << space(level) << "- mLocationType             = " << C_INT(mLocationType) << "\n";
    stream << space(level) << "- mParam                    = " << mParam << "\n";
    stream << space(level) << "- mOrigParam                = " << mOrigParam << "\n";
    stream << space(level) << "- mSymbolicName             = " << mSymbolicName << "\n";
    stream << space(level) << "- mParameterKeyType         = " << C_INT(mParameterKeyType) << "\n";
    stream << space(level) << "- mParameterKey             = " << mParameterKey << "\n";
    stream << space(level) << "- mProducerName             = " << mProducerName << "\n";
    stream << space(level) << "- mGeometryId               = " << mGeometryId << "\n";
    stream << space(level) << "- mParameterLevelId         = " << C_INT(mParameterLevelId) << "\n";
    stream << space(level) << "- mParameterLevel           = " << mParameterLevel << "\n";
    stream << space(level) << "- mForecastType             = " << mForecastType << "\n";
    stream << space(level) << "- mForecastNumber           = " << mForecastNumber << "\n";
    stream << space(level) << "- mAreaInterpolationMethod  = " << mAreaInterpolationMethod << "\n";
    stream << space(level) << "- mTimeInterpolationMethod  = " << mTimeInterpolationMethod << "\n";
    stream << space(level) << "- mLevelInterpolationMethod = " << mLevelInterpolationMethod << "\n";
    stream << space(level) << "- mProducerId               = " << mProducerId << "\n";
    stream << space(level) << "- mGenerationFlags          = " << mGenerationFlags << "\n";
    stream << space(level) << "- mTimestepsBefore          = " << mTimestepsBefore << "\n";
    stream << space(level) << "- mTimestepsAfter           = " << mTimestepsAfter << "\n";
    stream << space(level) << "- mTimestepSizeInMinutes    = " << mTimestepSizeInMinutes << "\n";
    stream << space(level) << "- mTemporary                = " << mTemporary << "\n";
    stream << space(level) << "- mPrecision                = " << mPrecision << "\n";
    stream << space(level) << "- mFunction                 = " << mFunction << "\n";
    stream << space(level) << "- mFlags                    = " << mFlags << "\n";
    stream << space(level) << "- mCoordinates              = " << mCoordinates.size() << " coordinates\n";
    stream << space(level) << "- mAttributeList            = " << mAttributeList.getLength() << " attributes\n";

    mAttributeList.print(stream,level+2,optionFlags);

    stream << space(level) << "- mFunctionParams           = \n";
    for (auto it = mFunctionParams.begin();  it != mFunctionParams.end(); ++it)
    {
      stream << space(level) << "   * " << it->first << ":" << it->second << "\n";
    }

    stream << space(level) << "- mContourLowValues         = \n";
    for (auto it = mContourLowValues.begin();  it != mContourLowValues.end(); ++it)
    {
      stream << space(level) << "   * " << *it << "\n";
    }

    stream << space(level) << "- mContourHighValues        = \n";
    for (auto it = mContourHighValues.begin();  it != mContourHighValues.end(); ++it)
    {
      stream << space(level) << "   * " << *it << "\n";
    }

    stream << space(level) << "- mContourColors            = \n";
    for (auto it = mContourColors.begin();  it != mContourColors.end(); ++it)
    {
      char hex[20];
      sprintf(hex,"%08X",*it);
      stream << space(level) << "   * " << hex << "\n";
    }

    stream << space(level) << "- mValueList                = (" << mValueList.size() << " items)\n";

    for (auto it = mValueList.begin();  it != mValueList.end(); ++it)
    {
      (*it)->print(stream,level+2,optionFlags);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace QueryServer
}  // namespace SmartMet

