#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <map>
#include <set>

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <rocketmq/DefaultMQPullConsumer.h>
#include <rocketmq/Message.h>
#include <rocketmq/MessageExt.h>
#include <rocketmq/MessageQueue.h>
#include <rocketmq/PullResult.h>
#include <rocketmq/OffsetStore.h>
#include <rocketmq/ConsumeType.h>
#include <rocketmq/MQClientException.h>

typedef struct {
    DefaultMQPullConsumer* consumer;
    char *topic;
    char *tags;
} consumer_object_t;

typedef struct {
    consumer_object_t *inner;
    zend_object zo;
} consumer_object;

zend_class_entry *mqConsumer_ce;

static inline consumer_object_t* fetch_consumer_inner(zend_object *obj) {
    return (consumer_object_t* )(((consumer_object *)(((char *) obj - XtOffsetOf(consumer_object, zo))))->inner);
}

//static inline zend_object;

#define F_Consumer_Ptr(zv) fetch_consumer_inner(Z_OBJ_P((zv)))


// __construct
ZEND_BEGIN_ARG_INFO_EX(arginfo_consumer___construct, 0, 0, 1)
    ZEND_ARG_INFO(0, consumer_name)
    ZEND_ARG_INFO(0, consumer_group)
ZEND_END_ARG_INFO()

PHP_METHOD(MQConsumer, __construct) {
    zval* object = getThis();
    consumer_object_t *inner = F_Consumer_Ptr(object); 
    char *consumer_group;
    size_t consumer_group_len;


    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "s", &consumer_group, &consumer_group_len)) {
        RETURN_FALSE;
    }

    if (!consumer_group_len) {
        zend_error(E_WARNING, "set consumer_name empty.");
    }
    
    inner->consumer = new DefaultMQPullConsumer(consumer_group);
}

PHP_METHOD(MQConsumer, __destruct){
    zval* object = getThis();
    consumer_object_t *inner = F_Consumer_Ptr(object); 
    delete inner->consumer;
}


// set nameservers
ZEND_BEGIN_ARG_INFO(arginfo_consumer_set_namesrv_addr, 0)
    ZEND_ARG_INFO(0, nameservers)
ZEND_END_ARG_INFO()

PHP_METHOD(MQConsumer, setNamesrvAddr) {
    char *nameservers;
    size_t nameservers_len;
    zend_fcall_info fci;
    zend_fcall_info_cache fci_cache;

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "s", &nameservers, &nameservers_len)) {
        RETURN_FALSE;
    }


    consumer_object_t *inner = F_Consumer_Ptr(getThis());
    inner->consumer->setNamesrvAddr(nameservers);

    if (!nameservers_len) {
        zend_error(E_WARNING, "set nameservers empty.");
    }
}


// send message
ZEND_BEGIN_ARG_INFO(arginfo_consumer_set_topic, 0)
    ZEND_ARG_INFO(0, topic)
    ZEND_ARG_INFO(0, tag)
ZEND_END_ARG_INFO()

PHP_METHOD(MQConsumer, setTopic) {
    size_t topic_len, tags_len;
    consumer_object_t *inner = F_Consumer_Ptr(getThis());

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &inner->topic, &topic_len, &inner->tags,   &tags_len)) {
        RETURN_FALSE;
    }
}

bool consumer_message(PullResult& result, zend_fcall_info fci, zend_fcall_info_cache fci_cc)
{
    zval params[2];
    zval retval;
    bool ret;

    std::list<MessageExt*>::iterator it = result.msgFoundList.begin();

    for (;it!=result.msgFoundList.end();it++)
    {
        MessageExt* me = *it;
        std::string str;
        str.assign(me->getBody(),me->getBodyLen());
        ZVAL_STRING(&params[0], (char *) str.c_str());
        array_init(&params[1]);
        add_assoc_long(&params[1], "pullStatus", (int)result.pullStatus);
        add_assoc_long(&params[1], "nextBeginOffset", result.nextBeginOffset);
        add_assoc_long(&params[1], "minOffset", result.minOffset);
        add_assoc_long(&params[1], "maxOffset", result.maxOffset);
        add_assoc_string(&params[1], "msgId", (char *)me->getMsgId().c_str());

        fci.params = params;
        fci.param_count = 2;
        fci.retval = &retval;
        
        zend_call_function(&fci, &fci_cc);
        if (Z_TYPE(retval) == IS_TRUE){
        }else{
            
        }
        zval_ptr_dtor(&params[0]);
        zval_ptr_dtor(&params[1]);
        zval_ptr_dtor(&retval);
    }
}


