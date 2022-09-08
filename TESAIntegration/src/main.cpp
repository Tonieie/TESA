#include <Arduino.h>
#include <M5StickCPlus.h>
#include "Filters.h"

class IMUFilter
{
  public:
    IMUFilter()
    {
		filter = new FilterOnePole(LOWPASS,200);
    }
    ~IMUFilter()
    {
		delete(filter);
    }

    void update()
    {
      filter->input(raw);
	  filtered = filter->output() - offset;
    }  

    float raw;
    float filtered;
	float offset = 0;

  private:
    FilterOnePole *filter;

};

IMUFilter ax,ay,az,gx,gy,gz;

void updateIMUFilter()
{
    ax.update();
    ay.update();
    az.update();
    gx.update();
    gy.update();
    gz.update();
}


void setup() {
    M5.begin();             // Init M5StickC Plus.  初始化 M5StickC Plus
    M5.Imu.Init();          // Init IMU.  初始化IMU
    M5.Imu.SetGyroFsr(M5.Imu.GFS_1000DPS);
    M5.Imu.SetAccelFsr(M5.Imu.AFS_2G);
    M5.Lcd.setRotation(3);  // Rotate the screen. 将屏幕旋转
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(80, 15);  // set the cursor location.  设置光标位置
    M5.Lcd.println("IMU TEST");
    M5.Lcd.setCursor(30, 30);
    M5.Lcd.println("  X       Y       Z");
    M5.Lcd.setCursor(30, 70);
    M5.Lcd.println("  Pitch   Roll    Yaw");

    for(uint8_t i=0; i < 50; i++)
    {
		M5.Imu.getGyroData(&gx.raw, &gy.raw, &gz.raw);
		M5.Imu.getAccelData(&ax.raw, &ay.raw, &az.raw);
		gx.offset += gx.raw;
		gy.offset += gy.raw;
		gz.offset += gz.raw;
		delay(50);
    }

	gx.offset /= 50.0f;
	gy.offset /= 50.0f;
	gz.offset /= 50.0f;
}

void loop() {

    // static uint32_t count=0;
    static uint32_t last_loop = millis();
    if(millis() - last_loop > 10)
    {
		last_loop = millis();

		M5.Imu.getGyroData(&gx.raw, &gy.raw, &gz.raw);
		M5.Imu.getAccelData(&ax.raw, &ay.raw, &az.raw);
		updateIMUFilter();
    }

}