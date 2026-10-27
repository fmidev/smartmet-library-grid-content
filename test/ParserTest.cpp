// Tests for the query server configuration file parsers: parameter mapping files and alias files.

#define BOOST_TEST_MODULE ParserTest
#include <boost/test/included/unit_test.hpp>

#include "TestCommon.h"
#include "queryServer/definition/AliasFile.h"
#include "queryServer/definition/ParameterMappingFile.h"

#include <string>

using namespace SmartMet;
using namespace SmartMet::QueryServer;
using namespace GridTest;

BOOST_AUTO_TEST_CASE(parameter_mapping_file)
{
  TempFile file(
      "# comment line\n"
      // 16 fields: everything given, a conversion function with a quoted semicolon
      "ECMWF;Temperature;2;T-K;1008;1;6;2;1;1;1;0;E;\"SUM{$;-273.15}\";SUM{$,273.15};1;\n"
      // exactly 13 fields: no conversion functions or precision
      "ECMWF;Pressure;2;P-PA;1008;1;1;0;2;1;1;0;D\n"
      // same name for another geometry
      "ECMWF;Temperature;2;T-K;1009;1;6;2;1;1;1;0;E;;;1;\n"
      // too few fields: ignored
      "ECMWF;Broken;2;X\n"
      // ignored mapping
      "ECMWF;Ignored;2;T-K;1008;1;6;2;1;1;1;0;I;;;1;\n",
      "mapping");

  withFmiErrors(
      [&]
      {
        ParameterMappingFile mappings(file.name());
        mappings.init();

        ParameterMapping_vec vec;
        mappings.getMappings("ECMWF", "Temperature", 1008, false, vec);
        BOOST_TEST_REQUIRE(vec.size() == 1U);
        BOOST_TEST(vec[0].mParameterKey == "T-K");
        BOOST_TEST(vec[0].mParameterLevelId == 6);
        BOOST_TEST(vec[0].mParameterLevel == 2);
        BOOST_TEST(vec[0].mSearchEnabled);
        BOOST_TEST(vec[0].mConversionFunction == "\"SUM{$;-273.15}\"");
        BOOST_TEST(vec[0].mReverseConversionFunction == "SUM{$,273.15}");
        BOOST_TEST(vec[0].mDefaultPrecision == 1);

        // Lookups are case insensitive
        ParameterMapping_vec vec2;
        mappings.getMappings("ecmwf", "TEMPERATURE", 1008, false, vec2);
        BOOST_TEST(vec2.size() == 1U);

        // All geometries
        ParameterMapping_vec all;
        mappings.getMappings("ECMWF", "Temperature", false, all);
        BOOST_TEST(all.size() == 2U);

        ParameterMapping_vec pressure;
        mappings.getMappings("ECMWF", "Pressure", 1008, false, pressure);
        BOOST_TEST_REQUIRE(pressure.size() == 1U);
        BOOST_TEST(!pressure[0].mSearchEnabled);
        BOOST_TEST(pressure[0].mConversionFunction.empty());
        BOOST_TEST(pressure[0].mAreaInterpolationMethod == 2);

        // Search enabled mappings only
        ParameterMapping_vec enabled;
        mappings.getMappings("ECMWF", "Pressure", 1008, true, enabled);
        BOOST_TEST(enabled.empty());

        ParameterMapping_vec broken;
        mappings.getMappings("ECMWF", "Broken", false, broken);
        BOOST_TEST(broken.empty());

        // Ignored mappings are never returned
        ParameterMapping_vec ignored;
        mappings.getMappings("ECMWF", "Ignored", 1008, false, ignored);
        BOOST_TEST(ignored.empty());

        // Reverse lookup by the parameter key
        ParameterMapping_vec byKey;
        mappings.getMappingsByParamKey("ECMWF", T::ParamKeyTypeValue::FMI_NAME, "P-PA", 1008, 1, 0, byKey);
        BOOST_TEST(byKey.size() == 1U);
      });
}

BOOST_AUTO_TEST_CASE(missing_mapping_file)
{
  // A missing file is tolerated (automatically generated mapping files may appear later and
  // are picked up by checkUpdates), it just has no mappings
  ParameterMappingFile mappings("/nonexistent/mapping.csv");
  BOOST_CHECK_NO_THROW(mappings.init());
  BOOST_TEST(mappings.getNumberOfMappings() == 0U);
}

BOOST_AUTO_TEST_CASE(alias_file)
{
  TempFile file(
      "# comment\n"
      "TempK:T-K\n"
      "TempC:K2C{TempK}\n"
      "Long:SUM{\\\n"
      "TempK;\\\n"
      "TempC}\n"
      "Spaces:  X  \n",
      "alias");

  withFmiErrors(
      [&]
      {
        AliasFile aliases;
        aliases.init(file.name());

        std::string value;
        BOOST_TEST(aliases.getAlias("TempK", value));
        BOOST_TEST(value == "T-K");
        BOOST_TEST(aliases.getAlias("TempC", value));
        BOOST_TEST(value == "K2C{TempK}");
        BOOST_TEST(!aliases.getAlias("NoSuchAlias", value));

        // Continuation lines are joined
        BOOST_TEST(aliases.getAlias("Long", value));
        BOOST_TEST(value.find("TempK") != std::string::npos);
        BOOST_TEST(value.find("TempC}") != std::string::npos);
        BOOST_TEST(value.find('\\') == std::string::npos);
      });
}

BOOST_AUTO_TEST_CASE(very_long_continued_alias_does_not_overflow)
{
  // Continued lines longer than the line buffer used to overflow it
  std::string text = "Long:A";
  for (int i = 0; i < 4000; i++)
    text += "1234567890\\\n";
  text += "B\nShort:S\n";
  TempFile file(text, "alias");

  withFmiErrors(
      [&]
      {
        AliasFile aliases;
        aliases.init(file.name());
        std::string value;
        aliases.getAlias("Long", value);
        BOOST_TEST(!value.empty());
      });
}
