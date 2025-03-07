/* MQTT implementation uses Eclipse MQTT C lib. 
    It sends DSMR telegram raw reading to MQTT server, and has a queue in case of network loss.
    DSMR messages comes at every 5 to 10 seconds, in future a lite db can be used to store mqtt mesaages for a long period of wifi error. 
    This is a trade off between a lightweight MQTT client vs more intelligence at the edge. 
*/

#include "mqtt_module.h"
#include "MQTTClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* MQTT connection settings */
#define ADDRESS     "tcp://your_broker_address:1883"
#define CLIENTID    "DSMR_Client"
#define TIMEOUT     10000L

/* Queue configuration */
#define QUEUE_SIZE  100

/* Security option constants */
#define INSECURE             0
#define SECURE_CERT_NONE     1
#define SECURE_CERT_REQUIRED 2

/* QoS option constants */
#define QOS_0 0
#define QOS_1 1
#define QOS_2 2

/* Configuration: change these as needed or read from a config file */
static int secure_option = SECURE_CERT_REQUIRED;  // Options: INSECURE, SECURE_CERT_NONE, SECURE_CERT_REQUIRED
static int qos_option = QOS_1;                     // Options: QOS_0, QOS_1, QOS_2

/* MQTT Message and Queue Structures */
typedef struct {
    char topic[256];
    char payload[1024];
} MQTTMessage;

typedef struct {
    MQTTMessage messages[QUEUE_SIZE];
    int front;
    int rear;
    int count;
} MessageQueue;

/* Static global MQTT client and queue */
static MQTTClient client;
static MessageQueue msgQueue;

/* Forward declarations for queue helper functions */
static void initQueue(MessageQueue *q);
static int enqueueMessage(MessageQueue *q, const char *topic, const char *payload);
static int dequeueMessage(MessageQueue *q, MQTTMessage *msg);

/* Initialize the message queue */
static void initQueue(MessageQueue *q) {
    q->front = 0;
    q->rear = -1;
    q->count = 0;
}

/* Enqueue a message into the queue */
static int enqueueMessage(MessageQueue *q, const char *topic, const char *payload) {
    if (q->count >= QUEUE_SIZE) {
        fprintf(stderr, "MQTT Queue is full. Message dropped.\n");
        return -1;
    }
    q->rear = (q->rear + 1) % QUEUE_SIZE;
    strncpy(q->messages[q->rear].topic, topic, sizeof(q->messages[q->rear].topic) - 1);
    strncpy(q->messages[q->rear].payload, payload, sizeof(q->messages[q->rear].payload) - 1);
    q->messages[q->rear].topic[sizeof(q->messages[q->rear].topic) - 1] = '\0';
    q->messages[q->rear].payload[sizeof(q->messages[q->rear].payload) - 1] = '\0';
    q->count++;
    return 0;
}

/* Dequeue a message from the queue */
static int dequeueMessage(MessageQueue *q, MQTTMessage *msg) {
    if (q->count <= 0) {
        return -1;
    }
    *msg = q->messages[q->front];
    q->front = (q->front + 1) % QUEUE_SIZE;
    q->count--;
    return 0;
}

/* Initialize the MQTT client and connect to the broker */
int mqtt_module_init(void) {
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;
    int rc;

    /* Create the MQTT client */
    rc = MQTTClient_create(&client, ADDRESS, CLIENTID,
                           MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (rc != MQTTCLIENT_SUCCESS) {
        fprintf(stderr, "Failed to create MQTT client, return code %d\n", rc);
        return rc;
    }

    /* Configure connection options */
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    /* Configure SSL/TLS options if needed */
    if (secure_option != INSECURE) {
        conn_opts.ssl = &ssl_opts;
        if (secure_option == SECURE_CERT_NONE) {
            /* Use TLS but ignore certificate validation errors */
            ssl_opts.enableServerCertAuth = 0;
        } else if (secure_option == SECURE_CERT_REQUIRED) {
            /* Use TLS and require a valid certificate */
            ssl_opts.enableServerCertAuth = 1;
            /* Specify your CA certificate location */
            ssl_opts.trustStore = "/path/to/ca.crt";
        }
    }

    rc = MQTTClient_connect(client, &conn_opts);
    if (rc != MQTTCLIENT_SUCCESS) {
        fprintf(stderr, "Initial connection to MQTT broker failed, return code %d\n", rc);
        /* Continue even if connection fails; messages will be queued */
    } else {
        printf("Connected to MQTT broker at %s\n", ADDRESS);
    }

    initQueue(&msgQueue);
    return 0;
}

/* Enqueue a message to be sent via MQTT */
int mqtt_module_enqueue(const char *topic, const char *payload) {
    return enqueueMessage(&msgQueue, topic, payload);
}

/* Process the message queue: try to publish all queued messages */
void mqtt_module_process_queue(void) {
    int rc;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;

    /* If not connected, attempt to reconnect */
    if (!MQTTClient_isConnected(client)) {
        conn_opts.keepAliveInterval = 20;
        conn_opts.cleansession = 1;
        if (secure_option != INSECURE) {
            conn_opts.ssl = &ssl_opts;
            if (secure_option == SECURE_CERT_NONE) {
                ssl_opts.enableServerCertAuth = 0;
            } else if (secure_option == SECURE_CERT_REQUIRED) {
                ssl_opts.enableServerCertAuth = 1;
                ssl_opts.trustStore = "/path/to/ca.crt";
            }
        }
        rc = MQTTClient_connect(client, &conn_opts);
        if (rc == MQTTCLIENT_SUCCESS) {
            printf("Reconnected to MQTT broker\n");
        } else {
            fprintf(stderr, "Reconnection attempt failed, return code %d\n", rc);
            return;
        }
    }

    /* Process queued messages */
    while (msgQueue.count > 0) {
        MQTTMessage msg;
        if (dequeueMessage(&msgQueue, &msg) == 0) {
            MQTTClient_message pubmsg = MQTTClient_message_initializer;
            pubmsg.payload = msg.payload;
            pubmsg.payloadlen = (int)strlen(msg.payload);
            pubmsg.qos = qos_option;
            pubmsg.retained = 0;
            MQTTClient_deliveryToken token;

            rc = MQTTClient_publishMessage(client, msg.topic, &pubmsg, &token);
            if (rc != MQTTCLIENT_SUCCESS) {
                fprintf(stderr, "Publish failed for topic %s, re-enqueueing message\n", msg.topic);
                enqueueMessage(&msgQueue, msg.topic, msg.payload);
                break; // Exit loop and try again later.
            } else {
                MQTTClient_waitForCompletion(client, token, TIMEOUT);
                printf("Published message to %s: %s\n", msg.topic, msg.payload);
            }
        }
    }
}

/* Clean up the MQTT client */
void mqtt_module_cleanup(void) {
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
}
