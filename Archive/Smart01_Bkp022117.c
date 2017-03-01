/*
	Daniel Velazquez
	Sept 2016
	Smart ForTwo Tachometer-mod
*/

#include <allegro.h>
#include <time.h>    //Time
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "linux/i2c-dev.h"

double time_diff(struct timeval x , struct timeval y);

int main (void)
{
	/* I2C Part */
        char FileName[70]="FileTest.txt";
	FILE *Log;
        Log= fopen(FileName,"w");

	struct timeval before , after;

	int i2c_dev1_node = 0;
	int i2c_dev2_node = 0;
	int i2c_dev1_address = 0x68;  /* Device Address 01 */
	int i2c_dev2_address = 0x69;  /* Device Address 02 */
	int i2c_dev_reg_addr = 0x6B; /* POWER_CTL Register */
	char i2c_dev_node_path[] = "/dev/i2c-1"; /* Adapter Device Node*/

	int ret_val = 0;
	int i;
	/* Initialize all empty */
	float GForcesX= 0;
	float GForcesY= 0;
	float GForcesZ= 0;
	float Temp= 0;
	float GyroX= 0;
	float GyroY= 0;
	float GyroZ= 0;
	/* wildcard to be used on all H and L registers */
	int Read_Value_High = 0;
	int Read_Value_Low = 0;
	int Read_Value;

	int MPU6050_dev1_Register_[16];	//this one contains all MPU6050 Registers for Dev1
	int MPU6050_dev2_Register_[16];	//this one contains all MPU6050 Registers for Dev2
	/* Open the device node for the I2C adapter of bus 1 */
	i2c_dev1_node = open(i2c_dev_node_path, O_RDWR);
	if (i2c_dev1_node < 0) {
		perror("Unable to open device node.");
		exit(1);
	}
	/* Open the device node for the I2C adapter of bus 2 */
	i2c_dev2_node = open(i2c_dev_node_path, O_RDWR);
	if (i2c_dev2_node < 0) {
		perror("Unable to open device node.");
		exit(1);
	}
	/* Set I2C_SLAVE for Sensor 1 */
	ret_val = ioctl(i2c_dev1_node,I2C_SLAVE,i2c_dev1_address);
	if (ret_val < 0) {
		perror("Could not set I2C_SLAVE.");
		exit(2);
	}
	/* Set I2C_SLAVE for Sensor 2 */
	ret_val = ioctl(i2c_dev2_node,I2C_SLAVE,i2c_dev2_address);
	if (ret_val < 0) {
		perror("Could not set I2C_SLAVE.");
		exit(2);
	}

    /* I2C Part above here */

   /* the Allegro part starts here */
   BITMAP *Background;
   BITMAP *ALightTT;
   PALETTE the_palette;

   int RPMs_i;

//   FONT *Bigfont = load_font("ArialBlack.ttf", NULL, NULL);

   if (allegro_init() != 0)
      return 1;

   install_keyboard();

   set_color_depth(32); 
   if (set_gfx_mode(GFX_AUTODETECT, 320, 240, 0, 0) != 0) {
      if (set_gfx_mode(GFX_SAFE, 320, 240, 0, 0) != 0) {
	 set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
	 allegro_message("Unable to set any graphic mode\n%s\n", allegro_error);
	 return 1;
      }
   }

   Background = load_bitmap("SmartBackground.bmp", the_palette);	   // Load the Background
   ALightTT = load_bitmap("AutoLightsTT.bmp", the_palette);   		   // Load the AutoLights Telltale

/*   if (!the_image) {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Error reading bitmap file '%s'\n", argv[1]);
      return 1;
   }
*/

   /* select the bitmap palette */
   set_palette(the_palette);

   // Show Background
   blit(Background, screen, 0, 0, (SCREEN_W-Background->w)/2,(SCREEN_H-Background->h)/2, Background->w, Background->h);
   // Show Auto Lights Telltale
   draw_sprite(screen,ALightTT, 0,187);
   destroy_bitmap(ALightTT);


   // Temperature Sign
   textout_ex(screen, font, "TEMP", 10, 10, makecol(0,0,0), -1);

	while(1){
	    gettimeofday(&before , NULL);
		// Read All Values
		for(i=59; i<=72;i++){
			MPU6050_dev1_Register_[i] = i2c_smbus_read_byte_data(i2c_dev1_node, i);
			MPU6050_dev2_Register_[i] = i2c_smbus_read_byte_data(i2c_dev2_node, i);
	}
		Temp= (MPU6050_dev2_Register_[65] << 8) | MPU6050_dev2_Register_[66]; 
		Temp= ((Temp-65535)/340)+36.53;
		textprintf_ex(screen, font, 150, 20, makecol(0,0,0), makecol(200,200,200), "%f", Temp);
		fprintf(Log,"%f\n",Temp);
		gettimeofday(&after , NULL);
//     sleep(1);
    }


   /* destroy the bitmap */
   destroy_bitmap(Background);
   fclose(Log);
   readkey();
   return 0;
}

double time_diff(struct timeval x , struct timeval y)
{
    double x_ms , y_ms , diff;
     
    x_ms = (double)x.tv_sec*1000000 + (double)x.tv_usec;
    y_ms = (double)y.tv_sec*1000000 + (double)y.tv_usec;
     
    diff = (double)y_ms - (double)x_ms;
     
    return diff;
}

END_OF_MAIN()
