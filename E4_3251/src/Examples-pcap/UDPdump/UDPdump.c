/*
 * Copyright (c) 1999 - 2005 NetGroup, Politecnico di Torino (Italy)
 * Copyright (c) 2005 - 2006 CACE Technologies, Davis (California)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Politecnico di Torino, CACE Technologies 
 * nor the names of its contributors may be used to endorse or promote 
 * products derived from this software without specific prior written 
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifdef _MSC_VER
/*
 * we do not want the warnings about the old deprecated and unsecure CRT functions
 * since these examples can be compiled under *nix as well
 */
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "pcap.h"
#include <Packet32.h>
#include <ntddndis.h>
#pragma comment(lib, "Packet.lib")
#pragma comment(lib, "wpcap.lib")
#pragma comment(lib, "WS2_32.lib")


/* IPv4 header */
typedef struct ip_header
{
	u_char	ver_ihl;		// Version (4 bits) + Internet header length (4 bits)
	u_char	tos;			// Type of service 
	u_short tlen;			// Total length 
	u_short identification; // Identification
	u_short flags_fo;		// Flags (3 bits) + Fragment offset (13 bits)
	u_char	ttl;			// Time to live
	u_char	proto;			// Protocol
	u_short crc;			// Header checksum
	u_char	saddr[4];		// Source address
	u_char	daddr[4];		// Destination address
	//u_short	op_pad[2];			// Option + Padding
}ip_header;

/* UDP header*/


typedef struct mac_header {
	u_char dest_addr[6];
	u_char src_addr[6];
	u_char type[2];
} mac_header;

/*FTP header*/
typedef struct ftp_header {
	u_short	src_port;
	u_short	des_port;
	u_short	seq_num[2];
	u_short	ack_num[2];
	u_char bit_len;			// Total bit length 
	u_char flags;			//��־λ
	u_short win_size;		//window size value
	u_short checksum;		//Checksum
	u_short urg_pointer;	//urgent pointer
} ftp_header;

/* prototype of the packet handler */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);

