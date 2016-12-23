<?php

ini_set("display_errors", "On");
error_reporting(E_ALL | E_STRICT);
$consumer = new MQConsumer("consumer");
$consumer->setNamesrvAddr("10.255.209.167:9876");

$consumer->setTopic("ITEM_BASE", "*");
$consumer->on(function ($message, $result){
    echo $message ."\n";
    return true;
});
