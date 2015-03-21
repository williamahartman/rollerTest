#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#define SCREEN_WIDTH 80
#define NUM_ROLLERS 4

typedef struct rollerState {
	int lsX, lsY, rsX, rsY, rT, lT;
	short a, b, x, y, rb, lb, rs, ls, dpUp, dpDown, dpLeft, dpRight, start, back, guide;
} RollerState;

SDL_GameController* pads[NUM_ROLLERS];
SDL_Haptic* padHaptics[NUM_ROLLERS];
int activeRoller;
int numRollers;
RollerState activeRollerState;

void setRollerButton(RollerState* s, SDL_Event sdlEvent, short targetVal) {
	switch(sdlEvent.cbutton.button) {
		case SDL_CONTROLLER_BUTTON_A: s->a = targetVal; break;
		case SDL_CONTROLLER_BUTTON_B: s->b = targetVal; break;
		case SDL_CONTROLLER_BUTTON_X: s->x = targetVal; break;
		case SDL_CONTROLLER_BUTTON_Y: s->y = targetVal; break;
		case SDL_CONTROLLER_BUTTON_BACK: s->back = targetVal; break;
		case SDL_CONTROLLER_BUTTON_GUIDE: s->guide = targetVal; break;
		case SDL_CONTROLLER_BUTTON_START: s->start = targetVal; break;
		case SDL_CONTROLLER_BUTTON_LEFTSTICK: s->ls = targetVal; break;
		case SDL_CONTROLLER_BUTTON_RIGHTSTICK: s->rs = targetVal; break;
		case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: s->lb = targetVal; break;
		case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: s->rb = targetVal; break;
		case SDL_CONTROLLER_BUTTON_DPAD_UP: s->dpUp = targetVal; break;
		case SDL_CONTROLLER_BUTTON_DPAD_DOWN: s->dpDown = targetVal; break;
		case SDL_CONTROLLER_BUTTON_DPAD_LEFT: s->dpLeft = targetVal; break;
		case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: s->dpRight = targetVal; break;
	}
}

void setRollerAxis(RollerState* s, SDL_Event sdlEvent) {
	switch(sdlEvent.caxis.axis) {
		case SDL_CONTROLLER_AXIS_LEFTX: s->lsX = sdlEvent.caxis.value; break;
		case SDL_CONTROLLER_AXIS_LEFTY: s->lsY = sdlEvent.caxis.value; break;
		case SDL_CONTROLLER_AXIS_RIGHTX: s->rsX = sdlEvent.caxis.value; break;
		case SDL_CONTROLLER_AXIS_RIGHTY: s->rsY = sdlEvent.caxis.value; break;
		case SDL_CONTROLLER_AXIS_TRIGGERLEFT: s->lT = sdlEvent.caxis.value; break;
		case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: s->rT = sdlEvent.caxis.value; break;
	}
}

