#define MQTT_Reserved1 		0		//Reserved
#define MQTT_CONNECT 		0x10	//Client request to connect to Server		CLIENT->OK
#define MQTT_CONNACK 		0x20	//Connect Acknowledgment
#define MQTT_PUBLISH 		0x30 	//Publish message							IO  CLIENT->OK
#define MQTT_PUBACK 		0x40 	//Publish Acknowledgment					IO FATTO SOLO COME RISPOSTA
#define MQTT_PUBREC 		0x50 	//Publish Received							IO FATTO SOLO COME RISPOSTA
#define MQTT_PUBREL 		0x60	//Publish Release							IO FATTO SOLO COME RISPOSTA
#define MQTT_PUBCOMP 		0x70	//Publish Complete							IO FATTO SOLO COME RISPOSTA
#define MQTT_SUBSCRIBE 		0x80	//Client Subscribe request
#define MQTT_SUBACK 		0x90	//Subscribe Acknowledgment
#define MQTT_UNSUBSCRIBE	0xA0	//Client Unsubscribe request
#define MQTT_UNSUBACK 		0xB0	//Unsubscribe Acknowledgment
#define MQTT_PINGREQ 		0xC0	//PING Request
#define MQTT_PINGRESP 		0xD0	//PING Response
#define MQTT_DISCONNECT 	0xE0	//Client is Disconnecting					CLIENT->OK
#define MQTT_Reserved2 		0xF0	//Reserved

#define MQTT_PROTOCOLVERSION 3

#define MQTT_QOS_0        0x00
#define MQTT_QOS_1        0x02
#define MQTT_QOS_2        0x04
#define MQTT_QOS_3        0x06

#define MQTT_MAX_RESPONSE 800

struct MQTT_Response
{
  BYTE 	MQTT_COMMAND;
  QWORD MQTT_LENGTH;
  BYTE 	MQTT_RCODE;
  int 	MQTT_ID;
  BYTE	MQTT_QoS;
  BOOL 	MQTT_READY;
};

int MQTT_ConnectID(char * dest, char * id, int keepalive, BOOL cleanflag);
int MQTT_ConnectLOGIN(char * dest, char * id, int keepalive, BOOL cleanflag,char * user, char * password);
int MQTT_ConnectWILL(char * dest, char * id, int keepalive, BOOL cleanflag, char * willTopic, char * willMessage, BOOL willRetain, BYTE willQoS);
int MQTT_Connect(char * dest, char * id, int keepalive, char * user, char * password, char * willTopic, char * willMessage, BOOL cleanflag, BOOL willRetain, BYTE willQoS);
int MQTT_Disconnect(char * dest);
void MQTT_Check_Responce(char data);
int MQTT_Publish(char * dest, char * message, char * topic, int messID, BYTE QoS);
int MQTT_Subscribe(char * dest, char * topic, int messID, BYTE QoS);

