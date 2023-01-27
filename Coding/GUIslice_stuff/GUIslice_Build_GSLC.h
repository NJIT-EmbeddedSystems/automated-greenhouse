//<File !Start!>
// FILE: [GUIslice_Build_GSLC.h]
// Created by GUIslice Builder version: [0.17.b12]
//
// GUIslice Builder Generated GUI Framework File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<File !End!>

#ifndef _GUISLICE_GEN_H
#define _GUISLICE_GEN_H

// ------------------------------------------------
// Headers to include
// ------------------------------------------------
#include "GUIslice.h"
#include "GUIslice_drv.h"

// Include any extended elements
//<Includes !Start!>
// Include extended elements
#include "elem/XProgress.h"
//<Includes !End!>

// ------------------------------------------------
// Headers and Defines for fonts
// Note that font files are located within the Adafruit-GFX library folder:
// ------------------------------------------------
//<Fonts !Start!>
#if defined(DRV_DISP_TFT_ESPI)
  #error E_PROJECT_OPTIONS tab->Graphics Library should be TFT_eSPI
#endif
#include <Adafruit_GFX.h>
//<Fonts !End!>

// ------------------------------------------------
// Defines for resources
// ------------------------------------------------
//<Resources !Start!>
//<Resources !End!>

// ------------------------------------------------
// Enumerations for pages, elements, fonts, images
// ------------------------------------------------
//<Enum !Start!>
enum {MAIN_PG,WATER_PG,DATA_PG,LIGHT_PG};
enum {DATA_BUTTON,E_ELEM_BTN7,E_ELEM_BTN8,E_ELEM_TEXT1,E_ELEM_TEXT2
      ,E_ELEM_TEXT3,E_ELEM_TEXT4,LIGHT_BUTTON,RETURN_BTN,RETURN_BTN
      ,RETURN_BTN,WATER_BUTTON,WATER_INDICATOR};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {E_BUILTIN15X24,E_BUILTIN20X32,E_BUILTIN25X40,MAX_FONT};
//<Enum !End!>

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// ------------------------------------------------
// Define the maximum number of elements and pages
// ------------------------------------------------
//<ElementDefines !Start!>
#define MAX_PAGE                4

#define MAX_ELEM_MAIN_PG 4 // # Elems total on page
#define MAX_ELEM_MAIN_PG_RAM MAX_ELEM_MAIN_PG // # Elems in RAM

#define MAX_ELEM_WATER_PG 5 // # Elems total on page
#define MAX_ELEM_WATER_PG_RAM MAX_ELEM_WATER_PG // # Elems in RAM

#define MAX_ELEM_DATA_PG 1 // # Elems total on page
#define MAX_ELEM_DATA_PG_RAM MAX_ELEM_DATA_PG // # Elems in RAM

#define MAX_ELEM_LIGHT_PG 3 // # Elems total on page
#define MAX_ELEM_LIGHT_PG_RAM MAX_ELEM_LIGHT_PG // # Elems in RAM
//<ElementDefines !End!>

// ------------------------------------------------
// Create element storage
// ------------------------------------------------
gslc_tsGui                      m_gui;
gslc_tsDriver                   m_drv;
gslc_tsFont                     m_asFont[MAX_FONT];
gslc_tsPage                     m_asPage[MAX_PAGE];

//<GUI_Extra_Elements !Start!>
gslc_tsElem                     m_asPage1Elem[MAX_ELEM_MAIN_PG_RAM];
gslc_tsElemRef                  m_asPage1ElemRef[MAX_ELEM_MAIN_PG];
gslc_tsElem                     m_asPage2Elem[MAX_ELEM_WATER_PG_RAM];
gslc_tsElemRef                  m_asPage2ElemRef[MAX_ELEM_WATER_PG];
gslc_tsElem                     m_asPage3Elem[MAX_ELEM_DATA_PG_RAM];
gslc_tsElemRef                  m_asPage3ElemRef[MAX_ELEM_DATA_PG];
gslc_tsElem                     m_asPage4Elem[MAX_ELEM_LIGHT_PG_RAM];
gslc_tsElemRef                  m_asPage4ElemRef[MAX_ELEM_LIGHT_PG];
gslc_tsXProgress                m_sXBarGauge1;

#define MAX_STR                 100

//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Program Globals
// ------------------------------------------------

// Element References for direct access
//<Extern_References !Start!>
extern gslc_tsElemRef* m_pElemProgress1;
//<Extern_References !End!>

// Define debug message function
static int16_t DebugOut(char ch);

// ------------------------------------------------
// Callback Methods
// ------------------------------------------------
bool CbBtnCommon(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY);
bool CbCheckbox(void* pvGui, void* pvElemRef, int16_t nSelId, bool bState);
bool CbDrawScanner(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);
bool CbKeypad(void* pvGui, void *pvElemRef, int16_t nState, void* pvData);
bool CbListbox(void* pvGui, void* pvElemRef, int16_t nSelId);
bool CbSlidePos(void* pvGui,void* pvElemRef,int16_t nPos);
bool CbSpinner(void* pvGui, void *pvElemRef, int16_t nState, void* pvData);
bool CbTickScanner(void* pvGui,void* pvScope);