void clearRollerState(RollerState* s) {
	s->lsX = 0;
	s->lsY = 0;
	s->rsX = 0;
	s->rsY = 0;
	s->rT = 0;
	s->lT = 0;
	s->a = 0;
	s->b = 0;
	s->x = 0;
	s->y = 0;
	s->rb = 0;
	s->lb = 0;
	s->rs = 0;
	s->ls = 0;
	s->dpUp = 0;
	s->dpDown = 0;
	s->dpLeft = 0;
	s->dpRight = 0;
	s->start = 0;
	s->back = 0;
	s->guide = 0;
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

void addRollers() {
	int i;
	int exit = 0;
	SDL_GameController* pad = SDL_GameControllerOpen(numRollers);

	if(pad) {
		for(i = 0; i < NUM_ROLLERS && !exit; i++) {
			if(!pads[i]) {
				pads[i] = pad;
				padHaptics[i] = SDL_HapticOpenFromJoystick(SDL_GameControllerGetJoystick(pad));
				numRollers++;

				if(activeRoller == -1) {
					activeRoller = i;
					clearRollerState(&activeRollerState);
				}
				exit = 1;
			}
		}
	}
}

void removeRollers() {
	int i;
	int exit = 0;

	for(i = 0; i < NUM_ROLLERS && !exit; i++) {
		if(pads[i] && !SDL_GameControllerGetAttached(pads[i])) {
			pads[i] = NULL;
			if(padHaptics[i]) {
				SDL_HapticClose(padHaptics[i]);
				padHaptics[i] = NULL;
			}

			numRollers--;
			if(activeRoller == i) {
				scrollThroughGamepads();
				clearRollerState(&activeRollerState);
			}
			exit = 1;
		}
	}
}

void rumble(SDL_GameController* pad, SDL_Haptic* haptic) {
	if (SDL_HapticRumbleInit(haptic) != 0) {
		printf("\e[1mController \"%s\" does not support rumble!\e[0m\n", SDL_GameControllerName(pad));
	   	return;
	}

	SDL_HapticRumblePlay(haptic, 1, 500);
}

void printAxis(int val, const char* axisName) {
	int charAxisAmount = 32767 / (SCREEN_WIDTH / 4);

	printf("%s [", axisName);
	int i;

	if(val < 0) {
		for(i = 0; i < charAxisAmount * (SCREEN_WIDTH / 4); i += charAxisAmount) {
			if(charAxisAmount * (SCREEN_WIDTH / 4) + val < i)
				printf("#");
			else
				printf(" ");
		}
		printf("|");
		for(i = 0; i < (SCREEN_WIDTH / 4); i++) {printf(" ");}
	} else {
		for(i = 0; i < (SCREEN_WIDTH / 4); i++) {printf(" ");}
		printf("|");
		for(i = 0; i < charAxisAmount * (SCREEN_WIDTH / 4); i += charAxisAmount) {
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
	int i, j;
	int tabTextLen = (SCREEN_WIDTH / NUM_ROLLERS) - 7;
	char tabText[tabTextLen];

	printf("\e[7m");
	if(numRollers == 0) {
		printf(" No roller founds :( \n  Plug 'em in baby!  \n");	
    } else {
    	for(i = 0; i < NUM_ROLLERS; i++) {
    		if(pads[i]) {
    			if(i == activeRoller)
    				printf("\e[27m");

    			strncpy(tabText, SDL_GameControllerName(pads[i]), tabTextLen);
    			printf(" %s (#%d)", tabText, i);

	    		if(i == activeRoller)
	    			printf(" \e[7m");
	    		else
	    			printf("█");
    		} else {
    			for(j = 0; j < tabTextLen + 7; j++)
    				printf(" ");
    		}
    	}
    }
	printf("\e[27m\n");
}

int main() {
	if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_JOYSTICK | 
	        		SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) != 0){
		printf("Could not init SDL\n");
		return 1;
	}

	if(SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt") < 0) {
		printf("Failed to load mappings from \"gamecontrollerdb.txt\"\n");
		return 1;
	}

	int i;
	numRollers = 0;
	activeRoller = -1;

	addRollers();

	while(1) {
		SDL_Delay(16);
		printf("\e[1;1H\e[2J");

		SDL_Event sdlEvent;
		int controllerNum;
		SDL_GameController* pad;
		while(SDL_PollEvent(&sdlEvent)) {
			switch(sdlEvent.type) {
				case SDL_CONTROLLERDEVICEADDED:
					addRollers();

				case SDL_CONTROLLERDEVICEREMOVED:
					removeRollers();

				case SDL_CONTROLLERBUTTONDOWN:
					if(sdlEvent.cbutton.which == 
						SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(pads[activeRoller]))) {
						setRollerButton(&activeRollerState, sdlEvent, 1);
					}
					break;

				case SDL_CONTROLLERBUTTONUP:
					if(sdlEvent.cbutton.which == 
						SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(pads[activeRoller]))) {
						setRollerButton(&activeRollerState, sdlEvent, 0);
					}
					break;

				case SDL_CONTROLLERAXISMOTION:
					if(sdlEvent.caxis.which == 
						SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(pads[activeRoller]))) {
						setRollerAxis(&activeRollerState, sdlEvent);
					}
					break;
	        
		        case SDL_QUIT:
		        	for(i = 0; i < NUM_ROLLERS; i++) {
		        		SDL_GameControllerClose(pads[i]);
		        	}
		        	SDL_QuitSubSystem(SDL_INIT_EVENTS | SDL_INIT_JOYSTICK | 
		        		SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC);
		        	return 0;
		    }
	    }
	    printTabBar();

    	if(activeRoller > -1) {
	    	printAxis(activeRollerState.lsX, "  Left Stick X");
	    	printAxis(activeRollerState.lsY, "  Left Stick Y");
	    	printAxis(activeRollerState.rsX, " Right Stick X");
	    	printAxis(activeRollerState.rsY, " Right Stick Y");
	    	printAxis(activeRollerState.lT,  "  Left Trigger");
	    	printAxis(activeRollerState.rT,  " Right Trigger");

	    	printDPad(activeRollerState.dpUp, activeRollerState.dpDown, 
	    			  activeRollerState.dpLeft, activeRollerState.dpRight);

	    	printButton(activeRollerState.a,     "           A");
	    	printButton(activeRollerState.b,     "           B");
	    	printButton(activeRollerState.x,     "           X");
	    	printButton(activeRollerState.y,     "           Y");
	    	printButton(activeRollerState.lb,    "          LB");
	    	printButton(activeRollerState.rb,    "          RB"); 
	    	printButton(activeRollerState.ls,    "  Left Stick");
	    	printButton(activeRollerState.rs,    " Right Stick");
	    	printButton(activeRollerState.start, "       Start");
	    	printButton(activeRollerState.back,  "        Back");
	    	printButton(activeRollerState.guide, "       Guide");
	    	printf("\n");

	    	if(activeRollerState.rb && activeRollerState.lb) {
	    		printf("Testing rumble.\n");
	    		rumble(pads[activeRoller], padHaptics[activeRoller]);
	    	} else {
	    		printf("RB + LB to test rumble. ");
	    	}

	    	if(numRollers > 1) {
	    		printf("Start + Back to move to next Controller.\n");
	    		if(activeRollerState.start && activeRollerState.back) {
		    		scrollThroughGamepads();
		    		clearRollerState(&activeRollerState);
		    	}
	    	} else {
	    		printf("\n");
	    	}
	    }
	}

	return 0;
}
