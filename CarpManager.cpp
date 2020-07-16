#include "CarpManager.h"

CarpManager::CarpManager() 
{
	printf("CarpManager Start \n");
	iSdForRedirectionARP = INVALID_SOCKET;
	tArpPacket.clear();
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
	char strIP[50]={0,};
	int	usOpcode=0;
	
	FILE* fp;
	if ((fp = fopen(path, "r")) == NULL) {
		printf("File Read Error: %s \n", path);
		return false;
	}
	printf("File Read Success : %s \n", path);

	fscanf(fp, "%s %s", strType, strAdaptorName);
	if(0 != strncmp("ADAPTOR_NAME", strType, strlen("ADAPTOR_NAME"))) {
		printf("read Error ADAPTOR_NAME Type : %s \n ", strType);
		return false;
	}

	fscanf(fp, "%s %02x:%02x:%02x:%02x:%02x:%02x"
			, strType
			, &tArpPacket.ether_dst[0], &tArpPacket.ether_dst[1], &tArpPacket.ether_dst[2], &tArpPacket.ether_dst[3], &tArpPacket.ether_dst[4], &tArpPacket.ether_dst[5]);
	if(0 != strncmp("DST_ETH", strType, strlen("DST_ETH"))) {
		printf("read Error DST_ETH Type : %s \n ", strType);
		return false;
	}

	
	fscanf(fp, "%s %02x:%02x:%02x:%02x:%02x:%02x"
			, strType
			, &tArpPacket.ether_src[0], &tArpPacket.ether_src[1], &tArpPacket.ether_src[2], &tArpPacket.ether_src[3], &tArpPacket.ether_src[4], &tArpPacket.ether_src[5]);
	if(0 != strncmp("SRC_ETH", strType, strlen("SRC_ETH"))) {
		printf("read Error SRC_ETH Type : %s \n ", strType);
		return false;
	}
	

	fscanf(fp, "%s %d", strType, &usOpcode);
	if(0 != strncmp("OP_CODE", strType, strlen("OP_CODE"))) {
		printf("read Error OP_CODE Type : %s \n ", strType);
		return false;
	}
	tArpPacket.arp_opcode = htons(usOpcode);

	fscanf(fp, "%s %02x:%02x:%02x:%02x:%02x:%02x"
			, strType
			, &tArpPacket.arp_src_mac[0], &tArpPacket.arp_src_mac[1], &tArpPacket.arp_src_mac[2], &tArpPacket.arp_src_mac[3], &tArpPacket.arp_src_mac[4], &tArpPacket.arp_src_mac[5]);
	if(0 != strncmp("ARP_SRC_MAC", strType, strlen("ARP_SRC_MAC"))) {
		printf("read Error ARP_SRC_MAC Type : %s \n ", strType);
		return false;
	}


	fscanf(fp, "%s %s", strType, strIP);
	if(0 != strncmp("ARP_SRC_IP", strType, strlen("ARP_SRC_IP"))) {
		printf("read Error ARP_SRC_IP : %s \n ", strType, strIP);
		return false;
	}
	tArpPacket.arp_src_ip.s_addr = inet_addr(strIP);


	fscanf(fp, "%s %02x:%02x:%02x:%02x:%02x:%02x"
			, strType
			, &tArpPacket.arp_dst_mac[0], &tArpPacket.arp_dst_mac[1], &tArpPacket.arp_dst_mac[2], &tArpPacket.arp_dst_mac[3], &tArpPacket.arp_dst_mac[4], &tArpPacket.arp_dst_mac[5]);
	if(0 != strncmp("ARP_DST_MAC", strType, strlen("ARP_DST_MAC"))) {
		printf("read Error ARP_DST_MAC Type : %s \n ", strType);
		return false;
	}

	fscanf(fp, "%s %s", strType, strIP);
	if(0 != strncmp("ARP_DST_IP", strType, strlen("ARP_DST_IP"))) {
		printf("read Error ARP_DST_IP : %s \n ", strType, strIP);
		return false;
	}
	tArpPacket.arp_dst_ip.s_addr = inet_addr(strIP);

	//printf("AdaptorName: %s\n", strAdaptorName);
	//tArpPacket.print();

	fclose(fp);
	return true;
}





bool CarpManager::sendPacket() 
{
	int ret = 0;

	if(iSdForRedirectionARP == INVALID_SOCKET) {
		printf("sendPacket(): Error INVALID_SOCKET \n");
		return false;
	}

	if( (ret = sendto(iSdForRedirectionARP, &tArpPacket, sizeof(tArpPacket), 0, (struct sockaddr *)&sll, sizeof(sll)) ) < 0) {
		printf("[%s] sendto error: %s, %s:%d\n", strAdaptorName, strerror(errno));
		sleep(0);
		return false;
	}

	printf("sendto : ARP\n");
	tArpPacket.print();

	return true;	
}









