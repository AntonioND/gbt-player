
#include <gb/gb.h>
#include "gbt_player.h"

extern const unsigned char * song_Data[];

void main()
{
	disable_interrupts();

	gbt_play(song_Data, 2, 7);
	gbt_loop(0);

	add_VBL(gbt_update);
	
	set_interrupts(VBL_IFLAG);
	enable_interrupts();
	
	while(1) wait_vbl_done();
}
