#ifndef _GUISLICE_DRV_PICO_VGA_H_
#define _GUISLICE_DRV_PICO_VGA_H_


// =======================================================================
// Driver Layer for Raspberry Pi Pico VGA
// =======================================================================

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "GUIslice.h"

#include <stdio.h>

// =======================================================================
// API support definitions
// - These defines indicate whether the driver includes optimized
//   support for various APIs. If a define is set to 0, then the
//   GUIslice core emulation will be used instead.
// - At the very minimum, the point draw routine must be available:
//   gslc_DrvDrawPoint()
// =======================================================================

#define DRV_HAS_DRAW_POINT             1 ///< Support gslc_DrvDrawPoint()

#define DRV_HAS_DRAW_POINTS            0 ///< Support gslc_DrvDrawPoints()
#define DRV_HAS_DRAW_LINE              0 ///< Support gslc_DrvDrawLine()
#define DRV_HAS_DRAW_RECT_FRAME        1 ///< Support gslc_DrvDrawFrameRect()
#define DRV_HAS_DRAW_RECT_FILL         1 ///< Support gslc_DrvDrawFillRect()
#define DRV_HAS_DRAW_RECT_ROUND_FRAME  0 ///< Support gslc_DrvDrawFrameRoundRect()
#define DRV_HAS_DRAW_RECT_ROUND_FILL   0 ///< Support gslc_DrvDrawFillRoundRect()
#define DRV_HAS_DRAW_CIRCLE_FRAME      0 ///< Support gslc_DrvDrawFrameCircle()
#define DRV_HAS_DRAW_CIRCLE_FILL       0 ///< Support gslc_DrvDrawFillCircle()
#define DRV_HAS_DRAW_TRI_FRAME         0 ///< Support gslc_DrvDrawFrameTriangle()
#define DRV_HAS_DRAW_TRI_FILL          0 ///< Support gslc_DrvDrawFillTriangle()
#define DRV_HAS_DRAW_TEXT              1 ///< Support gslc_DrvDrawTxt()
#define DRV_HAS_DRAW_BMP_MEM           0 ///< Support gslc_DrvDrawBmp24FromMem()

#define DRV_OVERRIDE_TXT_ALIGN         0 ///< Driver provides text alignment

// =======================================================================
// Driver-specific members
// =======================================================================
typedef struct {
  gslc_tsColor  nColBkgnd;      ///< Background color (if not image-based)

  gslc_tsRect   rClipRect;      ///< Clipping rectangle

} gslc_tsDriver;


// =======================================================================
// Public APIs to GUIslice core library
// - These functions define the renderer / driver-dependent
//   implementations for the core drawing operations within
//   GUIslice.
// =======================================================================


// -----------------------------------------------------------------------
// Configuration Functions
// -----------------------------------------------------------------------

///
/// Initialize the Pico VGA library 
///
/// PRE:
/// - The environment variables should be configured before
///   calling gslc_DrvInit(). This can be done with gslc_DrvInitEnv()
///   or manually in user function.
///
///
/// \param[in]  pGui:      Pointer to GUI
///
/// \return true if success, false if fail
///
bool gslc_DrvInit(gslc_tsGui* pGui);


///
/// Perform any touchscreen-specific initialization
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  acDev:       Device path to touchscreen
///                          eg. "/dev/input/touchscreen"
///
/// \return true if successful
///
bool gslc_DrvInitTs(gslc_tsGui* pGui,const char* acDev);


///
/// Free up any members associated with the driver
/// - Eg. renderers, windows, background surfaces, etc.
///
/// \param[in]  pGui:         Pointer to GUI
///
/// \return none
///
void gslc_DrvDestruct(gslc_tsGui* pGui);


///
/// Get the display driver name
///
/// \param[in]  pGui:      Pointer to GUI
///
/// \return String containing driver name
///
const char* gslc_DrvGetNameDisp(gslc_tsGui* pGui);


///
/// Get the touch driver name
///
/// \param[in]  pGui:      Pointer to GUI
///
/// \return String containing driver name
///
const char* gslc_DrvGetNameTouch(gslc_tsGui* pGui);

