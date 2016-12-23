cmake_minimum_required(VERSION 3.6)
project(rocketmqphpclient)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
    example/consumer.php
    example/producer.php
    .gitignore
    config.m4
    consumer.h
    CREDITS
    EXPERIMENTAL
    php_rocketmqclient.h
    producer.h
    rocketmqclient.cpp)

add_executable(rocketmqphpclient ${SOURCE_FILES})