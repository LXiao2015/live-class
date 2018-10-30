<?php
error_reporting(0);
require_once 'GetData.php';
?>
<!DOCTYPE html>
<html>
<head lang="en">
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <!-- 引入 Bootstrap -->
    <link href="http://netdna.bootstrapcdn.com/twitter-bootstrap/2.3.0/css/bootstrap-combined.min.css" rel="stylesheet">
    <!-- HTML5 Shim 和 Respond.js 用于让 IE8 支持 HTML5元素和媒体查询 -->
    <!-- 注意： 如果通过 file://  引入 Respond.js 文件，则该文件无法起效果 -->
    <!--[if lt IE 9]>
        <script src="https://oss.maxcdn.com/libs/html5shiv/3.7.0/html5shiv.js"></script>
        <script src="https://oss.maxcdn.com/libs/respond.js/1.3.0/respond.min.js"></script>
    <![endif]-->
    <style type="text/css">
        body {
            padding-top: 80px;
            padding-bottom: 40px;
        }
        .form-control{
            width: 400px;
            height: 110px;
            margin: 5px;
        }
        @font-face {
            font-family: Calligraphy; /*这里是说明调用来的字体名字*/
            src: url('myfont.ttf'); /*这里是字体文件路径*/
        }
    </style>
    <title>北京邮电大学在线教育</title>
</head>
<body>
<div class="navbar navbar-inverse navbar-fixed-top">
    <div class="navbar-inner">
        <div class="container">
            <a class="brand" style="font-family: Calligraphy;font-size: 35px;margin: 10px">北京邮电大学实时课堂</a>
            <form class="form-search pull-right"  style="margin: 10px">
                <div>
                    <input type="text" class="form-control" name="rtmp_url" id="rtmp_url" placeholder="输入地址">
                    <button type="button" class="btn btn-info" onclick="toPlayer()">Go</button>
                    <script src="http://qzonestyle.gtimg.cn/open/qcloud/video/live/h5/live_connect.js"></script>
                    <script type="text/javascript">
                        (function(){
                            var option ={
                                "live_url": ''
                                ,"width":"1024","height":"581"
                                , 'cache_time': 0.3
                                ,"debug": 1
                            };
                            window.toPlayer = function(){
                                option['live_url'] = document.getElementById('rtmp_url').value;
                                new qcVideo.Player(
                                    "player",
                                    option, {
                                        playStatus: function(status){
                                            if(status === 'ready'){
                                                //此时调用弹幕功能
                                            }
                                        }
                                    }
                                );
                            };
                        })()
                    </script>
                </div>
            </form>
        </div>
    </div>
</div>
<div id="player"></div>
<div class="container">
    <div class="body" style="margin:20px">
        <div class="live">
            <p><strong>正在直播</strong></p>
            <div>
<?php
$conn = mysql_connect('127.0.0.1','root','') or die ("Unable to connect to the MySQL!");
mysql_select_db("LVB", $conn);
$sql="select * from live"; //先执行SQL语句显示所有记录以与插入后相比较
$result=mysql_query($sql,$conn); //使用mysql_query()发送SQL请求
?>
                <table class="table table-striped">
                    <tr><th>频道名称</th><th>开课班级</th><th>授课教师</th><th>创建时间</th><th>我要听课</th><th>观看人数</th></tr>
                    <?php
                    while($row=mysql_fetch_array($result)){
                    ?>
                        <tr>
                            <td><?php echo $row['name']?></td>
                            <td><?php echo $row['des']?></td>
                            <td></td>
                            <td><?php echo $row['time']?></td>
                            <td>
                                <?php
                                $url = "http://play.video.qcloud.com/live.html?app_id=1251903613&channel_id=".$row['channelid']."&sw=724&sh=543";
                                echo "<script>var addr=\"$url\";</script>"
                                ?>
                                <a href="" id="addr">
                                    <script>
                                        document.getElementById("addr").href = addr;
                                    </script>
                                    <i class="icon-play"></i>
                                </a>
                            </td>
                            <td><?php echo $row['audience']?></td>
                        </tr>
                    <?php
                    }
                    ?>
                </table>
            </div>
        </div>
        <div class="history">
            <p><strong>历史频道</strong></p>
            <div>
<?php
$conn = mysql_connect('127.0.0.1','root','') or die ("Unable to connect to the MySQL!");
mysql_select_db("LVB", $conn);
$sql="select * from history"; //先执行SQL语句显示所有记录以与插入后相比较
$result2=mysql_query($sql,$conn); //使用mysql_query()发送SQL请求
?>
                <table class="table table-striped">
                    <tr><th>频道名称</th><th>频道描述</th></tr>
                    <?php
                    while($row=mysql_fetch_array($result2)){
                        ?>
                        <tr>
                            <td><?php echo $row['name']?></td>
                            <td><?php echo $row['des']?></td>
                        </tr>
                    <?php
                    }
                    ?>
                </table>
            </div>
        </div>
    </div>
    <div class="footing">
        <p id="contact"><br/>给我们提建议3360281447@qq.com</p>
    </div>
</div>
<!-- jQuery (Bootstrap 的 JavaScript 插件需要引入 jQuery) -->
<script src="https://code.jquery.com/jquery.js"></script>
<!-- 包括所有已编译的插件 -->
<script src="js/bootstrap.min.js"></script>

</body>
</html>