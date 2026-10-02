// End-to-end tests of the query server on an in-process grid data stack (see QueryHarness.h).
//
// The expected values are read from the GRIB file with grid-files directly, so the tests check
// the query server layers on top of it: producer and parameter mapping, content searches, the
// data server, time interpolation, functions and the Query interface used by the grid engine.

#define BOOST_TEST_MODULE QueryServerTest
#include <boost/test/included/unit_test.hpp>

#include "QueryHarness.h"
#include "queryServer/definition/Query.h"
#include "queryServer/definition/QueryConfigurator.h"
#include "queryServer/definition/QueryParameter.h"

#include <grid-files/common/GeneralFunctions.h>

#include <cmath>
#include <memory>

using namespace SmartMet;
using namespace GridTest;

namespace
{
const std::string PAL = testData("grib/pal/200808050729_pal_skandinavia_pinta.grib");
const std::string PRODUCER = "pal_skandinavia";

// Helsinki and a point in Lapland
const double LON1 = 24.94;
const double LAT1 = 60.17;
const double LON2 = 25.72;
const double LAT2 = 66.50;

struct Fixture
{
  std::unique_ptr<GridWorld> world;
  GRID::GridFile file;

  Fixture()
  {
    requireFixture(CONFIG);
    requireFixture(PAL);
    world = std::make_unique<GridWorld>();
    world->addProducer(PRODUCER, "20080805T050000", {PAL});
    world->start();
    file.read(PAL);
  }

  // The message of the given parameter and forecast time, read directly
  GRID::Message *message(const std::string &param, const std::string &time)
  {
    for (uint i = 0; i < file.getNumberOfMessages(); i++)
    {
      GRID::Message *m = file.getMessageByIndex(i);
      if (m->getFmiParameterName() == param && m->getForecastTime() == time)
        return m;
    }
    BOOST_FAIL("no message " << param << " " << time);
    return nullptr;
  }

  double direct(const std::string &param, const std::string &time, double lat, double lon, short interpolation)
  {
    return message(param, time)->getGridValueByLatLonCoordinate(lat, lon, interpolation);
  }

  double query(const std::string &param,
               const std::string &time,
               double lat,
               double lon,
               short area = T::AreaInterpolationMethod::Linear,
               short timeInterpolation = T::TimeInterpolationMethod::Linear,
               int *result = nullptr)
  {
    T::ParamValue value = ParamValueMissing;
    int ret = world->queryServer.getParameterValueByPointAndTime(0,
                                                                 PRODUCER,
                                                                 param,
                                                                 T::CoordinateTypeValue::LATLON_COORDINATES,
                                                                 lon,
                                                                 lat,
                                                                 time,
                                                                 area,
                                                                 timeInterpolation,
                                                                 T::LevelInterpolationMethod::Linear,
                                                                 value);
    if (result != nullptr)
      *result = ret;
    return value;
  }
};

bool close(double a, double b, double tol = 1e-3)
{
  return std::fabs(a - b) <= tol * std::max(1.0, std::fabs(b));
}

}  // namespace

BOOST_AUTO_TEST_SUITE(query_server, *fixtures({CONFIG, PAL}))

BOOST_FIXTURE_TEST_CASE(point_values_match_the_grib_file, Fixture)
{
  withFmiErrors(
      [&]
      {
        for (const char *param : {"Temperature", "Pressure", "WindSpeedMS", "Humidity"})
        {
          for (const char *time : {"20080805T050000", "20080805T170000", "20080806T040000"})
          {
            for (auto method : {T::AreaInterpolationMethod::Nearest, T::AreaInterpolationMethod::Linear})
            {
              for (auto [lat, lon] : {std::pair{LAT1, LON1}, std::pair{LAT2, LON2}})
              {
                const double expected = direct(param, time, lat, lon, method);
                const double actual = query(param, time, lat, lon, method);
                BOOST_TEST_CONTEXT(param << " " << time << " method " << method << " at " << lat << "," << lon)
                {
                  BOOST_TEST(expected != ParamValueMissing);
                  BOOST_TEST(close(actual, expected), "query " << actual << ", grib " << expected);
                }
              }
            }
          }
        }
      });
}