///
/// Get the native display driver instance
/// - This can be useful to access special commands
///   available in the selected driver.
///
/// \param[in]  pGui:      Pointer to GUI
///
/// \return Void pointer to the display driver instance.
///         This pointer should be typecast to the particular
///         driver being used. If no driver was created then
///         this function will return NULL.
///
void* gslc_DrvGetDriverDisp(gslc_tsGui* pGui);

///
/// Get the native touch driver instance
/// - This can be useful to access special commands
///   available in the selected driver.
///
/// \param[in]  pGui:      Pointer to GUI
///
/// \return Void pointer to the touch driver instance.
///         This pointer should be typecast to the particular
///         driver being used. If no driver was created then
///         this function will return NULL.
///
void* gslc_DrvGetDriverTouch(gslc_tsGui* pGui);

// -----------------------------------------------------------------------
// Image/surface handling Functions
// -----------------------------------------------------------------------


///
/// Load a bitmap (*.bmp) and create a new image resource.
/// Transparency is enabled by GSLC_BMP_TRANS_EN
/// through use of color (GSLC_BMP_TRANS_RGB).
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  sImgRef:     Image reference
///
/// \return Image pointer (surface/texture) or NULL if error
///
void* gslc_DrvLoadImage(gslc_tsGui* pGui,gslc_tsImgRef sImgRef);


///
/// Configure the background to use a bitmap image
/// - The background is used when redrawing the entire page
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  sImgRef:     Image reference
///
/// \return true if success, false if fail
///
bool gslc_DrvSetBkgndImage(gslc_tsGui* pGui,gslc_tsImgRef sImgRef);

///
/// Configure the background to use a solid color
/// - The background is used when redrawing the entire page
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nCol:        RGB Color to use
///
/// \return true if success, false if fail
///
bool gslc_DrvSetBkgndColor(gslc_tsGui* pGui,gslc_tsColor nCol);

///
/// Set an element's normal-state image
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElem:       Pointer to Element to update
/// \param[in]  sImgRef:     Image reference
///
/// \return true if success, false if error
///
bool gslc_DrvSetElemImageNorm(gslc_tsGui* pGui,gslc_tsElem* pElem,gslc_tsImgRef sImgRef);

///
/// Set an element's glow-state image
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElem:       Pointer to Element to update
/// \param[in]  sImgRef:     Image reference
///
/// \return true if success, false if error
///
bool gslc_DrvSetElemImageGlow(gslc_tsGui* pGui,gslc_tsElem* pElem,gslc_tsImgRef sImgRef);


///
/// Release an image surface
///
/// \param[in]  pvImg:          Void ptr to image
///
/// \return none
///
void gslc_DrvImageDestruct(void* pvImg);


///
/// Set the clipping rectangle for future drawing updates
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  pRect:         Rectangular region to constrain edits
///
/// \return true if success, false if error
///
bool gslc_DrvSetClipRect(gslc_tsGui* pGui,gslc_tsRect* pRect);


// -----------------------------------------------------------------------
// Font handling Functions
// -----------------------------------------------------------------------

///
/// Load a font from a resource and return pointer to it
///
/// \param[in]  eFontRefType:   Font reference type (GSLC_FONTREF_PTR for Arduino)
/// \param[in]  pvFontRef:      Font reference pointer (Pointer to the GFXFont array)
/// \param[in]  nFontSz:        Typeface size to use
///
/// \return Void ptr to driver-specific font if load was successful, NULL otherwise
///
const void* gslc_DrvFontAdd(gslc_teFontRefType eFontRefType,const void* pvFontRef,uint16_t nFontSz);

///
/// Release all fonts defined in the GUI
///
/// \param[in]  pGui:          Pointer to GUI
///
/// \return none
///
void gslc_DrvFontsDestruct(gslc_tsGui* pGui);


