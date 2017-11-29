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

typedef std::map<std::string,GRID::VirtualGridFilePtr> VirtualGridFilePtr_map;



class VirtualContentFactory
{
  public:
                   VirtualContentFactory();
    virtual       ~VirtualContentFactory();

    void          setContentServer(ContentServer::ServiceInterface* contentServer);
    void          setGridFileManager(GridFileManager* GridFileManager);
    void          setLuaFileCollection(Lua::LuaFileCollection *luaFileCollection);
    void          setFunctionCollection(Functions::FunctionCollection *functionCollection);

    virtual void  addFile(T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList,VirtualGridFilePtr_map& gridFileMap);

  protected:

    virtual void addContent(T::FileInfo& fileInfo,T::ContentInfo& contentInfo,VirtualGridFilePtr_map& gridFileMap);

  protected:

    ContentServer::ServiceInterface* mContentServer;
    GridFileManager*                 mGridFileManager;
    Lua::LuaFileCollection*          mLuaFileCollection;
    Functions::FunctionCollection*   mFunctionCollection;
};


typedef VirtualContentFactory* VirtualContentFactory_ptr;



}
}
