// ListBoxEndpoint.cpp : implementation file
//

#include "stdafx.h"
#include "QAVSDKApp.h"
#include "ListBoxEndpoint.h"
#include "CustomWinMsg.h"
#include "Util.h"

// ListBoxEndpoint

IMPLEMENT_DYNAMIC(ListBoxEndpoint, CListBox)

ListBoxEndpoint::ListBoxEndpoint()
{
	m_curSelIdx = -1;
}

ListBoxEndpoint::~ListBoxEndpoint()
{
}


BEGIN_MESSAGE_MAP(ListBoxEndpoint, CListBox)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_MENU_ITEM_REQUEST_CAMERA_VIEW, &ListBoxEndpoint::OnMenuItemRequestCameraView)
	ON_COMMAND(ID_MENU_ITEM_CANCEL_CAMERA_VIEW, &ListBoxEndpoint::OnMenuItemCancelCameraView)
	ON_COMMAND(ID_MENU_ITEM_REQUEST_SCREEN_VIEW, &ListBoxEndpoint::OnMenuItemRequestScreenView)
	ON_COMMAND(ID_MENU_ITEM_CANCEL_SCREEN_VIEW, &ListBoxEndpoint::OnMenuItemCancelScreenView)
	ON_COMMAND(ID_MENU_ITEM_NOT_MUTE_AUDIO, &ListBoxEndpoint::OnMenuItemNotMuteAudio)
	ON_COMMAND(ID_MENU_ITEM_MUTE_AUDIO, &ListBoxEndpoint::OnMenuItemMuteAudio)
END_MESSAGE_MAP()



// ListBoxEndpoint message handlers


void ListBoxEndpoint::OnRButtonDown(UINT nFlags, CPoint point)
{
	CListBox::OnRButtonDown(nFlags, point);

	int nItemCount = GetCount();
	for (int n = 0; n < nItemCount; n++)
	{
		CRect rcItem;
		GetItemRect(n, &rcItem);
		if (rcItem.PtInRect(point))
		{
			SetCurSel(n);
			m_curSelIdx = n;
			CMenu menu;
			ClientToScreen(&point);
			menu.LoadMenuW(IDR_MENU_ENDPOINT);
			menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
	}
}

void ListBoxEndpoint::DrawItem(LPDRAWITEMSTRUCT lpSt)
{
	CListBox::DrawItem(lpSt);
}

void ListBoxEndpoint::OnMenuItemRequestCameraView()
{
	FireListCommand(LISTCOMMAND_REQEUST_CAMERA_VIEW);
}

void ListBoxEndpoint::OnMenuItemCancelCameraView()
{
	FireListCommand(LISTCOMMAND_CANCEL_CAMERA_VIEW);
}

void ListBoxEndpoint::OnMenuItemRequestScreenView()
{
	FireListCommand(LISTCOMMAND_REQEUST_SCREEN_VIEW);
}

void ListBoxEndpoint::OnMenuItemCancelScreenView()
{
	FireListCommand(LISTCOMMAND_CANCEL_SCREEN_VIEW);
}

void ListBoxEndpoint::OnMenuItemNotMuteAudio()
{
	FireListCommand(LISTCOMMAND_NOT_MUTE_AUDIO);
}

void ListBoxEndpoint::OnMenuItemMuteAudio()
{
	FireListCommand(LISTCOMMAND_MUTE_AUDIO);
}

void ListBoxEndpoint::FireListCommand(listCommand command)
{
	if(m_curSelIdx < 0)return;
	int n = m_curSelIdx;//GetCurSel();
	m_curSelIdx = 0;
	CString itemText = _T("");
	GetText(n, itemText);
	CString identifier = itemText.Left(itemText.Find(_T("(")));
	DWORD_PTR pItem =  GetItemData(n);
	::SendMessage(GetMainHWnd(), WM_ENDPOINT_MENU_ITEM, (WPARAM)&identifier, command);
}


void ListBoxEndpoint::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rc;
	GetClientRect(&rc);
	//DrawFocusRect(dc, &rc);
	//dc.FillRect(&rc, )
	CListBox::OnPaint();
}
