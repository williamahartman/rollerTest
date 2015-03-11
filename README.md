# rollerTest

RollerTest is a test application for SDL Gamepads. This was written to familiarize myself with SDL's gamepad API, but could probably be used by people who want to test new mappings or something.

The application allows you to view the states of the axes and buttons, as well as test vibration. The application supports controllers being connected as it runs.

The application may not work correctly due to this bug: https://bugzilla.libsdl.org/show_bug.cgi?id=2309
To fix this, recompile SDL after applying the diff attached to the page.

gamecontrollerdb.txt comes from gabomdq's repo here: https://github.com/gabomdq/SDL_GameControllerDB
