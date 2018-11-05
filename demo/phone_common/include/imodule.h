/**
* imodule.h
* this file define the IModule interface
*
* @version 1.0 (08 ,10, 2009)
* @author ken
*
*/

#pragma  once


struct IModuleManager;

struct IModule
{
  virtual void Load(IModuleManager *) = 0;
  virtual void Unload() = 0;
  virtual wchar_t const * const GetModuleName() = 0;

};

struct IModuleFactory
{
  //uint32参数表明本工厂将生成的IModule接口数目。比如1，2等
  virtual bool QueryIModuleCounter(uint32 & )=0;
  //uint32参数表明后面buf的维度。如果是2，表明后面buf的维度是2
  //IModule指向一个IModule buf的开始位置，buf的内容由实现者填充
  virtual bool QueryIModulePoint(uint32 counter,IModule * & )=0;
  //内容同QueryIModulePoint
  virtual void ReleaseIModulePoint(uint32 counter,IModule * )=0;
};

