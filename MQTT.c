/** \file MQTT.c
 *  \brief MQTT communication protocol library
 */

/* **************************************************************************
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 *
 *            openSource wireless Platform for sensors and Internet of Things
 * **************************************************************************
 *  FileName:        MQTT.c
 *  Module:          FlyPort
 *  Compiler:        Microchip C30 v3.12 or higher
 *
 *  Author               Rev.    Date              Comment
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Stefano Saccucci     1.0     01/23/2014        First release  (core team)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Software License Agreement
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  This is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License (version 2) as published by
 *  the Free Software Foundation AND MODIFIED BY OpenPicus team.
 *	
 *  Inspired by http://knolleary.net/arduino-client-for-mqtt/
 *
 *  ***NOTE*** The exception to the GPL is included to allow you to distribute
 *  a combined work that includes OpenPicus code without being obliged to
 *  provide the source code for proprietary components outside of the OpenPicus
 *  code.
 *  OpenPicus software is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 *
 *
 * Warranty
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * WE ARE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 **************************************************************************/

/**
\defgroup MQTT
@{

The MQTT library contains all the command to use the MQTT communication protocol.
*/

#include "taskFlyport.h"
#include "MQTT.h"


struct MQTT_Response MQTT_Last_Response;

/// @cond debug
char response_temp[MQTT_MAX_RESPONSE];

QWORD z=0;

int multiplier = 0; 
QWORD value = 0;
int p = 1;
/// @endcond

/**
 * Function to send the CONNECT message with only the ID
 * \param dest - pointer in which to store the message
 * \param id - string with the client id
 * \param keepalive - keep alive interval in seconds
 * \param cleanflag - clean session flag
 * \return the message length
 */
int MQTT_ConnectID(char * dest, char * id, int keepalive, BOOL cleanflag)
{
	return MQTT_Connect(dest, id, keepalive, NULL, NULL, NULL, NULL, cleanflag, NULL, NULL);
}

/**
 * Function to send the CONNECT message with the ID and USER-PASSWORD
 * \param dest - pointer in which to store the message
 * \param id - string with the client id
 * \param keepalive - keep alive interval in seconds
 * \param cleanflag - clean session flag
 * \param user - string with the login user
 * \param password - string with the login password
 * \return the message length
 */
int MQTT_ConnectLOGIN(char * dest, char * id, int keepalive, BOOL cleanflag, char * user, char * password)
{
	return MQTT_Connect(dest, id, keepalive, user, password, NULL, NULL, cleanflag, NULL, NULL);
}

/**
 * Function to send the CONNECT message with the ID and WILL message
 * \param dest - pointer in which to store the message
 * \param id - string with the client id
 * \param keepalive - keep alive interval in seconds
 * \param cleanflag - clean session flag
 * \param willTopic - string with the will message topic
 * \param willMessage - string with the will message
 * \param willRetain - will message retain flag
 * \param willQoS - will message QoS mode - MQTT_QOS_0, MQTT_QOS_1, MQTT_QOS_2, MQTT_QOS_3
 * \return the message length
 */
int MQTT_ConnectWILL(char * dest, char * id, int keepalive, BOOL cleanflag, char * willTopic, char * willMessage, BOOL willRetain, BYTE willQoS)
{
	return MQTT_Connect(dest, id, keepalive, NULL, NULL, willTopic, willMessage, cleanflag, willRetain, willQoS);
}

/**
 * Function to send the CONNECT message
 * \param dest - pointer in which to store the message
 * \param id - string with the client id
 * \param keepalive - keep alive interval in seconds
 * \param user - string with the login user
 * \param password - string with the login password
 * \param willTopic - string with the will message topic
 * \param willMessage - string with the will message
 * \param cleanflag - clean session flag
 * \param willRetain - will message retain flag
 * \param willQoS - will message QoS mode - MQTT_QOS_0, MQTT_QOS_1, MQTT_QOS_2, MQTT_QOS_3
 * \return the message length
 */
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
			temp[i]=temp[i]|(willQoS<<3);
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

/**
 * Function to check the server response
 * \param response - response message pointer
 */