///
/// Get the extent (width and height) of a text string
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pFont:       Ptr to Font structure
/// \param[in]  pStr:        String to display
/// \param[in]  eTxtFlags:   Flags associated with text string
/// \param[out] pnTxtX:      Ptr to offset X of text
/// \param[out] pnTxtY:      Ptr to offset Y of text
/// \param[out] pnTxtSzW:    Ptr to width of text
/// \param[out] pnTxtSzH:    Ptr to height of text
///
/// \return true if success, false if failure
///
bool gslc_DrvGetTxtSize(gslc_tsGui* pGui,gslc_tsFont* pFont,const char* pStr,gslc_teTxtFlags eTxtFlags,
        int16_t* pnTxtX,int16_t* pnTxtY,uint16_t* pnTxtSzW,uint16_t* pnTxtSzH);


///
/// Draw a text string at the given coordinate
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nTxtX:       X coordinate of top-left text string
/// \param[in]  nTxtY:       Y coordinate of top-left text string
/// \param[in]  pFont:       Ptr to Font
/// \param[in]  pStr:        String to display
/// \param[in]  eTxtFlags:   Flags associated with text string
/// \param[in]  colTxt:      Color to draw text
/// \param[in]  colBg:       Color of text background 
///
/// \return true if success, false if failure
///
bool gslc_DrvDrawTxt(gslc_tsGui* pGui,int16_t nTxtX,int16_t nTxtY,gslc_tsFont* pFont,const char* pStr,gslc_teTxtFlags eTxtFlags,gslc_tsColor colTxt,gslc_tsColor colBg);


// -----------------------------------------------------------------------
// Screen Management Functions
// -----------------------------------------------------------------------

///
/// Force a page flip to occur. This generally copies active
/// screen surface to the display.
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return none
///
void gslc_DrvPageFlipNow(gslc_tsGui* pGui);


// -----------------------------------------------------------------------
// Graphics Primitives Functions
// -----------------------------------------------------------------------

///
/// Draw a point
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nX:          X coordinate of point
/// \param[in]  nY:          Y coordinate of point
/// \param[in]  nCol:        Color RGB value to draw
///
/// \return true if success, false if error
///
bool gslc_DrvDrawPoint(gslc_tsGui* pGui,int16_t nX,int16_t nY,gslc_tsColor nCol);

///
/// Draw a point
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  asPt:        Array of points to draw
/// \param[in]  nNumPt:      Number of points in array
/// \param[in]  nCol:        Color RGB value to draw
///
/// \return true if success, false if error
///
bool gslc_DrvDrawPoints(gslc_tsGui* pGui,gslc_tsPt* asPt,uint16_t nNumPt,gslc_tsColor nCol);

///
/// Draw a framed rectangle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  rRect:       Rectangular region to frame
/// \param[in]  nCol:        Color RGB value to frame
///
/// \return true if success, false if error
///
bool gslc_DrvDrawFrameRect(gslc_tsGui* pGui,gslc_tsRect rRect,gslc_tsColor nCol);


///
/// Draw a filled rectangle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  rRect:       Rectangular region to fill
/// \param[in]  nCol:        Color RGB value to fill
///
/// \return true if success, false if error
///
bool gslc_DrvDrawFillRect(gslc_tsGui* pGui,gslc_tsRect rRect,gslc_tsColor nCol);


///
/// Draw a framed rounded rectangle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  rRect:       Rectangular region to frame
/// \param[in]  nRadius:     Radius for rounded corners
/// \param[in]  nCol:        Color RGB value to frame
///
/// \return true if success, false if error
///
bool gslc_DrvDrawFrameRoundRect(gslc_tsGui* pGui,gslc_tsRect rRect,int16_t nRadius,gslc_tsColor nCol);


///
/// Draw a filled rounded rectangle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  rRect:       Rectangular region to fill
/// \param[in]  nRadius:     Radius for rounded corners
/// \param[in]  nCol:        Color RGB value to fill
///
/// \return true if success, false if error
///
bool gslc_DrvDrawFillRoundRect(gslc_tsGui* pGui,gslc_tsRect rRect,int16_t nRadius,gslc_tsColor nCol);



