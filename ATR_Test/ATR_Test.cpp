// ATR_Test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_STRING 1024					//每行最大字节数
#define NUM_DATA 15						//每行最大数据值
#define BUFF_NUM 100					//最大缓存值

int count1, count2, count3=0;				//计算缓存长度
char  imu_buff[BUFF_NUM][NUM_DATA][MAX_STRING];					//imu的缓存
char  wifi_buff[BUFF_NUM][NUM_DATA][MAX_STRING];				//wifi的缓存
char  gnss_buff[BUFF_NUM][NUM_DATA][MAX_STRING];				//gnss的缓存

bool flag1, flag2, flag3= false;			//第一次读取的标志
char Time_Ref[20];
void imu(char buff[][MAX_STRING])
{

}
void wifi(char buff[][MAX_STRING])
{

}
void gnss(char(*buff)[MAX_STRING], FILE* fp)
{
	if (flag3 == false)
	{
		memcpy(Time_Ref, buff[1], 20);
		flag3 = true;
	}

	int i = 1;
	while (strlen(buff[i]) > 0)  //将buff进行缓存
	{
		memcpy(gnss_buff[count3][i-1], buff[i], MAX_STRING );
		i++;
	}
	count3++;
	if (count3 == 100)//缓存100次后，进行数据处理并写入
	{
		for (int j = 0; j < 100; j++)
		{
		
			double temp = atof(gnss_buff[j][0]) - atof(Time_Ref);
			_gcvt(temp,10, gnss_buff[j][0] );
			for(i=0;strlen(gnss_buff[j][i]) > 0;i++)
			{
				fputs(gnss_buff[j][i], fp);
				if(strlen(gnss_buff[j][i+1]) > 0)
					fputs(",", fp);
			}
		}
		count3 = 0;
	}
}

int main()
{

	FILE* fp, *fp1,*fp2,*fp3;
	int count = 1;
	char strLine[MAX_STRING];	//读取缓冲区
	char buff[NUM_DATA][MAX_STRING]; //每一行数据分割缓存
	int i = 0;
	for (i=0;i< NUM_DATA;i++)
		memset(buff[i], '\0', MAX_STRING);
	
	fp1 = fopen("imu.txt", "w+");
	fp2 = fopen("wifi.txt", "w+");
	fp3 = fopen("gnss.txt", "w+");
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
			i = 0;
			/* 将字符内容进行分割，并存至buff缓存区中*/
			char *token = strtok(strLine, ";");
			while (token != NULL) {
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
				gnss(buff, fp3);
			}
		}
	}
	fclose(fp);	 // 关闭读文件
	fclose(fp1);	 // 关闭读文件
	fclose(fp2);	 // 关闭读文件
	fclose(fp3);	 // 关闭读文件

	return 0;
}

