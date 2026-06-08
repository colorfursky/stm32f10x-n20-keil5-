# stm32f10x-n20-keil5-
本工程是基于stm32f103c8t6最小开发板所编写的工程。依赖于外接9-TB6612电机驱动驱动n20电机每天定时控制转动设定的秒数。也支持按钮设定时间或电机闹钟，增加闹钟，删除闹钟。  
硬件：  
OLED采用四针脚（I2C协议通信）  
      SCL接PB8 SDA接PB9 剩下两个供电引脚  
9-TB6612电机驱动模块  
      PWMA接PA8 AIN2接PA5 AIN1接PA4 STBY接PA3  
      VM接5V VCC接3.3V GND接GND AIO两端接电机  
左中右按钮分别接 PA10 PA11 PA12  
3.3V给VBT接上  
软件：  
Display.c依赖Alarm.c  
编译产生大小约13KB： Program Size: Code=11352 RO-data=3456 RW-data=32 ZI-data=2760  
