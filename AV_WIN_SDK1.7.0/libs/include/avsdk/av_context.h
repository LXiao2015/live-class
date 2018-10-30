﻿#ifndef AV_CONTEXT_H_
#define AV_CONTEXT_H_

#include "av_common.h"
#include "av_room.h"
#include "av_device_mgr.h"
#include "av_audio_ctrl.h"
#include "av_video_ctrl.h"
#ifdef WIN32
#include "av_device_test.h"
#endif
namespace tencent {
namespace av {

/**
@brief AVContext对象代表着一个SDK运行实例。

@details AVContext对内负责管理SDK的工作线程，控制各种内部对象的生命周期；
  对外提供了一系列的接口方法，App可以通过AVContext的成员函数进一步访问SDK的其他对象。
*/
class AV_EXPORT AVContext {
 public:
  virtual ~AVContext() {}

  /**
  @brief 获得SDK的版本信息。

  @return 以字符串形式返回SDK的版本信息。
    版本信息由三个数字组成，数字间以“.”分隔，比如“1.2.20150501”，
    前两个数字“1.1”表示SDK版本号，最后一个数字表示SDK编译时间。

  @remark SDK在特性变化和bug修复之后，会更改版本信息重新发布。
    一般来说，版本号代表特性的变化，SDK的对外接口和内部架构都可能有较大的变化和调整。
    而对于同一个版本的SDK，编译时间能够体现bugfix的情况，
    比如“1.0.20150501”和“1.0.20150601”，后者可以视作是前者的bugfix版本。
  */
  static std::string GetSDKVersion();

  /**
  @brief CreateContext()的入参。

  @details Config结构体包含了创建AVContext对象所需的配置信息。
  */
  struct Config {
    /// 默认构造函数。
    Config() {
      sdk_app_id = 0;
      account_type = "";
      app_id_at3rd = "";
      identifier = "";
    }

    /// 拷贝构造函数。
    Config(const Config& other) {
      sdk_app_id = other.sdk_app_id;
      account_type = other.account_type;
      app_id_at3rd = other.app_id_at3rd;
      identifier = other.identifier;
    }

    virtual ~Config() {}

    uint32 sdk_app_id; ///< 腾讯为每个使用SDK的App分配的AppId。
    std::string account_type; ///< 腾讯为每个接入方分配的账号类型。
    std::string app_id_at3rd; ///< App使用的OAuth授权体系分配的AppId。
    std::string identifier; ///< 帐号名(用户名)。
  };

  /**
  @brief 创建AVContext对象。

  @details App使用SDK总是从CreateContext()开始的，该静态成员函数返回一个新的AVContext实例。

  @param [in] config 创建AVContext所需的配置信息。

  @return 成功则返回AVContext的实例指针；否则返回NULL。

  @remark
    - 创建成功之后，App需要进一步调用StartContext()来启动AVContext对象。
    - App必须保证AVContext实例的唯一性，同时创建多个AVContext实例可能会因为设备占用等方面的原因，出现无法预料的结果。
    - 由CreateContext()创建的AVContext对象必须调用DestroyContext()来销毁。
  */
  static AVContext* CreateContext(const AVContext::Config& config);

  /**
  @brief 销毁AVContext对象。

  @details App通过此静态成员函数来销毁由CreateContext()创建的AVContext对象。

  @remark App应该设法保证CreateContext()和DestroyContext()的配对调用。
  */
  static void DestroyContext(AVContext* context);

  /**
  @brief StartContext()的回调函数。

  @details 此函数是和StartContext()一起使用的回调函数，用来异步返回执行结果。

  @param result 错误码：
    \n AV_OK 执行成功；
    \n AV_ERROR_INITSDKFAIL 一般原因为鉴权失败。
  @param custom_data 值等于调用StartContext()时的入参custom_data。
  */
  typedef void (*StartCompleteCallback)(int32 result, void* custom_data);

