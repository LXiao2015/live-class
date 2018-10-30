#ifndef AV_ROOM_MULTI_H_
#define AV_ROOM_MULTI_H_

#include "av_common.h"
#include "av_room.h"

namespace tencent {
namespace av {

/// 不同类型的房间对象转换的宏定义，用于把AVRoom*类型的对象转换成AVRoomMulti*类型的对象。
#define AV_ROOM_MULTI(x) dynamic_cast<AVRoomMulti*>(x)

/**
@brief 多人音视频房间类。
*/
class AVRoomMulti : public AVRoom {
 public:

  /// 房间委托类。业务侧需要实现该类来处理房间异步操作返回的结果。
  struct Delegate : public AVRoom::Delegate {

  };

  /// 调用AVContext::EnterRoom()进入多人房间时所需传入的参数。
  struct EnterRoomParam : public AVRoom::EnterRoomParam {
    EnterRoomParam()
      : app_room_id(0)
      , auth_bits(AUTH_BITS_DEFUALT)
      , audio_category(AUDIO_CATEGORY_VOICECHAT)
      , auto_create_room(true) {
      room_type = AVRoom::ROOM_TYPE_MULTI;
    }

    uint32 app_room_id; ///< 业务侧的房间id，也就是由业务侧创建并维护的房间id，区别于SDK侧的房间id。该id可以是业务侧的讨论组号、群号、座位号等一切用于区分每个音视频通话的id。
    uint64 auth_bits; ///< 通话能力权限位。
    std::string auth_buffer; ///< 通话能力权限位的加密串。
    std::string av_control_role; ///< 角色名，web端音视频参数配置工具所设置的角色名。
    AudioCategory audio_category; ///< 音视场景策略。
    bool auto_create_room; ///< 是否自动创建音视频房间。如果SDK侧房间不存在，是否自动创建它。
  };

  /**
  @brief 获取房间成员个数。

  @details 获取当前正在房间内的成员个数。

  @return 返回成员个数。
  */
  virtual int32 GetEndpointCount() = 0;

  /**
  @brief 获取房间成员对象列表。

  @details 获取当前正在房间内的成员对象列表。

  @param [out] endpoints 正在房间内的成员对象列表。

  @return 返回成员对象个数。0表示获取失败。

  @remark 所返回的成员对象的生命周期由SDK侧负责，业务侧不需要关心。最好业务侧不要一直持有它，可以在每次需要时去重新获取它即可。
  */
  virtual int32 GetEndpointList(AVEndpoint** endpoints[]) = 0;

  /**
  @brief 获取房间成员对象。

  @details 根据成员id获取成员对象。

  @param identifier 要获取的成员对象的成员id。

  @return 返回对应的成员对象。如果失败时则返回NULL。

  @remark 所返回的成员对象的生命周期由SDK侧负责，业务侧不需要关心。最好业务侧不要一直持有它，可以在每次需要时去重新获取它即可。
  */
  virtual AVEndpoint* GetEndpointById(const std::string& identifier) = 0;

  /**
    @brief 修改通话能力权限操作的回调函数。

    @details 此函数用来返回AVRoomMulti::ChangeAuthority()的异步操作结果。

    @param [in] ret_code 返回码。AV_OK表示操作成功；AV_ERR_FAILED表示操作失败。

    @param [in] custom_data 业务侧自定义的参数，会在执行回调函数时原封不动地返回给业务侧。由于回调函数通常是某个类的静态函数，可以通过该参数指定当前所对应的类的具体对象。
  */

  typedef void(*ChangeAuthorityCallback)(int32 ret_code, void* custom_data);

  /**
  @brief 修改通话能力权限。

  @details 修改通话能力权限。通话能力权限包括是否可以创建房间/进入房间/发送音频/接收音频/发送摄像头视频/接收摄像头视频/发送屏幕视频/接收屏幕视频等。

  @param [in] auth_bits 通话能力权限位。

  @param [in] auth_buffer 通话能力权限位的加密串。

  @param [in] callback 回调函数。

  @param [in] custom_data 业务侧自定义的参数，会在执行回调函数时原封不动地返回给业务侧。由于回调函数通常是某个类的静态函数，可以通过该参数指定当前所对应的类的具体对象。

  @return 返回操作结果。
  */
  virtual int32 ChangeAuthority(uint64 auth_bits, const std::string& auth_buffer, ChangeAuthorityCallback callback,
                                void* custom_data) = 0;

  DISALLOW_EVIL_DESTRUCTIONS(AVRoomMulti)
};

} // namespace av
} // namespace tencent

#endif // #ifndef AV_ROOM_MULTI_H_