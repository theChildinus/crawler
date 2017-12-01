#include "connserver.h"

char* HttpHeadCreate(const char* strUrl)
{
	char* strHost = getHostAddrFromUrl(strUrl);
	char* strParam = getParamFromUrl(strUrl);
	char* strHttpHead = (char*)malloc(BUFSIZE);
	memset(strHttpHead, 0x0, BUFSIZE);

	strcat(strHttpHead, "GET");
	strcat(strHttpHead, " /");
	strcat(strHttpHead, strParam);
	if (strParam != NULL)
	{
		free(strParam);
		strParam = NULL;
	}
	strcat(strHttpHead, " HTTP/1.1\r\n");
	strcat(strHttpHead, "Accept: */*\r\n");
	strcat(strHttpHead, "Accept-Language: cn\r\n");
	strcat(strHttpHead, "User-Agent: Mozilla/4.0\r\n");
	strcat(strHttpHead, "Host: ");
	strcat(strHttpHead, strHost);
	strcat(strHttpHead, "\r\n");
	strcat(strHttpHead, "Cache-Control: no-cache\r\n");
	strcat(strHttpHead, "Connection: Keep-Alive\r\n");
	strcat(strHttpHead, "\r\n\r\n");
	if (strHost != NULL)
	{
		free(strHost);
		strHost = NULL;
	}
	return strHttpHead;
}


char* getParamFromUrl(const char* strUrl)
{
	char url[URLSIZE];
	memset(url, 0x0, sizeof(url));
	snprintf(url, URLSIZE, "%s", strUrl);

	char* strAddr = strstr(url, "http://");
	if (strAddr == NULL) 
	{
		strAddr = strstr(url, "https://");
		if (strAddr != NULL) 
		{
			strAddr += 8;
		}
	}
	else {
		strAddr += 7;
	}

	if (strAddr == NULL) {
		strAddr = url;
	}
	int iLen = strlen(strAddr) + 1;
	char *strParam = (char*)malloc(iLen);
	memset(strParam, 0x0, iLen);
	int iPos = -1;
	int i = 0;
	for (i = 0; i < iLen + 1; ++i) 
	{
		if (strAddr[i] == '/') 
		{
			iPos = i;
			break;
		}
	}
	if (iPos == -1) 
	{
		strcpy(strParam, "");
	}
	else 
	{
		strcpy(strParam, strAddr + iPos + 1);
	}
	return strParam;
}

char* getHostAddrFromUrl(const char* strUrl)
{
	char url[URLSIZE];
	memset(url, 0x0, sizeof(url));
	snprintf(url, URLSIZE, "%s", strUrl);
	char* strAddr = strstr(url, "http://");
	if (strAddr == NULL) 
	{
		strAddr = strstr(url, "https://");
		if (strAddr != NULL) 
		{
			strAddr += 8;
		}
	}
	else {
		strAddr += 7;
	}

	if (strAddr == NULL) 
	{
		strAddr = url;
	}

	int iLen = strlen(strAddr) + 1;
	char *strHostAddr = (char*)malloc(iLen);
	memset(strHostAddr, 0x0, iLen);
	int i = 0;
	for (i = 0; i < iLen; ++i) 
	{
		if (strAddr[i] == '/') 
		{
			break;
		}
		else 
		{
			strHostAddr[i] = strAddr[i];
		}
	}

	return strHostAddr;
}
