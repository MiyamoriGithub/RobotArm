#include <reg51.h>

#define uchar unsigned char
#define uint unsigned int

// 定义舵机动作结构体
struct Motion {
	// 舵机识别号，1-4表示4个舵机，0为无效
	uchar iden;
	// 舵机位置
	uint motion;
};

// 定义计时器0周期，设置为5ms
#define TIME 5000

// 定义舵机动作组，初值根据实际情况修改
struct Motion motion0[10]={{1, 200}, {2, 210}, {3, 200}, {4, 200}};
struct Motion motion1[10]={{1, 135}, {2, 135}, {3, 135}, {4, 135}};
// mo0 和 mo1 为动作组记忆计数
uchar mo0=0;
uchar mo1=0;

// 定义舵机位置，当前初值为舵机中位
uint servo1=135;
uint servo2=135;
uint servo3=135;
uint servo4=135;

// 定义舵机PWM波输出
sbit PWM1=P3^0;	
sbit PWM2=P3^1;
sbit PWM3=P3^2;
sbit PWM4=P3^3;

// 定义LED状态指示灯信号输出
sbit LED1=P0^1;
sbit LED2=P0^2;

// key0-key7定义舵机控制按键信号，两个一组控制舵机位置加减
sbit key0=P2^0;
sbit key1=P2^1;
sbit key2=P2^2;
sbit key3=P2^3;
sbit key4=P2^4;
sbit key5=P2^5;
sbit key6=P2^6;
sbit key7=P2^7;
// but0-but3定义功能按键信号
// 0、1为开启动作组按键，2、3为动作组记忆状态按键
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
uchar state2=0;
uchar state3=0;

// 舵机控制按键标志位
uchar turn_flag=0;

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

// 转动函数，iden为舵机识别号，target为目标数值，
// 在动作组中被调用
void turn(uchar iden, int target) {
	int servo;
	int angle=0;
	uint i;
	switch (iden) {
		case 1: servo=servo1;break;
		case 2: servo=servo2;break;
		case 3: servo=servo3;break;
		case 4: servo=servo4;break;
	}

	// 正转
	if(target-servo>=0) {
		angle=target-servo;
		for(i=0;i<angle;i++) {
			delay1();
			switch (iden) {
				case 1: servo1++;break;
				case 2: servo2++;break;
				case 3: servo3++;break;
				case 4:	servo4++;break;
			}
		}
	} else {	// 反转
		angle=servo-target;
		for(i=0;i<angle;i++) {
			delay1();
			switch (iden) {
				case 1: servo1--;break;
				case 2: servo2--;break;
				case 3: servo3--;break;
				case 4:	servo4--;break;
			}
		}
	}
}

// 转动函数，iden为按键识别号。按键点动控制舵机时调用
// 225为舵机占空比的数值上限，42为舵机占空比的数值下限，调整可以限制舵机转动角度
void turn_inf(uchar iden) {
	switch (iden) {
		case 1:
			while(turn_flag>0 && servo1 < 225) {
				servo1++;
				delay1();
			}
			break;
		case 2:
			while(turn_flag>0 && servo1 > 42) {
				servo1--;
				delay1();
			}
			break;
		case 3:
			while(turn_flag>0 && servo2 < 225) {
				servo2++;
				delay1();
			}
			break;
		case 4:
			while(turn_flag>0 && servo2 > 42) {
				servo2--;
				delay1();
			}
			break;
		case 5:
			while(turn_flag>0 && servo3 < 225) {
				servo3++;
				delay1();
			}
			break;
		case 6:
			while(turn_flag>0 && servo3 > 42) {
				servo3--;
				delay1();
			}
			break;
		case 7:
			while(turn_flag>0 && servo4 < 225) {
				servo4++;
				delay1();
			}
			break;
		case 8:
			while(turn_flag>0 && servo4 > 42) {
				servo4--;
				delay1();
			}
			break;
	}
	// 实现记忆功能，当标志位打开且动作组计数小于10，则将当前的舵机识别号和舵机状态存入动作组
	if(state2==1 && mo0 <10) {
		switch (iden) {
			case 1: motion0[mo0].iden=1; motion0[mo0].motion=servo1; break;
			case 2: motion0[mo0].iden=1; motion0[mo0].motion=servo1; break;
			case 3: motion0[mo0].iden=2; motion0[mo0].motion=servo2; break;
			case 4: motion0[mo0].iden=2; motion0[mo0].motion=servo2; break;
			case 5: motion0[mo0].iden=3; motion0[mo0].motion=servo3; break;
			case 6: motion0[mo0].iden=3; motion0[mo0].motion=servo3; break;
			case 7: motion0[mo0].iden=4; motion0[mo0].motion=servo4; break;
			case 8: motion0[mo0].iden=4; motion0[mo0].motion=servo4; break;
		}
		++mo0;
	}
	if(state3==1 && mo1 <10) {
		switch (iden) {
			case 1: motion0[mo1].iden=1; motion1[mo1].motion=servo1; break;
			case 2: motion0[mo0].iden=1; motion1[mo1].motion=servo1; break;
			case 3: motion0[mo0].iden=2; motion1[mo1].motion=servo2; break;
			case 4: motion0[mo0].iden=2; motion1[mo1].motion=servo2; break;
			case 5: motion0[mo0].iden=3; motion1[mo1].motion=servo3; break;
			case 6: motion0[mo0].iden=3; motion1[mo1].motion=servo3; break;
			case 7: motion0[mo0].iden=4; motion1[mo1].motion=servo4; break;
			case 8: motion0[mo0].iden=4; motion1[mo1].motion=servo4; break;
		}
		++mo1;
	}
}

