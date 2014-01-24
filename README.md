lib_mqtt
========

Flyport library for the MQTT communication, released under GPL v.3.<br>
The library allows to use the MQTT communication.<br>
More info on http://wiki.openpicus.com and download it http://wiki.openpicus.com/index.php/MQTT.<br>
Inspired by http://knolleary.net/arduino-client-for-mqtt/<br>
<br>
1) import files inside Flyport IDE using the external libs button.<br>
2) add following code example in FlyportTask.c:<br>

<pre>
#include "taskFlyport.h"
#include "MQTT.h"

void FlyportTask()
{
	char cmd='X';
	BOOL flagCMD=FALSE;
	char bff[5];
	int RxLenUART=0;
	TCP_SOCKET sock = INVALID_SOCKET;

	int cnt=0;

	// Flyport connects to default network
	WFConnect(WF_DEFAULT);
	while(WFGetStat() != CONNECTED);
	vTaskDelay(25);
	UARTWrite(1,"Flyport Wi-fi G connected...hello world!\r\n");
	
	char buffer[250];
	int len=0;
	
	while(1)
	{
		switch (MQTT_Response_Sniffer(sock))
		{
			case MQTT_CONNACK:
				UARTWrite(1,"\r\nCONNACK detected\r\n");
			break;
			case MQTT_PUBLISH:
				{
					UARTWrite(1,"\r\nPUBLISH detected\r\n");
					QWORD lenmess=MQTT_Last_Response_Length();
					char message[lenmess];
					MQTT_Last_Response_Message(message);
					QWORD i=0;
					for(i=0;i<lenmess;i++)
						UARTWriteCh(1,message[i]);
				}
			break;
			case MQTT_PUBACK:				
				UARTWrite(1,"\r\nPUBACK detected\r\n");
			break;
			case MQTT_PUBREC:				
				UARTWrite(1,"\r\nPUBREC detected\r\n");
			break;
			case MQTT_PUBCOMP:				
				UARTWrite(1,"\r\nPUBCOMP detected\r\n");
			break;
			case MQTT_SUBACK:				
				UARTWrite(1,"\r\nSUBACK detected\r\n");
			break;
			case MQTT_UNSUBACK:				
				UARTWrite(1,"\r\nUNSUBACK detected\r\n");
			break;
			case MQTT_PINGRESP:				
				UARTWrite(1,"\r\nPINGRESP detected\r\n");
			break;
			case 1:				
				UARTWrite(1,"\r\nTCP Socket is disconnected\r\n");
			break;
		}
	
		while((RxLenUART=UARTBufferSize(1))>0)
		{
			UARTRead(1,bff,1);
			//UARTWriteCh(1,bff[0]);
			if(flagCMD)
			{
				cmd = bff[0];
				flagCMD = FALSE;
			}
			if(bff[0]=='#')	
				flagCMD=TRUE;
		}

		if(cmd!='X')
		{
			if(cmd=='Q')
			{
				sock = TCPClientOpen("test.mosquitto.org","1883");
				while(!TCPisConn(sock))
				{
					if(cnt==30)
					{
						UARTWrite(1,"Timeout");
						sock = INVALID_SOCKET;
						break;
					}
					vTaskDelay(25);
					cnt++;
				}
				if(TCPisConn(sock))
				{
					UARTWrite(1,"TCP is connected to the Server");
				}
			}
			else if(cmd=='W')
				len = MQTT_ConnectID(buffer, "myFLYPORT", 120, 1);
			else if(cmd=='E')
				len = MQTT_ConnectWILL(buffer, "myFLYPORT", 120, 1,"myPICUSHOUSE","myPICUSHOUSE is disconnected",0,MQTT_QOS_0);
			else if(cmd=='R')
				len = MQTT_Disconnect(buffer);
			else if(cmd=='T')
				len = MQTT_Publish(buffer, "{\"value\":\"32.8 C\"}", "myPICUSHOUSE", 50, MQTT_QOS_1);
			else if(cmd=='Y')
				len = MQTT_Publish(buffer, "{\"value\":\"32.8 C\"}", "myPICUSHOUSE", 50, MQTT_QOS_2);
			else if(cmd=='U')
				len = MQTT_Pubrel(buffer, 50);
			else if(cmd=='I')
				len = MQTT_Pingreq(buffer);
			else if(cmd=='O')
				len = MQTT_Subscribe(buffer, "myPICUSHOUSE/WEST/#", 50, MQTT_QOS_0);
			
			if(len>0)
				TCPWrite(sock,buffer,len);
				
			len=0;
			cmd = 'X';
		}
	}
}
</pre>
