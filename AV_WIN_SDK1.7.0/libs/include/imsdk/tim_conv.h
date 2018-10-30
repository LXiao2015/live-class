#ifndef __TIM_IMPL_H__
#define __TIM_IMPL_H__

#include <vector>
#include "tim_comm.h"

namespace imcore {

class TIM_DECL TIMMessage;

class TIM_DECL TIMConversation {
public:
	TIMConversation() {}
	void SendMsg(TIMMessage &msg, TIMCallBack *cb);
	int  SaveMsg(TIMMessage &msg, const std::string& sender, bool readed);
	void GetMsgs(int count, const TIMMessage *last_msg, TIMValueCallBack<const std::vector<TIMMessage> &> *cb);
	void GetLocalMsgs(int count, const TIMMessage *last_msg, TIMValueCallBack<const std::vector<TIMMessage> &> *cb);
	void DeleteLocalMsg(TIMCallBack *cb);
	void SetReadMsg(const TIMMessage &last_read_msg);
	void SetReadMsg();
	uint64_t GetUnreadMessageNum();

	const std::string &peer() const;
	const TIMConversationType &type() const;
	void set_peer(const std::string &peer);
	void set_type(const TIMConversationType &type);

private:
	TIMConversationType type_;
	std::string peer_;

friend class TIMMessageImpl;
friend class TIMManager;
};

}

#endif