#define FROM_NIC
int main()
{
	pcap_if_t *alldevs;
	pcap_if_t *d;
	int inum;
	int i=0;
	pcap_t *adhandle;
	char errbuf[PCAP_ERRBUF_SIZE];
	u_int netmask;
	//char packet_filter[] = "ip and udp";
	char packet_filter[] = "tcp port ftp";	//ֻ����ftp���ݰ�
	struct bpf_program fcode;
#ifdef FROM_NIC
	/* Retrieve the device list */
	if(pcap_findalldevs(&alldevs, errbuf) == -1)
	{
		fprintf(stderr,"Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}
	
	/* Print the list */
	for(d=alldevs; d; d=d->next)
	{
		printf("%d. %s", ++i, d->name);
		if (d->description)
			printf(" (%s)\n", d->description);
		else
			printf(" (No description available)\n");
	}

	if(i==0)
	{
		printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
		return -1;
	}
	
	printf("Enter the interface number (1-%d):",i);
	scanf("%d", &inum);
	
	/* Check if the user specified a valid adapter */
	if(inum < 1 || inum > i)
	{
		printf("\nAdapter number out of range.\n");
		
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* Jump to the selected adapter */
	for(d=alldevs, i=0; i< inum-1 ;d=d->next, i++);
	
	/* Open the adapter */
	if ((adhandle= pcap_open_live(d->name,	// name of the device
							 65536,			// portion of the packet to capture. 
											// 65536 grants that the whole packet will be captured on all the MACs.
							 1,				// promiscuous mode (nonzero means promiscuous)
							 1000,			// read timeout
							 errbuf			// error buffer
							 )) == NULL)
	{
		fprintf(stderr,"\nUnable to open the adapter. %s is not supported by WinPcap\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}
	
	/* Check the link layer. We support only Ethernet for simplicity. */
	if(pcap_datalink(adhandle) != DLT_EN10MB)
	{
		fprintf(stderr,"\nThis program works only on Ethernet networks.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}
	
	if(d->addresses != NULL)
		/* Retrieve the mask of the first address of the interface */
		netmask=((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		/* If the interface is without addresses we suppose to be in a C class network */
		netmask=0xffffff; 


	//compile the filter
	if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) <0 )
	{
		fprintf(stderr,"\nUnable to compile the packet filter. Check the syntax.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}
	
	//set the filter
	if (pcap_setfilter(adhandle, &fcode)<0)
	{
		fprintf(stderr,"\nError setting the filter.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}
	
	printf("\nlistening on %s...\n", d->description);
	
	/* At this point, we don't need any more the device list. Free it */
	pcap_freealldevs(alldevs);
	
	/* start the capture */
	pcap_loop(adhandle, 0, packet_handler, NULL);
#else

	/* Open the capture file */
	if ((adhandle = pcap_open_offline("C:\\Users\\Administrator\\Desktop\\dns.pcap",			// name of the device
		errbuf					// error buffer
	)) == NULL)
	{
		fprintf(stderr, "\nUnable to open the file.\n");
		return -1;
	}

	/* read and dispatch packets until EOF is reached */
	pcap_loop(adhandle, 0, packet_handler, NULL);

	pcap_close(adhandle);
#endif
	return 0;
}

/* Callback function invoked by libpcap for every incoming packet */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
	struct tm *ltime;
	char timestr[16];
	ip_header *ih;
	mac_header *mh;
	u_int ip_len;
	u_short sport,dport;
	time_t local_tv_sec;

	/*
	 * unused parameter
	 */
	(VOID)(param);

	/* convert the timestamp to readable format */
	local_tv_sec = header->ts.tv_sec;
	ltime=localtime(&local_tv_sec);
	strftime( timestr, sizeof timestr, "%H:%M:%S", ltime);

	/* print timestamp and length of the packet */
	printf("%s.%.6d len:%d \n", timestr, header->ts.tv_usec, header->len);

	/* print ip addresses and udp ports */
	int length = sizeof(mac_header) + sizeof(ip_header);
	for (int i = 0; i < length; i++)
	{
		printf("%02X ", pkt_data[i]);
		if ((i & 0xF) == 0xF)
			printf("\n");
	} 
	printf("\n");
	mh = (mac_header*)pkt_data;
	printf("mac_header:\n");
	printf("\tdest_addr: "); 
	for (int i = 0; i < 6; i++) 
	{ 
		printf("%02X ", mh->dest_addr[i]);
	} 
	printf("\n");
	printf("\tsrc_addr: "); 
	for (int i = 0; i < 6; i++) 
	{ 
		printf("%02X ", mh->src_addr[i]);
	} 
	printf("\n");

	printf("\ttype: %04X", ntohs(mh->type[0]));

	printf("\n");
	/* retireve the position of the ip header */ 
	ih = (ip_header *)(pkt_data + sizeof(mac_header)); //length of ethernet header
	printf("ip_header\n"); 
	printf("\t%-10s: %02X\n", "ver_ihl", ih->ver_ihl); 
	printf("\t%-10s: %02X\n", "tos", ih->tos);
	printf("\t%-10s: %04X\n", "tlen", ntohs(ih->tlen));
	printf("\t%-10s: %04X\n", "identification", ntohs(ih -> identification));
	printf("\t%-10s: %04X\n", "flags_fo", ntohs(ih->flags_fo));
	printf("\t%-10s: %02X\n", "ttl", ih->ttl);
	printf("\t%-10s: %02X\n", "proto", ih->proto);
	printf("\t%-10s: %04X\n", "crc", ntohs(ih->crc));
	printf("\t%-10s: ", "saddr:");
	for (int i = 0; i < 4; i++) 
	{
		printf("%02X ", ih->saddr[i]);
	}
	printf(" ");
	for (int i = 0; i < 4; i++)
	{
		printf("%d.", ih->saddr[i]);
	} printf("\n");
	printf("\t%-10s: ", "daddr");
	for (int i = 0; i < 4; i++) 
	{
		printf("%02X ", ih->daddr[i]);
	}
	printf(" ");
	for (int i = 0; i < 4; i++) 
	{ 
		printf("%d.", ih->daddr[i]);
	}


	ftp_header *ftp;
	ftp = (ftp_header *)(pkt_data + sizeof(mac_header)+ sizeof(ip_header));
	printf("\n");
	/*���ftp header*/
	printf("ftp_header\n");
	printf("\t%-10s: %04X\n", "src_port", ntohs(ftp->src_port));
	printf("\t%-10s: %04X\n", "des_port", ntohs(ftp->des_port));
	printf("\t%-10s: %04X%04X\n", "seq_num", ntohs(ftp->seq_num[0]), ntohs(ftp->seq_num[1]));
	printf("\t%-10s: %04X%04X\n", "ack_num", ntohs(ftp->ack_num[0]), ntohs(ftp->ack_num[1]));
	printf("\t%-10s: %02X\n", "bit_len", ftp->bit_len);
	printf("\t%-10s: %02X\n", "flags", ftp->flags);
	printf("\t%-10s: %04X\n", "win_size", ntohs(ftp->win_size));
	printf("\t%-10s: %04X\n", "checksum", ntohs(ftp->checksum));
	printf("\t%-10s: %04X\n", "urg_pointer", ntohs(ftp->urg_pointer));


	printf("\ndata is:");
	/*������ݶ�*/
	int head = 54;//14λ��̫��ͷ��20λipͷ��20λtcpͷ   
	if (pkt_data[head] == 'U'&&pkt_data[head+1] == 'S')	//���ݶ���USER��ͷ
	{

		FILE *fp = NULL;
		fp = fopen("test.xls", "a");
		char *a = "FTP:";
		fprintf(fp, "%s\t%s", timestr, a);
		for (int k = 0; k < 4; k++)
		{
			fprintf(fp, "%d.", ih->daddr[k]);
		}
		fprintf(fp, "\t");
		for (int i = head + 1; (i < header->caplen - 1); i++)
		{
			fprintf(fp,"%c", pkt_data[i - 1]);
			if(i==head+4)
				fprintf(fp, ":");
		}
		fclose(fp);
		
	}
	else if (pkt_data[head] == 'P'&&pkt_data[head + 1] == 'A')	//���ݶ���PASS��ͷ
	{
		FILE *fp = NULL;
		fp = fopen("test.xls", "a");
		fprintf(fp, "\t");
		for (int i = head + 1; (i < header->caplen - 1); i++)
		{
			fprintf(fp, "%c", pkt_data[i - 1]);
			if (i == head + 4)
				fprintf(fp, ":");
		}
		fclose(fp);
	}
	else if (pkt_data[head] == '2'&&pkt_data[head + 1] == '3'&&pkt_data[head + 2] == '0')	//���ݶ���230��ͷ
	{
		FILE *fp = NULL;
		fp = fopen("test.xls", "a");
		char *a = "STA:OK";
		fprintf(fp, "\t%s\n", a);
		fclose(fp);
	}
	else if (pkt_data[head] == '5'&&pkt_data[head + 1] == '3'&&pkt_data[head + 2] == '0')	//���ݶ���530��ͷ
	{
		FILE *fp = NULL;
		fp = fopen("test.xls", "a");
		char *a = "STA:FAILD";
		fprintf(fp, "\t%s\n", a);
		fclose(fp);
	}
	for (int i = head + 1; (i < header->caplen + 1); i++)
	{
		printf("%c", pkt_data[i - 1]);
	}
	printf("\n");
}
