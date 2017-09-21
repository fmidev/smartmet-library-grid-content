#pragma once

#include "GridValue.h"
#include <vector>


namespace SmartMet
{
namespace T
{


class GridValueList
{
  public:
                  GridValueList();
                  GridValueList(GridValueList& gridValueList);
     virtual      ~GridValueList();

     void         operator=(GridValueList& gridValueList);

     void         addGridValue(GridValue *gridValue);
     void         clear();
     GridValue*   getGridValueByIndex(uint index);
     uint         getLength();
     void         print(std::ostream& stream,uint level,uint optionFlags);

  protected:

     std::vector<GridValue*>  mList;
};


}
}
