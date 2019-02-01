# rocketmqphpclient
rocketMQ的php客户端

## 项目说明
* 使用 php7 进行开发 （php5版本未进行测试）
* 使用 [rocketmq-client4cpp](https://github.com/lpflpf/rocketmq-client4cpp-crack "")
* Linux 环境
* g++ (version > 4.8,有c++11依赖)  
* 可能存在少量内存泄露，可忽略不计.


## 功能说明
*  通过pull的方式进行消费，将偏移量保存在server端。
*  消费不会产生阻塞，若消费完成，则程序结束。  
*  使用方法，请参考example/目录下样例。

## 安装方法

1.	Install rocketmq c++ client.   
	 
2.  下载phpclient 源码
3.  配置相应phpize 和php-config 路径
4.  执行命令 phpize
5.  修改Makefile 中的C++编译选项，增加`-std=c++11`.
6.  执行： `make && make install`
7.  修改php.ini 文件，增加  
	`extension=rocketmqclient.so`  
8.	在example/目录下，修改相应mq servername 配置，topic相应配置，进行测试.

[NEW VERSION TO SEE](https://github.com/lpflpf/rocketmq-client-php)