void MQTT_Check_Response(char * response)
{
	if(MQTT_Last_Response.BUSY==0)
	{
		MQTT_Last_Response.COMMAND=response[0]&0xF0;
		multiplier = 1;
		value = 0;		
		MQTT_Last_Response.READY=0;
		MQTT_Last_Response.RCODE=0xFF;
		MQTT_Last_Response.LENGTH=0;		
		value += (response[1]&127) * multiplier;		
		MQTT_Last_Response.FLAG_128BIT=0;		
		if((response[1]&128) != 0)
		{
			multiplier *= 128;
			MQTT_Last_Response.FLAG_128BIT=1;
		}		
		MQTT_Last_Response.LENGTH=value;
		MQTT_Last_Response.BUSY=1;
	}
	else if(MQTT_Last_Response.BUSY==1)
	{
		if(MQTT_Last_Response.FLAG_128BIT==1)
		{
			value += (response[0]&127) * multiplier;
			MQTT_Last_Response.FLAG_128BIT=0;
			if((response[0]&128) != 0)
			{
				multiplier *= 128;
				MQTT_Last_Response.FLAG_128BIT=1;
			}
			MQTT_Last_Response.LENGTH=value;
		}
		else
		{
			int i=0;
			for(i=0;i<value;i++)
					MQTT_Last_Response.MESSAGE[i]=response[i];
			
			switch (MQTT_Last_Response.COMMAND)
			{
				case MQTT_CONNACK:
					MQTT_Last_Response.RCODE=response[1];
				break;

				default:				
				break;		
			}
			MQTT_Last_Response.BUSY=0;
			MQTT_Last_Response.FLAG_128BIT=0;
			MQTT_Last_Response.READY=1;
		}
	}
}

/**
 * Function to send the DISCONNECT message
 * \param dest - pointer in which to store the message
 * \return the message length
 */
int MQTT_Disconnect(char * dest)
{
	dest[0]=MQTT_DISCONNECT;
	dest[1]=0x00;
	return 2;
}

/**
 * Function to send the PUBLISH message
 * \param dest - pointer in which to store the message
 * \param message - string with the message that you want to publish
 * \param topic - string with the message topic
 * \param messID - message id number
 * \param willQoS - will message QoS mode - MQTT_QOS_0, MQTT_QOS_1, MQTT_QOS_2, MQTT_QOS_3
 * \return the message length
 */