//registerMessageListener
ZEND_BEGIN_ARG_INFO(arginfo_consumer_on, 0)
    ZEND_ARG_INFO(0, function_name)
ZEND_END_ARG_INFO()

PHP_METHOD(MQConsumer, on) {
    zend_fcall_info fci;
    zend_fcall_info_cache fci_cache;
    long long offset;
    bool noNewMsg = false;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "f*", &fci, &fci_cache, &fci.params, &fci.param_count) == FAILURE) {
        return;
    }

    consumer_object_t *inner = F_Consumer_Ptr(getThis());
    DefaultMQPullConsumer *consumer = inner->consumer;
    consumer->setMessageModel(CLUSTERING);
    consumer->start();
    std::set<MessageQueue>* mqs = inner->consumer->fetchSubscribeMessageQueues(inner->topic);
    std::set<MessageQueue>::iterator it = mqs->begin();
    
    const std::string tags(inner->tags);

    OffsetStore* offsetStore = inner->consumer->getOffsetStore();

    for (; it != mqs->end(); it ++){
        MessageQueue mq = *it;
        noNewMsg = false;
        offset = consumer->fetchConsumeOffset(mq, true);

        while (!noNewMsg){
            try {
                PullResult* pullResult = consumer->pull(mq, tags, offset, 32);
                switch (pullResult->pullStatus)
                {
                    case FOUND:
                        consumer_message(*pullResult, fci, fci_cache);
                        offset = pullResult->nextBeginOffset;
                        break;
                    case NO_NEW_MSG:
                        noNewMsg = true;
                        break;
                    case OFFSET_ILLEGAL:
                    case NO_MATCHED_MSG:
                        break;
                    default:
                        break;
                }
               delete pullResult;

            }catch(MQClientException& e){
                std::cout<<e<<std::endl;
            }
        }
        offsetStore->updateOffset(mq, offset, false);
    }
    offsetStore->persistAll(*mqs);

    delete mqs;
    consumer->shutdown();
}

const zend_function_entry consumer_class_functions[] = {
    PHP_ME(MQConsumer, __construct,     arginfo_consumer___construct,       ZEND_ACC_PUBLIC| ZEND_ACC_CTOR)
    PHP_ME(MQConsumer, __destruct,      NULL,                               ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(MQConsumer, setNamesrvAddr,  arginfo_consumer_set_namesrv_addr,  ZEND_ACC_PUBLIC)
    PHP_ME(MQConsumer, setTopic,        arginfo_consumer_set_topic,         ZEND_ACC_PUBLIC)
    PHP_ME(MQConsumer, on,              arginfo_consumer_on,                ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};


static zend_object_handlers consumer_object_handlers;

zend_object * consumer_obj_new(zend_class_entry *ce){
    consumer_object *obj;
    obj = (consumer_object *)ecalloc(1, sizeof(consumer_object) + zend_object_properties_size(ce));
    zend_object_std_init(&obj->zo, ce);
    object_properties_init(&obj->zo, ce);
    rebuild_object_properties(&obj->zo);

    obj -> inner = (consumer_object_t *)ecalloc(1, sizeof(consumer_object_t));
    obj->inner->consumer = NULL;

    obj->zo.handlers = &consumer_object_handlers;
    return &obj->zo;
}

static void consumer_free_storage(zend_object *std) {
    zend_object_std_dtor(std);
}

static HashTable *consumer_get_gc(zval *object, zval **gc_data, int *gc_count){
    
}
