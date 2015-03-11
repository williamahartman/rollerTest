#include <stdio.h>
#include <SDL.h>

#define BAR_SIZE 40
#define NUM_ROLLERS 4

void scrollThroughGamepads();

SDL_GameController* pads[NUM_ROLLERS];
SDL_Haptic* padHaptics[NUM_ROLLERS];
int activeRoller;
int numRollers;

void TryAddControllers()
{
	if (numRollers < SDL_NumJoysticks()){
		SDL_GameController *pad = SDL_GameControllerOpen(numRollers);
		if(pad) {
			int i;
			for(i = 0; i < NUM_ROLLERS; i++) {
				if(!pads[i]) {
					pads[i] = pad;
					padHaptics[i] = SDL_HapticOpenFromJoystick(SDL_GameControllerGetJoystick(pad));
					numRollers++;

					if(activeRoller == -1) {
						activeRoller = i;
					}
					return;
				}
			}
		}
	}
}

void TryRemoveControllers() {
	if(numRollers > SDL_NumJoysticks()){
		int i;
		for(i = 0; i < NUM_ROLLERS; i++) {
			if(pads[i] && !SDL_GameControllerGetAttached(pads[i])) {
				pads[i] = NULL;
				if(padHaptics[i]) {
					SDL_HapticClose(padHaptics[i]);
					padHaptics[i] = NULL;
				}

				numRollers--;
				if(activeRoller == i) {
					scrollThroughGamepads();
				}
				return;
			}
		}
	}
}

void scrollThroughGamepads() {
	if(numRollers == 0) {
		activeRoller = -1;
		return;
	}

	do {
		if(activeRoller < numRollers) {
			activeRoller++;
		} else {
			activeRoller = 0;
		}
	} while(!pads[activeRoller]);
}

void rumble(SDL_GameController* pad, SDL_Haptic* haptic) {
	if (SDL_HapticRumbleInit( haptic ) != 0) {
		printf("Controller \"%s\" does not support rumble\n", SDL_GameControllerName(pad));
	   	return;
	}

	SDL_HapticRumblePlay(haptic, 1, 1000);
}

void printAxis(int val, const char* axisName) {
	int charAxisAmount = 32767 / (BAR_SIZE / 2);

	printf("%s [", axisName);
	int i;

	if(val < 0) {
		for(i = 0; i < charAxisAmount * (BAR_SIZE / 2); i += charAxisAmount) {
			if(charAxisAmount * (BAR_SIZE / 2) + val < i)
				printf("#");
			else
				printf(" ");
		}
		printf("|");
		for(i = 0; i < (BAR_SIZE / 2); i++) {printf(" ");}
	} else {
		for(i = 0; i < (BAR_SIZE / 2); i++) {printf(" ");}
		printf("|");
		for(i = 0; i < charAxisAmount * (BAR_SIZE / 2); i += charAxisAmount) {
			if(i < val)
				printf("#");
			else
				printf(" ");
		}
	}

	printf("]\n");
}

void printButton(int isOn, const char* buttonName) {
	printf("%s [%c]\n", buttonName, isOn ? '#' : ' ');
}

void printDPad(int up, int down, int left, int right) {
	printf("          [%c]\n", up ? '#' : ' ');
	printf("D-Pad: [%c]   [%c]\n", left ? '#' : ' ', right ? '#' : ' ');
	printf("          [%c]\n", down ? '#' : ' ');
}

void printTabBar() {
	int i;
	printf("\e[7m");
	if(numRollers == 0) {
		printf(" No roller founds :( \n  Plug 'em in baby!  \n");	
    } else {
    	for(i = 0; i < NUM_ROLLERS; i++) {
    		if(pads[i]) {
    			if(i == activeRoller)
    				printf("\e[27m");
    	
    			printf("  %s (#%d)  ", SDL_GameControllerName(pads[i]), i);

	    		if(i == activeRoller)
	    			printf("\e[7m");
    		} else {
    			printf("               ");
    		}
    	}
    }
	printf("\e[27m\n");
}

