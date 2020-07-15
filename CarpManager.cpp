#include "CarpManager.h"

CarpManager::CarpManager() 
{
	printf("CarpManager Start \n");

	tArpPacket.clear();
	iSdForRedirectionARP = INVALID_SOCKET;
	// strAdaptorName



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
	strncpy(ifr.ifr_name, strAdaptorName, sizeof(ifr.ifr_name));

	if(ioctl (iSdForRedirectionARP, SIOCGIFINDEX, &ifr) == -1)
	{
		printf ("[%s] Adapter error\n", strAdaptorName);
		return SOCKET_ERROR;
	}

	memset(&sll, 0, sizeof(sll));
	sll.sll_family          = AF_PACKET;
	sll.sll_ifindex         = ifr.ifr_ifindex;
	sll.sll_protocol        = htons(ETH_P_ARP);

	return 0;
}



bool CarpManager::readSettingFile(const char* path) 
{
	printf("readSettingFile start \n");
	char strType[1000]={0,};
	
	FILE* fp;
	if ((fp = fopen(path, "r")) == NULL) {
		printf("File Read Error: %s \n", path);
		return false;
	}
	printf("File Read Success : %s \n", path);

	fscanf(fp, "%s %s", strType, strAdaptorName);
	// if: diff
	if(0 != strncmp("ADAPTOR_NAME", strType, strlen("ADAPTOR_NAME"))) {
		printf("read Error ADAPTOR_NAME Type : %s \n ", strType);
		return false;
	}
	
	fscanf(fp, "%s %02x:%02x:%02x:%02x:%02x:%02x", strType, arp_src_mac[0], arp_src_mac[1], arp_src_mac[2], arp_src_mac[3], arp_src_mac[4], arp_src_mac[5]);
	if(0 != strncmp("SRC_ETH", strType, strlen("SRC_ETH"))) {
		printf("read Error SRC_ETH Type : %s \n ", strType);
		return false;
	}


	fclose(fp);
	return true;
}






