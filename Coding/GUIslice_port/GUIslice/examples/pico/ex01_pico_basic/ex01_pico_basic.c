#include "pico/platform.h"

#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "stdio.h"
#include "pico/stdio.h"
#include "pico/multicore.h"

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX};

// Instantiate the GUI
#define MAX_PAGE            1

// Define the maximum number of elements per page
// - To enable the same code to run on devices that support storing
//   data into Flash (PROGMEM) and those that don't, we can make the
//   number of elements in Flash dependent upon GSLC_USE_PROGMEM
// - This should allow both Arduino and ARM Cortex to use the same code
#define MAX_ELEM_PG_MAIN          1                                         // # Elems total
#if (GSLC_USE_PROGMEM)
  #define MAX_ELEM_PG_MAIN_PROG   1                                         // # Elems in Flash
#else
  #define MAX_ELEM_PG_MAIN_PROG   0                                         // # Elems in Flash
#endif
#define MAX_ELEM_PG_MAIN_RAM      MAX_ELEM_PG_MAIN - MAX_ELEM_PG_MAIN_PROG  // # Elems in RAM


gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];

// Define debug message function
static int16_t DebugOut(char ch) { printf("%c", ch); return 0; }
void loop();

void main()
{
//  stdio_usb_init();
  // Initialize debug output
  gslc_InitDebug(&DebugOut);

  // Initialize
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,NULL,0)) { return; }

  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN_RAM,m_asPageElemRef,MAX_ELEM_PG_MAIN);

  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_RED);

  // Create page elements
  gslc_ElemCreateBox_P(&m_gui,E_ELEM_BOX,E_PG_MAIN,10,50,300,150,GSLC_COL_RED,GSLC_COL_WHITE,true,true,NULL,NULL);

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  loop();
}

void loop()
{
  // Periodically call GUIslice update function
  gslc_Update(&m_gui);

  // Since we can't really exit an Arduino program, we're just going to
  // enter an infinite loop here.
  while (1) { 
    tight_loop_contents();
    //printf("Hello World\n");
    //sleep_ms(1000);
  }
}