QWORD MQTT_Publish(char * dest, char * message, char * topic, int messID, BYTE QoS)
{
	int lenTopic=strlen(topic);
	int lenMess=strlen(message);
	char temp[lenTopic+lenMess+100];
	
	QWORD i=0;
	
	temp[i++]=MQTT_PUBLISH|(QoS<<1);
	
	temp[i++] = lenTopic>>8;
	temp[i++] = lenTopic;
	
	QWORD k=0;
	
	for(k=0;k<lenTopic;k++)
		temp[i++]=topic[k];
		
	if(QoS>0)
	{
		temp[i++] = messID>>8;
		temp[i++] = messID;
	}

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
	
	QWORD j=0;
	
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

/**
 * Function to send the SUBSCRIBE message
 * \param dest - pointer in which to store the message
 * \param topic - string with the topic in which you want to subscribe
 * \param messID - message id number
 * \param willQoS - will message QoS mode - MQTT_QOS_0, MQTT_QOS_1, MQTT_QOS_2, MQTT_QOS_3
 * \return the message length
 */
int MQTT_Subscribe(char * dest, char * topic, int messID, BYTE QoS)
{
	int lenTopic=strlen(topic);
	char temp[lenTopic+100];
	
	int i=0;
	
	temp[i++]=MQTT_SUBSCRIBE|(MQTT_QOS_1<<1);
	
	temp[i++] = messID>>8;
	temp[i++] = messID;
	
	temp[i++] = lenTopic>>8;
	temp[i++] = lenTopic;
	
	int k=0;
	
	for(k=0;k<lenTopic;k++)
		temp[i++]=topic[k];

	temp[i++]=QoS;	
		
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

/**
 * Function to send the PUBACK message
 * \param dest - pointer in which to store the message
 * \param messID - message id number
 * \return the message length
 */
int MQTT_Puback(char * dest, int messID)
{
	dest[0]=MQTT_PUBACK;
	dest[1]=0x02;
	dest[2]=messID>>8;
	dest[3]=messID;
	return 4;
}

/**
 * Function to send the PUBREC message
 * \param dest - pointer in which to store the message
 * \param messID - message id number
 * \return the message length
 */
int MQTT_Pubrec(char * dest, int messID)
{
	dest[0]=MQTT_PUBREC;
	dest[1]=0x02;
	dest[2]=messID>>8;
	dest[3]=messID;
	return 4;
}

/**
 * Function to send the PUBREL message
 * \param dest - pointer in which to store the message
 * \param messID - message id number
 * \return the message length
 */
int MQTT_Pubrel(char * dest, int messID)
{
	dest[0]=MQTT_PUBREL;
	dest[1]=0x02;
	dest[2]=messID>>8;
	dest[3]=messID;
	return 4;
}

/**
 * Function to send the PUBCOMP message
 * \param dest - pointer in which to store the message
 * \param messID - message id number
 * \return the message length
 */
int MQTT_Pubcomp(char * dest, int messID)
{
	dest[0]=MQTT_PUBCOMP;
	dest[1]=0x02;
	dest[2]=messID>>8;
	dest[3]=messID;
	return 4;
}

/**
 * Function to send the UNSUBSCRIBE message
 * \param dest - pointer in which to store the message
 * \param topic - string with the topic in which you want to unsubscribe
 * \param messID - message id number
 * \return the message length
 */
int MQTT_Unsubscribe(char * dest, char * topic, int messID)
{
	int lenTopic=strlen(topic);
	char temp[lenTopic+100];
	
	int i=0;
	
	temp[i++]=MQTT_UNSUBSCRIBE|MQTT_QOS_1;
	
	temp[i++] = messID>>8;
	temp[i++] = messID;
	
	temp[i++] = lenTopic>>8;
	temp[i++] = lenTopic;
	
	int k=0;
	
	for(k=0;k<lenTopic;k++)
		temp[i++]=topic[k];

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

/**
 * Function to send the PINGREQ message
 * \param dest - pointer in which to store the message
 * \return the message length
 */
int MQTT_Pingreq(char * dest)
{
	dest[0]=MQTT_PINGREQ;
	dest[1]=0x00;
	return 2;
}

/**
 * Function to intercept the MQTT response
 * \param socket - the handle of the socket to use
 * \return the hex (see defines) of MQTT message type or 0.
 */
BYTE MQTT_Response_Sniffer(TCP_SOCKET socket)
{
	int len=0;
	while((len=statusTCP(socket))>0)
	{
		if(len>1&&MQTT_Last_Response.BUSY==0)
		{
			#if defined (FLYPORTGPRS)
				TCPRead(&socket,response_temp,2);	
			#else
				TCPRead(socket,response_temp,2);
			#endif
			MQTT_Check_Response(response_temp);
		}
		else if(MQTT_Last_Response.BUSY==1&&MQTT_Last_Response.FLAG_128BIT==1)
		{
			#if defined (FLYPORTGPRS)
				TCPRead(&socket,response_temp,1);
			#else
				TCPRead(socket,response_temp,1);
			#endif
			MQTT_Check_Response(response_temp);
		}
		else if(MQTT_Last_Response.BUSY==1&&MQTT_Last_Response.FLAG_128BIT==0)
		{
			if(len>(MQTT_Last_Response.LENGTH-1))
			{
				#if defined (FLYPORTGPRS)
					TCPRead(&socket,response_temp,MQTT_Last_Response.LENGTH);
				#else
					TCPRead(socket,response_temp,MQTT_Last_Response.LENGTH);
				#endif
				MQTT_Check_Response(response_temp);
				return MQTT_Last_Response.COMMAND;
			}
		}
	}
	return 0;
}

/**
 * Function to get the last MQTT response message
 * \param dest - pointer in which to store the message
 */
void MQTT_Last_Response_Message(char * dest)
{
	QWORD i=0;
	for(i=0;i<MQTT_Last_Response.LENGTH;i++)
		dest[i]=MQTT_Last_Response.MESSAGE[i];
		
	MQTT_Last_Response.READY=0;	//new
}

/**
 * Function to get the last MQTT response length
 * \return last MQTT response length
 */
QWORD MQTT_Last_Response_Length()
{
	return MQTT_Last_Response.LENGTH;
}

/**
 * Function to get the last MQTT Ready flag
 * \return last MQTT ready flag
 */
BOOL MQTT_Last_Response_Ready()
{
	return MQTT_Last_Response.READY;
}

int statusTCP(TCP_SOCKET socket)
{
	#if ( defined (FLYPORT_WF) || defined (FLYPORT_ETH) )
		if(TCPisConn(socket)==TRUE)
			return TCPRxLen(socket);
		else
			return 0;
	#else
		BYTE blackout=0;
		TCPStatus(&socket);
		while(LastExecStat() == OP_EXECUTION);
		vTaskDelay(20);
		blackout=0;
		for(blackout=0; blackout<10;blackout++)
		{
			if(LastExecStat() == OP_SUCCESS)
			{				
				if(socket.status == SOCK_CONNECT)
				return socket.rxLen;
			}
			vTaskDelay(20);
		}
		return 0;
	#endif			
}


