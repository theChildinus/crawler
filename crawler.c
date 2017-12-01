#include "crawler.h"

QueueType* urlQueue;
map_t allUrlMap;   //所有处理过的url所构成hashmap
BF* bf;

static int sockfdCount = 0;
//static int urlCount = 0;
int count = 0;

FILE* outfile;
//FILE* logfile;
FILE* confile;

void initParse(char* strUrl)
{
	bf = bloom_create();
	bloom_add(&bf, strUrl);

    allUrlMap = hashmap_new();
    struct KEY_VALUE* kv = malloc(sizeof(struct KEY_VALUE));
    snprintf(kv->urlname, URLSIZE, "%s", strUrl);
    kv->urlid = count;
    hashmap_put(allUrlMap, kv->urlname, &kv->urlid);
}


void read_cb(evutil_socket_t fd, short what, void *arg)
{
    //fprintf(logfile, "read_cb: %d\n", fd);
    //printf("readfd:%d\n", fd);
    struct EVENT_ARG* evarg = (struct EVENT_ARG*)arg;

    map_t curUrlMap = hashmap_new(); //当前页面处理过的hashmap
    int i = 0, j = 0, n;
    int state = 0;
    char buf[BUFSIZE];
    char urlbuf[URLSIZE];
    char urlhttp[21]="http://news.sohu.com/";
    char curChar;
    while (1)
    {
        memset(buf, 0x0, BUFSIZE);
        int ret = recv(fd, (void*)buf, BUFSIZE, 0);
        if (ret == 0)
        {
            //fprintf(logfile, "==0\n");
            hashmap_free(curUrlMap);
            event_free(evarg->ev_r);
            if (evarg != NULL)
            {
                free(evarg);
                evarg = NULL;
            }
            //printf("%s\n", strResponse);
            //getUrl(strResponse, urlQueue, evarg->cur, outfile);
            //printf("<<< read_cb ret == 0\n");
            break;
        }
        else if (ret > 0)
        {
            if (evarg->firstPack == 1)
            {
                //printf("first page\n");
                evarg->firstPack = 0;
                char *substr = "200 OK";
                if (strstr(buf, substr) == NULL)
                {
                    //printf("Not found 200 OK\n");
                    hashmap_free(curUrlMap);
                    event_free(evarg->ev_r);
                    if (evarg != NULL)
                    {
                        free(evarg);
                        evarg = NULL;
                    }
                    break;
                }
                else
                {
                    struct KEY_VALUE* kv_a = malloc(sizeof(struct KEY_VALUE));
                    snprintf(kv_a->urlname, URLSIZE, "%s", evarg->url);
                    kv_a->urlid = count;
                    int error1 = hashmap_put(allUrlMap, kv_a->urlname, &kv_a->urlid);
                    if (error1 != MAP_OK)
                    {
                        //fprintf(logfile, "hashmap_put error\n");
                        printf("hashmap_put error\n");
                    }                               
                    fprintf(outfile, "%s %d\n", kv_a->urlname, count++);
                }
            }

            //printf("recvlen:%d\n", ret);
            int len = strlen(buf);
            for(i = 0; i < len; i++)
	        {
                curChar = buf[i];
                switch(state)
                {
                    case 0:if (curChar == '<'){
                                state = 1; break;
                            } else {
                                state = 0; j = 0; break;
                            }
                    case 1:if (curChar == 'a' || curChar == 'A'){
                                state = 2; break;
                            } else {
                                state = 0; j = 0; break;
                            }
                    case 2:if (curChar == 'h'){
                                state = 3; break;
                            } else if(curChar == '>'){
                                state = 0; j = 0; break;
                            } else {
                                state = 2; break;
                            }
                    case 3:if (curChar == 'r'){
                                state = 4; break;
                            } else if (curChar == '>') {
                                state = 0; j = 0; break;
                            } else {
                                state = 2; break;
                            }
                    case 4:if (curChar == 'e'){
                                state = 5; break;
                            }
                            else if (curChar == '>'){
                                state = 0; j = 0; break;
                            } else {
                                state = 2; break;
                            }
                    case 5:if (curChar == 'f') {
                                state = 6; break;
                            } else if (curChar == '>'){
                                state = 0; j = 0; break;
                            } else {
                                state = 2; break;
                            }
                    case 6:if (curChar == '='){
                                state = 7; break;
                            }
                            else if (curChar == '>'){
                                state = 0; j = 0; break;
                            } else {
                                state = 2; break;
                            }
                    case 7:if (curChar == '"') {
                                state = 10; break;
                            } else if (curChar == ' ') {
                                state = 7; break;
                            } else {
                                state = 0; j = 0; break;
                            }
                    case 10:if ((curChar=='"')||(curChar=='|')||(curChar=='>')||(curChar=='#')) {
                                state = 0; j = 0; break;
                            } else if (curChar == '/') {
                                state = 8;
                                urlbuf[j++] = curChar;
                                break;
                            } else {
                                state = 10;
                                urlbuf[j++] = curChar;
                                break;
                            }
                    case 8:if (curChar == '"'){
                                state = 9; break;
                            } else if (curChar == '>') {
                                state = 0; j = 0; break;
                            } else {
                                state = 8;
                                urlbuf[j++] = curChar;
                                break;
                            }
                    case 9:urlbuf[j] = '\0';      //char urlhttp[21]="http://news.sohu.com/";
                            state = 0;
                            for(n = 0;n <= 20; ++n){
                                if(urlbuf[n] != urlhttp[n]){
                                    break;
                                }
                            }
                            if(n == 21){
                                dealWithUrl(urlbuf, evarg->url, curUrlMap);
                                memset(urlbuf, 0x0, sizeof(urlbuf));
                                state = 0;
                                j = 0;
                            }
                            break;
                }//switch
            }//for
        }//if(ret > 0)
        else
        {
            if ((ret < 0) && (errno == EAGAIN || \
                    errno == EWOULDBLOCK || errno == EINTR))
            {
                continue;
            }
            //fprintf(logfile, "recv ret < 0\n");
            hashmap_free(curUrlMap);
            event_free(evarg->ev_r);
            if (evarg != NULL)
            {
                free(evarg);
                evarg = NULL;
            }
            break;
        }
	}//while(1)

    sockfdCount--;
    close(fd);
}

