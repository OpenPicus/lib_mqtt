#include "TCPLib.h"
#include "MQTT.h"

struct MQTT_Response MQTT_Last_Response;

char response_temp[MQTT_MAX_RESPONSE];

QWORD z=0;

int multiplier = 0; 
QWORD value = 0;
int p = 1;

int MQTT_ConnectID(char * dest, char * id, int keepalive, BOOL cleanflag)
{
	return MQTT_Connect(dest, id, keepalive, NULL, NULL, NULL, NULL, cleanflag, NULL, NULL);
}

int MQTT_ConnectLOGIN(char * dest, char * id, int keepalive, BOOL cleanflag,char * user, char * password)
{
	return MQTT_Connect(dest, id, keepalive, user, password, NULL, NULL, cleanflag, NULL, NULL);
}

int MQTT_ConnectWILL(char * dest, char * id, int keepalive, BOOL cleanflag, char * willTopic, char * willMessage, BOOL willRetain, BYTE willQoS)
{
	return MQTT_Connect(dest, id, keepalive, NULL, NULL, willTopic, willMessage, cleanflag, willRetain, willQoS);
}

int MQTT_Connect(char * dest, char * id, int keepalive, char * user, char * password, char * willTopic, char * willMessage, BOOL cleanflag, BOOL willRetain, BYTE willQoS)
{
	int lenUser=0;
	int lenPass=0;
	int lenTopic=0;
	int lenMess=0;

	int lenId=strlen(id);
	
	if(user!=NULL)
	{
		lenUser=strlen(user);
		if(password!=NULL)
		{
			lenPass=strlen(password);
		}
	}
	if(willTopic!=NULL)
	{
		lenTopic=strlen(willTopic);
		lenMess=strlen(willMessage);
	}
	
	char temp[lenUser+lenPass+lenTopic+lenMess+lenId+100];
	
	int i=0;
	
	temp[i++]=MQTT_CONNECT;
	
	temp[i++]=0x00;
	temp[i++]=0x06;
	temp[i++]='M';
	temp[i++]='Q';
	temp[i++]='I';
	temp[i++]='s';
	temp[i++]='d';
	temp[i++]='p';
	temp[i++]=MQTT_PROTOCOLVERSION;
	
	temp[i]=0;
	
	if(user!=NULL)
	{
		temp[i]=dest[i]|0x80;
		if(password!=NULL)
			temp[i]=dest[i]|0x40;
	}

	if(willTopic!=NULL)
	{
		if(willRetain!=NULL)
			temp[i]=temp[i]|0x20;
		if(willQoS!=NULL)
			temp[i]=temp[i]|willQoS;
		temp[i]=temp[i]|0x04;
	}
	if(cleanflag==TRUE)
		temp[i]=temp[i]|0x02;
	i++;
	temp[i++]=keepalive>>8;
	temp[i++]=keepalive;
	
	temp[i++] = lenId>>8;
	temp[i++] = lenId;
	
	int k=0;
	
	for(k=0;k<lenId;k++)
		temp[i++]=id[k];

	if(lenTopic!=0)
	{
		temp[i++] = lenTopic>>8;
		temp[i++] = lenTopic;
		for(k=0;k<lenTopic;k++)
			temp[i++]=willTopic[k];
		temp[i++] = lenMess>>8;
		temp[i++] = lenMess;	
		for(k=0;k<lenMess;k++)
			temp[i++]=willMessage[k];			
	}
	if(lenUser!=0)
	{
		temp[i++] = lenUser>>8;
		temp[i++] = lenUser;	
		for(k=0;k<lenUser;k++)
			temp[i++]=user[k];
		if(lenPass!=0)
		{
			temp[i++] = lenPass>>8;
			temp[i++] = lenPass;	
			for(k=0;k<lenPass;k++)
				temp[i++]=password[k];
		}
	}
	
	k=0;
	char digit[5];
	
	int X=i-1;
	
	do
	{
		digit[k] = X%128;
		X = X/128;
		if (X>0)
			digit[k] = digit[k]|0x80;
		k++;
	}while(X>0);
	
	int j=0;
	
	dest[0]=temp[0];
	
	for(j=1;j<k+i;j++)
	{
		if(j>=1&&j<=k)
		{
			dest[j]=digit[j-1];
		}
		else
		{
			dest[j]=temp[j-k];
		}		
	}
	
	return j;
}

