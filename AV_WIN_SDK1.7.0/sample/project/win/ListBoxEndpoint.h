#pragma once

enum listCommand
{
	LISTCOMMAND_REQEUST_CAMERA_VIEW = 0,
	LISTCOMMAND_CANCEL_CAMERA_VIEW = 1,
	LISTCOMMAND_REQEUST_SCREEN_VIEW = 2,
	LISTCOMMAND_CANCEL_SCREEN_VIEW = 3,
	LISTCOMMAND_NOT_MUTE_AUDIO = 4,
	LISTCOMMAND_MUTE_AUDIO = 5,
};
// ListBoxEndpoint

class ListBoxEndpoint : public CListBox
{
	DECLARE_DYNAMIC(ListBoxEndpoint)

public:
	ListBoxEndpoint();
	virtual ~ListBoxEndpoint();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMenuItemRequestCameraView();
	afx_msg void OnMenuItemCancelCameraView();
	afx_msg void OnMenuItemRequestScreenView();
	afx_msg void OnMenuItemCancelScreenView();
	afx_msg void OnMenuItemNotMuteAudio();
	afx_msg void OnMenuItemMuteAudio();
	

	void	DrawItem(LPDRAWITEMSTRUCT);


	

protected:
	void	FireListCommand(listCommand command);
public:
	afx_msg void OnPaint();
	int m_curSelIdx;
};


