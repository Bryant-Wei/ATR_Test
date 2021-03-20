// ATR_Test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_STRING 1024					//每行最大字节数
#define NUM_DATA 10						//每行最大数据值

void imu(char buff[][MAX_STRING])
{

}
void wifi(char buff[][MAX_STRING])
{

}
void gnss(char buff[][MAX_STRING])
{
	fputs()
}

int main()
{

	FILE* fp, *fp1,*fp2,*fp3;
	int count = 1;
	char strLine[MAX_STRING];	//读取缓冲区
	char buff[NUM_DATA][MAX_STRING]; //每一行数据分割缓存
	fp3 = fopen("gnss.txt", "w");
	if ((fp = fopen("EVALUATION(1).txt", "r")) == NULL)	// 判断文件是否存在及可读
	{
		printf("Open Falied!");
		return -1;
	}

	while (!feof(fp))	//循环读取每一行，直到文件尾
	{
		fgets(strLine, MAX_STRING, fp); // 将fp所指向的文件一行内容读到strLine缓冲区
		if (strLine[0] != '%' && strLine[0] != '\n')
		{
			/* 将字符内容进行分割，并存至buff缓存区中*/
			int i = 0;
			char *token = strtok(strLine, ";");
			while (token != NULL) {
				memset(buff[i], '\0', MAX_STRING);
				strcpy(buff[i++], token);
				token = strtok(NULL, ";");
			}
			//调用imu函数
			if (!strcmp(buff[0],"ACCE")|| !strcmp(buff[0], "GYRO") || !strcmp(buff[0], "MAGN") || !strcmp(buff[0], "AHRS") || !strcmp(buff[0], "PRES") || !strcmp(buff[0], "LIGH"))
			{
				imu(buff);
			}
			//调用wifi函数
			else if (!strcmp(buff[0], "WIFI"))
			{
				wifi(buff);
			}
			//调用gnss函数
			else if (!strcmp(buff[0], "GNSS"))
			{
				gnss(buff);
			}
		}
		int len = strlen(strLine);	 // 输出所读到的内容

		count++;
	}
	fclose(fp);	 // 关闭读文件
	printf("\n");
}

