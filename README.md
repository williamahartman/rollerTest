# rollerTest

RollerTest is a test application for SDL Gamepads. This was written to familiarize myself with SDL's gamepad API, but could probably be used by people who want to test new mappings or something.

The application allows you to view the states of the axes and buttons, as well as test vibration. The application supports controllers being connected as it runs.

The program has been tested on Linux and Windows. Windows stuff is pretty busted with all the fancy ANSI escape sequence stuff (Something like https://github.com/adoxa/ansicon is totally needed). OSX would probably work, but I don't have a mac to test it on right now.

The application may not work correctly due to this SDL bug: https://bugzilla.libsdl.org/show_bug.cgi?id=2309
To fix this, recompile SDL after applying the diff attached to the page.

gamecontrollerdb.txt comes from gabomdq's repo here: https://github.com/gabomdq/SDL_GameControllerDB
