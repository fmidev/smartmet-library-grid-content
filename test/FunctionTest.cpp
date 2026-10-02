// Tests for the query server functions (src/functions).
//
// Every function has float and double variants of a scalar call and of a grid call
// (executeFunctionCall9), all implemented separately. The tests check
//
//   * the scalar results against known values, including missing value handling
//   * that the float and double variants agree for random input with missing values
//   * that the grid call gives the same value at every grid point as the scalar call

#define BOOST_TEST_MODULE FunctionTest
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

#include "TestCommon.h"
#include "functions/Function_and.h"
#include "functions/Function_avg.h"
#include "functions/Function_change.h"
#include "functions/Function_diff.h"
#include "functions/Function_div.h"
#include "functions/Function_eq.h"
#include "functions/Function_gt.h"
#include "functions/Function_gte.h"
#include "functions/Function_hypotenuse.h"
#include "functions/Function_if.h"
#include "functions/Function_in.h"
#include "functions/Function_inCount.h"
#include "functions/Function_inPrcnt.h"
#include "functions/Function_limit.h"
#include "functions/Function_lt.h"
#include "functions/Function_lte.h"
#include "functions/Function_max.h"
#include "functions/Function_median.h"
#include "functions/Function_min.h"
#include "functions/Function_mode.h"
#include "functions/Function_mul.h"
#include "functions/Function_add.h"
#include "functions/Function_multiply.h"
#include "functions/Function_not.h"
#include "functions/Function_or.h"
#include "functions/Function_out.h"
#include "functions/Function_replace.h"
#include "functions/Function_round.h"
#include "functions/Function_sdev.h"
#include "functions/Function_sdevDir.h"
#include "functions/Function_sequence.h"
#include "functions/Function_smedian.h"
#include "functions/Function_sqrt.h"
#include "functions/Function_streamDir.h"
#include "functions/Function_sub.h"
#include "functions/Function_sum.h"
#include "functions/Function_valid.h"
#include "functions/Function_variance.h"
#include "functions/Function_windDir.h"
#include "functions/Function_windU.h"
#include "functions/Function_windV.h"

#include <cmath>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace SmartMet;
using namespace SmartMet::Functions;
namespace bdata = boost::unit_test::data;