void write_cb(evutil_socket_t fd, short what, void *arg)
{
    //fprintf(logfile, "write_cb: %d\n", fd);
    //printf("write_cb: %d ", fd);
    struct EVENT_ARG* evarg = (struct EVENT_ARG*)arg;
    char* url = evarg->url;
    //printf("writeUrl:%s\n", url);
    char* strHttpHead = HttpHeadCreate(url);
    int ret = send(fd, (void*)strHttpHead, strlen(strHttpHead) + 1, 0);
    if (strHttpHead != NULL)
    {
        free(strHttpHead);
        strHttpHead = NULL;
    }
    if (ret < 0)
	{
		printf("send HttpHead ERROR\n");
		close(fd);
		return;
    }
    else if (ret >= 0)
    {
        //printf("send SUCCESS\n");
        return;
    }
}


int initbase(char* ip, int port)
{
    printf(">>> init base\n");
    //struct THREAD_ARG* tharg = (struct THREAD_ARG*)arg;
    struct event_base* base = event_base_new();
    while (urlQueue->length != 0)
    {
        while (urlQueue->length != 0 && sockfdCount < 100)
        {
            //fprintf(logfile, ">>> while sockfdCount & urlQueue\n");
            char* nextUrl = deQueue(urlQueue);

            evutil_socket_t sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            struct sockaddr_in serverAddr;
            struct event* ev_write = NULL;
            struct event* ev_read = NULL;

            // char* strHost = getHostAddrFromUrl(nextUrl);
            // struct hostent *he = gethostbyname(strHost);
            // if (he == NULL)
            // {
            //     //fprintf(logfile, "%s gethostbyname fail \n", nextUrl);
            //     printf("gethostbyname error for host\n");
            //     continue;
            // }
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(port);
            serverAddr.sin_addr.s_addr = inet_addr(ip);
            //serverAddr.sin_addr = *((struct in_addr*)he->h_addr);
            
            int ret = connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
            if (ret < 0 && ret != EINPROGRESS)
            {
                printf("errono = %d\n", errno);
                continue;
            }
            else if (ret == 0)
            {
                sockfdCount++;
                //fprintf(logfile, "Cur#:%s urlId:%d qlen:%d conn OK\n", \
                        nextUrl, urlCount, urlQueue->length);
                
            }
            evutil_make_socket_nonblocking(sock);
            //printf("before event new\n");
            struct EVENT_ARG* evarg = (struct EVENT_ARG*)malloc(sizeof(struct EVENT_ARG));
            ev_write = event_new(base, sock, EV_WRITE, write_cb, (void*)evarg);
            ev_read = event_new(base, sock, EV_READ|EV_PERSIST, read_cb, (void*)evarg);
            evarg->ev_r = ev_read;
            evarg->ev_w = ev_write;
            evarg->firstPack = 1;
            snprintf(evarg->url, URLSIZE, "%s", nextUrl);
            //evarg->ev_t = ev_time;
            //evarg->sock = sock;
            //varg->base = base;
            //evarg->cur.id = next->id;
            event_add(ev_write, 0);
            event_add(ev_read, 0);
            if (nextUrl != NULL)
            {
                free(nextUrl);
                nextUrl = NULL;
            }
        }
        //fprintf(logfile, "before loop\n");
        //printf("before loop\n");
        event_base_dispatch(base);
        printf("getUrl:%d qlen:%d\n", count, urlQueue->length);
    }
    event_base_free(base);
    printf("<<< init base\n");
    return 0;
}

