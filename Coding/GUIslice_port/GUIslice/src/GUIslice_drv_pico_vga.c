// Compiler guard for requested driver
#include "GUIslice_config.h" // Sets DRV_DISP_*
#if defined(DRV_DISP_PICO_VGA)

// =======================================================================
// Driver Layer for Raspberry Pi Pico VGA 
// =======================================================================

// GUIslice library
#include "GUIslice_drv_pico_vga.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "pico.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/clocks.h"
#include "hardware/structs/bus_ctrl.h"

#include "font8x8_basic.h"

#include "timing.pio.h"
#include "rgb.pio.h"

#define INTERLACED 0

#ifdef __cplusplus
extern "C" {
#endif

#define WIN_HEIGHT  240
#define WIN_WIDTH   320

const char* m_acDrvDisp = "Pico VGA";
const char* m_acDrvTouch = "NONE";

static uint8_t framebuffer[WIN_HEIGHT][WIN_WIDTH+4]; // Framebuffer with extra word for black pixels (required for vga signal generation)
#define SYNC_SIZE 200
uint8_t hsync_buffer[SYNC_SIZE];
uint8_t vsync_buffer[SYNC_SIZE];
int rgb_dma_channel, timing_dma_channel;
volatile bool can_render = false;
volatile uint16_t scanline = 0;

// =======================================================================
// Public APIs to GUIslice core library
// =======================================================================

// -----------------------------------------------------------------------
// Configuration Functions
// -----------------------------------------------------------------------

bool gslc_DrvInit(gslc_tsGui* pGui) {
    if (pGui->pvDriver) {
        gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
        pDriver->nColBkgnd = GSLC_COL_BLACK;
      
        pGui->nDispW = WIN_WIDTH;
        pGui->nDispH = WIN_HEIGHT;
        pGui->bRedrawPartialEn = true;
 
        set_sys_clock_khz(130000, true);

        //stdio_usb_init();

        // Generate buffers that hold sync data bits
        gslc_DrvGenerateHsyncBuffer(hsync_buffer);
        gslc_DrvGenerateVsyncBuffer(vsync_buffer);    

        // Clear framebuffer to black
        for( int i = 0; i < WIN_HEIGHT; ++i ) {
            memset(&framebuffer[i][0], 0, (WIN_WIDTH)+4);
        }

        PIO pio = pio0;
        float freq = 25175000.0; // frequency for vga timing
        float div = (float)clock_get_hz(clk_sys) / freq;
        
        uint offset_timing = pio_add_program(pio, &timing_program);
        uint sm_timing = pio_claim_unused_sm(pio, true);

        uint offset_rgb = pio_add_program(pio, &rgb_program);
        uint sm_rgb = pio_claim_unused_sm(pio, true);
        
        rgb_program_init(pio, sm_rgb, offset_rgb, PICO_VGA_RGB_PIN_BASE, div);
        timing_program_init(pio, sm_timing, offset_timing, PICO_VGA_SYNC_PIN_BASE, div);

        rgb_dma_channel = dma_claim_unused_channel(true);
        dma_channel_config dc_rgb = dma_channel_get_default_config(rgb_dma_channel);
        channel_config_set_transfer_data_size(&dc_rgb, DMA_SIZE_32);
        channel_config_set_write_increment(&dc_rgb, false);
        channel_config_set_read_increment(&dc_rgb, true);
        channel_config_set_dreq(&dc_rgb, pio_get_dreq(pio, sm_rgb, true));
        dma_channel_configure(
            rgb_dma_channel,
            &dc_rgb,
            &pio0_hw->txf[sm_rgb],
            NULL,
            (WIN_WIDTH+4)/4,
            false
        );

        timing_dma_channel = dma_claim_unused_channel(true);
        dma_channel_config dc_timing = dma_channel_get_default_config(timing_dma_channel);
        channel_config_set_transfer_data_size(&dc_timing, DMA_SIZE_8);
        channel_config_set_write_increment(&dc_timing, false);
        channel_config_set_read_increment(&dc_timing, true);
        channel_config_set_dreq(&dc_timing, pio_get_dreq(pio, sm_timing, true));
        dma_channel_configure(
            timing_dma_channel,
            &dc_timing,
            &pio0_hw->txf[sm_timing],
            NULL,
            SYNC_SIZE,
            false
        );

        pio_sm_set_enabled(pio, sm_rgb, true);
        pio_sm_set_enabled(pio, sm_timing, true);
      
        dma_channel_set_irq0_enabled(timing_dma_channel, true);
        irq_set_exclusive_handler(DMA_IRQ_0, gslc_DrvSyncSignalInterrupt);
        irq_set_enabled(DMA_IRQ_0, true);

        gslc_DrvSyncSignalInterrupt(); 
    
    }

    pGui->bRedrawPartialEn = true;

    return true;
}

bool gslc_DrvInitTs(gslc_tsGui* pGui,const char* acDev) {
    return false;
}

void* gslc_DrvGetDriverDisp(gslc_tsGui* pGui) {
    return NULL; 
}

void gslc_DrvDestruct(gslc_tsGui* pGui) {
    return;
}

const char* gslc_DrvGetNameDisp(gslc_tsGui* pGui) {
    return m_acDrvDisp;
}

const char* gslc_DrvGetNameTouch(gslc_tsGui* pGui) {
    return m_acDrvTouch;
}

void* gslc_DrvLoadImage(gslc_tsGui* pGui,gslc_tsImgRef sImgRef) {
    return NULL;
}

bool gslc_DrvSetBkgndImage(gslc_tsGui* pGui,gslc_tsImgRef sImgRef) {
    return true;
}

bool gslc_DrvSetBkgndColor(gslc_tsGui* pGui,gslc_tsColor nCol) {
    if( pGui->pvDriver) {
        gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
        pDriver->nColBkgnd = nCol;
    }
    return true;
}

bool gslc_DrvSetElemImageNorm(gslc_tsGui* pGui,gslc_tsElem* pElem,gslc_tsImgRef sImgRef) {
    pElem->sImgRefNorm = sImgRef;
    return true;
}

bool gslc_DrvSetElemImageGlow(gslc_tsGui* pGui,gslc_tsElem* pElem,gslc_tsImgRef sImgRef) {
    pElem->sImgRefGlow = sImgRef;
    return true;
}

void gslc_DrvImageDestruct(void* pvImg) {
    return;
}

bool gslc_DrvSetClipRect(gslc_tsGui* pGui,gslc_tsRect* pRect) {
  gslc_tsDriver*  pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  if (pRect == NULL) {
    // Default to entire display
    pDriver->rClipRect.x = 0;
    pDriver->rClipRect.y = 0;
    pDriver->rClipRect.w = pGui->nDispW;
    pDriver->rClipRect.h = pGui->nDispH;
  } else {
    pDriver->rClipRect = *pRect;
  }

  return true;
}

// -----------------------------------------------------------------------
// Font handling Functions
// -----------------------------------------------------------------------


const void* gslc_DrvFontAdd(gslc_teFontRefType eFontRefType,const void* pvFontRef,uint16_t nFontSz) {
    if( eFontRefType != GSLC_FONTREF_PTR ) {
        GSLC_DEBUG2_PRINT("ERROR: DrvFontAdd(%s) failed - Pico only supports memory-based fonts\n", "");
        return NULL;
    }
    if( pvFontRef == NULL ) {
        pvFontRef = (void*)font8x8_basic;
    }
    return pvFontRef;
}

void gslc_DrvFontsDestruct(gslc_tsGui* pGui) {
    return;
}

bool gslc_DrvGetTxtSize(gslc_tsGui* pGui,gslc_tsFont* pFont,const char* pStr,gslc_teTxtFlags eTxtFlags,
        int16_t* pnTxtX,int16_t* pnTxtY,uint16_t* pnTxtSzW,uint16_t* pnTxtSzH) {
     
    *pnTxtSzW = strlen(pStr) * 8; 
    *pnTxtSzH = 8;

    *pnTxtX = 0;
    *pnTxtY = 0;
    
    return true;
}

bool gslc_DrvDrawTxt(gslc_tsGui* pGui,int16_t nTxtX,int16_t nTxtY,gslc_tsFont* pFont,const char* pStr,gslc_teTxtFlags eTxtFlags,gslc_tsColor colTxt,gslc_tsColor colBg) {

    uint8_t fontCol = gslc_DrvAdaptColorToRaw(colTxt);
    uint8_t bgCol = gslc_DrvAdaptColorToRaw(colBg);
    
    int16_t nTxtXStart = nTxtX;
    printf("Drawing Text\n");

    for( unsigned i = 0; pStr[i] != 0; ++i ) { // Loop through characters
        //uint8_t *fontChar = ((char**)(pFont->pvFont))[pStr[i]];
        uint8_t *fontChar = font8x8_basic[pStr[i]];
        if( pStr[i] == '\n' ) {
            nTxtX = nTxtXStart;
            nTxtY += 8;
            continue;
        }
        for( unsigned j = 0; j < 8; ++j ) { // Loop through scanlines
            for( unsigned k = 0; k < 8; ++k ) { // Loop through pixels/scanline
                //framebuffer[nTxtY+j][nTxtX+k] = ((fontChar[j] >> k) & 0x1) ? fontCol : bgCol;
                if((fontChar[j] >> k) & 0x1) {
                    framebuffer[nTxtY+j][nTxtX+k] = fontCol;
                }
            }
        }
        nTxtX += 8;
    }    

    return true;
}

void gslc_DrvPageFlipNow(gslc_tsGui* pGui) {
    return;
}

bool gslc_DrvDrawPoint(gslc_tsGui* pGui,int16_t nX,int16_t nY,gslc_tsColor nCol) {
    if( (nX > 0 && nX < WIN_WIDTH) && (nY > 0 && nY < WIN_HEIGHT) ) {
        framebuffer[nY][nX] = gslc_DrvAdaptColorToRaw(nCol);
    }
    return true;
}

bool gslc_DrvDrawPoints(gslc_tsGui* pGui,gslc_tsPt* asPt,uint16_t nNumPt,gslc_tsColor nCol) {
    return true;
}

bool gslc_DrvDrawFrameRect(gslc_tsGui* pGui,gslc_tsRect rRect,gslc_tsColor nCol) {
    if(!pGui) {
        return false;
    }

    if(rRect.x > WIN_WIDTH || rRect.y > WIN_HEIGHT) {
        return true;
    }

    bool drawTopLine = true, drawBottomLine = true, drawLeftLine = true, drawRightLine = true;
    uint8_t color = gslc_DrvAdaptColorToRaw(nCol);

    int x_min=rRect.x, y_min=rRect.y;
    unsigned width=rRect.w, height=rRect.h;
    if(x_min < 0) {
        width += rRect.x;
        x_min = 0;
        drawLeftLine = false;
    }
    if(y_min < 0) {
        height += rRect.y;
        y_min = 0; 
        drawTopLine = false;
    }

    if(width+x_min > WIN_WIDTH) {
        width = WIN_WIDTH-x_min;
        drawRightLine = false;
    }
    if(height+y_min > WIN_HEIGHT) {
        height = WIN_HEIGHT-y_min; 
        drawBottomLine = false;
    }    
  
    if( drawTopLine ) memset(&framebuffer[y_min][x_min], color, width); 
    if( drawBottomLine ) memset(&framebuffer[y_min+height-1][x_min], color, width); 
    if( drawLeftLine ) {
        for(int y=y_min; y<height+y_min; ++y) {
            framebuffer[y][x_min] = color;
        }
    }
    if( drawRightLine ) {
        for(int y=y_min; y<height+y_min; ++y) {
            framebuffer[y][x_min+width-1] = color;
        }
    }

    return true;
}

bool gslc_DrvDrawFillRect(gslc_tsGui* pGui,gslc_tsRect rRect,gslc_tsColor nCol) {
    if(!pGui) {
        return false;
    }

    printf("Drawing Fill Rect\n");
    if(rRect.x > WIN_WIDTH || rRect.y > WIN_HEIGHT) {
        printf("Rect.x=%d, Rect.y=%d\n",rRect.x, rRect.y);
        return true;
    }

    uint8_t color = gslc_DrvAdaptColorToRaw(nCol);

    int x_min=rRect.x, y_min=rRect.y;
    unsigned width=rRect.w, height=rRect.h;
    if(x_min < 0) {
        width += rRect.x;
        x_min = 0;
    }
    if(y_min < 0) {
        height += rRect.y;
        y_min = 0; 
    }

    if(width+x_min > WIN_WIDTH) {
        width = WIN_WIDTH-x_min;
    }
    if(height+y_min > WIN_HEIGHT) {
        height = WIN_HEIGHT-y_min; 
    }    

    for(int y=y_min; y<height+y_min; ++y) {
       memset(&framebuffer[y][x_min], color, width); 
    }
    return true;
}

bool gslc_DrvDrawFrameRoundRect(gslc_tsGui* pGui,gslc_tsRect rRect,int16_t nRadius,gslc_tsColor nCol){return true;}

bool gslc_DrvDrawFillRoundRect(gslc_tsGui* pGui,gslc_tsRect rRect,int16_t nRadius,gslc_tsColor nCol){return true;}

bool gslc_DrvDrawLine(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,int16_t nX1,int16_t nY1,gslc_tsColor nCol){return true;}

bool gslc_DrvDrawFrameCircle(gslc_tsGui* pGui,int16_t nMidX,int16_t nMidY,uint16_t nRadius,gslc_tsColor nCol){return true;}

bool gslc_DrvDrawFillCircle(gslc_tsGui* pGui,int16_t nMidX,int16_t nMidY,uint16_t nRadius,gslc_tsColor nCol){return true;}

bool gslc_DrvDrawFrameTriangle(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,
        int16_t nX1,int16_t nY1,int16_t nX2,int16_t nY2,gslc_tsColor nCol){return true;}

bool gslc_DrvDrawFillTriangle(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,
        int16_t nX1,int16_t nY1,int16_t nX2,int16_t nY2,gslc_tsColor nCol){return true;}

bool gslc_DrvDrawImage(gslc_tsGui* pGui,int16_t nDstX,int16_t nDstY,gslc_tsImgRef sImgRef){

    return true;
}

void gslc_DrvDrawMonoFromMem(gslc_tsGui* pGui,int16_t nDstX, int16_t nDstY, const unsigned char *pBitmap,bool bProgMem){return;}

void gslc_DrvDrawBmp24FromMem(gslc_tsGui* pGui,int16_t nDstX, int16_t nDstY,const unsigned char* pBitmap,bool bProgMem){return;}

void gslc_DrvDrawBmp24FromSD(gslc_tsGui* pGui,const char *filename, uint16_t x, uint16_t y){return;}

void gslc_DrvDrawBkgnd(gslc_tsGui* pGui){
    if( !pGui->pvDriver ) return;
    
    gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);

    uint8_t color = gslc_DrvAdaptColorToRaw(pDriver->nColBkgnd);
    for( unsigned i = 0; i < WIN_HEIGHT; ++i ) {
        memset( &framebuffer[i][0], color, WIN_WIDTH );
    } 
}