  /**
  @brief 启动AVContext对象。

  @details StartContext()是一个异步操作，内部会启动工作线程，创建各种内部对象。

  @param [in] start_complete_callback 函数指针，指向App定义的回调函数。
  @param [in] custom_data App指定的一个没有类型的指针，SDK会在回调函数中把该值回传给App。

  @return 返回值表示异步操作启动是否成功；异步操作执行结果通过回调函数返回。

  @retval AV_OK 启动成功。
  @retval 其他值 启动失败。

  @remark
    - 只有当异步操作启动成功的时候，才会通过回调函数异步返回执行结果。
    - 当异步操作启动失败的时候，直接通过StartContext()的返回值返回错误，不会再触发回调函数。
    - App收到StartCompleteCallback回调的时候，表示StartContext()执行完毕。
    - StartContext()执行成功之后，App才能进一步调用AVContext的其他成员函数。
    - StartContext()执行成功之后，App必须调用StopContext()来终止AVContext对象。

  @attention 以下两点约定也适用于SDK的其他异步操作接口：
    -# 只有当异步操作启动成功的时候，才会通过回调函数异步返回执行结果；
    -# 当异步操作启动失败的时候，直接通过StartContext()的返回值返回错误，不会再触发回调函数。
  */
  virtual int32 StartContext(StartCompleteCallback start_complete_callback, void* custom_data = NULL) = 0;

  /**
  @brief 终止AVContext对象。

  @details StopContext()是一个异步操作，内部会终止工作线程，销毁各种内部对象。

  @param [in] stop_complete_callback 函数指针，指向App定义的回调函数。
  @param [in] custom_data App指定的一个没有类型的指针，SDK会在回调函数中把该值回传给App。

  @return 返回值表示异步操作启动是否成功；异步操作执行结果通过回调函数返回。

  @retval AV_OK 启动成功。
  @retval 其他值 启动失败。

  @remark
    - App应该设法保证StartContext()和StopContext()的配对调用；
    - StopContext()内部如果判断到当前还没退出音视频房间，会自动调用ExitRoom()；
    - StopContext()执行成功之后，App必须调用DestroyContext()来销毁AVContext对象。
  */
  virtual int32 StopContext(AVClosure stop_complete_callback, void* custom_data = NULL) = 0;

  /**
  @brief 进入音视频房间。

  @details EnterRoom()是一个异步操作，内部会连接服务器，申请进入音视频房间。

  @param [in] room_delegate 房间委托对象：
    \n 进入多人房间，必须传递AVRoomMulti::Delegate类型的委托对象。
  @param [in] param 进入房间所需的参数：
    \n 进入多人房间，必须传递AVRoomMulti::EnterRoomParam类型的参数。

  @return 返回值表示异步操作启动是否成功；
    异步操作执行结果通过入参room_delegate的OnEnterRoomComplete()返回。

  @retval AV_OK 启动成功。
  @retval AV_ERR_CONTEXT_NOT_STARTED StartContext()没有执行成功。
  @retval AV_ERR_ALREADY_EXISTS 没有退出上一个音视频房间。
  @retval AV_ERR_INVALID_ARGUMENT 委托或房间配置信息为空，或者委托和房间类型不匹配。

  @remark EnterRoom()必须在StartContext()执行成功之后调用才有效。
    目前同一时刻只支持进入一个音视频房间，所以：
    - 调用EnterRoom()之前，必须先退出上一个音视频房间；
    - 如果当前AVContext中已经存在一个音视频房间，调用EnterRoom()会同步返回失败。

  @attention Windows版本的SDK暂时没有开放双人音视频房间的能力。
  */
  virtual int32 EnterRoom(AVRoom::Delegate* room_delegate, const AVRoom::EnterRoomParam* param) = 0;

  /**
  @brief 退出音视频房间。

  @details ExitRoom()是一个异步操作，内部会连接服务器，申请退出音视频房间。

  @return 返回值表示异步操作启动是否成功；
    异步操作执行结果通过EnterRoom()入参room_delegate的OnRoomExitComplete()返回。

  @retval AV_OK 启动成功。
  @retval 其他值 启动失败。
  */
  virtual int32 ExitRoom() = 0;

  /**
  @brief 获得AVRoom对象。

  @details EnterRoom()执行成功之后，App可以通过此成员函数获得当前音视频房间对应的AVRoom对象。

  @return 如果EnterRoom()执行成功，则返回当前音视频房间对应的AVRoom对象；否则返回NULL。
  */
  virtual AVRoom* GetRoom() const = 0;

  /**
  @brief 获得AVAudioCtrl对象。

  @details StartContext()执行成功之后，App可以通过此成员函数获得AVAudioCtrl对象。

  @return 如果StartContext()执行成功，则返回AVAudioCtrl对象；否则返回NULL。
  */
  virtual AVAudioCtrl* GetAudioCtrl() = 0;

