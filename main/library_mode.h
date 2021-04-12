#ifndef FREQ_FINDER_H
#define FREQ_FINDER_H


/**
*@brief initializes the library mode
*/
int library_mode_init();

/**
*@brief checks sound for too high off a volume
*@return returns 1 if sound is beyond threshold
*/
int library_mode_loop();

/**
*@brief deinits the library mode
*/
void library_mode_breakdown();


#endif