bool gslc_DrvInitTouch(gslc_tsGui* pGui,const char* acDev){return true;}

bool gslc_DrvGetTouch(gslc_tsGui* pGui,int16_t* pnX,int16_t* pnY,uint16_t* pnPress,gslc_teInputRawEvent* peInputEvent,int16_t* pnInputVal){return true;}

bool gslc_DrvRotate(gslc_tsGui* pGui, uint8_t nRotation){return true;}



// =======================================================================
// Private Functions
// =======================================================================

// Interrupt handler to ensure that the vga sync data is copied to the output
void gslc_DrvSyncSignalInterrupt() {
    static uint8_t *sync;     
    static uint32_t *rgb;
    #if INTERLACED
    static bool interlace = 0;
    #endif

    if( scanline < 480 ) {
        can_render = 0;
        sync = hsync_buffer;
    } else if( scanline < 490 ) {
        can_render = 1;   
    } else if( scanline < 492 ) {
        sync = vsync_buffer;
    } else if( scanline < 525 ) {
        sync = hsync_buffer;
    } else {
        scanline = 0;
        can_render = 0;
        #if INTERLACED
        interlace = !interlace;
        #endif
    }
    
    dma_hw->ints0 = 1u << timing_dma_channel;
        
    unsigned mask = 1u<<timing_dma_channel;
    if( scanline < 480 /*&& scanline != 0*/ ) {
        #if INTERLACED
        if( scanline % 2 == interlace ) {
        #endif
            //dma_hw_t dma_hw_control = dma_hw->ch[timing_dma_channel];
            mask |= 1u<<rgb_dma_channel;
            rgb = (uint32_t*)&framebuffer[scanline/2][0];
            dma_channel_set_read_addr(rgb_dma_channel, rgb, false);
        #if INTERLACED
        }
        #endif
    }

    dma_channel_set_read_addr(timing_dma_channel, sync, false);
    dma_start_channel_mask(mask);
    scanline++;
}

