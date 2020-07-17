#include "CarpManager.h"

CarpManager::CarpManager() 
{
	// printf("CarpManager Start \n");
	iSdForRedirectionARP = INVALID_SOCKET;
	tArpPacket.clear();
}


CarpManager::~CarpManager() 
{
	// printf("CarpManager End \n");
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
	// printf("readSettingFile start \n");
	char 	strType[1000]={0,};
	char 	strIP[50]={0,};
	int		usOpcode=0;
	u_char  ucEth[HW_ADDR_LEN]={0,};   // MAC 임시
	
	FILE* fp;
	if ((fp = fopen(path, "r")) == NULL) {
		printf("File Read Error: %s \n", path);
		return false;
	}
	printf("File Read Success : %s \n", path);

	fscanf(fp, "%s %s", strType, strAdaptorName);
	if(0 != strncmp("ADAPTOR_NAME", strType, strlen("ADAPTOR_NAME"))) {
		goto __return_error; 
	}

	fscanf(fp, "%s %02x:%02x:%02x:%02x:%02x:%02x", strType, &ucEth[0], &ucEth[1], &ucEth[2], &ucEth[3], &ucEth[4], &ucEth[5]);
	if(0 != strncmp("SRC_ETH", strType, strlen("SRC_ETH"))) {
		goto __return_error; 
	}
	memcpy(tArpPacket.ether_src, ucEth, sizeof(ucEth));


	memset(ucEth, 0, sizeof(ucEth));
	fscanf(fp, "%s %02x:%02x:%02x:%02x:%02x:%02x", strType, &ucEth[0], &ucEth[1], &ucEth[2], &ucEth[3], &ucEth[4], &ucEth[5]);
	if(0 != strncmp("DST_ETH", strType, strlen("DST_ETH"))) {
		goto __return_error; 
	}
	memcpy(tArpPacket.ether_dst, ucEth, sizeof(ucEth));


	fscanf(fp, "%s %d", strType, &usOpcode);
	if(0 != strncmp("OP_CODE", strType, strlen("OP_CODE"))) {
		goto __return_error; 
	}
	tArpPacket.arp_opcode = htons(usOpcode);


	memset(ucEth, 0, sizeof(ucEth));
	fscanf(fp, "%s %02x:%02x:%02x:%02x:%02x:%02x", strType, &ucEth[0], &ucEth[1], &ucEth[2], &ucEth[3], &ucEth[4], &ucEth[5]);
	if(0 != strncmp("ARP_SRC_MAC", strType, strlen("ARP_SRC_MAC"))) {
		goto __return_error; 
	}
	memcpy(tArpPacket.arp_src_mac, ucEth, sizeof(ucEth));


	fscanf(fp, "%s %s", strType, strIP);
	if(0 != strncmp("ARP_SRC_IP", strType, strlen("ARP_SRC_IP"))) {
		goto __return_error; 
	}
	tArpPacket.arp_src_ip.s_addr = inet_addr(strIP);

	memset(ucEth, 0, sizeof(ucEth));
	fscanf(fp, "%s %02x:%02x:%02x:%02x:%02x:%02x", strType, &ucEth[0], &ucEth[1], &ucEth[2], &ucEth[3], &ucEth[4], &ucEth[5]);
	if(0 != strncmp("ARP_DST_MAC", strType, strlen("ARP_DST_MAC"))) {
		goto __return_error; 
	}
	memcpy(tArpPacket.arp_dst_mac, ucEth, sizeof(ucEth));

	fscanf(fp, "%s %s", strType, strIP);
	if(0 != strncmp("ARP_DST_IP", strType, strlen("ARP_DST_IP"))) { 
		goto __return_error; 
	}
	tArpPacket.arp_dst_ip.s_addr = inet_addr(strIP);


	fclose(fp);
	return true;

__return_error:
	printf("read Error %s \n", strType);
	fclose(fp);
	return false;
}


// 패킷 횟수 : -1은 무한대
bool CarpManager::sendPacket(unsigned long long _ull_packet_cnt) 
{
	unsigned long long ullSendCnt = 0;
	if(iSdForRedirectionARP == INVALID_SOCKET) {
		printf("sendPacket(): Error INVALID_SOCKET \n");
		return false;
	}

	printf("         --- Packet count : %u (zero is infinite)", _ull_packet_cnt);
	tArpPacket.print();

	while(ullSendCnt < _ull_packet_cnt || _ull_packet_cnt == 0) {
		if( (sendto(iSdForRedirectionARP, &tArpPacket, sizeof(tArpPacket), 0, (struct sockaddr *)&sll, sizeof(sll)) ) < 0) {
			printf("[%s] sendto error: %s, %s:%d\n", strAdaptorName, strerror(errno));
			return false;
		}

		ullSendCnt++;
		sleep(0);
	}

	return true;	
}




