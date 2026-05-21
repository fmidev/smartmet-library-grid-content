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

    void        configure(Query& query,T::AttributeList& attributeList);

  protected:

    void        configureProducer(Query& query,T::AttributeList& attributeList);
    void        configureGeneration(Query& query,T::AttributeList& attributeList);
    void        configureTime(Query& query,T::AttributeList& attributeList);
    void        configureLocation(Query& query,T::AttributeList& attributeList);
    void        configureMisc(Query& query,T::AttributeList& attributeList);
    void        configureParameters(Query& query,T::AttributeList& attributeList);
    void        configureParameter(Query& query,QueryParameter& param,T::AttributeList& attributeList);
};


}  // namespace QueryServer
}  // namespace SmartMet

