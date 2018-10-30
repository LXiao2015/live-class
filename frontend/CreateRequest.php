<?php
/**
 * Created by PhpStorm.
 * User: Administrator
 * Date: 2016/5/12
 * Time: 16:09
 */
require_once 'SendPost.php';

class Create{
    public function CreateRequest($HttpUrl,$HttpMethod,$COMMON_PARAMS,$secretKey, $PRIVATE_PARAMS, $isHttps)
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
            $h = new Send();
            if ($isHttps === true) {
                $Rsp = $h->SendPost("https://" . $FullHttpUrl, $Req, $isHttps);
            } else {
                $Rsp = $h->SendPost("http://" . $FullHttpUrl, $Req, $isHttps);
            }
        }

//        var_export(json_decode($Rsp, true));

        //这两行自己加的，注意要先把$Rsp变成数组
        $test = json_decode($Rsp, true);
        return $test;
    }
}
?>