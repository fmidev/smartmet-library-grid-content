#pragma once

#include "Query.h"
#include <grid-files/common/AttributeList.h>


namespace SmartMet
{
namespace QueryServer
{

// ====================================================================================
/*! \brief Populates a Query object from a flat AttributeList of key-value pairs.
 *
 *  Translates HTTP-style request attributes (producer, time range, location, etc.)
 *  into the structured fields of a Query and its QueryParameter list.  Used by
 *  plugins to translate incoming HTTP requests into QueryServer calls. */
// ====================================================================================

class QueryConfigurator
{
  public:
                QueryConfigurator();
    virtual     ~QueryConfigurator();
    QueryConfigurator(const QueryConfigurator&) = default;
    QueryConfigurator& operator=(const QueryConfigurator&) = default;

    void        configure(Query& query,T::AttributeList& attributeList);

  protected:

    void        configureProducer(Query& query,const T::AttributeList& attributeList);
    void        configureGeneration(Query& query,const T::AttributeList& attributeList);
    void        configureTime(Query& query,const T::AttributeList& attributeList);
    void        configureLocation(Query& query,const T::AttributeList& attributeList);
    void        configureMisc(Query& query,const T::AttributeList& attributeList);
    void        configureParameters(Query& query,const T::AttributeList& attributeList);
    void        configureParameter(const Query& query,QueryParameter& param,const T::AttributeList& attributeList);
};


}  // namespace QueryServer
}  // namespace SmartMet

