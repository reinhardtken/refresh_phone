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
  //uint32�������������������ɵ�IModule�ӿ���Ŀ������1��2��
  virtual bool QueryIModuleCounter(uint32 & )=0;
  //uint32������������buf��ά�ȡ������2����������buf��ά����2
  //IModuleָ��һ��IModule buf�Ŀ�ʼλ�ã�buf��������ʵ�������
  virtual bool QueryIModulePoint(uint32 counter,IModule * & )=0;
  //����ͬQueryIModulePoint
  virtual void ReleaseIModulePoint(uint32 counter,IModule * )=0;
};

