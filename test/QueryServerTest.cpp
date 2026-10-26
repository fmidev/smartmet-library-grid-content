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
const std::string EC = testData("grib/ecpinta/200808041930_ecmwf_eurooppa_pinta.grib");

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

BOOST_FIXTURE_TEST_CASE(execute_query_areas, Fixture)
{
  // The Query interface with the area location types the timeseries plugin uses for bbox, wkt
  // and radius: every grid point inside the area, the values those of the GRIB file
  withFmiErrors(
      [&]
      {
        const std::string time = "20080805T060000";
        auto run = [&](uchar locationType, const T::Coordinate_vec &coordinates, double radius)
        {
          T::AttributeList attributes;
          attributes.addAttribute("starttime", time);
          attributes.addAttribute("endtime", time);
          attributes.addAttribute("timestep", "data");
          attributes.addAttribute("producer", PRODUCER);
          attributes.addAttribute("param", "Temperature");
          attributes.addAttribute("areaInterpolationMethod", "1");

          QueryServer::Query q;
          QueryServer::QueryConfigurator configurator;
          configurator.configure(q, attributes);
          q.mCoordinateType = T::CoordinateTypeValue::LATLON_COORDINATES;
          q.mAreaCoordinates.push_back(coordinates);
          q.mRadius = radius;
          for (auto &qp : q.mQueryParameterList)
            qp.mLocationType = locationType;

          BOOST_TEST_REQUIRE(world->queryServer.executeQuery(0, q) == 0);
          BOOST_TEST_REQUIRE(q.mQueryParameterList.size() == 1U);
          const auto &values = q.mQueryParameterList[0].mValueList;
          BOOST_TEST_REQUIRE(values.size() == 1U);
          return values[0]->mValueList;
        };

        // A box around Helsinki
        T::Coordinate_vec box;
        box.emplace_back(LON1 - 0.5, LAT1 - 0.25);
        box.emplace_back(LON1 + 0.5, LAT1 - 0.25);
        box.emplace_back(LON1 + 0.5, LAT1 + 0.25);
        box.emplace_back(LON1 - 0.5, LAT1 + 0.25);
        box.emplace_back(LON1 - 0.5, LAT1 - 0.25);

        // A 20 km circle around Helsinki
        T::Coordinate_vec centre;
        centre.emplace_back(LON1, LAT1);

        auto check = [&](const std::string &context)
        {
          for (const auto &[name, list] :
               {std::pair{"polygon", run(QueryServer::QueryParameter::LocationType::Polygon, box, 0)},
                std::pair{"circle", run(QueryServer::QueryParameter::LocationType::Circle, centre, 20)}})
          {
            BOOST_TEST_CONTEXT(context << " " << name)
            {
              BOOST_TEST(list.getLength() > 1U);
              for (uint i = 0; i < list.getLength(); i++)
              {
                T::GridValue gv;
                list.getGridValueByIndex(i, gv);
                BOOST_TEST_REQUIRE(gv.mValue != ParamValueMissing);
                const double expected = direct("Temperature", time, gv.mY, gv.mX, T::AreaInterpolationMethod::Nearest);
                BOOST_TEST(close(gv.mValue, expected), "at " << gv.mY << "," << gv.mX << " query " << gv.mValue << ", grib " << expected);
              }
            }
          }
        };

        check("producers_test.csv");

        // The producer is also concatenated to a nowcast producer on an earlier line of the
        // producer file, like VIRE is to VIRENWC in production
        world = std::make_unique<GridWorld>();
        world->addProducer(PRODUCER, "20080805T050000", {PAL});
        world->start("nowcast:5900;" + PRODUCER + ":5900\n" + PRODUCER + ":5900\n");
        check("concatenated");

        // The producer is continued with another geometry which has no data (yet), like
        // VIRE:1110;VIRE:1125. The other geometry is not concatenated to an area, but the
        // producer itself is used.
        world = std::make_unique<GridWorld>();
        world->addProducer(PRODUCER, "20080805T050000", {PAL});
        world->start(PRODUCER + ":5900;" + PRODUCER + ":5908\n" + PRODUCER + ":5908\n");
        check("continued with another geometry");
      });
}

