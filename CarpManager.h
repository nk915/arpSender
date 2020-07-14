#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> 
#include <sys/ioctl.h> 
#include <net/if.h>
#include <string.h>

#define ARP_PROTOTYPE 0x0800    // ARP 헤더의 프로토콜 타입
#define ARP_HWTYPE 0x0001   // 하드웨어 주소 타입
#define ARP_REQUEST 0x0001  // ARP Request
#define ARP_REPLY 0x0002    // ARP Reply

#define INVALID_SOCKET  (-1)
#define SOCKET_ERROR    (-1)
#define MAX_VLAN_ADAPTER_NAME   50

#define CLEAR_MAC(a)    { a[0] = 0; \
	            a[1] = 0;   \
	            a[2] = 0;   \
	            a[3] = 0;   \
	            a[4] = 0;   \
	            a[5] = 0; }

// ARP 패킷 전송 구조체
struct t_SendARP
{
	// 이더넷 헤더
	u_char  ether_dst[6];   // 이더넷 목적지 주소
	u_char  ether_src[6];   // 이더넷 근원지 주소

	//ARP 헤더
	u_short arp_opcode;     	// 패킷 타입 => ARP_REQUEST, ARP_REPLY
	u_char  arp_src_mac[6];     // ARP 근원지 Mac 주소
	in_addr arp_src_ip;     	// ARP 근원지 IP  주소
	u_char  arp_dst_mac[6];     // ARP 목적지 Mac 주소
	in_addr arp_dst_ip;     	// ARP 목적지 IP  주소
	in_addr org_dst_ip;     	// ARP 목적지 IP주소 (변조 ARP를 전송할때 목적지 IP를 확인못하는 경우가 발생하기 때문임)

	t_SendARP() {
		clear();
	}

	int clear() {
		CLEAR_MAC(ether_dst);
		CLEAR_MAC(ether_src);

		arp_opcode= 0;
		CLEAR_MAC(arp_src_mac);
		arp_src_ip.s_addr = 0;
		CLEAR_MAC(arp_dst_mac);
		arp_dst_ip.s_addr  = 0;
		return 0;
	}
//	void print(char* strTemp) {
//		if(strTemp){
//			sprintf(strTemp,"type %d ether src %02x:%02x:%02x:%02x:%02x:%02x, dst %02x:%02x:%02x:%02x:%02x:%02x arp src mac %02x:%02x:%02x:%02x:%02x:%02x arp src ip %s arp dst mac %02x:%02x:%02x:%02x:%02x:%02x arp dst ip %s, org dst ip %s ",
//					arp_opcode,
//					MAC2STR(ether_src), MAC2STR(ether_dst),
//					MAC2STR(arp_src_mac), IP2STR(arp_src_ip),
//					MAC2STR(arp_dst_mac), IP2STR(arp_dst_ip),
//					IP2STR(org_dst_ip));
//		}
//	}
};


class CarpManager 
{

private:
	int 					iSdForRedirectionARP;             			// 소켓 디스크립터
	struct ifreq     		ifr;                 						// iSdForRedirectionARP 관련 변수
	struct sockaddr_ll     	sll;    	       							// iSdForRedirectionARP 관련 변수
	char 					sAdaptorName[MAX_VLAN_ADAPTER_NAME + 1];

public:
	CarpManager();
	~CarpManager();
	
	int initSocket();

};
