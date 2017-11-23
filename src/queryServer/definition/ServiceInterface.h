#pragma once

#include "ServiceResults.h"
#include "Query.h"
#include "Alias.h"
#include "ParameterValues.h"
#include "contentServer/definition/ContentInfoList.h"

#include <grid-files/grid/Typedefs.h>
#include <grid-files/grid/GridPointValueList.h>
#include <grid-files/common/Log.h>


namespace SmartMet
{
namespace QueryServer
{

// This service is used for fetching grid values from grid files. The basic idea
// is that the 'contentList' parameter identifies grids (i.e. messages in grid
// files) that we are using when picking grid data.
//
// The easiest way to create the contentList parameter is to use services offered
// by the contentServer. For example:
//
//    contentServer->getContentListByParameterAndGenerationName(
//        0,                                     => Session identifier
//        "HIRMLAM:20170320T000000",             => Generation name
//        1,                                     => Type of the parameter key (1 = Radon-name)
//        "T-K",                                 => Parameter key
//        3,                                     => Type of the level id (3 = GRIB2 level identifier)
//        103,                                   => Level id (103 = height above ground)
//        2,                                     => Minimum level (2m)
//        2,                                     => Maximum level (2m)
//        "20170320T120000",                     => Start time
//        "20170320T160000",                     => End time
//        0,                                     => Request flags
//        contentList                            => Returned content info list.
//     );
//
// When we have found correct grids then we should decide how to fetch data from these
// grids. For example, we might be interested in data in specific coordinates (=> point data)
// or in a specific area (=> area data). That's why we have several methods for fetching
// data.
//
// It is good to realize that some times the fetching methods calculate the fetched data
// instead of returning original grid values. For example, if we want to get data from
// coordinates x,y then we most likely get data that is interpolated from the closest grid
// point values.
//
// It is also good to realize that usually the requested data is returned in "a smart result-set"
// that allows several advanced operations. For example, if we fetch temperature forecasts
// from the grid files, then we most likely get one forecast value per an hour.
//
// If we want to get one forecast value per every 15 minutes the we ask it from the original
// result-set. In this case the new result-set is based on interpolation over time.
//
//   valueList->getGridPointValueListByTimeSteps(
//        23.50,                                 => Grid X-coordinate (=> longitude)
//        65.20,                                 => Grid Y-coordinate (=> latitude)
//        2,                                     => Grid level (2m)
//        "20170320T120000",                     => Start time
//        48,                                    => Number of time steps,
//        900,                                   => Time step size in seconds
//        newValueList                           => Returned new result-set.
//   );



class ServiceInterface
{
  public:
                    ServiceInterface();
     virtual        ~ServiceInterface();

     virtual void   setProcessingLog(Log *processingLogPointer);
     virtual void   shutdown();

     virtual int    executeQuery(T::SessionId sessionId,Query& query);

     virtual int    getValuesByGridPoint(T::SessionId sessionId,T::ContentInfoList& contentInfoList,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod,T::GridPointValueList& valueList);

  protected:

     virtual int    _executeQuery(T::SessionId sessionId,Query& query);
     virtual int    _getValuesByGridPoint(T::SessionId sessionId,T::ContentInfoList& contentInfoList,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod,T::GridPointValueList& valueList);

     Log            *mProcessingLogPointer;
};


}
}

