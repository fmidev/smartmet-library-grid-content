#pragma once

#include "GridFileManager.h"
#include "../../contentServer/definition/ServiceInterface.h"
#include "../../lua/LuaFileCollection.h"
#include "../../functions/FunctionCollection.h"

#include <grid-files/grid/VirtualGridFile.h>


namespace SmartMet
{
namespace DataServer
{

typedef std::unordered_map<std::string,GRID::VirtualGridFilePtr> VirtualGridFilePtr_map;



class VirtualContentFactory
{
  public:
                   VirtualContentFactory();
    virtual       ~VirtualContentFactory();

    void          setContentServer(ContentServer::ServiceInterface* contentServer);
    void          setGridFileManager(GridFileManager* GridFileManager);
    void          setLuaFileCollection(Lua::LuaFileCollection *luaFileCollection);
    void          setFunctionCollection(Functions::FunctionCollection *functionCollection);

    virtual void  addFile(T::ProducerInfo& producerInfo,T::GenerationInfo& generationInfo,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList,VirtualGridFilePtr_map& gridFileMap);
    virtual bool  checkUpdates();

  protected:

    virtual void  addContent(T::ProducerInfo& producerInfo,T::GenerationInfo& generationInfo,T::FileInfo& fileInfo,T::ContentInfo& contentInfo,VirtualGridFilePtr_map& gridFileMap);

  protected:

    ContentServer::ServiceInterface* mContentServer;
    GridFileManager*                 mGridFileManager;
    Lua::LuaFileCollection*          mLuaFileCollection;
    Functions::FunctionCollection*   mFunctionCollection;
};


typedef VirtualContentFactory* VirtualContentFactory_ptr;



}
}
