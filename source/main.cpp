//
//  main.cpp
//  Cornell University Game Library (CUGL)
//
//  This is the main entry class for your application.  You may need to modify
//  it slightly for your application class or platform.
//
//  CUGL zlib License:
//      This software is provided 'as-is', without any express or implied
//      warranty.  In no event will the authors be held liable for any damages
//      arising from the use of this software.
//
//      Permission is granted to anyone to use this software for any purpose,
//      including commercial applications, and to alter it and redistribute it
//      freely, subject to the following restrictions:
//
//      1. The origin of this software must not be misrepresented; you must not
//      claim that you wrote the original software. If you use this software
//      in a product, an acknowledgment in the product documentation would be
//      appreciated but is not required.
//
//      2. Altered source versions must be plainly marked as such, and must not
//      be misrepresented as being the original software.
//
//      3. This notice may not be removed or altered from any source distribution.
//
//  Author: Luke Leh
//  Version: 12/8/25

// Include your application class
#include "FidgetApp.h"

// This keeps us from having to write cugl:: all the time
using namespace cugl;

// These really only apply if the game is windowed (not on mobile device)
#define GAME_WIDTH 576
#define GAME_HEIGHT 1024

/**
 * The main entry point of any CUGL application.
 *
 * This class creates the application and runs it until done.  You may
 * need to tailor this to your application, particularly the application
 * settings.  However, never modify anything below the line marked.
 *
 * @return the exit status of the application
 */
int main(int argc, char * argv[]) {
    // Change this to your application class
    FidgetApp app;
    
    /// SET YOUR APPLICATION PROPERTIES
    
    // The unique application name
    app.setName("Tuggle");
    
    // The name of your studio (for organizing save files)
    app.setOrganization("Innate Studios");

    // Set the window properties (Only applies to OS X/Windows Desktop)
    app.setDisplaySize(GAME_WIDTH, GAME_HEIGHT);
    app.setFPS(60.0f);
    app.setHighDPI(true);
    
    /// DO NOT MODIFY ANYTHING BELOW THIS LINE
    if (!app.init()) {
        return 1;
    }
    
    // Run the application until completion
    app.onStartup();
    while (app.step());
    app.onShutdown();
    
    exit(0);    // Necessary to quit on mobile devices
    return 0;   // This line is never reached
}
