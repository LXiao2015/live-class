#ifndef AV_ENDPOINT_H_
#define AV_ENDPOINT_H_

#include "av_common.h"

namespace tencent {
namespace av {

/**
@brief 房间成员类。

@details 房间成员对象由SDK侧负责创建和销毁，业务侧不能创建和销毁它们，只能通过获取成员对象引用来使用它们。

@remark 建议业务侧不要一直持有成员对象引用，即显式地把成员对象引用保存到另一个容器，可以只在需要的时候再去重新获取它们即可。因为随着成员加入和退出房间，SDK侧会自动创建和销毁成员对象，如果业务侧一直持有成员对象引用，则可能持有的是一个已经被销毁的对象的引用，而使用这样的错误的引用很可能导致程序异常。
*/
class AV_EXPORT AVEndpoint {
 public:
  /// 房间成员信息。
  struct Info {
    Info()
      : sdk_version(0)
      , terminal_type(0)
      , has_audio(false)
      , is_mute(false)
      , has_camera_video(false)
      , has_screen_video(false) {
    }

    virtual ~Info() {}

    std::string identifier; ///< 成员id。
    uint32 sdk_version; ///< SDK版本号。
    uint32 terminal_type; ///< 终端类型。
    bool has_audio; ///< 是否有发音频。
    bool is_mute; ///< 是否屏蔽这个成员的音频。这边的屏蔽是指会接收他的音频流，但不把其音频流送去解码和播放，也就听不到他的声音。
    bool has_camera_video; ///< 是否有发来自摄像头或外部视频捕获设备的视频。
    bool has_screen_video; ///< 是否有发来自屏幕的视频。
  };

  /**
  @brief RequestViewList()的回调函数。

  @brief RequestViewList()的回调函数，用来返回RequestViewList()的异步操作结果。

  @param identifier_list 成员id列表。
  @param view_list 视频画面参数列表。
  @param ret_code 返回码。
  @param custom_data 业务侧自定义的参数，会在执行回调函数时原封不动地返回给业务侧。由于回调函数通常是某个类的静态函数，可以通过该参数指定当前所对应的类的具体对象。

  */
  typedef void (*RequestViewListCompleteCallback)(std::vector<std::string> identifier_list, std::vector<View> view_list,
      int32 ret_code, void* custom_data);

  /**
  @brief CancelAllView()的回调函数。

  @brief CancelAllView()的回调函数，用来返回CancelAllView()的异步操作结果。

  @param ret_code 返回码。
  @param custom_data 业务侧自定义的参数，会在执行回调函数时原封不动地返回给业务侧。由于回调函数通常是某个类的静态函数，可以通过该参数指定当前所对应的类的具体对象。
  */
  typedef void (*CancelAllViewCompleteCallback)(int32 ret_code, void* custom_data);

 public:


  /**
  @brief 获得成员id。

  @return 返回成员id。
  */
  virtual const std::string& GetId() const = 0;

  /**
  @brief 获得成员信息。

  @return 返回成员信息。
  */
  virtual const Info& GetInfo() const = 0;

#ifndef WIN32
  /**
  @brief 同时请求一个或多个成员的视频画面。

  @details 同时请求一个或多个成员的视频画面。同一时刻只能请求一次成员的画面，并且必须等待异步结果返回后，才能进行新的请求画面操作。

  @param [in] identifier_list 成员id列表。
  @param [in] view_list 视频画面参数列表。
  @param [in] complete_callback 该操作的回调函数。
  @param [in] custom_data 业务侧自定义的参数，会在执行回调函数时原封不动地返回给业务侧。由于回调函数通常是某个类的静态函数，可以通过该参数指定当前所对应的类的具体对象。

  @return AV_OK表示调用成功；AV_ERR_BUSY表示上一次操作(包括RequestViewList和CancelAllView)还在进行中；AV_ERR_FAILED表示操作失败，可能是因为所请求的成员当前已经没有对应视频源的视频、所请求成员已经退出房间等。

  @remark
      . 画面大小可以根据业务层实际需要及硬件能力决定。
      . 如果是手机，建议只有其中一路是大画面，其他都是小画面，这样硬件更容易扛得住，同时减少流量。
      . 这边把320×240及以上大小的画面认为是大画面；反之，认为是小画面。
      . 实际上请求到的画面大小，由发送方决定。如A传的画面是小画面，即使这边即使想请求它的大画面，也只能请求到的小画面。
      . 发送方传的画面大小，是否同时有大画面和小画面，由其所设置的编解码参数、场景、硬件、网络等因素决定。
      . RequestViewList和CancelAllView不能并发执行，即同一时刻只能进行一种操作。
      . identifier_list和view_list的元素个数必须等于count，同时每个元素是一一对应的。
    . 在请求画面前最好先检查该成员是否有对应的视频源。
  */
  static int32 RequestViewList(std::vector<std::string> identifier_list, std::vector<View> view_list,
                               RequestViewListCompleteCallback complete_callback, void* custom_data);

  /**
  @brief 取消所有请求的视频画面。

  @details 取消所有请求的视频画面。

  @param [in] complete_callback 该操作的回调函数。
  @param [in] custom_data 业务侧自定义的参数，会在执行回调函数时原封不动地返回给业务侧。由于回调函数通常是某个类的静态函数，可以通过该参数指定当前所对应的类的具体对象。

  @return AV_OK表示调用成功；AV_ERR_BUSY表示上一次操作(包括RequestViewList和CancelAllView)还在进行中；AV_ERR_FAILED表示操作失败。

  @remark
      . RequestViewList和CancelAllView不能并发执行，即同一时刻只能进行一种操作。
  */
  static int32 CancelAllView(CancelAllViewCompleteCallback complete_callback, void* custom_data);
#endif

  /**
  @brief 屏蔽成员音频。

  @details 屏蔽成员音频。这边的屏蔽是指会接收他的音频流，但不把其音频流送去解码和播放，也就听不到他的声音。

  @param is_mute 是否屏蔽。

  @return true表示操作成功，false表示操作失败。

  @remark 不支持控制房间其他成员是否发送音频，只支持控制是否屏蔽它们的音频。
  */
  virtual bool MuteAudio(bool is_mute = false) = 0;

  /**
  @brief 是否已屏蔽音频。

  @return true表示是，false表示否。
  */
  virtual bool IsAudioMute() = 0;

  /**
  @brief 是否正在发音频。

  @details 是否正在发音频。

  @return true表示是，false表示否。
  */
  virtual bool HasAudio() = 0;

  /**
  @brief 是否正在发来自摄像头或外部视频捕获设备的视频。

  @details 是否正在发来自摄像头或外部视频捕获设备的视频。

  @return true表示是，false表示否。
  */
  virtual bool HasCameraVideo() = 0;

  /**
  @brief 是否正在发来自屏幕的视频。

  @details 是否正在发来自屏幕的视频。

  @return true表示是，false表示否。
  */
  virtual bool HasScreenVideo() = 0;

  DISALLOW_EVIL_DESTRUCTIONS(AVEndpoint)
};

} // namespace av
} // namespace tencent

#endif // #ifndef AV_ENDPOINT_H_