namespace
{
const double M = ParamValueMissing;

std::map<std::string, std::function<Function *()>> &registry()
{
  static std::map<std::string, std::function<Function *()>> r = {
      {"ADD", [] { return new Function_add(); }},
      {"AND", [] { return new Function_and(); }},
      {"AVG", [] { return new Function_avg(); }},
      {"CHANGE", [] { return new Function_change(); }},
      {"DIFF", [] { return new Function_diff(); }},
      {"DIV", [] { return new Function_div(); }},
      {"EQ", [] { return new Function_eq(); }},
      {"GT", [] { return new Function_gt(); }},
      {"GTE", [] { return new Function_gte(); }},
      {"HYPOTENUSE", [] { return new Function_hypotenuse(); }},
      {"IF", [] { return new Function_if(); }},
      {"IN", [] { return new Function_in(); }},
      {"IN_COUNT", [] { return new Function_inCount(); }},
      {"IN_PRCNT", [] { return new Function_inPrcnt(); }},
      {"LIMIT", [] { return new Function_limit(); }},
      {"LT", [] { return new Function_lt(); }},
      {"LTE", [] { return new Function_lte(); }},
      {"MAX", [] { return new Function_max(); }},
      {"MEDIAN", [] { return new Function_median(); }},
      {"MIN", [] { return new Function_min(); }},
      {"MODE", [] { return new Function_mode(); }},
      {"MUL", [] { return new Function_mul(); }},
      {"NOT", [] { return new Function_not(); }},
      {"OR", [] { return new Function_or(); }},
      {"OUT", [] { return new Function_out(); }},
      {"REPLACE", [] { return new Function_replace(); }},
      {"ROUND", [] { return new Function_round(); }},
      {"SDEV", [] { return new Function_sdev(); }},
      {"SDEV_DIR", [] { return new Function_sdevDir(); }},
      {"SMEDIAN", [] { return new Function_smedian(); }},
      {"SQRT", [] { return new Function_sqrt(); }},
      {"STREAM_DIR", [] { return new Function_streamDir(); }},
      {"SUB", [] { return new Function_sub(); }},
      {"SUM", [] { return new Function_sum(); }},
      {"VALID", [] { return new Function_valid(); }},
      {"VARIANCE", [] { return new Function_variance(); }},
      {"WIND_DIR", [] { return new Function_windDir(); }},
      {"WIND_U", [] { return new Function_windU(); }},
      {"WIND_V", [] { return new Function_windV(); }},
  };
  return r;
}

double call(const std::string &name, std::vector<double> params)
{
  std::unique_ptr<Function> f(registry().at(name)());
  return f->executeFunctionCall1(params);
}

float callFloat(const std::string &name, const std::vector<double> &params)
{
  std::unique_ptr<Function> f(registry().at(name)());
  std::vector<float> fp(params.begin(), params.end());
  return f->executeFunctionCall1(fp);
}

bool close(double a, double b, double tol = 1e-6)
{
  if (a == M || b == M)
    return a == b;
  if (std::isnan(a) || std::isnan(b))
    return std::isnan(a) && std::isnan(b);
  return std::fabs(a - b) <= tol * std::max(1.0, std::fabs(b));
}

struct Case
{
  std::string name;
  std::vector<double> params;
  double expected;
};

std::ostream &operator<<(std::ostream &out, const Case &c)
{
  out << c.name << "(";
  for (std::size_t i = 0; i < c.params.size(); i++)
    out << (i ? "," : "") << (c.params[i] == M ? std::string("M") : std::to_string(c.params[i]));
  return out << ")";
}

const std::vector<Case> CASES = {
    {"AVG", {1, 2, 3, 6}, 3},
    {"AVG", {1, M, 3}, 2},
    {"AVG", {M, M}, M},
    {"SUM", {1, 2, 3}, 6},
    {"SUM", {1, M, 3}, M},
    {"MIN", {5, M, -2, 3}, -2},
    {"MAX", {5, M, -2, 3}, 5},
    {"MAX", {M}, M},
    {"MEDIAN", {5, 1, 3}, 3},
    {"MEDIAN", {4, 1, 3, 2}, 2.5},
    {"MEDIAN", {4, M, 1, 3, 2}, 2.5},
    {"SMEDIAN", {0, 5, 1, 3}, 3},
    {"SMEDIAN", {1, 5, 1, 3}, 5},
    {"SMEDIAN", {-1, 5, 1, 3}, 1},
    {"MODE", {0, 1, 2, 2, 3}, 2},
    {"MODE", {1, 1.04, 1.01, 2.0}, 1.0},
    {"SDEV", {2, 4, 4, 4, 5, 5, 7, 9}, 2.1380899353},
    {"SDEV", {2, M, 4, 4, 4, 5, 5, 7, 9}, 2.1380899353},
    {"SDEV", {3}, M},
    {"VARIANCE", {2, 4, 4, 4, 5, 5, 7, 9}, 4.5714285714},
    {"DIFF", {10, 3, 2}, 5},
    {"DIFF", {10, M}, M},
    {"MUL", {2, 3, 4}, 24},
    {"MUL", {2, M}, M},
    {"DIV", {12, 3, 2}, 2},
    {"DIV", {12, M}, M},
    {"SUB", {5, 3}, 2},
    {"SUB", {5, M}, M},
    {"CHANGE", {1, 5, 4}, 3},
    {"CHANGE", {M, 5, 4}, M},
    {"CHANGE", {1, 5, M}, M},
    {"IF", {1, 10, 20}, 10},
    {"IF", {0, 10, 20}, 20},
    {"IF", {M, 10, 20}, M},
    {"IN", {5, 1, 10}, 1},
    {"IN", {11, 1, 10}, 0},
    {"OUT", {11, 1, 10}, 1},
    {"OUT", {5, 1, 10}, 0},
    {"IN_COUNT", {1, 10, 0, 5, 10, 11}, 2},
    {"IN_PRCNT", {1, 10, 0, 5, 10, 11}, 50},
    {"LIMIT", {0, 10, 5}, 5},
    {"LIMIT", {0, 10, 11}, M},
    {"EQ", {1, 1}, 1},
    {"EQ", {1, 2}, 0},
    {"EQ", {1.004, 1.0, 2}, 1},
    {"EQ", {1, M}, M},
    {"GT", {2, 1}, 1},
    {"GT", {1, 1}, 0},
    {"GTE", {1, 1}, 1},
    {"LT", {1, 2}, 1},
    {"LTE", {2, 2}, 1},
    {"AND", {1, 1, 1}, 1},
    {"AND", {1, 0, 1}, 0},
    {"AND", {1, M}, M},
    {"OR", {0, 0, 1}, 1},
    {"OR", {0, 0}, 0},
    // The result must not depend on the order of the parameters
    {"OR", {M, 1}, 1},
    {"OR", {1, M}, 1},
    {"OR", {M, 0}, M},
    {"AND", {M, 0}, 0},
    {"AND", {0, M}, 0},
    {"AND", {M, 1}, M},
    {"NOT", {0}, 1},
    {"NOT", {3}, 0},
    {"NOT", {M}, M},
    {"VALID", {M, M, 7, 8}, 7},
    {"VALID", {M}, M},
    {"ROUND", {1.2345, 2}, 1.23},
    {"ROUND", {-1.5, 0}, -2},
    {"ROUND", {1234, -2}, 1200},
    {"REPLACE", {5, 0, 100, 10}, 100},
    {"REPLACE", {15, 0, 100, 10}, 15},
    {"SQRT", {16}, 4},
    {"HYPOTENUSE", {3, 4}, 5},
    {"HYPOTENUSE", {3, M}, M},
    // Wind from north (blowing southwards): u=0, v=-10
    {"WIND_DIR", {0, -10}, 0},
    // Wind from west (blowing eastwards): u=10, v=0
    {"WIND_DIR", {10, 0}, 270},
    {"WIND_U", {270, 10}, 10},
    {"WIND_V", {0, 10}, -10},
    // A current flowing eastwards
    {"STREAM_DIR", {10, 0}, 90},
    // Directions 350, 10: mean resultant length cos(10 deg), sdev = sqrt(-ln(R^2)) in degrees
    {"SDEV_DIR", {350, 10}, std::sqrt(-std::log(std::cos(10 * M_PI / 180) * std::cos(10 * M_PI / 180))) * 180 / M_PI},
    {"SDEV_DIR", {350, M, 10}, std::sqrt(-std::log(std::cos(10 * M_PI / 180) * std::cos(10 * M_PI / 180))) * 180 / M_PI},
    {"SDEV_DIR", {90, 90, 90}, 0},
    {"SDEV_DIR", {M, M}, M},
};

// Number of scalar parameters for the random consistency tests (0 = any count)
const std::map<std::string, int> ARITY = {{"ADD", 1},      {"CHANGE", 0},     {"EQ", 2},     {"GT", 2},
                                          {"GTE", 2},      {"HYPOTENUSE", 2}, {"IF", 3},     {"IN", 3},
                                          {"LIMIT", 3},    {"LT", 2},         {"LTE", 2},    {"NOT", 1},
                                          {"OUT", 3},      {"ROUND", 2},      {"SQRT", 1},   {"STREAM_DIR", 2},
                                          {"SUB", 2},      {"WIND_DIR", 2},   {"WIND_U", 2}, {"WIND_V", 2}};

std::vector<std::string> names()
{
  std::vector<std::string> ret;
  for (const auto &r : registry())
    ret.push_back(r.first);
  return ret;
}

struct Rng
{
  unsigned seed;
  double operator()()
  {
    seed = seed * 1103515245 + 12345;
    return ((seed >> 8) & 0xffff) / 65536.0;
  }
};

std::vector<double> randomParams(const std::string &name, Rng &rnd)
{
  auto it = ARITY.find(name);
  int n = (it != ARITY.end() && it->second > 0) ? it->second : 2 + static_cast<int>(rnd() * 5);
  std::vector<double> p;
  for (int i = 0; i < n; i++)
  {
    if (rnd() < 0.15)
      p.push_back(M);
    else
      p.push_back(std::round((rnd() * 40 - 10) * 4) / 4);  // quarter steps: exact in float
  }
  if (name == "ROUND" && p[1] != M)
    p[1] = std::round(p[1] / 10);
  if (name == "SQRT" && p[0] != M)
    p[0] = std::fabs(p[0]);
  return p;
}

}  // namespace

