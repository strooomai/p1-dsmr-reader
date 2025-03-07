#ifndef MQTT_MODULE_H
#define MQTT_MODULE_H

/* Initializes the MQTT client and message queue.
 * Returns 0 on success, non-zero on error.
 */
int mqtt_module_init(void);

/* Enqueues a message for sending via MQTT.
 * 'topic' is the MQTT topic and 'payload' is the message to send.
 * Returns 0 if enqueued successfully.
 */
int mqtt_module_enqueue(const char *topic, const char *payload);

/* Processes the MQTT message queue.
 * Attempts to reconnect if necessary and publish any queued messages.
 */
void mqtt_module_process_queue(void);

/* Cleans up the MQTT client and frees any resources. */
void mqtt_module_cleanup(void);

#endif /* MQTT_MODULE_H */
