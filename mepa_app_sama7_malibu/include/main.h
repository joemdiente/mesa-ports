/* 
* mepa_app debug header
*
* Author: Joemel John Diente <joemeljohn.diente@microchip.com>
* 
*/

#ifndef __MEPA_APP_DEBUG_
#define __MEPA_APP_DEBUG_

// Debug
// #define DEBUG_MEPA_APP
// #define DEBUG_MEPA_APP_SHOW_ADVANCED
// #define DEBUG_MEPA_APP_SHOW_FUNCTION

/* Debug utilities */
#ifdef DEBUG_MEPA_APP // debug MEPA_APP
#define PRINT_BUF_RANGE(buf,y,z) {if (buf == NULL) {printf("DEBUG: invalid buffer\r\n");} \
                    else { printf("DEBUG: print buffer\r\n"); \
                        int i = 0; for (i = y; i <= z; i++) \
                        printf("buffer[%d]: 0x%X\n", i, buf[i]);} }
#ifdef DEBUG_MEPA_APP_SHOW_ADVANCED
#define PRINT_RES(y,z) 	{ printf("[DEBUG: %s] %s: 0x%X\r\n", __FUNCTION__, y, z); }
#define PRINT_FUN() { printf("[DEBUG: Entered %s] \r\n", __FUNCTION__); }
#else //Show function
#define PRINT_FUN() {}
#define PRINT_RES(y,z) {}
#endif //Show Function

#else // debug MEPA_APP
#define PRINT_BUF_RANGE(x,y,z) {}
#define PRINT_FUN() {}
#define PRINT_RES(y,z) {}
#endif // debug MEPA_APP

/* End of User Configuration */

#endif /* __MEPA_APP_DEBUG_ */