// ATR_Test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "math.h"

#define MAX_STRING 1024					//每行最大字节数
#define NUM_DATA 15						//原始数据每行数据量
#define BUFF_NUM 100					//最大缓存值

#define IMU_NUM 15						//WIFI每行需要输出的数据个数
#define WIFI_NUM 3						//WIFI每行需要输出的数据个数
#define GNSS_NUM 11						//GNSS每行需要输出的数据个数

int count1, count2, count3=0;				//计算缓存长度
char  imu_buff[BUFF_NUM][IMU_NUM][MAX_STRING];					//imu的缓存
char  imu_output[IMU_NUM][MAX_STRING];							//imu的输出
char  wifi_buff[BUFF_NUM][WIFI_NUM][MAX_STRING];				//wifi的缓存
char  gnss_buff[BUFF_NUM][GNSS_NUM][MAX_STRING];				//gnss的缓存

bool flag= false;			//第一次读取的标志
bool acce = false, gyro = false, magn = false, ahrs = false, pres = false;
char Time_Ref[20];
double Min_Time1= INT_MAX,Min_Time2= INT_MAX;
void imu(char(*buff)[MAX_STRING], FILE* fp)//采样率只有5Hz，没做线性插值
{
	if (Min_Time1 > atof(buff[2]))//找出最小SensorTimestamp
	{
		Min_Time1 = atof(buff[2]);
	}
	for (int i = 0; strlen(buff[i]) > 0;i++)  //将buff进行缓存
	{
		memcpy(imu_buff[count1][i], buff[i], MAX_STRING);
	}
	count1++;
	if (count1 == 100)//缓存100次后，对数据进行处理，线性插值等运算，并将数据写入
	{
		for (int j = 0; j < 100; j++)
		{
			//计算时间差TIME
			int temp = (atof(imu_buff[j][2]) - Min_Time1) * 1000;
			_itoa(temp,  imu_buff[j][2],10);
			//分别检测"ACCE"、"GYRO"、"MAGN"、"AHRS"、 "PRES"
			if (strcmp(imu_buff[j][0], "PRES") == 0 && pres == false)
			{
				memcpy(imu_output[13], imu_buff[j][3], MAX_STRING);
				memcpy(imu_output[0], imu_buff[j][2], MAX_STRING);

				pres = true;
	}
			else if (strcmp(imu_buff[j][0], "ACCE")==0 &&acce == false && pres == true)
			{
				memcpy(imu_output[1],imu_buff[j][3] , MAX_STRING);
				memcpy(imu_output[2],imu_buff[j][4], MAX_STRING);
				memcpy(imu_output[3],imu_buff[j][5], MAX_STRING);
				acce = true;
			}
			else if (strcmp(imu_buff[j][0], "GYRO") == 0 && gyro == false && pres == true)
			{
				memcpy(imu_output[4], imu_buff[j][3], MAX_STRING);
				memcpy(imu_output[5], imu_buff[j][4], MAX_STRING);
				memcpy(imu_output[6], imu_buff[j][5], MAX_STRING);
				gyro = true;
			}
			else if (strcmp(imu_buff[j][0], "MAGN") == 0 && magn == false && pres == true)
			{
				memcpy(imu_output[7], imu_buff[j][3], MAX_STRING);
				memcpy(imu_output[8], imu_buff[j][4], MAX_STRING);
				memcpy(imu_output[9], imu_buff[j][5], MAX_STRING);
				magn = true;
			}
			else if (strcmp(imu_buff[j][0], "AHRS") == 0 && ahrs == false && pres == true)
			{
				memcpy(imu_output[10], imu_buff[j][3], MAX_STRING);
				memcpy(imu_output[11], imu_buff[j][4], MAX_STRING);
				memcpy(imu_output[12], imu_buff[j][5], MAX_STRING);
				ahrs = true;
			}
			

			if (acce && gyro && magn && ahrs && pres)//获取到所有参数的数据后进行一次写入
			{
				acce = false; gyro = false; magn = false; ahrs = false; pres = false;
				for (int i = 0; strlen(imu_output[i])>0; i++)
				{
					fputs(imu_output[i], fp);
					if (strlen(imu_output[i + 1]) > 0)
						fputs(",", fp);
				}
				fputs("\n", fp);
			}
		
		}
		count1 = 0;
	}
}
void wifi(char(*buff)[MAX_STRING], FILE* fp)
{
	if(Min_Time2> atof(buff[2]))//找出最小SensorTimestamp
	{
		Min_Time2 = atof(buff[2]);
	}
	//MAC地址换算
	char save[MAX_STRING];
	long long mac=0;
	int j = 0;
	for (int i=0;i<strlen(buff[4]);i++)
	{
		if (buff[4][i] != ':')
			save[j++] = buff[4][i];
	}
	save[j++] = '\0';
	for (int i = 0; i < strlen(save); i++)
	{
		mac = mac + pow(2,4*(11-i))* (save[i]-'0');
	}
	_i64toa(mac, save, 10);//将计算的整型数值转为字符

	//将buff进行缓存
	memcpy(wifi_buff[count2][0], buff[2], MAX_STRING);
	memcpy(wifi_buff[count2][1], save, MAX_STRING);
	memcpy(wifi_buff[count2][2], buff[6], MAX_STRING);

	count2++;
	if (count2 == 100)//缓存100次后，进行数据处理并写入
	{
		for (j = 0; j < 100; j++)
		{
			//计算时间差TIME
			int temp =( atof(wifi_buff[j][0]) - Min_Time2)*1000;
			_itoa(temp, wifi_buff[j][0],10);
			for ( int i = 0; i<=2; i++)
			{
				fputs(wifi_buff[j][i], fp);
				if (i<2)
					fputs(",", fp);
			}
		}
		count2 = 0;
	}
}
void gnss(char(*buff)[MAX_STRING], FILE* fp)
{
	if (flag == false)   //保存第一个数据的AppTimestamp
	{
		memcpy(Time_Ref, buff[1], 20);
		flag = true;
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
			//计算时间差CTIME
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
				imu(buff, fp1);
			}
			//调用wifi函数
			else if (!strcmp(buff[0], "WIFI"))
			{
				wifi(buff, fp2);
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

