// ATR_Test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE 1024					//每行最大字节数

int main()
{

	FILE* fp;
	int count = 1;
	char strLine[MAX_LINE];	//读取缓冲区
	if ((fp = fopen("EVALUATION(1).txt", "r")) == NULL)	// 判断文件是否存在及可读
	{
		printf("Open Falied!");
		return -1;
	}

	while (!feof(fp))	//循环读取每一行，直到文件尾
	{
		fgets(strLine, MAX_LINE, fp); // 将fp所指向的文件一行内容读到strLine缓冲区
		printf("%s", strLine);
		int len = strlen(strLine);	 // 输出所读到的内容
		
		count++;
	}
	fclose(fp);	 // 关闭读文件
	printf("\n");

