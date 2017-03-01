/*
	Daniel Velazquez
	Sept 2016
	Smart ForTwo Tachometer-mod
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


double time_diff(struct timeval x , struct timeval y);

int main (void)
{
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

   textprintf_ex(screen, font, 150, 20, makecol(0,0,0), makecol(200,200,200), "%f", Temp);

   /* destroy the bitmap */
   destroy_bitmap(Background);
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
