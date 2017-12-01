#include<limits.h>
#include<string.h>
#include<time.h>

#include "bloomfilter.h"
#include "hash.h"

#define BITSIZE 234881024
#define BUFFERSIZE 1024
#define FUNCNUM 11 
#define SETBIT(a, n) (a[n/(sizeof(unsigned int)*8)] |= (1<<(sizeof(unsigned int)*8-n%(sizeof(int)*8)-1)))
#define GETBIT(a, n) (a[n/(sizeof(unsigned int)*8)] & (1<<((sizeof(unsigned int)*8-n%(sizeof(int)*8)-1))))
phashfunc func[FUNCNUM];

BF* bloom_create()
{
	BF *p = (BF *)malloc(sizeof(BF));
	int count = BITSIZE / sizeof(int)/8 ;
	int i;
	p->size = 0;
	p->filter = (unsigned int *) malloc(BITSIZE/8);
	for(i=0 ; i< count ; i++) {
		p->filter[i] = 0;
	}

	/* Init the function point */

	func[0] = RSHash;
	func[1] = JSHash;
	func[2] = PJWHash;
	func[3] = ELFHash;
	func[4] = BKDRHash;
	func[5] = SDBMHash;
	func[6] = DJBHash;
	func[7] = DEKHash;
	func[8] = BPHash;
	func[9] = FNVHash;
	func[10] = APHash;

	return p;
}

int bloom_destroy(BF **bfArray)
{
	BF *p = *bfArray;
	if (p->filter != NULL)
	{
		free(p->filter);
		p->filter = NULL;
	}
	if (p != NULL)
	{
		free(p);
		p = NULL;
	}
	p = NULL;
	return 0;
}

int bloom_add(BF **bfArray,char *str)
{
	BF *p = *bfArray;
	int len = strlen(str);
	int i;
	for(i = 0 ; i<FUNCNUM ; i++ ) {
		unsigned int temp = func[i](str,len);
		temp %= BITSIZE;
		SETBIT(p->filter,temp);
	}
	return 0;
}

int bloom_check(BF **bfArray,char *str)
{
	BF *p = *bfArray;
	int len = strlen(str);
	int i;
	for(i= 0 ;i <FUNCNUM ;i++) {
		unsigned int temp = func[i](str,len);
		unsigned int result=1;
		temp %= BITSIZE;
		result = GETBIT(p->filter,temp);
		if(result == 0) return 0;
	}
	return 1;
}

void bloom(FILE *pool,FILE *check,FILE *result) {
	clock_t start,end,start1;
	BF *b=bloom_create();
	int count;
	char line[BUFFERSIZE];
	start = clock();
	/* int pos = 0; */
	while(fgets(line,BUFFERSIZE,pool)) {
		bloom_add(&b,line);
		if(!(++count%100000)){
			end = clock();
			printf("%d,%f \n",count,(double)(end -start)/CLOCKS_PER_SEC);
		}

	/*	printf("%d -> %s",pos++,line); */
	}
	end = clock();
	printf("Creating tree using %f",(double)(end -start)/CLOCKS_PER_SEC);
	start1 = clock();		
	while(fgets(line,BUFFERSIZE,check)) {
		if(bloom_check(&b,line)){
			fprintf(result,"yes\n");
		}else {
			fprintf(result,"no\n");
		}
	}
	end = clock();
	printf("Finding in %f",(double)(end -start1)/CLOCKS_PER_SEC);
	
}