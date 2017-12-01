/*
 * id_url.c
 *
 *  Created on: 2017年11月13日
 *      Author: wanglang
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
int main(int argc, char* argv[]) {
	if (argc != 3) {
		printf("input error\n输入格式./pagerank url.txt top10.txt\n");
		return 0;
	}
	//计算程序执行时间
	clock_t begin, end;
	double time_spent;
	begin = clock();

	//统计url个数，链接个数
	FILE *fr, *fw;
	if ((fr = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "[Error] Cannot open the file");
		exit(1);
	}
	char str[100];
	int node_num = 0;
	int edges_num = 0;
	while (!feof(fr)) {
		fgets(str, 100 - 1, fr);
		if (str[0] == 'h') {
			node_num++;
		} else {
			if (str[0] >= '0' && str[0] <= '9') {
				edges_num++;
			}
		}
	}
	fclose(fr);

	if ((fr = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "[Error] Cannot open the file");
		exit(1);
	}

	if ((fw = fopen(argv[2], "w+")) == NULL) {
		fprintf(stderr, "[Error] Cannot open the file");
		exit(1);
	}

	//使用CSR压缩方式
	double *val = calloc(edges_num, sizeof(double));
	int *col_ind = calloc(edges_num, sizeof(int));
	int *row_ptr = calloc(node_num, sizeof(int));
	row_ptr[0] = 0;
	int fwomnode, tonode;
	int cur_row = 0;
	int i = 0;
	int j = 0;
	int elrow = 0;
	int curel = 0;
	char url[300];
	char **index = malloc(sizeof *index * node_num);
	int m;
	for (m = 0; m < node_num; m++) {
		index[m] = (char*) malloc(sizeof(url));
	}
	int id = 0;

	//保存url和id对应关系
	while (!feof(fr)) {
		fgets(str, 100 - 1, fr);
		sscanf(str, "%s %d", url, &id);
		if (url[0] == 'h') {
			strcpy(index[id], url);
			//printf("%s %d\n", url, id);
		} else {
			break;
		}
	}
    //读入所有链接，保存到压缩矩阵中
	while (!feof(fr)) {
		fscanf(fr, "%d%d", &fwomnode, &tonode);
		if (fwomnode > cur_row) { // change the row
			curel = curel + elrow;
			int k;
			for (k = cur_row + 1; k <= fwomnode; k++) {
				row_ptr[k] = curel;
			}
			elrow = 0;
			cur_row = fwomnode;
		}
		val[i] = 1.0;
		col_ind[i] = tonode;
		elrow++;
		i++;
	}
	row_ptr[cur_row + 1] = curel + elrow - 1;
	int out_link[node_num];
	for (i = 0; i < node_num; i++) {
		out_link[i] = 0;
	}
	int rowel = 0;
	//统计每个url出度
	for (i = 0; i < node_num; i++) {
		if (row_ptr[i + 1] != 0) {
			rowel = row_ptr[i + 1] - row_ptr[i];
			out_link[i] = rowel;
		}
	}
	int curcol = 0;
	for (i = 0; i < node_num; i++) {
		rowel = row_ptr[i + 1] - row_ptr[i];
		for (j = 0; j < rowel; j++) {
			val[curcol] = val[curcol] / out_link[i];
			curcol++;
		}
	}
	//设置阻尼因子
	double d = 0.85;
	double p[node_num];
	for (i = 0; i < node_num; i++) {
		p[i] = 1.0 / node_num;
	}
	int looping = 1;
	int k = 0;
	double p_new[node_num];
	//开始迭代计算Ap = p  幂迭代法
	while (looping) {
		for (i = 0; i < node_num; i++) {
			p_new[i] = 0.0;
		}

		int rowel = 0;
		int curcol = 0;

		for (i = 0; i < node_num; i++) {
			rowel = row_ptr[i + 1] - row_ptr[i];
			for (j = 0; j < rowel; j++) {
				p_new[col_ind[curcol]] = p_new[col_ind[curcol]]
						+ val[curcol] * p[i];
				curcol++;
			}
		}

		for (i = 0; i < node_num; i++) {
			p_new[i] = d * p_new[i] + (1.0 - d) / node_num;
		}

		double error = 0.0;
		for (i = 0; i < node_num; i++) {
			error = error + fabs(p_new[i] - p[i]);
		}
		//设置迭代终止条件
		if (error < 0.000001) {
			looping = 0;
		}
		for (i = 0; i < node_num; i++) {
			p[i] = p_new[i];
		}
		//迭代次数
		k = k + 1;
	}

#define TOPNUM 10
	double max_pr[TOPNUM];
	int max_index[TOPNUM];
	for (i = 0; i < TOPNUM; i++) {
		max_pr[i] = 0.0;
		max_index[i] = 0;
	}
	for (i = 0; i < node_num; i++) {
		if (p[i] > max_pr[TOPNUM - 1]) {
			max_pr[TOPNUM - 1] = p[i];
			max_index[TOPNUM - 1] = i;
		}
		//冒泡排序，每次排序10大小数组
		int p, q;
		for (p = 0; p < TOPNUM - 1; p++)
			for (q = 0; q < TOPNUM - p - 1; q++) {
				if (max_pr[q] < max_pr[q + 1]) {
					double temp = max_pr[q];
					max_pr[q] = max_pr[q + 1];
					max_pr[q + 1] = temp;

					int temp2 = max_index[q];
					max_index[q] = max_index[q + 1];
					max_index[q + 1] = temp2;
				}
			}
	}

	for (i = 0; i < TOPNUM; i++) {
		fprintf(fw, "%s %.9lf\n", index[max_index[i]], max_pr[i]);
	}

	fclose(fr);
	fclose(fw);

    end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("Time spent: %.9lf seconds.\n", time_spent);
	printf("nodes = %d, edges = %d\n", node_num, edges_num);
	return 0;
}