// ------------------------------------------------
// Create page elements
// ------------------------------------------------
void InitGUIslice_gen()
{
  gslc_tsElemRef* pElemRef = NULL;

  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { return; }

  // ------------------------------------------------
  // Load Fonts
  // ------------------------------------------------
//<Load_Fonts !Start!>
    if (!gslc_FontSet(&m_gui,E_BUILTIN15X24,GSLC_FONTREF_PTR,NULL,3)) { return; }
    if (!gslc_FontSet(&m_gui,E_BUILTIN20X32,GSLC_FONTREF_PTR,NULL,4)) { return; }
    if (!gslc_FontSet(&m_gui,E_BUILTIN25X40,GSLC_FONTREF_PTR,NULL,5)) { return; }
//<Load_Fonts !End!>

//<InitGUI !Start!>
  gslc_PageAdd(&m_gui,MAIN_PG,m_asPage1Elem,MAX_ELEM_MAIN_PG_RAM,m_asPage1ElemRef,MAX_ELEM_MAIN_PG);
  gslc_PageAdd(&m_gui,WATER_PG,m_asPage2Elem,MAX_ELEM_WATER_PG_RAM,m_asPage2ElemRef,MAX_ELEM_WATER_PG);
  gslc_PageAdd(&m_gui,DATA_PG,m_asPage3Elem,MAX_ELEM_DATA_PG_RAM,m_asPage3ElemRef,MAX_ELEM_DATA_PG);
  gslc_PageAdd(&m_gui,LIGHT_PG,m_asPage4Elem,MAX_ELEM_LIGHT_PG_RAM,m_asPage4ElemRef,MAX_ELEM_LIGHT_PG);

  // NOTE: The current page defaults to the first page added. Here we explicitly
  //       ensure that the main page is the correct page no matter the add order.
  gslc_SetPageCur(&m_gui,MAIN_PG);
  
  // Set Background to a flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_TEAL);

  // -----------------------------------
  // PAGE: MAIN_PG
  
  
  // Create E_ELEM_TEXT1 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT1,MAIN_PG,(gslc_tsRect){189,30,421,42},
    (char*)"Greenhouse GUI",0,E_BUILTIN25X40);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  
  // create WATER_BUTTON button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,WATER_BUTTON,MAIN_PG,
    (gslc_tsRect){10,290,160,60},(char*)"Water",0,E_BUILTIN20X32,&CbBtnCommon);
  
  // create LIGHT_BUTTON button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,LIGHT_BUTTON,MAIN_PG,
    (gslc_tsRect){320,290,160,60},(char*)"Light",0,E_BUILTIN20X32,&CbBtnCommon);
  
  // create DATA_BUTTON button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,DATA_BUTTON,MAIN_PG,
    (gslc_tsRect){630,290,160,60},(char*)"Data",0,E_BUILTIN20X32,&CbBtnCommon);

  // -----------------------------------
  // PAGE: WATER_PG
  

  // Create progress bar WATER_INDICATOR 
  pElemRef = gslc_ElemXProgressCreate(&m_gui,WATER_INDICATOR,WATER_PG,&m_sXBarGauge1,
    (gslc_tsRect){250,50,300,30},0,100,0,GSLC_COL_GREEN,false);
  m_pElemProgress1 = pElemRef;
  
  // Create E_ELEM_TEXT2 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT2,WATER_PG,(gslc_tsRect){170,10,460,34},
    (char*)"Current Water Levels",0,E_BUILTIN20X32);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  
  // create RETURN_BTN button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,RETURN_BTN,WATER_PG,
    (gslc_tsRect){320,280,160,60},(char*)"Return",0,E_BUILTIN20X32,&CbBtnCommon);
  
  // create E_ELEM_BTN7 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN7,WATER_PG,
    (gslc_tsRect){220,120,160,60},(char*)"Return",0,E_BUILTIN20X32,&CbBtnCommon);
  
  // create E_ELEM_BTN8 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN8,WATER_PG,
    (gslc_tsRect){460,110,160,60},(char*)"Return",0,E_BUILTIN20X32,&CbBtnCommon);

  // -----------------------------------
  // PAGE: DATA_PG
  
  
  // create RETURN_BTN button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,RETURN_BTN,DATA_PG,
    (gslc_tsRect){320,260,160,60},(char*)"Return",0,E_BUILTIN20X32,&CbBtnCommon);

  // -----------------------------------
  // PAGE: LIGHT_PG
  
  
  // Create E_ELEM_TEXT3 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT3,LIGHT_PG,(gslc_tsRect){100,20,600,34},
    (char*)"Current Temperature Level",0,E_BUILTIN20X32);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  
  // Create E_ELEM_TEXT4 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT4,LIGHT_PG,(gslc_tsRect){350,80,91,26},
    (char*)"58* F",0,E_BUILTIN15X24);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GREEN_LT3);
  
  // create RETURN_BTN button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,RETURN_BTN,LIGHT_PG,
    (gslc_tsRect){320,270,160,60},(char*)"Return",0,E_BUILTIN20X32,&CbBtnCommon);
//<InitGUI !End!>

//<Startup !Start!>
//<Startup !End!>

}

#endif // end _GUISLICE_GEN_H
