#include "CarpManager.h"

CarpManager::CarpManager() 
{
	printf("CarpManager Start \n");
	iSdForRedirectionARP = INVALID_SOCKET;	
}


CarpManager::~CarpManager() 
{
	printf("CarpManager End \n");
}



int CarpManager::initSocket() 
{
	// socket()
	if((iSdForRedirectionARP = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP)) ) < 0 ) {
		printf ("SD Socket error\n");
		return SOCKET_ERROR;
	}

	// get ifindex -- ioctl()
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, sAdaptorName, sizeof(ifr.ifr_name));

	if(ioctl (iSdForRedirectionARP, SIOCGIFINDEX, &ifr) == -1)
	{
		printf ("[%s] Adapter error\n", sAdaptorName);
		return SOCKET_ERROR;
	}

	memset(&sll, 0, sizeof(sll));
	sll.sll_family          = AF_PACKET;
	sll.sll_ifindex         = ifr.ifr_ifindex;
	sll.sll_protocol        = htons(ETH_P_ARP);

	return 0;
}


