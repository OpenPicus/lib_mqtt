/** \file MQTT.h
 *  \brief MQTT communication protocol library
 */

/**
\mainpage Main page

 Library to manage the MQTT communication protocol.
*/
#include<HWlib.h>

#define MQTT_Reserved1 		0		//Reserved
#define MQTT_CONNECT 		0x10	//Client request to connect to Server		CLIENT->OK
#define MQTT_CONNACK 		0x20	//Connect Acknowledgment					RESPONSE->OK
#define MQTT_PUBLISH 		0x30 	//Publish message							IO  CLIENT->OK, RESPONSE->OK
#define MQTT_PUBACK 		0x40 	//Publish Acknowledgment					IO  CLIENT->OK, RESPONSE->OK
#define MQTT_PUBREC 		0x50 	//Publish Received							IO  CLIENT->OK, RESPONSE->OK
#define MQTT_PUBREL 		0x60	//Publish Release							IO  CLIENT->OK, RESPONSE->OK
#define MQTT_PUBCOMP 		0x70	//Publish Complete							IO  CLIENT->OK, RESPONSE->OK
#define MQTT_SUBSCRIBE 		0x80	//Client Subscribe request					CLIENT->OK
#define MQTT_SUBACK 		0x90	//Subscribe Acknowledgment					RESPONSE->OK
#define MQTT_UNSUBSCRIBE	0xA0	//Client Unsubscribe request				CLIENT->OK
#define MQTT_UNSUBACK 		0xB0	//Unsubscribe Acknowledgment				RESPONSE->OK
#define MQTT_PINGREQ 		0xC0	//PING Request								CLIENT->OK
#define MQTT_PINGRESP 		0xD0	//PING Response								RESPONSE->OK
#define MQTT_DISCONNECT 	0xE0	//Client is Disconnecting					CLIENT->OK
#define MQTT_Reserved2 		0xF0	//Reserved

#define MQTT_PROTOCOLVERSION 3

#define MQTT_QOS_0        0x00
#define MQTT_QOS_1        0x01
#define MQTT_QOS_2        0x02
#define MQTT_QOS_3        0x03

#define MQTT_MAX_RESPONSE 800

/*****************************************************************************
        MQTT_Response struct declaration
*****************************************************************************/

struct MQTT_Response
{
  BYTE 	COMMAND;
  QWORD LENGTH;
  BYTE 	RCODE;
  char  MESSAGE[MQTT_MAX_RESPONSE];
  BOOL 	READY;
  BOOL 	BUSY;
  BOOL  FLAG_128BIT;
};

/*****************************************************************************
        MQTT function declarations
*****************************************************************************/

void MQTT_Check_Response(char * response);
int MQTT_ConnectID(char * dest, char * id, int keepalive, BOOL cleanflag);
int MQTT_ConnectLOGIN(char * dest, char * id, int keepalive, BOOL cleanflag,char * user, char * password);
int MQTT_ConnectWILL(char * dest, char * id, int keepalive, BOOL cleanflag, char * willTopic, char * willMessage, BOOL willRetain, BYTE willQoS);
int MQTT_Connect(char * dest, char * id, int keepalive, char * user, char * password, char * willTopic, char * willMessage, BOOL cleanflag, BOOL willRetain, BYTE willQoS);
int MQTT_Disconnect(char * dest);
int MQTT_Pingreq(char * dest);
int MQTT_Puback(char * dest, int messID);
int MQTT_Pubcomp(char * dest, int messID);
QWORD MQTT_Publish(char * dest, char * message, char * topic, int messID, BYTE QoS);
int MQTT_Pubrec(char * dest, int messID);
int MQTT_Pubrel(char * dest, int messID);
int MQTT_Subscribe(char * dest, char * topic, int messID, BYTE QoS);
int MQTT_Unsubscribe(char * dest, char * topic, int messID);
BYTE MQTT_Response_Sniffer(TCP_SOCKET socket);
void MQTT_Last_Response_Message(char * dest);
QWORD MQTT_Last_Response_Length();
BOOL MQTT_Last_Response_Ready();
int statusTCP(TCP_SOCKET socket);