// Generates buffer for hsync signal
void gslc_DrvGenerateHsyncBuffer(uint8_t *buffer) {
    for( unsigned i = 0; i < 164; ++i ) {
        buffer[i] = 0xFF;
    }
    for( unsigned i = 164; i < 188; ++i ) {
        buffer[i] = 0x55;
    }
    for( unsigned i = 188; i < 200; ++i ) {
        buffer[i] = 0xFF;
    }
}

// Generates buffer for vsync signal
void gslc_DrvGenerateVsyncBuffer(uint8_t *buffer) {
    for( unsigned i = 0; i < 164; ++i ) {
        buffer[i] = 0xAA;
    }
    for( unsigned i = 164; i < 188; ++i ) {
        buffer[i] = 0x00;
    }
    for( unsigned i = 188; i < 200; ++i ) {
        buffer[i] = 0xAA;
    }
}

// Convert RGB struct to native screen format
uint8_t gslc_DrvAdaptColorToRaw(gslc_tsColor nCol) {
    uint8_t nColRaw = 0;    

    //Data layout for display B:2 G:3 R:3

    #if defined(DRV_COLORMODE_MONO)
        if( (nCol.r == 0) && (nCol.g == 0) && (nCol.b == 0) ) {
            nColRaw = 0x00; // Black
        } else {
            nColRaw = 0xFF; // White
        }
    #elif defined(DRV_COLORMODE_BGR565)
        nColRaw |= ((nCol.b & 0xC0) >> 6) << 6; 
        nColRaw |= ((nCol.g & 0xE0) >> 5) << 3;
        nColRaw |= ((nCol.r & 0xE0) >> 5) << 0;
    #else
        // Default to DRV_COLORMODE_RGB565
        nColRaw |= ((nCol.r & 0xE0) >> 5) << 0; 
        nColRaw |= ((nCol.g & 0xE0) >> 5) << 3;
        nColRaw |= ((nCol.b & 0xC0) >> 6) << 6;
    #endif

    return nColRaw;
}

#ifdef __cplusplus
}
#endif

#endif // Compiler guard for requested driver