///
/// Draw a line
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nX0:         Line start (X coordinate)
/// \param[in]  nY0:         Line start (Y coordinate)
/// \param[in]  nX1:         Line finish (X coordinate)
/// \param[in]  nY1:         Line finish (Y coordinate)
/// \param[in]  nCol:        Color RGB value to draw
///
/// \return true if success, false if error
///
bool gslc_DrvDrawLine(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,int16_t nX1,int16_t nY1,gslc_tsColor nCol);


///
/// Draw a framed circle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nMidX:       Center of circle (X coordinate)
/// \param[in]  nMidY:       Center of circle (Y coordinate)
/// \param[in]  nRadius:     Radius of circle
/// \param[in]  nCol:        Color RGB value to frame
///
/// \return true if success, false if error
///
bool gslc_DrvDrawFrameCircle(gslc_tsGui* pGui,int16_t nMidX,int16_t nMidY,uint16_t nRadius,gslc_tsColor nCol);


///
/// Draw a filled circle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nMidX:       Center of circle (X coordinate)
/// \param[in]  nMidY:       Center of circle (Y coordinate)
/// \param[in]  nRadius:     Radius of circle
/// \param[in]  nCol:        Color RGB value to fill
///
/// \return true if success, false if error
///
bool gslc_DrvDrawFillCircle(gslc_tsGui* pGui,int16_t nMidX,int16_t nMidY,uint16_t nRadius,gslc_tsColor nCol);


///
/// Draw a framed triangle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nX0:         X Coordinate #1
/// \param[in]  nY0:         Y Coordinate #1
/// \param[in]  nX1:         X Coordinate #2
/// \param[in]  nY1:         Y Coordinate #2
/// \param[in]  nX2:         X Coordinate #3
/// \param[in]  nY2:         Y Coordinate #3
/// \param[in]  nCol:        Color RGB value to frame
///
/// \return true if success, false if error
///
bool gslc_DrvDrawFrameTriangle(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,
        int16_t nX1,int16_t nY1,int16_t nX2,int16_t nY2,gslc_tsColor nCol);


///
/// Draw a filled triangle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nX0:         X Coordinate #1
/// \param[in]  nY0:         Y Coordinate #1
/// \param[in]  nX1:         X Coordinate #2
/// \param[in]  nY1:         Y Coordinate #2
/// \param[in]  nX2:         X Coordinate #3
/// \param[in]  nY2:         Y Coordinate #3
/// \param[in]  nCol:        Color RGB value to fill
///
/// \return true if success, false if error
///
bool gslc_DrvDrawFillTriangle(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,
        int16_t nX1,int16_t nY1,int16_t nX2,int16_t nY2,gslc_tsColor nCol);


///
/// Copy all of source image to destination screen at specified coordinate
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nDstX:       Destination X coord for copy
/// \param[in]  nDstY:       Destination Y coord for copy
/// \param[in]  sImgRef:     Image reference
///
/// \return true if success, false if fail
///
bool gslc_DrvDrawImage(gslc_tsGui* pGui,int16_t nDstX,int16_t nDstY,gslc_tsImgRef sImgRef);


///
/// Draw a monochrome bitmap from a memory array
/// - Draw from the bitmap buffer using the foreground color
///   defined in the header (unset bits are transparent)
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nDstX:       Destination X coord for copy
/// \param[in]  nDstY:       Destination Y coord for copy
/// \param[in]  pBitmap:     Pointer to bitmap buffer
/// \param[in]  bProgMem:    Bitmap is stored in Flash if true, RAM otherwise
///
/// \return none
///
void gslc_DrvDrawMonoFromMem(gslc_tsGui* pGui,int16_t nDstX, int16_t nDstY, const unsigned char *pBitmap,bool bProgMem);


///
/// Draw a color 24-bit depth bitmap from a memory array
/// - Note that users must convert images from their native
///   format (eg. BMP, PNG, etc.) into a C array. Please
///   refer to the following guide for details:
///   https://github.com/ImpulseAdventure/GUIslice/wiki/Display-Images-from-FLASH
/// - The converted file (c array) can then be included in the sketch.
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nDstX:       X coord for copy
/// \param[in]  nDstY:       Y coord for copy
/// \param[in]  pBitmap:     Pointer to bitmap buffer
/// \param[in]  bProgMem:    Bitmap is stored in Flash if true, RAM otherwise
///
/// \return none
///
void gslc_DrvDrawBmp24FromMem(gslc_tsGui* pGui,int16_t nDstX, int16_t nDstY,const unsigned char* pBitmap,bool bProgMem);

