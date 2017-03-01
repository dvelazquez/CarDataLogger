/*
	Daniel Velazquez
	Feb 2017
	Smart ForTwo Tachometer-mod now with data from the CAN Bus
*/



#include <allegro.h>
#include <time.h>    //Time
#include <unistd.h>
#include <string.h> 
#include <signal.h>
#include <libgen.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/uio.h> 
#include <linux/can.h>
#include <net/if.h>

extern int optind, opterr, optopt;
#define PF_CAN 29
static int skt = -1;
static int running = 1;

void print_usage(char *prg){
	fprintf(stderr, "Usage: %s [can-interface]\n", prg);
}

void sigterm(int signo){
	printf("got signal %d\n", signo);
	running = 0;
}

int main (int argc, char **argv){
	/* the CAN Part first */
	int family = PF_CAN, type = SOCK_RAW, proto = CAN_RAW;
	int opt;
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;
	int nbytes, i;
	int verbose = 0;

	signal(SIGTERM, sigterm);
	signal(SIGHUP, sigterm);
	if (optind == argc) {
		print_usage(basename(argv[0]));
		exit(0);
	}
	//printf("interface = %s, family = PF_CAN, type = SOCK_RAW, proto = CAN_RAW\n", argv[optind]);

	if ((skt = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("socket");
		return 1;
	}

	addr.can_family = family;
	strcpy(ifr.ifr_name, argv[optind]);
	ioctl(skt, SIOCGIFINDEX, &ifr);
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(skt, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		return 1;
	}



   /* the Allegro part starts here */
   BITMAP *Background;
   BITMAP *ALightTT;
   BITMAP *buffer;
   PALETTE the_palette;

   int RPM_i;
	int SensorA, SensorB, SensorC;
	float Speed;
	char Gear;
	float Speed1;
	float Speed2;
	float Speed3;
	char Lights;
	int j=0;
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
   buffer= create_bitmap(SCREEN_W,SCREEN_H);


   /* select the bitmap palette */
   set_palette(the_palette);

	while (running) {
	for(j=0;j<=100;j=j+1){
		if ((nbytes = read(skt, &frame, sizeof(frame))) < 0) {
			perror("read");
			return 1;
		}
		/* RPM Data */		
		if(frame.can_id==0x212)
			RPM_i=((frame.data[2]<<8)|frame.data[3]);
		/* Steering Sensors */
		if(frame.can_id==0x0C2){
			SensorA=(frame.data[2]);
			SensorB=(frame.data[3]);
			SensorC=(frame.data[4]);
		}
		/* Speed */
		if(frame.can_id==0x23A){
			Speed=((frame.data[0]<<8)|frame.data[1]);
			Speed=Speed/17;
		}
		/* Transmission */
		if(frame.can_id==0x418)
			Gear=(frame.data[0]);
		/* Speed from other ID */
		if(frame.can_id==0x200){
			Speed1=((frame.data[2]<<8)|frame.data[3]);
			//Speed1=Speed1/50;
			Speed2=((frame.data[4]<<8)|frame.data[5]);
			//Speed2=Speed2/50;
			Speed3=((frame.data[6]<<8)|frame.data[7]);
			//Speed3=Speed3/50;
		}
		if(frame.can_id==0x423){
			if( frame.data[3]==0x02 || frame.data[3]==0x03 ){
				draw_sprite(screen,ALightTT, 0,187);
		//		blit(ALightTT, buffer, 0, 0, 0, 187, buffer->w, buffer->h);
				Lights="ON";
			}
			else
				Lights="OFF";
		}

		}
		/* The write to CAN BUS Part */		
		/*frame.can_id = 0x123;	
		frame.data[0] = 0x01;
		frame.data[1] = 0x02;
		frame.data[2] = 0x03;
		frame.data[3] = 0x04;
		frame.data[4] = 0x05;
		frame.data[5] = 0x06;
		frame.data[6] = 0x07;
		frame.data[7] = 0x08;
		frame.can_dlc = 8;               
		write(skt, &frame, sizeof(frame));
		*/


   // Show Background
//		blit(Background, screen, 0, 0, (SCREEN_W-Background->w)/2,(SCREEN_H-Background->h)/2, Background->w, Background->h);
		blit(Background, buffer, 0, 0, 0, 0, Background->w, Background->h);  
		draw_sprite(screen,buffer, 0,0);
   // Show Auto Lights Telltale
//		draw_sprite(screen,ALightTT, 0,187);
   	
   // RPMs
		textout_ex(screen, font, "Smart ForTwo", 10, 10, makecol(0,0,0), -1);
		textprintf_ex(screen, font, 10, 20, makecol(0,0,0), makecol(160,160,160), "RPM= %i", RPM_i);
		textprintf_ex(screen, font, 10, 30, makecol(0,0,0), makecol(160,160,160), "Speed= %.0f", Speed);		
		textprintf_ex(screen, font, 10, 40, makecol(0,0,0), makecol(160,160,160), "Gear= %c", Gear);
		textprintf_ex(screen, font, 10, 50, makecol(0,0,0), makecol(160,160,160), "Sensor A= %i", SensorA);
		textprintf_ex(screen, font, 10, 60, makecol(0,0,0), makecol(160,160,160), "Sensor B= %i", SensorB);
		textprintf_ex(screen, font, 10, 70, makecol(0,0,0), makecol(160,160,160), "Sensor C= %i", SensorC);
		textprintf_ex(screen, font, 10, 90, makecol(0,0,0), makecol(180,180,180), "Speed1= %.0f", Speed1);		
		textprintf_ex(screen, font, 10, 100, makecol(0,0,0), makecol(180,180,180), "Speed2= %.0f", Speed2);		
		textprintf_ex(screen, font, 10, 110, makecol(0,0,0), makecol(180,180,180), "Speed3= %.0f", Speed3);		
		//textprintf_ex(screen, font, 10, 120, makecol(0,0,0), makecol(180,180,180), "Lights= %c", Lights);		

		clear_bitmap(buffer);
	}


   /* destroy the bitmap */
	destroy_bitmap(ALightTT);
	destroy_bitmap(Background);
   readkey();
   return 0;
}

END_OF_MAIN()