BOOST_FIXTURE_TEST_CASE(time_interpolation, Fixture)
{
  withFmiErrors(
      [&]
      {
        const double v1 = direct("Temperature", "20080805T100000", LAT1, LON1, T::AreaInterpolationMethod::Linear);
        const double v2 = direct("Temperature", "20080805T110000", LAT1, LON1, T::AreaInterpolationMethod::Linear);

        // Linear interpolation halfway between the hourly steps
        const double mid = query("Temperature", "20080805T103000", LAT1, LON1);
        BOOST_TEST(close(mid, (v1 + v2) / 2), "interpolated " << mid << ", expected " << (v1 + v2) / 2);

        // Nearest, previous and next
        BOOST_TEST(close(query("Temperature", "20080805T102000", LAT1, LON1, T::AreaInterpolationMethod::Linear,
                               T::TimeInterpolationMethod::Nearest), v1));
        BOOST_TEST(close(query("Temperature", "20080805T104000", LAT1, LON1, T::AreaInterpolationMethod::Linear,
                               T::TimeInterpolationMethod::Nearest), v2));
        BOOST_TEST(close(query("Temperature", "20080805T105900", LAT1, LON1, T::AreaInterpolationMethod::Linear,
                               T::TimeInterpolationMethod::Previous), v1));
        BOOST_TEST(close(query("Temperature", "20080805T100100", LAT1, LON1, T::AreaInterpolationMethod::Linear,
                               T::TimeInterpolationMethod::Next), v2));

        // Outside the data: no value
        const double before = query("Temperature", "20080805T040000", LAT1, LON1);
        BOOST_TEST(before == ParamValueMissing);
      });
}

BOOST_FIXTURE_TEST_CASE(points_outside_the_grid_have_no_value, Fixture)
{
  withFmiErrors(
      [&]
      {
        // The equator and the south pole are far outside the Scandinavian grid
        BOOST_TEST(query("Temperature", "20080805T050000", 0.0, 25.0) == ParamValueMissing);
        BOOST_TEST(query("Temperature", "20080805T050000", -89.0, 25.0) == ParamValueMissing);
      });
}

BOOST_FIXTURE_TEST_CASE(unknown_producers_and_parameters, Fixture)
{
  withFmiErrors(
      [&]
      {
        int result = 0;
        const double v = query("NoSuchParameter", "20080805T050000", LAT1, LON1, T::AreaInterpolationMethod::Linear,
                               T::TimeInterpolationMethod::Linear, &result);
        BOOST_TEST((result != 0 || v == ParamValueMissing));

        T::ParamValue value = 0;
        int ret = world->queryServer.getParameterValueByPointAndTime(
            0, "no_such_producer", "Temperature", T::CoordinateTypeValue::LATLON_COORDINATES, LON1, LAT1,
            "20080805T050000", 1, 1, 1, value);
        BOOST_TEST((ret != 0 || value == ParamValueMissing));
      });
}

BOOST_FIXTURE_TEST_CASE(execute_query_time_series, Fixture)
{
  // The Query interface used by the grid engine: a point time series over the whole data
  withFmiErrors(
      [&]
      {
        T::AttributeList attributes;
        attributes.addAttribute("starttime", "20080805T050000");
        attributes.addAttribute("endtime", "20080806T040000");
        attributes.addAttribute("timestep", "data");
        attributes.addAttribute("producer", PRODUCER);
        attributes.addAttribute("param", "Temperature");
        attributes.addAttribute("areaInterpolationMethod", "1");

        QueryServer::Query q;
        QueryServer::QueryConfigurator configurator;
        configurator.configure(q, attributes);
        q.mCoordinateType = T::CoordinateTypeValue::LATLON_COORDINATES;
        T::Coordinate_vec point;
        point.emplace_back(LON1, LAT1);
        q.mAreaCoordinates.push_back(point);

        BOOST_TEST_REQUIRE(world->queryServer.executeQuery(0, q) == 0);
        BOOST_TEST_REQUIRE(q.mQueryParameterList.size() == 1U);
        const auto &values = q.mQueryParameterList[0].mValueList;
        BOOST_TEST_REQUIRE(values.size() == 24U);

        for (const auto &pv : values)
        {
          BOOST_TEST_REQUIRE(pv->mValueList.getLength() == 1U);
          T::GridValue gv;
          pv->mValueList.getGridValueByIndex(0, gv);
          const std::string time = utcTimeFromTimeT(pv->mForecastTimeUTC);
          const double expected = direct("Temperature", time, LAT1, LON1, T::AreaInterpolationMethod::Linear);
          BOOST_TEST_CONTEXT(time) { BOOST_TEST(close(gv.mValue, expected), "query " << gv.mValue << ", grib " << expected); }
        }
      });
}

BOOST_AUTO_TEST_SUITE_END()
