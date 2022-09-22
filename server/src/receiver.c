#include "receiver.h"

struct sockaddr_in serv_addr;
int rcv(int fd, void *message, int size)
{
	char s[10];
	int ret = 0;

	if (size <= 0)
		return 0;
	ret = read(fd, message, size);

	if (ret < 0)
	{
		if (ret < 0)
			writelog(logfd, TRACE, "수신 실패");
		return 0;
	}
	if (ret < size)
	{
		char *s;
		int tmp;
		char *buf = malloc(size - ret);
		while ((tmp = read(fd, buf, size - ret)) > 0)
		{
			s = make_packet(message, ret, buf, tmp);
			ret += tmp;
			free_s(message);
			free_s(buf);
			message = s;
		}
	}
	if (ret != size)
		return -1;
	itoa(ret, s);
	char *logmessage = ft_strjoin(s, "byte received");
	writelog(logfd, TRACE, logmessage);
	// printf("size: %d, %d received\n", size, ret);
	free_s(logmessage);
	return ret;
}

void *udp_open(void *queu)
{
    struct sockaddr_in server_addr, client_addr;
    int s;
    packet *queue = (packet *) queu;

	if((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
	    writelog(logfd, ERROR, "UDP socket error");
	}

	memset(&client_addr, 0, sizeof(client_addr));
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(DEFAULT_IP);
	server_addr.sin_port = htons(4243);

	// 서버 로컬 주소로 bind()
	if(bind(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		writelog(logfd, ERROR, "UDP bind error");
		return 0;
	}
	writelog(logfd, TRACE, "UDP bind success");
	udpbegin *begin = malloc(sizeof(udpbegin));
	udpend *end = malloc(sizeof(udpend));
	while (1)
	{
		rcv(s, begin, sizeof(udpbegin));
		rcv(s, end, sizeof(udpend));
		udppacket *tmp = malloc(sizeof(udppacket));
		tmp->id = begin->id;
		tmp->pid = begin->pid;
		int i = -1;
		while (begin->ip[++i])
			tmp->ip[i] = begin->ip[i];
		tmp->port = begin->port;
		tmp->begintime = begin->begintime;
		tmp->pkt_no = begin->pkt_no;
		tmp->byte = end->byte;
		tmp->elapse_time = end->elapse_time;
		udp_append(queue, tmp);
		if (end->flag)
		{
			udpmatric *matric = malloc(sizeof(udpmatric));
			rcv(s, matric, sizeof(udpmatric));
			matric_append(queue, matric);
		}
	}
	return 0;
}

int open_sock()
{
	// printf("connect\n");
	struct sockaddr_in client_addr;
	int client_sock;
	if (!serv_sock)
	{
		serv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

		if(serv_sock == -1)
			writelog(logfd, ERROR, "socket error");
			memset(&serv_addr, 0, sizeof(serv_addr));
			serv_addr.sin_family = AF_INET;
			serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
			serv_addr.sin_port = htons(DEFAULT_PORT);

			if(bind(serv_sock,(struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
			{
				printf("bind error\n");
				writelog(logfd, ERROR, "bind error");
			}
			writelog(logfd, TRACE, "bind success");

			if(listen(serv_sock,5) == -1)
				writelog(logfd, ERROR, "listen error");

			writelog(logfd, TRACE, "listen success");
	}
	writelog(logfd, TRACE, "wait client");
	socklen_t clnt_addr_size = sizeof(client_addr);
	client_sock = accept(serv_sock,(struct sockaddr*)&client_addr,&clnt_addr_size);
	writelog(logfd, TRACE, "client accpet");

	return client_sock;
}

void wrongpacket(int *sock)
{
	close(*sock);
	writelog(logfd, ERROR, "wrong packet received");
}

void *tcp_open(void *queu)
{
	packet *queue = (struct s_packet*) queu;
	int client_sock = open_sock();

	pthread_t thread;
	pthread_create(&thread, NULL, tcp_open, queu);

	procinfo *pinfo;
	diskinfo *dinfo;
	p_head *header = malloc(sizeof(struct s_packethead));
	char *buf = malloc(165483);
	time_t tmptime = time(NULL);

	while (1)
	{
		int n = 0;
		if (rcv(client_sock, header, sizeof(struct s_packethead)) <= 0)
		{
			client_sock = open_sock();
			writelog(logfd, TRACE, "client reconnect");
			rcv(client_sock, header, sizeof(struct s_packethead));
		}
		// printf("type: %c, size: %d\n", header->type, header->size);
		if (header->type != 'c' && header->type != 'm' && header->type != 'n' && header->type != 'p' && header->type != 'd' && header->type != 'a')
		{
			wrongpacket(&client_sock);
			break;
		}
		if (header->type == 'c')
		{
			cpuinfo *tmp = malloc(sizeof(struct s_cpuinfo));
			if (rcv(client_sock, tmp, sizeof(struct s_cpuinfo)) != sizeof(struct s_cpuinfo))
			{
				wrongpacket(&client_sock);
				break;
			}
			cpu_append(queue, tmp);
		}
		if (header->type == 'm')
		{	
			meminfo *tmp = malloc(sizeof(struct s_meminfo));
			if (rcv(client_sock, tmp, sizeof(struct s_meminfo)) != sizeof(struct s_meminfo))
			{
				wrongpacket(&client_sock);
				break;
			}
			mem_append(queue, tmp);
		}
		if (header->type == 'n')
		{
			netinfo *tmp = malloc(sizeof(struct s_netinfo));
			if (rcv(client_sock, tmp, sizeof(struct s_netinfo)) != sizeof(struct s_netinfo))
			{
				wrongpacket(&client_sock);
				break;
			}
			net_append(queue, tmp);
		}
		if (header->type == 'p')
		{
			int i = 0;
			int total = 0;
			plist *tmp = malloc(sizeof(struct s_plist));
			if ( rcv(client_sock, tmp, sizeof(struct s_plist)) != sizeof(struct s_plist))
			{
				wrongpacket(&client_sock);
				break;
			}
			tmp->HEAD = malloc(sizeof(struct s_procinfo));
			tmp->HEAD->next = NULL;
			while (i < tmp->len)
			{
				pinfo = malloc(sizeof(procinfo));
				if (rcv(client_sock, pinfo, sizeof(procinfo)) != sizeof(procinfo))
				{
					wrongpacket(&client_sock);
					break;
				}
				if (pinfo->cmdline_len)
				{
					if (rcv(client_sock, pinfo->cmdline, pinfo->cmdline_len) != pinfo->cmdline_len)
					{
						wrongpacket(&client_sock);
						break;
					}
				}
				append(tmp, pinfo);
				i++;
			}
			if (i == tmp->len)
				plist_append(queue, tmp);
			else
			{
				writelog(logfd, DEBUG, "ignored packet");
				procinfo *pinfo;
				while ((pinfo = pop(tmp)) != NULL)
					free_s(pinfo);
				free_s(tmp);
			}
		}
		if (header->type == 'd')
		{
			int i = 0;
			disklist *tmp = malloc(sizeof(struct s_disklist));
			if (rcv(client_sock, tmp, sizeof(struct s_disklist)) != sizeof(struct s_disklist))
			{
				wrongpacket(&client_sock);
				break;
			}
			tmp->HEAD = malloc(sizeof(struct s_diskinfo));
			tmp->HEAD->next = NULL;
			while (i < tmp->len)
			{
				dinfo = malloc(sizeof(diskinfo));
				if (rcv(client_sock, dinfo, sizeof(diskinfo)) != sizeof(diskinfo))
				{
					wrongpacket(&client_sock);
					break;
				}
				disk_append(tmp, dinfo);
				i++;
			}
			disklist_append(queue, tmp);
		}
		if (header->type == 'a')
		{
			float cpuavg;
			if (rcv(client_sock, &cpuavg, sizeof(float)) != sizeof(float))
			{
				wrongpacket(&client_sock);
				break;
			}
			float memavg;
			if (rcv(client_sock, &memavg, sizeof(float)) != sizeof(float))
			{
				wrongpacket(&client_sock);
				break;
			}
			printf("cpuavg: %f%%  memavg: %f%%\n", cpuavg * 100, memavg * 100);
		}
	}
	close(serv_sock);
	close(client_sock);
	return 0;
}