  /**
  @brief 获得音频对应的AVDeviceMgr对象。

  @details StartContext()执行成功之后，App可以通过此成员函数获得音频对应的AVDeviceMgr对象。

  @return 如果StartContext()执行成功，则返回音频对应的AVDeviceMgr对象；否则返回NULL。
  */
  virtual AVDeviceMgr* GetAudioDeviceMgr() = 0;

  /**
  @brief 获得AVVideoCtrl对象。

  @details StartContext()执行成功之后，App可以通过此成员函数获得指定的AVVideoCtrl对象。

  @return 如果StartContext()执行成功，则返回指定的AVVideoCtrl对象；否则返回NULL。
  */
  virtual AVVideoCtrl* GetVideoCtrl() = 0;

  /**
  @brief 获得视频对应的AVDeviceMgr对象。

  @details StartContext()执行成功之后，App可以通过此成员函数获得视频对应的AVDeviceMgr对象。

  @return 如果StartContext()执行成功，则返回视频对应的AVDeviceMgr对象；否则返回NULL。
  */
  virtual AVDeviceMgr* GetVideoDeviceMgr() = 0;

#ifdef WIN32
  /**
  @brief 开始设备测试。


  @param [in] start_complete_callback 函数指针，指向App定义的回调函数。
  @param [in] custom_data App指定的一个没有类型的指针，SDK会在回调函数中把该值回传给App。

  @return 返回值表示异步操作启动是否成功；异步操作执行结果通过回调函数返回。

  @retval AV_OK 启动成功。
  @retval 其他值 启动失败。

  @remark
    - 只有当异步操作启动成功的时候，才会通过回调函数异步返回执行结果；
    - 当异步操作启动失败的时候，直接通过StartDeviceTest()的返回值返回错误，不会再触发回调函数；
    - App收到StartCompleteCallback回调的时候，表示StartDeviceTest()执行完毕；
    - StartDeviceTest()执行成功之后，App才能通过GetAVDeviceTest()获取AVDeviceTest对象进行具体的设备测试；
    - StartDeviceTest()执行成功之后，App必须调用StopDeviceTest()来停止设备测试及释放AVDeviceTest对象。
  - 设备测试操作与房间进出操作是互斥的。在设备测试过程中，不能进入房间；如果要进入房间，必须先停止设备测试。在房间中，不能进行设备测试；如果要进行设备测试，必须先退出房间。
  @attention 以下两点约定也适用于SDK的其他异步操作接口：
    -# 只有当异步操作启动成功的时候，才会通过回调函数异步返回执行结果；
    -# 当异步操作启动失败的时候，直接通过StartDeviceTest()的返回值返回错误，不会再触发回调函数。
  */
  virtual int32 StartDeviceTest(StartCompleteCallback start_complete_callback, void* custom_data = NULL) = 0;

  /**
  @brief 停止设备测试。

  @param [in] stop_complete_callback 函数指针，指向App定义的回调函数。
  @param [in] custom_data App指定的一个没有类型的指针，SDK会在回调函数中把该值回传给App。

  @return 返回值表示异步操作启动是否成功；异步操作执行结果通过回调函数返回。

  @retval AV_OK 启动成功。
  @retval 其他值 启动失败。

  @remark
    - App应该设法保证StartDeviceTest()和StopDeviceTest()的配对调用；
  - 设备测试操作与房间进出操作是互斥的。在设备测试过程中，不能进入房间；如果要进入房间，必须先停止设备测试。在房间中，不能进行设备测试；如果要进行设备测试，必须先退出房间。
  */
  virtual int32 StopDeviceTest(AVClosure stop_complete_callback, void* custom_data = NULL) = 0;

  /**
  @brief 获得AVDeviceTest对象。

  @details StartDeviceTest()执行成功之后，App可以通过此成员函数获得AVDeviceTest对象。

  @return 如果StartDeviceTest()执行成功，则返回AVDeviceTest对象；否则返回NULL。
  @remark
  - 设备测试操作与房间进出操作是互斥的。在设备测试过程中，不能进入房间；如果要进入房间，必须先停止设备测试。在房间中，不能进行设备测试；如果要进行设备测试，必须先退出房间。
  */
  virtual AVDeviceTest* GetDeviceTest() = 0;
#endif

#if defined(ANDROID) || defined(TARGET_OS_IPHONE)
  virtual void InitNetType(int netType) = 0;
#endif

#if defined(TARGET_OS_IPHONE)
    virtual int32 TerminalRoom() = 0;
#endif
};

} // namespace av
} // namespace tencent

#endif // #ifndef AV_CONTEXT_H_
