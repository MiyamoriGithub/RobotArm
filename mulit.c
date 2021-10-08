#include <reg51.h>

#define uchar unsigned char
#define uint unsigned int

// 定义计时器0周期，设置为5ms
#define TIME 5000

// 定义舵机记忆位置，初值根据实际情况修改
// 00-03为位置0
uint motion00=200;
uint motion01=210;
uint motion02=200;
uint motion03=185;
// 10-13为位置1
uint motion10=100;
uint motion11=120;
uint motion12=110;
uint motion13=100; 

// 定义舵机初始位置，当前为中位
uint servo0=135;
uint servo1=135;
uint servo2=135;
uint servo3=135;

// 定义舵机PWM波输出
sbit PWM0=P3^0;
sbit PWM1=P3^1;	
sbit PWM2=P3^2;
sbit PWM3=P3^3;

// key0-key7定义舵机控制按键信号
// 两个一组控制舵机位置加减
sbit key0=P2^0;
sbit key1=P2^1;
sbit key2=P2^2;
sbit key3=P2^3;
sbit key4=P2^4;
sbit key5=P2^5;
sbit key6=P2^6;
sbit key7=P2^7;
// but0-but3定义功能按键信号
// 0、1为开启动作组按键，2、3为动作组记忆使能按键
sbit but0=P1^0;
sbit but1=P1^1;
sbit but2=P1^2;
sbit but3=P1^3;

// 定义功能按键标志位
uchar flag0=0;
uchar flag1=0;
uchar flag2=0;
uchar flag3=0;
uchar flag0f=0;
uchar flag1f=0;
uchar flag2f=0;
uchar flag3f=0;

// 定义舵机控制按键标志位
uchar flag0x=0;
uchar flag1x=0;
uchar flag2x=0;
uchar flag3x=0;
uchar flag4x=0;
uchar flag5x=0;
uchar flag6x=0;
uchar flag7x=0;

// 定时器中断计数
uchar count=0;

// 延时函数，PWM波用
void delay(uint time) {
    for(;time>0;time--)
        ;
}

// 延时函数1，缓速转动用
void delay1() {
	uint j;
	uint k;
	for(k=60;k>0;k--) {
		for(j=50;j>0;j--)
			;
	}
}

// 转动函数，iden为舵机识别号，target为目标数值
void turn(uchar iden, int target) {
	int servo;
	int angle=0;
	uint i;
	switch (iden) {
		case 0:
			servo=servo0;break;
		case 1:
			servo=servo1;break;
		case 2:
			servo=servo2;break;
		case 3:
			servo=servo3;break;
	}

	// 正转
	if(target-servo>=0) {
		angle=target-servo;
		for(i=0;i<angle;i++) {
			delay1();
			switch (iden) {
				case 0:	
					servo0++;break;
				case 1:
					servo1++;break;
				case 2:
					servo2++;break;
				case 3:
					servo3++;break;
			}
		}
	} else {	// 反转
		angle=servo-target;
		for(i=0;i<angle;i++) {
			delay1();
			switch (iden) {
				case 0:	
					servo0--;break;
				case 1:
					servo1--;break;
				case 2:
					servo2--;break;
				case 3:
					servo3--;break;
			}
		}
	}
}

// 转动函数，iden为舵机识别号
void turn_inf(uchar iden) {
	switch (iden) {
		case 0:
			while(flag0x>0 && servo0 < 222) {
				servo0++;
				delay1();
			}
			break;
		case 1:
			while(flag2x>0 && servo1 < 222) {
				servo1++;
				delay1();
			}
			break;
		case 2:
			while(flag4x>0 && servo2 < 222) {
				servo2++;
				delay1();
			}
			break;
		case 3:
			while(flag6x>0 && servo3 < 222) {
				servo3++;
				delay1();
			}
			break;
	}
}

// 反向转动函数，iden为舵机识别号
void reverse_inf(uchar iden) {
	switch (iden) {
		case 0:
			while(flag1x>0 && servo0 > 42) {
				servo0--;
				delay1();
			}
			break;
		case 1:
			while(flag3x>0 && servo1 > 42) {
				servo1--;
				delay1();
			}
			break;
		case 2:
			while(flag5x>0 && servo2 > 42) {
				servo2--;
				delay1();
			}
			break;
		case 3:
			while(flag7x>0 && servo3 > 42) {
				servo3--;
				delay1();
			}
			break;
	}
}

