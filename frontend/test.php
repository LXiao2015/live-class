<?php

$Rsp = file_get_contents("https://www.classair.cn/mhs.php/Mhs/Video/getLiveList?status_code=1");
$test = json_decode($Rsp, true);
print_r($test);

?>
