/* 
* debug header
*
* Author: Joemel John Diente <joemeljohn.diente@microchip.com>
* 
*/

#ifndef __MY_DEBUG_
#define __MY_DEBUG_

// Debug
// #define MY_DEBUG
// #define MY_DEBUG_SHOW_ADVANCED
// #define MY_DEBUG_SHOW_FUNCTION

/* Debug utilities */
#ifdef MY_DEBUG // debug 
#include <stdio.h>
#define PRINT_BUF_RANGE(buf,y,z) {if (buf == NULL) {printf("DEBUG: invalid buffer\r\n");} \
                    else { printf("DEBUG: print buffer\r\n"); \
                        int i = 0; for (i = y; i <= z; i++) \
                        printf("buffer[%d]: 0x%X\n", i, buf[i]);} }
#ifdef MY_DEBUG_SHOW_ADVANCED
#define PRINT_RES(y,z) 	{ printf("[DEBUG: %s] %s: 0x%X\r\n", __FUNCTION__, y, z); }
#define PRINT_FUN() { printf("[DEBUG: Entered %s] \r\n", __FUNCTION__); }
#else //Show function
#define PRINT_FUN() {}
#define PRINT_RES(y,z) {}
#endif //Show Function

#else // debug
#define PRINT_BUF_RANGE(x,y,z) {}
#define PRINT_FUN() {}
#define PRINT_RES(y,z) {}
#endif // debug 

/* End of User Configuration */

#endif /* __MY_DEBUG_ */