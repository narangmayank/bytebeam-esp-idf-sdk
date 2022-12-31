/**
 * @file bytebeam_sdk.h
 * @brief This file contains definition of data types, function prototypes and macros which
 * can be used by applications to integrate SDK.
 */
#ifndef BYTEBEAM_SDK_H
#define BYTEBEAM_SDK_H

#include "mqtt_client.h"

/*This macro is used to debug the sdk, we will keep all the unnecessary print under this macro*/
#define DEBUG_BYTEBEAM_SDK false

/*This macro is used to specify the maximum number of actions that need to be handled for particular device*/
#define BYTEBEAM_NUMBER_OF_ACTIONS 10

/*This macro is used to specify the maximum log level that need to be handled for particular device*/
#define BYTEBEAM_LOG_LEVEL BYTEBEAM_LOG_LEVEL_INFO

#define BYTEBEAM_LOGX(ESP_LOGX, level, levelStr, tag, fmt, ...)                               \
     do {                                                                                     \
        if((level != BYTEBEAM_LOG_LEVEL_NONE) && (level <= bytebeam_log_level_get())) {       \
            if (bytebeam_log_publish(levelStr, tag, fmt, ##__VA_ARGS__) == BB_FAILURE) {      \
            ESP_LOGE(tag, "Failed To Publish Bytebeam Log !");                                \
            } else {                                                                          \
            ESP_LOGX(tag, fmt, ##__VA_ARGS__);                                                \
            }                                                                                 \
        }                                                                                     \
    } while (0)

#define BYTEBEAM_LOGE(tag, fmt, ...)  BYTEBEAM_LOGX(ESP_LOGE, BYTEBEAM_LOG_LEVEL_ERROR, "ERROR", tag, fmt, ##__VA_ARGS__)
#define BYTEBEAM_LOGW(tag, fmt, ...)  BYTEBEAM_LOGX(ESP_LOGW, BYTEBEAM_LOG_LEVEL_WARN, "WARN", tag, fmt, ##__VA_ARGS__)
#define BYTEBEAM_LOGI(tag, fmt, ...)  BYTEBEAM_LOGX(ESP_LOGI, BYTEBEAM_LOG_LEVEL_INFO, "INFO", tag, fmt, ##__VA_ARGS__)
#define BYTEBEAM_LOGD(tag, fmt, ...)  BYTEBEAM_LOGX(ESP_LOGD, BYTEBEAM_LOG_LEVEL_DEBUG, "DEBUG", tag, fmt, ##__VA_ARGS__)
#define BYTEBEAM_LOGV(tag, fmt, ...)  BYTEBEAM_LOGX(ESP_LOGV, BYTEBEAM_LOG_LEVEL_VERBOSE, "VERBOSE", tag, fmt, ##__VA_ARGS__)


typedef enum {
    BYTEBEAM_LOG_LEVEL_NONE,
    BYTEBEAM_LOG_LEVEL_ERROR,
    BYTEBEAM_LOG_LEVEL_WARN,
    BYTEBEAM_LOG_LEVEL_INFO,
    BYTEBEAM_LOG_LEVEL_DEBUG,
    BYTEBEAM_LOG_LEVEL_VERBOSE,
} bytebeam_log_level_t;

/**
 * @struct bytebeam_device_config_t
 * This sturct contains authentication configurations for particular device created on Bytebeam platform
 * @var bytebeam_device_config_t::ca_cert_pem
 * Certificate signed by CA
 * @var bytebeam_device_config_t::client_cert_pem
 * Client certificate
 * @var bytebeam_device_config_t::client_key_pem
 * Device private key
 * @var bytebeam_device_config_t::broker_uri
 * URL of the MQTT broker
 * @var bytebeam_device_config_t::device_id
 * Device identification issued by Bytebeam platform for particular device
 * @var bytebeam_device_config_t::project_id
 * Identification of tenant to which particular device belongs.
 */
typedef struct bytebeam_device_config_t {
    char *ca_cert_pem;
    char *client_cert_pem;
    char *client_key_pem;
    char broker_uri[100];
    char device_id[10];
    char project_id[100];
} bytebeam_device_config_t;

typedef esp_mqtt_client_handle_t bytebeam_client_handle_t;
typedef esp_mqtt_client_config_t bytebeam_client_config_t;

struct bytebeam_client;

/**
 * @struct bytebeam_action_functions_map_t
 * This sturct contains name and function pointer for particular action 
 * @var bytebeam_action_functions_map_t::name
 * Name of particular action 
 * @var bytebeam_action_functions_map_t::func
 * Pointer to action handler function for particular action
 */
typedef struct {
    const char *name;
    int (*func)(struct bytebeam_client *bytebeam_client, char *args, char *action_id);
} bytebeam_action_functions_map_t;

/**
 * @struct bytebeam_client_t
 * This struct contains all the configuration for instance of MQTT client
 * @var bytebeam_client_t::device_cfg
 * structure for all the tls authentication related configs
 * @var bytebeam_client_t::client
 * ESP MQTT client handle 
 * @var bytebeam_client_t::mqtt_cfg
 * ESP MQTT client configuration structure
 * @var bytebeam_client_t::action_funcs
 * Array containing action handler structure for all the configured actions on Bytebeam platform
 * @var bytebeam_client_t::connection_status
 * Connection status of MQTT client instance.
 */
typedef struct bytebeam_client {
    bytebeam_device_config_t device_cfg;
    bytebeam_client_handle_t client;
    bytebeam_client_config_t mqtt_cfg;
    bytebeam_action_functions_map_t action_funcs[BYTEBEAM_NUMBER_OF_ACTIONS];
    int connection_status;
} bytebeam_client_t;

/*Status codes propogated via functions*/
typedef enum {
    BB_SUCCESS = 0,
    BB_FAILURE = -1
} bytebeam_err_t;

/**
 * @brief Initializes bytebeam MQTT client.
 *
 * @param[in] bytebeam_client bytebeam client handle
 * 
 * @return
 *      BB_SUCCESS : Bytebeam Client successfully initialised
 *      BB_FAILURE : Bytebeam Client initialisation failed 
 */
bytebeam_err_t bytebeam_init(bytebeam_client_t *bytebeam_client);

/**
 * @brief Publish response message after particular action is completed.
 *
 * @param[in] bytebeam_client bytebeam client handle
 * @param[in] action_id       action id for particular action
 * 
 * @return
 *      BB_SUCCESS : Message publish successful
 *      BB_FAILURE : Message publish failed
 */
bytebeam_err_t bytebeam_publish_action_completed(bytebeam_client_t *bytebeam_client, char *action_id);

/**
 * @brief Publish response message after particular action execution has failed.
 *
 * @param[in] bytebeam_client bytebeam client handle
 * @param[in] action_id       action id for particular action
 * 
 * @return
 *      BB_SUCCESS : Message publish successful
 *      BB_FAILURE : Message publish failed
 */
bytebeam_err_t bytebeam_publish_action_failed(bytebeam_client_t *bytebeam_client, char *action_id);

/**
 * @brief Publish message indicating progress of action being executed.
 *
 * @param[in] bytebeam_client     bytebeam client handle
 * @param[in] action_id           action id for particular action
 * @param[in] progress_percentage action execution progress in percentage
 * 
 * @return
 *      BB_SUCCESS : Message publish successful
 *      BB_FAILURE : Message publish failed
 */
bytebeam_err_t bytebeam_publish_action_progress(bytebeam_client_t *bytebeam_client, char *action_id, int progress_percentage);

/**
 * @brief Publish message to particualar stream
 *
 * @param[in] bytebeam_client     bytebeam client handle
 * @param[in] stream_name         name of the target stream
 * @param[in] payload             message to publish
 * 
 * @return
 *      BB_SUCCESS : Message publish successful
 *      BB_FAILURE : Message publish failed
 */
bytebeam_err_t bytebeam_publish_to_stream(bytebeam_client_t *bytebeam_client, char *stream_name, char *payload);

/**
 * @brief Starts bytebeam MQTT client after client is initialised.
 *
 * @param[in] bytebeam_client bytebeam client handle
 * 
 * @return
 *      BB_SUCCESS : Bytebeam Client started successfully 
 *      BB_FAILURE : Bytebeam Client start failed 
 */
bytebeam_err_t bytebeam_start(bytebeam_client_t *bytebeam_client);

/**
 * @brief Adds action handler for handling particular action.
 *
 * @param[in] bytebeam_client bytebeam client handle
 * @param[in] func_ptr        pointer to action handler function
 * @param[in] func_name       action name 
 * 
 * @return
 *      BB_SUCCESS : Action handler added successfully
 *      BB_FAILURE : Failure in adding action handler 
 */
bytebeam_err_t bytebeam_add_action_handler(bytebeam_client_t *bytebeam_client, int (*func_ptr)(bytebeam_client_t *, char *, char *), char *func_name);

/**
 * @brief Remove action handler from the array
 *
 * @param[in] bytebeam_client bytebeam client handle
 * @param[in] func_name       action name 
 * 
 * @return
 *      BB_SUCCESS : Action handler removed successfully
 *      BB_FAILURE : Failure in removing action handler 
 */

bytebeam_err_t bytebeam_remove_action_handler(bytebeam_client_t *bytebeam_client, char *func_name);

/**
 * @brief Update action handler for handling particular action.
 *
 * @param[in] bytebeam_client bytebeam client handle
 * @param[in] new_func_ptr    pointer to action handler function
 * @param[in] func_name       action name 
 * 
 * @return
 *      BB_SUCCESS : Action handler updated successfully
 *      BB_FAILURE : Failure in updating action handler 
 */
bytebeam_err_t bytebeam_update_action_handler(bytebeam_client_t *bytebeam_client, int (*new_func_ptr)(bytebeam_client_t *, char *, char *), char *func_name);

/**
 * @brief print action handler array.
 *
 * @param[in] bytebeam_client bytebeam client handle
 * 
 * @return
 *      void
 */
void bytebeam_print_action_handler_array(bytebeam_client_t *bytebeam_client);

/**
 * @brief reset action handler array.
 *
 * @param[in] bytebeam_client bytebeam client handle
 * 
 * @return
 *      void
 */
void bytebeam_reset_action_handler_array(bytebeam_client_t *bytebeam_client);

/**
 * @brief Download and update Firmware image 
 *
 * @param[in] bytebeam_client bytebeam client handle
 * @param[in] payload_string  buffer which stores payload received with OTA update init request
 * @param[in] action_id       action id for OTA update which is received with OTA update init request 
 * 
 * @return
 *      BB_SUCCESS : FW downloaded and updated successfully
 *      BB_FAILURE : FW download failed
 */
bytebeam_err_t handle_ota(bytebeam_client_t *bytebeam_client, char *payload_string, char *action_id);

/**
 * @brief Set the bytebeam log client handle
 *
 * @param[in] bytebeam_client bytebeam client handle
 * 
 * @return
 *      void
 */
void bytebeam_log_set_client(bytebeam_client_t *bytebeam_client);

/**
 * @brief Set the bytebeam log level
 *
 * @param[in] bytebeam_log_level_t log level
 * 
 * @return
 *      void
 */
void bytebeam_log_level_set(bytebeam_log_level_t level);

/**
 * @brief Get the bytebeam log level
 *
 * @param
 *      void
 * 
 * @return
 *      bytebeam_log_level_t log level
 */
bytebeam_log_level_t bytebeam_log_level_get(void);

/**
 * @brief Publish Log to Bytebeam
 *
 * @note  This api works on bytebeam log client handle so make sure to set the bytebeam log handle
 *        before calling this api, If called without setting it will return BB_BB_FAILURE
 * 
 * @param[in] level     indicates log level
 * @param[in] tag       indicates log level
 * @param[in] fmt       variable arguments
 * 
 * @return
 *      BB_SUCCESS : Log publish successful
 *      BB_FAILURE : Log publish failed
 */
bytebeam_err_t bytebeam_log_publish(const char *level, const char *tag, const char *fmt, ...);

#endif /* BYTEBEAM_SDK_H */