
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>

//---------------------------------------------------------------------------------
void consoleClear(void) {
//---------------------------------------------------------------------------------
	iprintf("\x1b[2J");
}

void writeChange(const u32* buffer) {
	// Input registers are at 0x08800000 - 0x088001FF
	*(vu32*) 0x08800184 = buffer[1];
	*(vu32*) 0x08800188 = buffer[2];
	*(vu32*) 0x0880018C = buffer[3];

	*(vu32*) 0x08800180 = buffer[0];
}

void readChange(void) {
	// Output registers are at 0x08800100 - 0x088001FF
	while (*(vu32*) 0x08000180 & 0x1000); // Busy bit
}

//---------------------------------------------------------------------------------
// Program entry point
//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------


	// the vblank interrupt must be enabled for VBlankIntrWait() to work
	// since the default dispatcher handles the bios flags no vblank handler
	// is required
	irqInit();
	irqEnable(IRQ_VBLANK);

	consoleDemoInit();

	int pressed;

	u32 romAddr = 0x100000;

	while (1) {
		u32 cmd[4] = {
			0x11, // Command
			romAddr, // ROM address
			0x08100000, // Virtual address
			0x1, // Size (in 0x200 byte blocks)
		};
		writeChange(cmd);
		readChange();

		consoleClear();
		iprintf("%s", (char*)0x08100000);
		while (1) {
			scanKeys();
			pressed = keysDown();
			if (pressed & KEY_LEFT) {
				romAddr -= 0x100000;
				if (romAddr < 0x100000) romAddr = 0x100000;
				break;
			} else if (pressed & KEY_RIGHT) {
				romAddr += 0x100000;
				if (romAddr > 0x03F00000) romAddr = 0x03F00000;
				break;
			}
			VBlankIntrWait();
		}
	}
}


