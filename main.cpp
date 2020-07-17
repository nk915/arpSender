#include "CarpManager.h"


bool initSettingFile(const char* path); 

int main(int argc, char* argv[]) 
{
	class CarpManager cArpManager;
//	for ( int i = 0; i < argc ; i++) {
//		printf("%s \n", argv[i]);
//	}
	
	// arp_packet.conf 파일 확인
	if(false == cArpManager.readSettingFile()) {
		initSettingFile(CONF_PATH);
		return 0;
	}

	if(SOCKET_ERROR == cArpManager.initSocket()) {
		printf("initSocket ERROR \n");
		return 0;
	}


	printf("sendto : ARP  (version 1.0)\n");
	if(argc == 2) {
		unsigned long long ullPacketCount =	strtoull(argv[1], NULL, 0);
		cArpManager.sendPacket(ullPacketCount);
	}
	else {
		cArpManager.sendPacket();
	}


	return 0;
}


bool initSettingFile(const char* path) 
{
	printf("initSettingFile start \n");

	FILE* fp;
	if ((fp = fopen(path, "w")) == NULL) {
		printf("File write Error: %s \n", path);
		return false;
	}

	fputs("ADAPTOR_NAME    eth0\n", fp);
	fputs("SRC_ETH         11:22:33:44:55:66 \n", fp);
	fputs("DST_ETH         ff:ff:ff:ff:ff:ff \n", fp);
	fputs("OP_CODE         1\n", fp);
	fputs("ARP_SRC_MAC     11:22:33:44:55:66 \n", fp);
	fputs("ARP_SRC_IP      192.168.100.100   \n", fp);
	fputs("ARP_DST_MAC     00:00:00:00:00:00 \n", fp);
	fputs("ARP_DST_IP      192.168.100.200   \n", fp);

	fflush(fp);
	fclose(fp);
	return true;
}


