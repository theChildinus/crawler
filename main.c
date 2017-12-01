#include <stdio.h>
#include <time.h>
#include "crawler.h"

int main(int argc, char* argv[])
{
	if (argc != 4)
		return 0;
	
	char ip[20];
	char filename[10];
	memset(ip, 0x0, sizeof(ip));
	memset(filename, 0x0, sizeof(filename));
	snprintf(ip, URLSIZE, "%s", argv[1]);
	int port = atoi(argv[2]);
	snprintf(filename, 10, "%s", argv[3]);

	printf("%s %d %s\n", ip, port, filename);
	clock_t start, end;
	start = clock();
    printf("--- start ---\n");
	runCrawler(ip, port, filename);
	printf("--- end ---\n");
	end = clock();
	double Total_time = (double)(end - start) / CLOCKS_PER_SEC; 
    printf( "%f seconds\n", Total_time); 
	return 0;
}
