#ifndef COMMON_H
#define COMMON_H

#define BUFSIZE 45920
#define URLSIZE 512
#define LINESIZE 1028
#define WHOLE_PAGE_SIZE 800000

typedef struct URL_NO
{
    char url[URLSIZE];
    int fatherid;
}Url_No;

typedef struct KEY_VALUE
{
    char urlname[URLSIZE];
    int urlid;
}key_value;

typedef struct EVENT_ARG
{
    char url[URLSIZE];
    struct event* ev_w;
    struct event* ev_r;
    int firstPack;
}event_arg;

typedef struct THREAD_ARG
{
    struct event_base* base;
    char* url;
}thread_arg;

#endif