int main() {
	if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) != 0){
		printf("Could not init SDL\n");
		return 1;
	}

	numRollers = 0;
	activeRoller = -1;
	while(1) {
		SDL_Delay(16);
		printf("\e[1;1H\e[2J");

		TryRemoveControllers();
	    TryAddControllers();

		SDL_Event sdlEvent;
		while(SDL_PollEvent(&sdlEvent)) {
	        if(sdlEvent.type == SDL_QUIT) {
	        	int i;
	        	for(i = 0; i < NUM_ROLLERS; i++) {
	        		SDL_GameControllerClose(pads[i]);
	        	}
	        	SDL_QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC);
	        	return 0;
	        }
	    }

	    printTabBar();

    	if(activeRoller > -1) {
	    	printAxis(SDL_GameControllerGetAxis(pads[activeRoller], SDL_CONTROLLER_AXIS_LEFTX), "  Left Stick X");
	    	printAxis(SDL_GameControllerGetAxis(pads[activeRoller], SDL_CONTROLLER_AXIS_LEFTY), "  Left Stick Y");
	    	printAxis(SDL_GameControllerGetAxis(pads[activeRoller], SDL_CONTROLLER_AXIS_RIGHTX), " Right Stick X");
	    	printAxis(SDL_GameControllerGetAxis(pads[activeRoller], SDL_CONTROLLER_AXIS_RIGHTY), " Right Stick Y");
	    	printAxis(SDL_GameControllerGetAxis(pads[activeRoller], SDL_CONTROLLER_AXIS_TRIGGERLEFT), "  Left Trigger");
	    	printAxis(SDL_GameControllerGetAxis(pads[activeRoller], SDL_CONTROLLER_AXIS_TRIGGERRIGHT), " Right Trigger");

	    	printDPad(SDL_GameControllerGetButton(pads[activeRoller], SDL_CONTROLLER_BUTTON_DPAD_UP),
	    			  SDL_GameControllerGetButton(pads[activeRoller], SDL_CONTROLLER_BUTTON_DPAD_DOWN), 
	    			  SDL_GameControllerGetButton(pads[activeRoller], SDL_CONTROLLER_BUTTON_DPAD_LEFT), 
	    			  SDL_GameControllerGetButton(pads[activeRoller], SDL_CONTROLLER_BUTTON_DPAD_RIGHT));

	    	printButton(SDL_GameControllerGetButton(pads[activeRoller], SDL_CONTROLLER_BUTTON_A), "           A");
	    	printButton(SDL_GameControllerGetButton(pads[activeRoller], SDL_CONTROLLER_BUTTON_B), "           B");
	    	printButton(SDL_GameControllerGetButton(pads[activeRoller], SDL_CONTROLLER_BUTTON_X), "           X");
	    	printButton(SDL_GameControllerGetButton(pads[activeRoller], SDL_CONTROLLER_BUTTON_Y), "           Y");
	    	printButton(SDL_GameControllerGetButton(pads[activeRoller], SDL_CONTROLLER_BUTTON_LEFTSHOULDER), "          LB");
	    	printButton(SDL_GameControllerGetButton(pads[activeRoller], SDL_CONTROLLER_BUTTON_RIGHTSHOULDER), "          RB"); 
	    	printButton(SDL_GameControllerGetButton(pads[activeRoller], SDL_CONTROLLER_BUTTON_LEFTSTICK), "  Left Stick");
	    	printButton(SDL_GameControllerGetButton(pads[activeRoller], SDL_CONTROLLER_BUTTON_RIGHTSTICK), " Right Stick");
	    	printButton(SDL_GameControllerGetButton(pads[activeRoller], SDL_CONTROLLER_BUTTON_START), "       Start");
	    	printButton(SDL_GameControllerGetButton(pads[activeRoller], SDL_CONTROLLER_BUTTON_BACK), "        Back");
	    	printButton(SDL_GameControllerGetButton(pads[activeRoller], SDL_CONTROLLER_BUTTON_GUIDE), "       Guide");
	    	printf("\n");

	    	if(SDL_GameControllerGetButton(pads[activeRoller], SDL_CONTROLLER_BUTTON_LEFTSHOULDER) && 
	    	   SDL_GameControllerGetButton(pads[activeRoller], SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)) {
	    		printf("Testing rumble.\t");
	    		rumble(pads[activeRoller], padHaptics[activeRoller]);
	    	} else {
	    		printf("RB + LB to test rumble.\t");
	    	}

	    	if(numRollers > 1) {
	    		printf("Start + Back to move to next Controller.\n");
	    	} else {
	    		printf("\n");
	    	}

	    	if(SDL_GameControllerGetButton(pads[activeRoller], SDL_CONTROLLER_BUTTON_START) && 
	    	   SDL_GameControllerGetButton(pads[activeRoller], SDL_CONTROLLER_BUTTON_BACK)) {
	    		scrollThroughGamepads();
	    	}
	    }
	}

	return 0;
}
