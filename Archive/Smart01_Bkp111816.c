/*
	Daniel Velazquez
	Sept 2016
	Smart ForTwo Tachometer-mod
*/

#include <allegro.h>

int main (void)
{
   BITMAP *Background;
   BITMAP *ALightTT;
   PALETTE the_palette;

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

   // Load the Background
   Background = load_bitmap("SmartBackground.bmp", the_palette);
   // Load the AutoLights Telltale
   ALightTT = load_bitmap("AutoLightsTT.bmp", the_palette);

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
   blit(ALightTT, screen, 0, 0, 0,187, ALightTT->w, ALightTT->h);

   // RPM Sign
   textout_ex(screen, font, "RPM", 10, 10, makecol(0,0,0), -1);
   // Actual RPM Counter
   textout_right_ex(screen, font, "2500", 150, 20, makecol(0,0,0), -1);



   /* destroy the bitmap */
   destroy_bitmap(Background);

   readkey();
   return 0;
}

END_OF_MAIN()
