/**
* IModule.h
* this file define the IModule interface
*
* @version 1.0 (08 ,10, 2009)
* @author ken
*
*/



#pragma  once

#include "imodule.h"

#include "base/logging.h"


class ModuleImpl : public IModule
{
public:
  ModuleImpl():m_pstIModuleManager(NULL){}
  ~ModuleImpl()
  {
    if(m_pstIModuleManager!=NULL)
      DCHECK(false);
  }
  virtual void Load(IModuleManager *p )
  {
    if(m_pstIModuleManager==NULL && p!=NULL)
      m_pstIModuleManager=p;
    else
      DCHECK(false);
  }
  virtual void Unload()
  {
    m_pstIModuleManager=NULL;
  }
  virtual  wchar_t const * const  GetModuleName()
  {
    return L"ModuleImpl_Example";
  }

/*
  virtual int32 Invoke(Param const value, Param lparam)
  {
    return -1;
  }*/


protected:
  IModuleManager * m_pstIModuleManager;
};


template <typename T>
class ModuleFactoryImpl :public  IModuleFactory
{
public:
  ModuleFactoryImpl():m_pstIModule(NULL){}
  ~ModuleFactoryImpl()
  {
    if(m_pstIModule!=NULL)
      DCHECK(false);
  }
  bool QueryIModuleCounter(uint32 & counter)
  {
    counter=1;
    return true;
  }
  bool QueryIModulePoint(uint32 counter,IModule * & pIModule)
  {
    if(counter==1 )
    {
      if(m_pstIModule)
      {
        pIModule=m_pstIModule;
        return true;
      }
      else
      {
        m_pstIModule=new T;
        //pIModule= new T;
        //m_pstIModule=static_cast<IModule*>pIModule;
        pIModule =m_pstIModule;
        return true;
      }
    }
    else 
      return false;
  }
  void   ReleaseIModulePoint(uint32 counter,IModule * pIModule)
  {
    if(counter==1 && pIModule!=NULL)
    {
      if(m_pstIModule==pIModule)
      {
        delete m_pstIModule;
        m_pstIModule=NULL;
      }
      else
        DCHECK(false);
    }
    else 
      DCHECK(false);
  }
private:
  T * m_pstIModule;
};