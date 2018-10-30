<?php
require_once 'CreateRequest.php';

$HttpUrl = "live.api.qcloud.com";
$HttpMethod = "GET";
$isHttps = true;
$secretKey = 'P118695htbzRvnaJMoGeeG7xvHGyF6k5';

$h = new Create();

$InfoByID = array();
$Online = array();
$ChannelID = array();
$ChannelDes = array();
$ChannelID2 = array();
$ChannelDes2 = array();
$InfoByID2 = array();

//获取直播列表
$COMMON_PARAMS_1 = array(
    'Nonce' => rand(),
    'Timestamp' => time(NULL),
    'Action' => 'DescribeLVBChannelList',
    'SecretId' => 'AKIDSgWGwAv7yM1GY9KpM9VoTtQLFfuvLLMi',
    'Region' => 'gz',
);
$COMMON_PARAMS_2 = array(
    'Nonce' => rand(),
    'Timestamp' => time(NULL),
    'Action' => 'DescribeLVBChannel',
    'SecretId' => 'AKIDSgWGwAv7yM1GY9KpM9VoTtQLFfuvLLMi',
    'Region' => 'gz',
);
$COMMON_PARAMS_3 = array(
    'Nonce' => rand(),
    'Timestamp' => time(NULL),
    'Action' => 'DescribeLVBOnlineUsers',
    'SecretId' => 'AKIDSgWGwAv7yM1GY9KpM9VoTtQLFfuvLLMi',
    'Region' => 'gz',
);

$PRIVATE_PARAMS_1 = array(
    'channelStatus' => '1',
    'ascDesc' => '0',
);
$PRIVATE_PARAMS_12 = array(
    'channelStatus' => '0',
    'ascDesc' => '0',
);
//echo '【频道列表】';
$ChannelList = $h->CreateRequest($HttpUrl,$HttpMethod,$COMMON_PARAMS_1,$secretKey, $PRIVATE_PARAMS_1, $isHttps);
//echo '【频道列表】';
$ChannelList2 = $h->CreateRequest($HttpUrl,$HttpMethod,$COMMON_PARAMS_1,$secretKey, $PRIVATE_PARAMS_12, $isHttps);

//获取直播描述和在线信息
for($i=0; $i<$ChannelList['all_count']; $i++) {
    $ChannelID[$i] = $ChannelList['channelSet'][$i]['channel_id'];
    $PRIVATE_PARAMS_2 = array(
        'channelId' => "$ChannelID[$i]",
    );
//    echo '【频道描述】';
    $InfoByID = $h->CreateRequest($HttpUrl, $HttpMethod, $COMMON_PARAMS_2, $secretKey, $PRIVATE_PARAMS_2, $isHttps);
//    print_r($InfoByID);
    $ChannelDes[$i] = $InfoByID['channelInfo'][0]['channel_describe'];  //频道描述
//    echo '【在线信息】';

    $PRIVATE_PARAMS_3 = array(
        'channelIds.n' => "$ChannelID[$i]",
    );
    $Online = $h->CreateRequest($HttpUrl, $HttpMethod, $COMMON_PARAMS_3, $secretKey, $PRIVATE_PARAMS_3, $isHttps);
}

for($i=0; $i<$ChannelList2['all_count']; $i++) {
    $ChannelID2[$i] = $ChannelList2['channelSet'][$i]['channel_id'];
    $PRIVATE_PARAMS_2 = array(
        'channelId' => "$ChannelID2[$i]",
    );
//    echo '【频道描述】';
    $InfoByID2 = $h->CreateRequest($HttpUrl, $HttpMethod, $COMMON_PARAMS_2, $secretKey, $PRIVATE_PARAMS_2, $isHttps);
//    print_r($InfoByID2);
    $ChannelDes2[$i] = $InfoByID2['channelInfo'][0]['channel_describe'];  //频道描述
}
$conn = mysql_connect('127.0.0.1','root','') or die ("Unable to connect to the MySQL!");
mysql_select_db("LVB", $conn);

//清空表的缓存
$sql = "DROP TABLE live";
mysql_query($sql,$conn);

$sql = "CREATE TABLE live(
    channelid varchar(255),
    name varchar(255),
    des varchar(127),
    roomid smallint(255),
    time varchar(127),
    audience smallint(127)
)";
mysql_query($sql,$conn);
$sql = 'use live';
mysql_query($sql,$conn);

for($i=0;$i<$ChannelList['all_count'];$i++){
    $ChannelName = $ChannelList['channelSet'][$i]['channel_name'];  //频道名称
    $CreateTime = $ChannelList['channelSet'][$i]['create_time'];  //创建时间
    $AudienceNum = $Online['list'][$i]['online'];  //在线人数
    $id = $ChannelID[$i];
    $ChannelDescription = $ChannelDes[$i];
    $sql = "insert into live(channelid,name,des,roomid,time,audience) values('$id','$ChannelName','$ChannelDescription','','$CreateTime','$AudienceNum')";
    mysql_query($sql,$conn);
}
mysql_close($conn);

$conn = mysql_connect('127.0.0.1','root','') or die ("Unable to connect to the MySQL!");
mysql_select_db("LVB", $conn);
$sql = "DROP TABLE history";
mysql_query($sql,$conn);

$sql = "CREATE TABLE history(
    channelid varchar(255),
    name varchar(255),
    des varchar(127)
)";
mysql_query($sql,$conn);
$sql = 'use history';
mysql_query($sql,$conn);

for($i=0;$i<$ChannelList2['all_count'];$i++){
    $ChannelName = $ChannelList2['channelSet'][$i]['channel_name'];  //频道名称
    $id = $ChannelID2[$i];
    $ChannelDescription = $ChannelDes2[$i];
    $sql = "insert into history(channelid,name,des) values('$id','$ChannelName','$ChannelDescription')";
    mysql_query($sql,$conn);
}
?>

