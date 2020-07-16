#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> 
#include <sys/ioctl.h> 
#include <net/if.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define CONF_PATH "./arpPacket.conf"

#define ARP_PROTOTYPE 0x0800    // ARP 헤더의 프로토콜 타입
#define ARP_HWTYPE 0x0001   // 하드웨어 주소 타입
#define ARP_REQUEST 0x0001  // ARP Request
#define ARP_REPLY 0x0002    // ARP Reply

#define RETURN_ERROR_0		0
#define RETURN_SUCCESS_1	1


#define INVALID_SOCKET  (-1)
#define SOCKET_ERROR    (-1)
#define MAX_VLAN_ADAPTER_NAME   50

#define CLEAR_MAC(a)    { a[0] = 'a'; \
	            a[1] = 'a';   \
	            a[2] = 'a';   \
	            a[3] = 'a';   \
	            a[4] = 'a';   \
	            a[5] = 'a'; }

#define MAC_TO_STR(mac) (u_char)mac[0], (u_char)mac[1], (u_char)mac[2], (u_char)mac[3], (u_char)mac[4], (u_char)mac[5]
#define IP_TO_STR(ip) 	inet_ntoa(ip)

struct t_SendARP
{
#define HW_ADDR_LEN		6
#define PADDING_LEN		18
	// 이더넷 헤더
	u_char  ether_dst[HW_ADDR_LEN];   // 이더넷 목적지 주소
	u_char  ether_src[HW_ADDR_LEN];   // 이더넷 근원지 주소
	u_short	frame_type;

	u_short	hw_type;
	u_short	proto_type;
	u_char	hw_addr_size;
	u_char	proto_addr_size;

	// ARP 헤더
	u_short arp_opcode;     				// 패킷 타입 => ARP_REQUEST, ARP_REPLY
	u_char  arp_src_mac[HW_ADDR_LEN];     	// ARP 근원지 Mac 주소
	in_addr arp_src_ip;     				// ARP 근원지 IP  주소
	u_char  arp_dst_mac[HW_ADDR_LEN];     	// ARP 목적지 Mac 주소
	in_addr arp_dst_ip;     				// ARP 목적지 IP  주소

	u_char	padding[PADDING_LEN];

	t_SendARP() {
		clear();
	}

	int clear() {

		CLEAR_MAC(ether_dst);
		CLEAR_MAC(ether_src);

		frame_type 		= 	htons(0x0806);
		hw_type 		= 	htons(1);
		proto_type		=	htons(0x0800);
		hw_addr_size 	=	HW_ADDR_LEN;
		proto_addr_size	=	4;

		arp_opcode 		= 0;
		arp_src_ip.s_addr = 0;
		arp_dst_ip.s_addr  = 0;
		CLEAR_MAC(arp_src_mac);
		CLEAR_MAC(arp_dst_mac);

		memset(padding, 0, PADDING_LEN);

		return 0;
	}
	void print() {
		printf("\n ==== ARP Packet ==== \n");
		printf("ether src %02x:%02x:%02x:%02x:%02x:%02x, dst %02x:%02x:%02x:%02x:%02x:%02x, frame_type %x\n", MAC_TO_STR(ether_src), MAC_TO_STR(ether_dst), ntohs(frame_type));
		printf("opcode %d\n", ntohs(arp_opcode));	
		printf("arp src_mac %02x:%02x:%02x:%02x:%02x:%02x src_ip %s\n", MAC_TO_STR(arp_src_mac), IP_TO_STR(arp_src_ip));
		printf("arp dst_mac %02x:%02x:%02x:%02x:%02x:%02x dst_ip %s\n\n", MAC_TO_STR(arp_dst_mac), IP_TO_STR(arp_dst_ip));
	}
};


class CarpManager 
{

private:
	int 					iSdForRedirectionARP;             			// 소켓 디스크립터
	struct ifreq     		ifr;                 						// iSdForRedirectionARP 관련 변수
	struct sockaddr_ll     	sll;    	       							// iSdForRedirectionARP 관련 변수
	char 					strAdaptorName[MAX_VLAN_ADAPTER_NAME + 1];
	struct t_SendARP		tArpPacket;									// ARP 패킷 정보
	unsigned int			uiSendCount;								// 패킷 횟수 : -1은 무한대

public:
	CarpManager();
	~CarpManager();
	
	int 	initSocket() ;
	bool 	readSettingFile(const char* path = CONF_PATH) ;
	bool 	sendPacket() ;

};
