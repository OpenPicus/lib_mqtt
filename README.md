lib_mqtt
========

Flyport library for the MQTT communication, released under GPL v.3.<br>
The library allows to use the MQTT communication.<br>
More info on http://wiki.openpicus.com and download it http://wiki.openpicus.com/index.php/MQTT.<br>
1) import files inside Flyport IDE using the external libs button.<br>
2) add following code example in FlyportTask.c:<br>

<pre>
#include "taskFlyport.h"
#include "MQTT.h"

extern struct MQTT_Response MQTT_Last_Response;

void FlyportTask()
{
	char cmd='X';
	BOOL flagCMD=FALSE;
	char bff[50];
	int RxLenTCP=0;
	int RxLenUART=0;
	TCP_SOCKET sock = INVALID_SOCKET;

	int cnt=0;
	
	// Flyport connects to default network
	//WFConnect(WF_DEFAULT);
	while(WFGetStat() != CONNECTED);
	vTaskDelay(25);
	UARTWrite(1,"Flyport Wi-fi G connected...hello world!\r\n");
	
	char buffer[250];
	int len=0;
	
	while(1)
	{
		while((RxLenTCP=TCPRxLen(sock))>0)
		{
			TCPRead(sock,bff,1);
			//UARTWriteCh(1,bff[0]);	
			MQTT_Check_Responce(bff[0]);
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
		if(MQTT_Last_Response.MQTT_READY==1)
		{
			MQTT_Last_Response.MQTT_READY=0;
			char db[250];
			sprintf(db,"\r\nCMD: %02x\r\nLEN: %2.1f\r\n",MQTT_Last_Response.MQTT_COMMAND,(double)MQTT_Last_Response.MQTT_LENGTH);
			UARTWrite(1,db);
			int i=0;
			UARTWrite(1,"Message: ");
			for(i=0;i<MQTT_Last_Response.MQTT_LENGTH;i++)
					UARTWriteCh(1,MQTT_Last_Response.MQTT_MESSAGE[i]);
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
				len = MQTT_Subscribe(buffer, "myPICUSHOUSE", 50, MQTT_QOS_0);
				
			if(len>0)
			{
				/*
				int i=0;
				for(i=0;i<len;i++)
					UARTWriteCh(1,buffer[i]);
				*/
				TCPWrite(sock,buffer,len);
				len=0;
			}
			cmd = 'X';
		}
	}
}
</pre>
