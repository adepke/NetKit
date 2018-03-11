/*
	Copyright (c) 2017-2018 Andrew Depke
*/
#pragma once

#include "Platform.h"
#include "Packet.h"

#include <stdlib.h>
#include <stdio.h>

int CreateSocket(int Protocol)
{
	int Result = socket(PF_PACKET, SOCK_RAW, Protocol);
	if (Result == -1)
	{
		printf("CreateSocket Failed\n");

		return -1;
	}

	int Option = 1;

	if (setsockopt(Result, IPPROTO_IP, IP_HDRINCL, (char*)&Option, sizeof(Option)) != 0)
	{
		printf("CreateSocket Failed at Setting Custom Headers\n");

		return Result;
	}

	return Result;
}

int BindSocket(int Socket, unsigned long Address, int Port)
{
	struct sockaddr_in Data;
	Data.sin_family = AF_PACKET;
	Data.sin_addr.s_addr = htonl(Address);
	Data.sin_port = htons(Port);

	return bind(Socket, (struct sockaddr*)&Data, sizeof(Data));
}

int SendPacket(int Socket, unsigned long DestinationAddress, int DestinationPort, struct Packet* Target)
{
	struct sockaddr_in Destination;
	Destination.sin_addr.s_addr = htonl(DestinationAddress);
	Destination.sin_port = htons(DestinationPort);

	int Sent = sendto(Socket, (char*)Target->Raw, (int)Target->Length, 0, (struct sockaddr*)&Destination, sizeof(Destination));

	if (Sent != (int)Target->Length)
	{
		return 1;
	}

	return 0;
}

void ShutdownSocket(int Socket)
{
	shutdown(Socket, 2);
#ifdef PLATFORM_WINDOWS
	closesocket(Socket);
#else
	close(Socket);
#endif
}