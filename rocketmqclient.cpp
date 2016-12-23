/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

extern "C" {
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"
}
#include "php_rocketmqclient.h"
ZEND_DECLARE_MODULE_GLOBALS(rocketmqclient)
#include "producer.h"
#include "consumer.h"

/* If you declare any globals in php_rocketmqclient.h uncomment this:
*/

/* True global resources - no need for thread safety here */
static int le_rocketmqclient;

/* {{{ PHP_INI
PHP_INI_BEGIN()
PHP_INI_END()
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("rocketmqclient.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_rocketmqclient_globals, rocketmqclient_globals)
    STD_PHP_INI_ENTRY("rocketmqclient.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_rocketmqclient_globals, rocketmqclient_globals)
PHP_INI_END()
*/
/* }}} */


PHP_MINIT_FUNCTION(rocketmqclient)
{

#if defined(ZTS) && defined(COMPILE_DL_ROCKETMQCLIENT)
ZEND_TSRMLS_CACHE_EXTERN();
#endif

    zend_class_entry mqProducer;
    INIT_CLASS_ENTRY(mqProducer, "MQProducer", producer_class_functions);
    mqProducer_ce = zend_register_internal_class(&mqProducer);
    mqProducer_ce->create_object = producer_obj_new;

    memcpy(&producer_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    producer_object_handlers.offset = XtOffsetOf(producer_object, zo);
    
    zend_class_entry mqConsumer;
    INIT_CLASS_ENTRY(mqConsumer, "MQConsumer", consumer_class_functions);

    mqConsumer_ce = zend_register_internal_class(&mqConsumer);
    mqConsumer_ce->create_object = consumer_obj_new;

    memcpy(&consumer_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    consumer_object_handlers.offset = XtOffsetOf(consumer_object, zo);
    consumer_object_handlers.dtor_obj = zend_objects_destroy_object;
    consumer_object_handlers.free_obj = consumer_free_storage;
    consumer_object_handlers.get_gc = consumer_get_gc;

//    MQ_G(call_func).fci.size = 0;
//    ZVAL_UNDEF(&(MQ_G(call_func).object));
    
//    ZVAL_UNDEF(&rocketmqclient_globals->call_func.object);
//    rocketmqclient_globals->call_func.fci.size = 0;
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(rocketmqclient)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(rocketmqclient)
{
#if defined(COMPILE_DL_ROCKETMQCLIENT) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(rocketmqclient)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(rocketmqclient)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "rocketmqclient support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ rocketmqclient_functions[]
 *
 * Every user visible function must have an entry in rocketmqclient_functions[].
 */
const zend_function_entry rocketmqclient_functions[] = {
	PHP_FE_END	/* Must be the last line in rocketmqclient_functions[] */
};
/* }}} */


/* {{{ rocketmqclient_module_entry
 */
zend_module_entry rocketmqclient_module_entry = {
	STANDARD_MODULE_HEADER,
	"rocketmqclient",
    rocketmqclient_functions,
	PHP_MINIT(rocketmqclient),
	PHP_MSHUTDOWN(rocketmqclient),
	PHP_RINIT(rocketmqclient),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(rocketmqclient),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(rocketmqclient),
	PHP_ROCKETMQCLIENT_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_ROCKETMQCLIENT
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE();
#endif
ZEND_GET_MODULE(rocketmqclient)
#endif