void MQTT_Check_Responce(char data)
{
	response_temp[z]=data;
	if(z==0)
	{
		multiplier = 1;
		value = 0;
		p=1;
		MQTT_Last_Response.MQTT_READY=0;
		MQTT_Last_Response.MQTT_RCODE=0xFF;
		MQTT_Last_Response.MQTT_ID=0;
		MQTT_Last_Response.MQTT_QoS=0;
	}
	else if(z<=p)
	{
		value += (data&127) * multiplier;
		if((data&128) != 0)
		{
			multiplier *= 128;
			p++;
		}
	}
	else if(z==value+1)
	{
		MQTT_Last_Response.MQTT_COMMAND=response_temp[0]&0xF0;
		MQTT_Last_Response.MQTT_LENGTH=value;
		MQTT_Last_Response.MQTT_READY=1;
		z=-1;
	}
	z++;
}

int MQTT_Disconnect(char * dest)
{
	dest[0]=MQTT_DISCONNECT;
	dest[1]=0x00;
	return 2;
}

int MQTT_Publish(char * dest, char * message, char * topic, int messID, BYTE QoS)
{
	int lenTopic=strlen(topic);
	int lenMess=strlen(message);
	char temp[lenTopic+lenMess+100];
	
	int i=0;
	
	temp[i++]=MQTT_PUBLISH|QoS;
	
	temp[i++] = lenTopic>>8;
	temp[i++] = lenTopic;
	
	int k=0;
	
	for(k=0;k<lenTopic;k++)
		temp[i++]=topic[k];

	temp[i++] = messID>>8;
	temp[i++] = messID;
		
	temp[i++] = lenMess>>8;
	temp[i++] = lenMess;
	for(k=0;k<lenMess;k++)
		temp[i++]=message[k];

	char digit[5];
	
	int X=i-1;
	k=0;
	do
	{
		digit[k] = X%128;
		X = X/128;
		if (X>0)
			digit[k] = digit[k]|0x80;
		k++;
	}while(X>0);
	
	int j=0;
	
	dest[0]=temp[0];
	
	for(j=1;j<k+i;j++)
	{
		if(j>=1&&j<=k)
		{
			dest[j]=digit[j-1];
		}
		else
		{
			dest[j]=temp[j-k];
		}		
	}
	
	return j;
}

int MQTT_Subscribe(char * dest, char * topic, int messID, BYTE QoS)
{
	int lenTopic=strlen(topic);
	char temp[lenTopic+100];
	
	int i=0;
	
	temp[i++]=MQTT_SUBSCRIBE|QoS;
	
	temp[i++] = messID>>8;
	temp[i++] = messID;
	
	temp[i++] = lenTopic>>8;
	temp[i++] = lenTopic;
	
	int k=0;
	
	for(k=0;k<lenTopic;k++)
		temp[i++]=topic[k];

	temp[i++]=QoS>>3;	
		
	char digit[5];
	
	int X=i-1;
	k=0;
	do
	{
		digit[k] = X%128;
		X = X/128;
		if (X>0)
			digit[k] = digit[k]|0x80;
		k++;
	}while(X>0);
	
	int j=0;
	
	dest[0]=temp[0];
	
	for(j=1;j<k+i;j++)
	{
		if(j>=1&&j<=k)
		{
			dest[j]=digit[j-1];
		}
		else
		{
			dest[j]=temp[j-k];
		}		
	}
	
	return j;
}



