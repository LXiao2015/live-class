<?php
/**
 * Created by PhpStorm.
 * User: Administrator
 * Date: 2016/4/27
 * Time: 16:02
 */

/***************真实调用时，需要根据不同接口修改下面的参数*********************************/

/*接口的 URL地址，可从对应的接口说明 “1.接口描述” 章节获取该接口的地址*/
$HttpUrl="live.api.qcloud.com";

/*除非有特殊说明，如MultipartUploadVodFile，其它接口都支持GET及POST*/
$HttpMethod="GET";

/*是否https协议，大部分接口都必须为https，只有少部分接口除外（如MultipartUploadVodFile）*/
$isHttps =true;

/*需要填写你的密钥，可从  https://console.qcloud.com/capi 获取 SecretId 及 $secretKey*/
$secretKey='P118695htbzRvnaJMoGeeG7xvHGyF6k5';

/*下面这五个参数为所有接口的 公共参数；对于某些接口没有地域概念，则不用传递Region（如DescribeDeals）*/
$COMMON_PARAMS_1 = array(
    'Nonce'=> rand(),
    'Timestamp'=>time(NULL),
    'Action'=>'DescribeLVBChannelList',
    'SecretId'=> 'AKIDSgWGwAv7yM1GY9KpM9VoTtQLFfuvLLMi',
    'Region' =>'gz',
);
$COMMON_PARAMS_2 = array(
    'Nonce'=> rand(),
    'Timestamp'=>time(NULL),
    'Action'=>'DescribeLVBChannel',
    'SecretId'=> 'AKIDSgWGwAv7yM1GY9KpM9VoTtQLFfuvLLMi',
    'Region' =>'gz',
);

/*下面为接口DescribeLVBChannelList的私有参数*/
$PRIVATE_PARAMS_1 = array(
    'channelStatus'=> '1',
    'ascDesc'=> '0',
);


$ChannelList = CreateRequest($HttpUrl,$HttpMethod,$COMMON_PARAMS_1,$secretKey, $PRIVATE_PARAMS_1, $isHttps);


//输出所有频道名称（正在直播），自己加的
for($i=0; $i<$ChannelList['all_count']; $i++){
    echo $ChannelList['channelSet'][$i]['channel_name'];
}
echo '<br>';



$PRIVATE_PARAMS_2 = array(
    'channelId' => "16093104850683168904",
);

$InfoByID = CreateRequest($HttpUrl, $HttpMethod, $COMMON_PARAMS_2, $secretKey, $PRIVATE_PARAMS_2, $isHttps);

function CreateRequest($HttpUrl,$HttpMethod,$COMMON_PARAMS,$secretKey, $PRIVATE_PARAMS, $isHttps)
{
    $FullHttpUrl = $HttpUrl . "/v2/index.php";

    /***************对请求参数 按参数名 做字典序升序排列，注意此排序区分大小写*************/
    $ReqParaArray = array_merge($COMMON_PARAMS, $PRIVATE_PARAMS);
    ksort($ReqParaArray);

    /**********************************生成签名原文**********************************
     * 将 请求方法, URI地址,及排序好的请求参数  按照下面格式  拼接在一起, 生成签名原文，此请求中的原文为
     * GETcvm.api.qcloud.com/v2/index.php?Action=DescribeInstances&Nonce=345122&Region=gz
     * &SecretId=AKIDz8krbsJ5yKBZQ    ·1pn74WFkmLPx3gnPhESA&Timestamp=1408704141
     * &instanceIds.0=qcvm12345&instanceIds.1=qcvm56789
     * ****************************************************************************/
    $SigTxt = $HttpMethod . $FullHttpUrl . "?";

    $isFirst = true;
    foreach ($ReqParaArray as $key => $value) {
        if (!$isFirst) {
            $SigTxt = $SigTxt . "&";
        }
        $isFirst = false;

        /*拼接签名原文时，如果参数名称中携带_，需要替换成.*/
        if (strpos($key, '_')) {
            $key = str_replace('_', '.', $key);
        }

        $SigTxt = $SigTxt . $key . "=" . $value;
    }

    /*********************根据签名原文字符串 $SigTxt，生成签名 Signature******************/
    $Signature = base64_encode(hash_hmac('sha1', $SigTxt, $secretKey, true));


    /***************拼接请求串,对于请求参数及签名，需要进行urlencode编码********************/
    $Req = "Signature=" . urlencode($Signature);
    foreach ($ReqParaArray as $key => $value) {
        $Req = $Req . "&" . $key . "=" . urlencode($value);
    }

    /*********************************发送请求********************************/
    if ($HttpMethod === 'GET') {
        if ($isHttps === true) {
            $Req = "https://" . $FullHttpUrl . "?" . $Req;
        } else {
            $Req = "http://" . $FullHttpUrl . "?" . $Req;
        }

        $Rsp = file_get_contents($Req);

    } else {
        if ($isHttps === true) {
            $Rsp = SendPost("https://" . $FullHttpUrl, $Req, $isHttps);
        } else {
            $Rsp = SendPost("http://" . $FullHttpUrl, $Req, $isHttps);
        }
    }

    var_export(json_decode($Rsp, true));

    //这两行自己加的，注意要先把$Rsp变成数组
    $test = json_decode($Rsp, true);
    return $test;
}


function SendPost($FullHttpUrl,$Req,$isHttps)
{
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_POST, 1);
    curl_setopt($ch, CURLOPT_POSTFIELDS, $Req);

    curl_setopt($ch, CURLOPT_URL, $FullHttpUrl);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    if ($isHttps === true) {
        curl_setopt($ch, CURLOPT_SSL_VERIFYPEER,  false);
        curl_setopt($ch, CURLOPT_SSL_VERIFYHOST,  false);
    }

    $result = curl_exec($ch);

    return $result;
}
?>