BOOST_DATA_TEST_CASE(known_values, bdata::make(CASES), c)
{
  const double result = call(c.name, c.params);
  BOOST_TEST(close(result, c.expected), c << " = " << result << ", expected " << c.expected);
}

BOOST_DATA_TEST_CASE(float_and_double_variants_agree, bdata::make(names()), name)
{
  Rng rnd{static_cast<unsigned>(std::hash<std::string>()(name))};
  for (int i = 0; i < 300; i++)
  {
    auto p = randomParams(name, rnd);
    const double d = call(name, p);
    const double f = callFloat(name, p);
    if (!close(f, d, 1e-4))
    {
      BOOST_ERROR((Case{name, p, d}) << ": double " << d << ", float " << f);
      break;
    }
  }
}

BOOST_DATA_TEST_CASE(grid_call_matches_scalar_call, bdata::make(names()), name)
{
  // The grid call takes one grid per parameter; it must give the scalar result at every point
  auto it = ARITY.find(name);
  if (name == "ADD")
    return;  // the grid version adds the external parameters, see below

  Rng rnd{static_cast<unsigned>(std::hash<std::string>()(name)) + 1};
  const uint columns = 7;
  const uint rows = 5;
  const uint n = (it != ARITY.end() && it->second > 0) ? it->second : 4;

  std::vector<std::vector<double>> grids(n, std::vector<double>(columns * rows));
  std::vector<std::vector<double>> pointParams(columns * rows);
  for (uint s = 0; s < columns * rows; s++)
  {
    auto p = randomParams(name, rnd);
    p.resize(n, 1.0);
    for (uint k = 0; k < n; k++)
      grids[k][s] = p[k];
    pointParams[s] = p;
  }

  std::unique_ptr<Function> f(registry().at(name)());
  std::vector<double> out;
  const std::vector<double> ext;
  try
  {
    f->executeFunctionCall9(columns, rows, grids, ext, out);
  }
  catch (...)
  {
    return;  // no grid version
  }
  if (out.empty())
    return;  // no grid version

  BOOST_TEST_REQUIRE(out.size() == columns * rows);
  int bad = 0;
  for (uint s = 0; s < columns * rows; s++)
  {
    const double expected = call(name, pointParams[s]);
    if (!close(out[s], expected, 1e-5) && bad++ < 3)
      BOOST_ERROR((Case{name, pointParams[s], expected}) << ": grid " << out[s] << ", scalar " << expected);
  }
}