void dealWithUrl(char* curUrl, char* fatherUrl, map_t curUrlMap)
{
    int ret = bloom_check(&bf, curUrl);

    if (strlen(curUrl) != 0 && ret == 0) //bloom not contain curUrl
    {
        //printf("ret == 0, error = missing\n");
        char* newUrl = (char*)malloc(URLSIZE * sizeof(char));
        snprintf(newUrl, URLSIZE, "%s", curUrl);

        bloom_add(&bf, newUrl);                                    
        enQueue(urlQueue, (void*)newUrl);
                                 
        fprintf(confile, "%s->%s\n", fatherUrl, newUrl);
    }
    else if (strlen(curUrl) != 0 && ret == 1) //bloom contain curUrl
    {
        //printf("ret == 1, error == OK\n");
        //fprintf(logfile, "match again:%s\n", curUrl);
        //printf("match again:%s\n", curUrl);
        int* findNo;
        int* tmpNo;
        int error1 = hashmap_get(allUrlMap, curUrl, (void**)&findNo);
        int error2 = hashmap_get(curUrlMap, curUrl, (void**)&tmpNo);
        if (error1 == MAP_OK && error2 == MAP_MISSING)
        {
            //fprintf(outfile, "%d %d\n", *curNo, *findNo);
            struct KEY_VALUE* kv_c = malloc(sizeof(struct KEY_VALUE));
            snprintf(kv_c->urlname, URLSIZE, "%s", curUrl);
            kv_c->urlid = *findNo;
            hashmap_put(curUrlMap, kv_c->urlname, &kv_c->urlid);
            fprintf(confile, "%s->%s\n", fatherUrl, kv_c->urlname);
        }
    }
    return;
}

void dealWithFile()
{
    fprintf(outfile, "\n");
    char line[LINESIZE];
    char strFather[URLSIZE];
    char strChild[URLSIZE];
    while (fgets(line, LINESIZE, confile) != NULL)
    {
        //printf("%s", line);
        memset(strFather, 0x0, sizeof(strFather));
        memset(strChild, 0x0, sizeof(strChild));
        int ret = sscanf(line, "%[^-]->%s", strFather, strChild);
        if (ret < 0)
            continue;

        //printf("%s#%s\n", strFather, strChild);
        int* fatherId;
        int* childId;
        int error1 = hashmap_get(allUrlMap, strFather, (void**)&fatherId);
        if (error1 != MAP_OK)
        {
            //printf("hashmap_get fatherId ERROR\n");
            continue;
        }
        int error2 = hashmap_get(allUrlMap, strChild, (void**)&childId);
        if (error2 == MAP_OK && *fatherId != *childId)
        {
            fprintf(outfile, "%d %d\n", *fatherId, *childId);
        }

    }
}


void runCrawler(char* ip, int port, char* filename)
{
    printf(">>> runCrawler\n");
    outfile = fopen(filename, "w+");
    //logfile = fopen("html.txt", "w+");
    confile = fopen("tmp.txt", "w+");

    char *first = (char*)malloc(URLSIZE * sizeof(char));
    snprintf(first, URLSIZE, "%s", "http://news.sohu.com/");

    urlQueue = (QueueType*)malloc(sizeof(QueueType));
    initQueue(urlQueue, 250000);
    enQueue(urlQueue, (void*)first);

    initParse(first);
    
    void* arg = NULL;
    initbase(ip, port);
    fclose(confile);
    confile = fopen("tmp.txt", "r");
    printf("generate url.txt please waiting...\n");
    dealWithFile();
    printf("generate url.txt finished \n");
    hashmap_free(allUrlMap);
    fclose(outfile);
    //fclose(logfile);
    fclose(confile);
    printf("<<< runCrawler\n");
}