BOOST_FIXTURE_TEST_CASE(valid_selects_the_first_parameter_with_data, Fixture)
{
  // VALID{nowcast;forecast} takes the first parameter which has data for the time, and its
  // value as such: a missing nowcast value (say no precipitation) is not filled from the
  // forecast. A parameter without data for the time, or whose grid does not cover the point,
  // is skipped. Here pal starts at 05 UTC and does not cover 40N 10E, ECMWF covers both.
  requireFixture(EC);
  withFmiErrors(
      [&]
      {
        world = std::make_unique<GridWorld>();
        world->addProducer(PRODUCER, "20080805T050000", {PAL});
        world->addProducer("ecmwf_eurooppa_pinta", "20080804T120000", {EC});
        world->start();

        auto run = [&](const std::string &param, const std::string &time, double lat, double lon)
        {
          T::AttributeList attributes;
          attributes.addAttribute("starttime", time);
          attributes.addAttribute("endtime", time);
          attributes.addAttribute("timestep", "data");
          attributes.addAttribute("producer", PRODUCER);
          attributes.addAttribute("param", param);
          attributes.addAttribute("areaInterpolationMethod", "1");

          QueryServer::Query q;
          QueryServer::QueryConfigurator configurator;
          configurator.configure(q, attributes);
          q.mCoordinateType = T::CoordinateTypeValue::LATLON_COORDINATES;
          T::Coordinate_vec point;
          point.emplace_back(lon, lat);
          q.mAreaCoordinates.push_back(point);
          BOOST_TEST_REQUIRE(world->queryServer.executeQuery(0, q) == 0);

          double value = ParamValueMissing;
          for (const auto &qp : q.mQueryParameterList)
          {
            T::GridValue gv;
            if (qp.mParam == param && !qp.mValueList.empty() &&
                qp.mValueList[0]->mValueList.getGridValueByIndex(0, gv))
              value = gv.mValue;
          }
          return value;
        };

        const std::string nwc = "Temperature:pal_skandinavia:5900";
        const std::string fc = "Temperature:ecmwf_eurooppa_pinta:5903";
        const std::string valid = "VALID{" + nwc + ";" + fc + "}";

        // Both have data: the first one
        const double helsinki = run(nwc, "20080805T060000", LAT1, LON1);
        BOOST_TEST_REQUIRE(helsinki != ParamValueMissing);
        BOOST_TEST(close(run(valid, "20080805T060000", LAT1, LON1), helsinki));

        // The first one has no data for the time: the second one
        BOOST_TEST_REQUIRE(run(nwc, "20080805T000000", LAT1, LON1) == ParamValueMissing);
        const double ec = run(fc, "20080805T000000", LAT1, LON1);
        BOOST_TEST_REQUIRE(ec != ParamValueMissing);
        BOOST_TEST(close(run(valid, "20080805T000000", LAT1, LON1), ec));

        // The point is outside the grid of the first one: the second one
        BOOST_TEST_REQUIRE(run(nwc, "20080805T060000", 40.0, 10.0) == ParamValueMissing);
        const double south = run(fc, "20080805T060000", 40.0, 10.0);
        BOOST_TEST_REQUIRE(south != ParamValueMissing);
        BOOST_TEST(close(run(valid, "20080805T060000", 40.0, 10.0), south));

        // The first one has a missing value inside its grid: missing. (Multiplying by the
        // missing value gives a pal parameter which is missing everywhere.)
        const std::string none = "MUL{" + nwc + ";-16777216}";
        BOOST_TEST_REQUIRE(run(none, "20080805T060000", LAT1, LON1) == ParamValueMissing);
        BOOST_TEST(run("VALID{" + none + ";" + fc + "}", "20080805T060000", LAT1, LON1) == ParamValueMissing);
      });
}


BOOST_AUTO_TEST_CASE(forecast_without_a_type_is_the_control_forecast)
{
  // An ensemble queried without a forecast type gives the control forecast, and all its values
  // come from it: the control forecast has no 06 UTC step, which is not in its data times and is
  // interpolated from it at a given time instead of taken from a perturbed member. (The perturbed
  // members sort first, so the first content of each time used to be member 1.) The forecasts
  // have constant values: the control forecast 4, the members their number
  // (ensemble/make_ensemble.sh).
  const std::vector<std::string> files = {"ensemble/member1.grib", "ensemble/member2.grib", "ensemble/control.grib"};
  for (const auto &file : files)
    requireFixture(file);
  withFmiErrors(
      [&]
      {
        auto world = std::make_unique<GridWorld>();
        world->addProducer(PRODUCER, "20080805T050000", files);
        world->start();

        auto run = [&](const std::string &param)
        {
          T::AttributeList attributes;
          attributes.addAttribute("starttime", "20080805T050000");
          attributes.addAttribute("endtime", "20080805T070000");
          attributes.addAttribute("timestep", "data");
          attributes.addAttribute("producer", PRODUCER);
          attributes.addAttribute("param", param);

          QueryServer::Query q;
          QueryServer::QueryConfigurator configurator;
          configurator.configure(q, attributes);
          q.mCoordinateType = T::CoordinateTypeValue::LATLON_COORDINATES;
          T::Coordinate_vec point;
          point.emplace_back(LON1, LAT1);
          q.mAreaCoordinates.push_back(point);
          BOOST_TEST_REQUIRE(world->queryServer.executeQuery(0, q) == 0);

          std::vector<double> values;
          for (const auto &qp : q.mQueryParameterList)
          {
            if (qp.mParam != param)
              continue;
            for (const auto &v : qp.mValueList)
            {
              T::GridValue gv;
              values.push_back(v->mValueList.getGridValueByIndex(0, gv) ? gv.mValue : ParamValueMissing);
            }
          }
          return values;
        };

        auto point = [&](const std::string &param, const std::string &time)
        {
          T::ParamValue value = ParamValueMissing;
          world->queryServer.getParameterValueByPointAndTime(0,
                                                             PRODUCER,
                                                             param,
                                                             T::CoordinateTypeValue::LATLON_COORDINATES,
                                                             LON1,
                                                             LAT1,
                                                             time,
                                                             T::AreaInterpolationMethod::Linear,
                                                             T::TimeInterpolationMethod::Linear,
                                                             T::LevelInterpolationMethod::Linear,
                                                             value);
          return value;
        };

        const std::string param = "Temperature:" + PRODUCER;
        const std::vector<double> control = {4, 4};
        const std::vector<double> member1 = {1, 1, 1};
        const std::vector<double> member2 = {2, 2, 2};

        BOOST_TEST(run(param) == control, boost::test_tools::per_element());
        for (const char *time : {"20080805T050000", "20080805T060000", "20080805T070000"})
          BOOST_TEST(point(param, time) == 4.0, time);

        // An explicit forecast is used as such
        BOOST_TEST(run(param + "::::4") == control, boost::test_tools::per_element());
        BOOST_TEST(run(param + "::::3:2") == member2, boost::test_tools::per_element());
        BOOST_TEST(point(param + "::::3:2", "20080805T060000") == 2.0);

        // A perturbed forecast without a number is the first member
        BOOST_TEST(run(param + "::::3") == member1, boost::test_tools::per_element());
      });
}

BOOST_AUTO_TEST_SUITE_END()
