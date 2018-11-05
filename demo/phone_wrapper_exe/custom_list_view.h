#pragma once


#include "../phone_common/include/ctp_define.h"

class MyListView : public CWindowImpl<MyListView, CListViewCtrl>,
                   public CCustomDraw<MyListView>                   
{
public:

  //  [7/3/2013 ken] not familiar with wtl ,just want it work ok... begin
  BOOL m_bHandledCD;

  BOOL IsMsgHandled() const
  {
    return m_bHandledCD;
  }

  void SetMsgHandled(BOOL bHandled)
  {
    m_bHandledCD = bHandled;
  }
  //  [7/3/2013 ken] not familiar with wtl ,just want it work ok... end

  BEGIN_MSG_MAP(MyListView)    
    CHAIN_MSG_MAP(CCustomDraw<MyListView>)
	END_MSG_MAP()          	

	DWORD OnPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/)
	{		
    return  CDRF_NOTIFYITEMDRAW;
	}

 	DWORD OnItemPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
	{
      NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( lpNMCustomDraw );    

     // This is the prepaint stage for an item. Here's where we set the
     // item's text color. Our return value will tell Windows to draw the
     // item itself, but it will use the new color we set here for the background     

      uint32 type = GetItemData(pLVCD->nmcd.dwItemSpec);

     if (type == phone_module::NORMAL) {
       pLVCD->clrText = RGB(0,0,0);  
     } else if (type == phone_module::UIERROR) {
       pLVCD->clrText = RGB(255,0,0);  
     } else if (type == phone_module::IMPORTANT) {
       pLVCD->clrText = RGB(128,0,0);  
     } else if (type == phone_module::CONSOLECMD) {
       pLVCD->clrText = RGB(0,0,255);  
     } else if (type == phone_module::ORDER) {
       pLVCD->clrText = RGB(255,0,128);  
     } else if (type == phone_module::MARKET_QUOTATION) {
       pLVCD->clrText = RGB(0,128,192);  
     } else if (type == phone_module::FROM_R) {
       pLVCD->clrText = RGB(219,39,48);  
     }

/*
     if ((pLVCD->nmcd.dwItemSpec % 2) == 0) {
      crText = RGB(200,200,255);
     } else { 
      crText = RGB(0,0,0);        
     }*/

      // Store the color back in the NMLVCUSTOMDRAW struct.
      //pLVCD->clrText = crText;

      // Tell Windows to paint the control itself.
      return CDRF_DODEFAULT;
	}

};