///
/// Draw a color 24-bit depth bitmap from SD card
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  filename:    Filename on SD card (usually in form "/pic.bmp")
/// \param[in]  x:           X coordinate to draw bitmap
/// \param[in]  y:           Y coordinate to draw bitmap
///
/// \return none
///
void gslc_DrvDrawBmp24FromSD(gslc_tsGui* pGui,const char *filename, uint16_t x, uint16_t y);

///
/// Copy the background image to destination screen
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return true if success, false if fail
///
void gslc_DrvDrawBkgnd(gslc_tsGui* pGui);


// -----------------------------------------------------------------------
// Touch Functions (if using display driver library)
// -----------------------------------------------------------------------

///
/// Perform any touchscreen-specific initialization
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  acDev:       Device path to touchscreen
///                          eg. "/dev/input/touchscreen"
///
/// \return true if successful
///
bool gslc_DrvInitTouch(gslc_tsGui* pGui,const char* acDev);


///
/// Get the last touch event from the internal touch handler
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[out] pnX:         Ptr to X coordinate of last touch event
/// \param[out] pnY:         Ptr to Y coordinate of last touch event
/// \param[out] pnPress:     Ptr to Pressure level of last touch event (0 for none, 1 for touch)
/// \param[out] peInputEvent Indication of event type
/// \param[out] pnInputVal   Additional data for event type
///
/// \return true if an event was detected or false otherwise
///
bool gslc_DrvGetTouch(gslc_tsGui* pGui,int16_t* pnX,int16_t* pnY,uint16_t* pnPress,gslc_teInputRawEvent* peInputEvent,int16_t* pnInputVal);


// -----------------------------------------------------------------------
// Touch Functions (if using external touch driver library)
// -----------------------------------------------------------------------

#if defined(DRV_TOUCH_TYPE_EXTERNAL)
///
/// Perform any touchscreen-specific initialization
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  acDev:       Device path to touchscreen
///                          eg. "/dev/input/touchscreen"
///
/// \return true if successful
///
bool gslc_TDrvInitTouch(gslc_tsGui* pGui,const char* acDev);


///
/// Get the last touch event from the SDL_Event handler
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[out] pnX:         Ptr to X coordinate of last touch event
/// \param[out] pnY:         Ptr to Y coordinate of last touch event
/// \param[out] pnPress:     Ptr to Pressure level of last touch event (0 for none, 1 for touch)
/// \param[out] peInputEvent Indication of event type
/// \param[out] pnInputVal   Additional data for event type
///
/// \return true if an event was detected or false otherwise
///
bool gslc_TDrvGetTouch(gslc_tsGui* pGui, int16_t* pnX, int16_t* pnY, uint16_t* pnPress, gslc_teInputRawEvent* peInputEvent, int16_t* pnInputVal);

#endif // DRV_TOUCH_*


// -----------------------------------------------------------------------
// Dynamic Screen rotation and Touch axes swap/flip functions
// -----------------------------------------------------------------------

///
/// Change rotation, automatically adapt touchscreen axes swap/flip
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nRotation:   Screen Rotation value (0, 1, 2 or 3)
///
/// \return true if successful
///
bool gslc_DrvRotate(gslc_tsGui* pGui, uint8_t nRotation);


// =======================================================================
// Private Functions
// - These functions are not included in the scope of APIs used by
//   the core GUIslice library. Instead, these functions are used
//   to support the operations within this driver layer.
// =======================================================================

void gslc_DrvSyncSignalInterrupt();
void gslc_DrvGenerateHsyncBuffer(uint8_t* buffer);
void gslc_DrvGenerateVsyncBuffer(uint8_t* buffer);
uint8_t gslc_DrvAdaptColorToRaw(gslc_tsColor nCol);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_DRV_PICO_VGA_H_
