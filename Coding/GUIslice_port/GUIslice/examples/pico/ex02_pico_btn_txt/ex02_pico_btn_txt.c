
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "stdio.h"
#include "pico/stdio.h"
#include "pico/multicore.h"

// Defines for resources

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX,E_ELEM_BTN_QUIT, E_ELEM_TXT};
enum {E_FONT_BTN,MAX_FONT}; // Use separate enum for fonts, MAX_FONT at end


bool    m_bQuit = false;

// Instantiate the GUI
#define MAX_PAGE            1
#define MAX_ELEM_PG_MAIN    2

// Define the maximum number of elements per page
// - To enable the same code to run on devices that support storing
//   data into Flash (PROGMEM) and those that don't, we can make the
//   number of elements in Flash dependent upon GSLC_USE_PROGMEM
// - This should allow both Arduino and ARM Cortex to use the same code
#define MAX_ELEM_PG_MAIN          2                                         // # Elems total
#if (GSLC_USE_PROGMEM)
  #define MAX_ELEM_PG_MAIN_PROG   2                                         // # Elems in Flash
#else
  #define MAX_ELEM_PG_MAIN_PROG   0                                         // # Elems in Flash
#endif
#define MAX_ELEM_PG_MAIN_RAM      MAX_ELEM_PG_MAIN - MAX_ELEM_PG_MAIN_PROG  // # Elems in RAM

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN_RAM];   // Storage for all elements in RAM
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];    // References for all elements in GUI

// Define debug message function
static int16_t DebugOut(char ch) { printf("%c", ch); return 0; }
void loop();

// Button callbacks
// - This function gets called when the button is pressed
bool CbBtnQuit(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  // Determine what type of event occurred on the button
  // - In this case we're just looking for the user releasing
  //   a touch over the button.
  if (eTouch == GSLC_TOUCH_UP_IN) {
    // Output a message when the button is pressed
    printf("Quit button pressed");
    // Set a variable flag that we can use elsewhere
    m_bQuit = true;
  }
  return true;
}

void main()
{
  //stdio_usb_init();
  gslc_InitDebug(&DebugOut);
  //delay(1000);  // NOTE: Some devices require a delay after Serial.begin() before serial port can be used

  // Initialize
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { return; }

  #if 0
  while(1) {
    printf("Init passed\n");
    tight_loop_contents();
  }
  #endif

  // Load Fonts
  if (!gslc_FontSet(&m_gui,E_FONT_BTN,GSLC_FONTREF_PTR,NULL,1)) { return; }

  // -----------------------------------
  // Create page elements
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN_RAM,m_asPageElemRef,MAX_ELEM_PG_MAIN);

  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);

  // Create background box
  gslc_ElemCreateBox_P(&m_gui,E_ELEM_BOX,E_PG_MAIN,10,50,300,150,GSLC_COL_WHITE,GSLC_COL_RED,true,true,NULL,NULL);

  // Create Quit button with text label
  gslc_ElemCreateBtnTxt_P(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,120,100,80,40,"Quit",&m_asFont[E_FONT_BTN],
    GSLC_COL_WHITE,GSLC_COL_BLUE,GSLC_COL_BLUE,GSLC_COL_BLUE,GSLC_COL_BLUE,GSLC_ALIGN_MID_MID,true,true,&CbBtnQuit,NULL);

  // -----------------------------------
  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  m_bQuit = false;

  multicore_launch_core1(loop);
  while(1){
    tight_loop_contents();
  }
}

void loop()
{
  while(1){
  // Periodically call GUIslice update function
  gslc_Update(&m_gui);

  // In most programs, we would detect the button press and take an
  // action. In this simplest of examples, the "Quit" button callback
  // just outputs a message to the serial monitor when pressed and
  // sets the variable m_bQuit to true but the main loop continues to run.
  sleep_ms(1000);
  }
}

