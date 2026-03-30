#include "epd_driver.h"
#include "config.h"
#include "DEV_Config.h"
#include "GUI_Paint.h"
#include <stdlib.h>

#if defined(USE_WAVESHARE_4IN2_V2)
#include "EPD_4in2_V2.h"
#define WS_WIDTH EPD_4IN2_V2_WIDTH
#define WS_HEIGHT EPD_4IN2_V2_HEIGHT
static inline void ws_init_panel() { EPD_4IN2_V2_Init_Fast(0); }
static inline void ws_clear_panel() { EPD_4IN2_V2_Clear(); }
static inline void ws_display_panel(UBYTE* img) { EPD_4IN2_V2_Display(img); }
static inline void ws_sleep_panel() { EPD_4IN2_V2_Sleep(); }
#else
#include "EPD_4in2.h"
#define WS_WIDTH EPD_4IN2_WIDTH
#define WS_HEIGHT EPD_4IN2_HEIGHT
static inline void ws_init_panel() { EPD_4IN2_Init_Fast(); }
static inline void ws_clear_panel() { EPD_4IN2_Clear(); }
static inline void ws_display_panel(UBYTE* img) { EPD_4IN2_Display(img); }
static inline void ws_sleep_panel() { EPD_4IN2_Sleep(); }
#endif

static bool g_epd_initialized = false;
static UBYTE* g_epd_image = nullptr;
static UWORD g_epd_image_size = 0;

static void ensureBuffer() {
  if (g_epd_image) return;
  g_epd_image_size = ((WS_WIDTH % 8 == 0) ? (WS_WIDTH / 8) : (WS_WIDTH / 8 + 1)) * WS_HEIGHT;
  g_epd_image = (UBYTE*)malloc(g_epd_image_size);
  if (!g_epd_image) {
    Serial.println("[EPD] malloc failed");
    while (1) delay(1000);
  }
}

void gpioInit() {
  DEV_Module_Init();
}

void epdInit() {
  if (g_epd_initialized) return;
  DEV_Module_Init();
  ensureBuffer();
  ws_init_panel();
  ws_clear_panel();
  g_epd_initialized = true;
}

void epdInitFast() { epdInit(); }

static void blitImageToPaint(const uint8_t* image) {
  Paint_NewImage(g_epd_image, WS_WIDTH, WS_HEIGHT, 0, WHITE);
  Paint_SelectImage(g_epd_image);
  Paint_Clear(WHITE);
  const int rowBytes = WS_WIDTH / 8;
  for (int y = 0; y < WS_HEIGHT && y < H; ++y) {
    for (int bx = 0; bx < rowBytes && bx < (ROW_BYTES); ++bx) {
      g_epd_image[y * rowBytes + bx] = image[y * ROW_BYTES + bx];
    }
  }
}

void epdDisplay(const uint8_t *image) {
  epdInit();
  blitImageToPaint(image);
  ws_display_panel(g_epd_image);
}

void epdDisplayDual(const uint8_t *blackImage, const uint8_t *colorImage) {
  (void)colorImage;
  epdDisplay(blackImage ? blackImage : colorImage);
}

void epdDisplayDualCombo(const uint8_t *blackImage, const uint8_t *colorImage) {
  (void)colorImage;
  epdDisplay(blackImage ? blackImage : colorImage);
}

void epdDisplayFast(const uint8_t *image) { epdDisplay(image); }

void epdPartialDisplay(uint8_t *data, int xStart, int yStart, int xEnd, int yEnd) {
  (void)xStart; (void)yStart; (void)xEnd; (void)yEnd;
  epdDisplay(data);
}

void epdSleep() {
  if (!g_epd_initialized) return;
  ws_sleep_panel();
  g_epd_initialized = false;
}

void epd_wft_4in2b_init() { epdInit(); }
void epd_wft_4in2b_clear(uint8_t color) {
  (void)color;
  epdInit();
  ws_clear_panel();
}
void epd_wft_4in2b_display(const uint8_t* image, size_t len) { (void)len; epdDisplay(image); }
void epd_wft_4in2b_sleep() { epdSleep(); }
void epd_wft_4in2b_dual_plane_test(const uint8_t* black, const uint8_t* color) { (void)color; epdDisplay(black); }
void epd_wft_4in2b_ram_channel_test(const uint8_t* plane10, const uint8_t* plane13) { (void)plane13; epdDisplay(plane10); }
void epd_wft_4in2b_native_test(const uint8_t* data1, const uint8_t* data2) { (void)data2; epdDisplay(data1); }
void epd_wft_4in2b_old_compat_test(const uint8_t* black, const uint8_t* color, bool doClear) { (void)color; (void)doClear; epdDisplay(black); }