// 计时器T0初始化
// 将20ms分成4段，分别控制4台电机高电平从而分别输出PWM波
void timer_int() {
	// 开启T0的16位工作模式
	TMOD = 0x11;
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
	// 两个按键为一组控制舵机的正转和反转。点动控制，不作消抖处理
	// 按键按下后，turn_flag 标志位置于对应状态，按键抬起后，turn_flag 标志位归零

	// 控制舵机1
	if(key0==0)
		turn_flag=1;
	if(key0==1 && turn_flag==1)
		turn_flag=0;
	if(key1==0)
		turn_flag=2;
	if(key1==1 && turn_flag==2)
		turn_flag=0;
	// 控制舵机2
	if(key2==0)
		turn_flag=3;
	if(key2==1 && turn_flag==3)
		turn_flag=0;
	if(key3==0)
		turn_flag=4;
	if(key3==1 && turn_flag==4)
		turn_flag=0;
	// 控制舵机3
	if(key4==0)
		turn_flag=5;
	if(key4==1 && turn_flag==5)
		turn_flag=0;
	if(key5==0)
		turn_flag=6;
	if(key5==1 && turn_flag==6)
		turn_flag=0;
	// 控制舵机4
	if(key6==0)
		turn_flag=7;
	if(key6==1 && turn_flag==7)
		turn_flag=0;
	if(key7==0)
		turn_flag=8;
	if(key7==1 && turn_flag==8)
		turn_flag=0;

	// 控制状态开关，使用标志位消除抖动
	// 动作组 0 的运行开关
	if(but0==0&&flag0==0) {
		flag0=1;
	}
	if(but0==1&&flag0==1) {
		flag0=0;
		flag0f=1;
	}
	// 动作组 1 的运行开关
	if(but1==0&&flag1==0) {
		flag1=1;
	}
	if(but1==1&&flag1==1) {
		flag1=0;
		flag1f=1;
	}
	// 动作组 0 的记忆按键，两个记忆按键是互锁关系
	if(but2==0&&flag2==0) {
		flag2=1;
	}
	if(but2==1&&flag2==1) {
		flag2=0;
		// 按键被按下后，在开关两个状态之间切换，状态通过 LED1 输出
		if(state2==0 && state3==0) {
			state2=1;
			LED1=0;
		} else {
			state2=0;
			LED1=1;
			// 记忆功能关闭，动作组计数归零
			mo0==0;
		}
	}
	// 动作组 1 的记忆按键，两个记忆按键是互锁关系
	if(but3==0&&flag3==0) {
		flag3=1;
	}
	if(but3==1&&flag3==1) {
		flag3=0;
		// 按键被按下后，在开关两个状态之间切换，状态通过 LED2 输出
		if(state3==0 && state2==0) {
			state3=1;
			LED2=0;
		} else {
			state3=0;
			LED2=1;
			// 记忆功能关闭，动作组计数归零
			mo1==0;
		}
	}
}

// 定时器中断
// 定时器0工作方式1，定时设置为5ms
void timer() interrupt 1 {
	TH0=(65536-TIME)/256;
	TL0=(65536-TIME)%256;
	// count控制单片机以5ms为单位向4个舵机分别发送高电平信号，从而控制4台舵机
	// 1~4分别对应4台舵机
    switch(count+1) {
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
        case 4:
            PWM4=1;
            delay(servo4);
            PWM4=0;
            break;
    }
    get_key();
    count=(count+1)%4;
}

void main() {
    timer_int();
    while(1) {
		// 按键控制舵机转动
		if(turn_flag>0) {
			turn_inf(turn_flag);
		}
		// 如果 but0 被按下，按照先后顺序执行存储在 motion0 中的动作组
		if(flag0f==1) {
			int ff;
			for(ff=0; ff<10; ff++) {
				// 如果识别码为无效，则之后无可执行动作，终止循环
				if(motion0[ff].iden==0) {
					break;
				}
				turn(motion0[ff].iden,motion0[ff].motion);
				motion0[ff].iden=0;
				
			}
			flag0f=0;
		}
		// 如果 but1 被按下，按照先后顺序执行存储在 motion1 中的动作组
		if(flag1f==1) {
			int gg;
			for(gg=0; gg<10; gg++) {
				// 如果识别码为无效，则之后无可执行动作，终止循环
				if(motion1[gg].iden==0) {
					break;
				}
				turn(motion1[gg].iden,motion1[gg].motion);
				motion1[gg].iden=0;
			}
			flag1f=0;
		}
    }
}