// 计时器T0初始化
// 将20ms分成4段，分别控制4台电机高电平从而分别输出PWM波
void timer_int() {
	// 开启T0的16位工作模式
	TMOD = 0x11;
	// 装入计时高8位和低8位
	TH0=(65536-TIME)/256;
	TL0=(65536-TIME)%256;
	// 开启定时器T0的溢出中断
	ET0=1;
	// 允许中断信号
	EA=1;
	// 设置外部中断为边沿触发，使精度更高
	IT0=1;
	// 启动定时器T0
	TR0=1;
}

// 按键扫描函数
void get_key() {
	// 控制舵机0
	if(key0==0)
		flag0x=1;
	if(key0==1)
		flag0x=0;
	if(key1==0)
		flag1x=1;
	if(key1==1)
		flag1x=0;
	// 控制舵机1
	if(key2==0)
		flag2x=1;
	if(key2==1)
		flag2x=0;
	if(key3==0)
		flag3x=1;
	if(key3==1)
		flag3x=0;
	// 控制舵机2
	if(key4==0)
		flag4x=1;
	if(key4==1)
		flag4x=0;
	if(key5==0)
		flag5x=1;
	if(key5==1)
		flag5x=0;
	// 控制舵机3
	if(key6==0)
		flag6x=1;
	if(key6==1)
		flag6x=0;
	if(key7==0)
		flag7x=1;
	if(key7==1)
		flag7x=0;

	// 控制状态开关
	if(but0==0&&flag0==0) {
		flag0=1;
	}
	if(but0==1&&flag0==1) {
		flag0=0;
		flag0f=1;
	}
	if(but1==0&&flag1==0) {
		flag1=1;
	}
	if(but1==1&&flag1==1) {
		flag1=0;
		flag1f=1;
	}
	if(but2==0&&flag2==0) {
		flag2=1;
	}
	if(but2==1&&flag2==1) {
		flag2=0;
		motion00=servo0;
		motion01=servo1;
		motion02=servo2;
		motion03=servo3;
	}
	if(but3==0&&flag3==0) {
		flag3=1;
	}
	if(but3==1&&flag3==1) {
		flag3=0;
		motion10=servo0;
		motion11=servo1;
		motion12=servo2;
		motion13=servo3;
	}
}

// 定时器中断
// 定时器0工作方式1，定时设置为5ms
void timer() interrupt 1 {
	TH0=(65536-TIME)/256;
	TL0=(65536-TIME)%256;
	// count控制单片机以5ms为单位向4个舵机分别发送高电平信号，从而控制4台舵机
	// 0~3分别对应4台舵机
    switch(count) {
        case 0:
            PWM0=1;
            delay(servo0);
            PWM0=0;
            break;
        case 1:
            PWM1=1;
            delay(servo1);
            PWM1=0;
            break;
        case 2:
            PWM2=1;
            delay(servo2);
            PWM2=0;
            break;
        case 3:
            PWM3=1;
            delay(servo3);
            PWM3=0;
            break;
    }
    get_key();
    count=(count+1)%4;
}

void main() {
    timer_int();
    while(1) {
		// 按键控制转动
		if(flag0x==1) {
			turn_inf(0);
		}
		if(flag1x==1) {
			reverse_inf(0);
		}
		if(flag2x==1) {
			turn_inf(1);
		}
		if(flag3x==1) {
			reverse_inf(1);
		}
		if(flag4x==1) {
			turn_inf(2);
		}
		if(flag5x==1) {
			reverse_inf(2);
		}
		if(flag6x==1) {
			turn_inf(3);
		}
		if(flag7x==1) {
			reverse_inf(3);
		}
		// 动作组
		if(flag0f==1) {
			turn(3,motion03);
			turn(1,motion01);
			turn(2,motion02);
			turn(0,motion00);
			flag0f=0;
		}
		if(flag1f==1) {
			turn(2,motion12);
			turn(1,motion11);
			turn(3,motion13);
			turn(0,motion10);
			flag1f=0;
		}
    }
}