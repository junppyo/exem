#include <stdlib.h>
#include <unistd.h>

typedef struct s_osinfo
{
	unsigned long cpu_usr;
	unsigned long cpu_sys;
	unsigned long cpu_iowait;
	unsigned long cpu_idle;
	unsigned long mem_free;
	unsigned long mem_total;
	unsigned long mem_used;
	unsigned long mem_swap;
	unsigned long packet_in_cnt;
	unsigned long packet_out_cnt;
	unsigned long packet_in_byte;
	unsigned long packet_out_byte;
} osinfo;

typedef struct s_procinfo
{
	char name[256];
	char uname[32];
	int pid;
	int ppid;
	float cpuusage;
	float cputime;
	// char *cmdline;
	char *cmdline;
	struct s_procinfo *next;	
} procinfo;

void append(procinfo *node);
procinfo *pop();

