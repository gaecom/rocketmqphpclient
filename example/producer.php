<?php

ini_set("display_errors", "On");
error_reporting(E_ALL | E_STRICT);
$test = new MqProducer("my_producer");

$test->setNamesrvAddr("10.255.209.167:9876");

for ($i = 0; $i < 100; $i ++){
    $result = $test->send("TopicTest", "tagB", "", "hello php client" . $i);
    var_dump($result);
}
