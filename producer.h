#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <rocketmq/DefaultMQProducer.h>
#include <rocketmq/Message.h>
#include <rocketmq/SendResult.h>
#include <rocketmq/MQClientException.h>

typedef struct {
    DefaultMQProducer* producer;
} producer_object_t;

typedef struct {
    producer_object_t *inner;
    zend_object zo;
} producer_object;

zend_class_entry *mqProducer_ce;

static inline producer_object_t* fetch_producer_inner(zend_object *obj) {
    return (producer_object_t* )(((producer_object *)(((char *) obj - XtOffsetOf(producer_object, zo))))->inner);
}

//static inline zend_object;

#define F_Producer_Ptr(zv) fetch_producer_inner(Z_OBJ_P((zv)))


// __construct
ZEND_BEGIN_ARG_INFO_EX(arginfo_producer___construct, 0, 0, 1)
    ZEND_ARG_INFO(0, producer_name)
ZEND_END_ARG_INFO()

PHP_METHOD(MQProducer, __construct) {
    zval* object = getThis();
    producer_object_t *inner = F_Producer_Ptr(object); 
    
    char * producer_name;
    size_t producer_name_len;

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "s", &producer_name, &producer_name_len)) {
        RETURN_FALSE;
    }

    if (!producer_name_len) {
        zend_error(E_WARNING, "set producer_name empty.");
    }
    
    inner->producer = new DefaultMQProducer(producer_name);
}

PHP_METHOD(MQProducer, __destruct){
}


// set nameservers
ZEND_BEGIN_ARG_INFO(arginfo_producer_set_namesrv_addr, 0)
    ZEND_ARG_INFO(0, nameservers)
ZEND_END_ARG_INFO()

PHP_METHOD(MQProducer, setNamesrvAddr) {
    char *nameservers;
    size_t nameservers_len;

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "s", &nameservers, &nameservers_len)) {
        RETURN_FALSE;
    }

    producer_object_t *inner = F_Producer_Ptr(getThis());
    inner->producer->setNamesrvAddr(nameservers);
    inner->producer->start();

    if (!nameservers_len) {
        zend_error(E_WARNING, "set nameservers empty.");
    }
}


// send message
ZEND_BEGIN_ARG_INFO(arginfo_producer_send, 0)
    ZEND_ARG_INFO(0, topic)
    ZEND_ARG_INFO(0, tag)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

PHP_METHOD(MQProducer, send) {
    char *topic, *tags, *key, *value;
    size_t topic_len, tags_len, key_len, value_len;

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "ssss", 
                                                        &topic, &topic_len,
                                                        &tags,   &tags_len,
                                                        &key,   &key_len,
                                                        &value, &value_len
                                        )) {
        RETURN_FALSE;
    }

    Message msg(topic, tags, key, value, value_len);
    producer_object_t *inner = F_Producer_Ptr(getThis());
    SendResult sendResult = inner->producer->send(msg);

    array_init(return_value);
    add_assoc_long(return_value, "status", (int)sendResult.getSendStatus());
    add_assoc_string(return_value, "msgId", (char *)sendResult.getMsgId().c_str());
}

const zend_function_entry producer_class_functions[] = {
    PHP_ME(MQProducer, __construct,     arginfo_producer___construct,       ZEND_ACC_PUBLIC| ZEND_ACC_CTOR)
    PHP_ME(MQProducer, __destruct,      NULL,                               ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(MQProducer, setNamesrvAddr,  arginfo_producer_set_namesrv_addr,  ZEND_ACC_PUBLIC)
    PHP_ME(MQProducer, send,            arginfo_producer_send,              ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};


static zend_object_handlers producer_object_handlers;

zend_object * producer_obj_new(zend_class_entry *ce){
    producer_object *obj;
    obj = (producer_object *)ecalloc(1, sizeof(producer_object) + zend_object_properties_size(ce));
    obj -> inner = (producer_object_t *)ecalloc(1, sizeof(producer_object_t));
    obj->inner->producer = NULL;
    zend_object_std_init(&obj->zo, ce);
    object_properties_init(&obj->zo, ce);
    obj->zo.handlers = & producer_object_handlers;
    return &obj->zo;
}

static void producer_free_storage(zend_object *std) {
    zend_object_std_dtor(std);
}