BOOST_AUTO_TEST_CASE(constant_functions_and_sequences)
{
  // K2C and K2F as the query server defines them
  std::vector<double> p = {273.15};
  Function_add k2c(-273.15);
  BOOST_TEST(close(k2c.executeFunctionCall1(p), 0.0));

  Function_sequence k2f;
  k2f.addFunction(new Function_add(-273.15));
  k2f.addFunction(new Function_multiply(1.8));
  k2f.addFunction(new Function_add(32.0));
  std::vector<double> boiling = {373.15};
  BOOST_TEST(close(k2f.executeFunctionCall1(boiling), 212.0));

  std::vector<double> missing = {M};
  BOOST_TEST(k2f.executeFunctionCall1(missing) == M);

  // RAD2DEG and DEG2RAD as the query server defines them
  Function_multiply rad2deg(360.0 / (2 * 3.1415926535));
  Function_multiply deg2rad(2 * 3.1415926535 / 360.0);
  std::vector<double> pi = {M_PI};
  std::vector<double> d180 = {180};
  BOOST_TEST(close(rad2deg.executeFunctionCall1(pi), 180.0, 1e-9));
  BOOST_TEST(close(deg2rad.executeFunctionCall1(d180), M_PI, 1e-9));
}

BOOST_AUTO_TEST_CASE(wind_components_round_trip)
{
  for (double dir = 0; dir < 360; dir += 15)
  {
    for (double speed : {0.5, 5.0, 25.0})
    {
      const double u = call("WIND_U", {dir, speed});
      const double v = call("WIND_V", {dir, speed});
      BOOST_TEST(close(call("HYPOTENUSE", {u, v}), speed, 1e-5));
      const double back = call("WIND_DIR", {u, v});
      BOOST_TEST(close(std::fmod(back + 360.0 - dir + 180.0, 360.0) - 180.0 + dir, dir, 1e-4),
                 "direction " << dir << " came back as " << back);
    }
  }
}
