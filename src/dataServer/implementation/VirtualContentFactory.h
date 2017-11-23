#pragma once

#include "contentServer/definition/ServiceInterface.h"
#include "lua/LuaFileCollection.h"
#include "functions/FunctionCollection.h"
#include "GridFileManager.h"

#include <grid-files/grid/VirtualGridFile.h>


namespace SmartMet
{
namespace DataServer
{


class VirtualContentFactory
{
  public:
                   VirtualContentFactory();
    virtual       ~VirtualContentFactory();

    void          setContentServer(ContentServer::ServiceInterface* contentServer);
    void          setGridFileManager(GridFileManager* GridFileManager);
    void          setLuaFileCollection(Lua::LuaFileCollection *luaFileCollection);
    void          setFunctionCollection(Functions::FunctionCollection *functionCollection);

    virtual void  addFile(T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList);

  protected:

    virtual void addContent(T::FileInfo& fileInfo,T::ContentInfo& contentInfo);

  protected:

    ContentServer::ServiceInterface* mContentServer;
    GridFileManager*                 mGridFileManager;
    Lua::LuaFileCollection*          mLuaFileCollection;
    Functions::FunctionCollection*   mFunctionCollection;
};


typedef VirtualContentFactory* VirtualContentFactory_ptr;


}
}
