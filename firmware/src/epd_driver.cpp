#include "epd_driver.h"
#include "config.h"

#include <SPI.h>

#if defined(WFT_CZ15) && defined(EPD_PANEL_42)
 #include <GxEPD2_3C.h>
 static GxEPD2_3C<GxEPD2_420c_CZ15, GxEPD2_420c_CZ15::HEIGHT> display(
 GxEPD2_420c_CZ15(PIN_EPD_CS, PIN_EPD_DC, PIN_EPD_RST, PIN_EPD_BUSY));
 #define WFT_DRIVER_NAME "GxEPD2_420c_CZ15"
#elif defined(WFT_4COLOR_F52) && defined(EPD_PANEL_42)
 #include <GxEPD2_4C.h>
 static GxEPD2_4C<GxEPD2_420c_GDEY0420F51, GxEPD2_420c_GDEY0420F51::HEIGHT /2> display(
 GxEPD2_420c_GDEY0420F51(PIN_EPD_CS, PIN_EPD_DC, PIN_EPD_RST, PIN_EPD_BUSY));
 #define WFT_DRIVER_NAME "GxEPD2_420c_GDEY0420F51"
#elif defined(WFT_4IN2B) && defined(EPD_PANEL_42)
 #include <GxEPD2_3C.h>
 #if defined(WFT_DRIVER_Z21)
  static GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT /2> display(
  GxEPD2_420c_Z21(PIN_EPD_CS, PIN_EPD_DC, PIN_EPD_RST, PIN_EPD_BUSY));
  #define WFT_DRIVER_NAME "GxEPD2_420c_Z21"
 #elif defined(WFT_DRIVER_Z98)
  static GxEPD2_3C<GxEPD2_420c_GDEY042Z98, GxEPD2_420c_GDEY042Z98::HEIGHT /2> display(
  GxEPD2_420c_GDEY042Z98(PIN_EPD_CS, PIN_EPD_DC, PIN_EPD_RST, PIN_EPD_BUSY));
  #define WFT_DRIVER_NAME "GxEPD2_420c_GDEY042Z98"
 #elif defined(WFT_DRIVER_1680)
  static GxEPD2_3C<GxEPD2_420c_1680, GxEPD2_420c_1680::HEIGHT /2> display(
  GxEPD2_420c_1680(PIN_EPD_CS, PIN_EPD_DC, PIN_EPD_RST, PIN_EPD_BUSY));
  #define WFT_DRIVER_NAME "GxEPD2_420c_1680"
 #else
  static GxEPD2_3C<GxEPD2_420c, GxEPD2_420c::HEIGHT /2> display(
  GxEPD2_420c(PIN_EPD_CS, PIN_EPD_DC, PIN_EPD_RST, PIN_EPD_BUSY));
  #define WFT_DRIVER_NAME "GxEPD2_420c"
 #endif
#else
 #include <GxEPD2_BW.h>
 #include <epd/GxEPD2_420.h>
 static GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT /2> display(
 GxEPD2_420(PIN_EPD_CS, PIN_EPD_DC, PIN_EPD_RST, PIN_EPD_BUSY));
 #define WFT_DRIVER_NAME "GxEPD2_420"
#endif

static bool g_epd_initialized = false;

static void configureSpiBus() {
 SPI.begin(PIN_EPD_SCK, -1, PIN_EPD_MOSI, PIN_EPD_CS);
 display.epd2.selectSPI(SPI, SPISettings(20000000, MSBFIRST, SPI_MODE0));
}

static void ensureDisplayReady() {
 if (g_epd_initialized) return;
 configureSpiBus();
 Serial.print("EPD init: ");
 Serial.println(WFT_DRIVER_NAME);
 display.init(0, true, 10, false);
 display.setRotation(1);
 g_epd_initialized = true;
}

void gpioInit() { pinMode(PIN_CFG_BTN, INPUT_PULLUP); configureSpiBus(); }
void epdInit() { ensureDisplayReady(); }
void epdInitFast() { ensureDisplayReady(); }

static uint8_t g_white_plane[IMG_BUF_LEN];
static uint8_t g_inverted_plane[IMG_BUF_LEN];
static uint8_t g_combo_plane[IMG_BUF_LEN];

static void ensureWhitePlane() {
 memset(g_white_plane, 0xFF, sizeof(g_white_plane));
}

static void addBlackMarkerPlane() {
#if defined(WFT_CZ15)
  // v19: expand black-plane test from tiny square to L-shape marker + short label bars
  auto setBlack = [](int x, int y) {
    if (x < 0 || x >= W || y < 0 || y >= H) return;
    int idx = y * (W / 8) + (x / 8);
    g_white_plane[idx] &= ~(0x80 >> (x % 8));
  };
  // L-shape in top-left
  for (int y = 12; y < 64; ++y) {
    for (int x = 12; x < 22; ++x) setBlack(x, y);
  }
  for (int y = 54; y < 64; ++y) {
    for (int x = 12; x < 74; ++x) setBlack(x, y);
  }
  // two short horizontal bars near title area as stronger black-plane probe
  for (int y = 84; y < 92; ++y) {
    for (int x = 24; x < 120; ++x) setBlack(x, y);
  }
  for (int y = 98; y < 106; ++y) {
    for (int x = 24; x < 90; ++x) setBlack(x, y);
  }
#endif
}

static void invertToColorPlane(const uint8_t* src) {
 for (size_t i = 0; i < IMG_BUF_LEN; ++i) g_inverted_plane[i] = ~src[i];
}

void epdDisplay(const uint8_t *image) {
 ensureDisplayReady();
#if defined(WFT_CZ15)
 ensureWhitePlane();
 addBlackMarkerPlane();
 invertToColorPlane(image);
 display.epd2.clearScreen(0xFF, 0xFF);
 display.writeImage(g_white_plane, g_inverted_plane, 0, 0, W, H, false, false, false);
 display.refresh(false);
#else
 display.writeImage(image, 0, 0, W, H, false, false, false);
 display.refresh(false);
#endif
}

void epdDisplayDual(const uint8_t *blackImage, const uint8_t *colorImage) {
 ensureDisplayReady();
#if defined(WFT_CZ15)
 ensureWhitePlane();
 const uint8_t* blackPlane = blackImage ? blackImage : g_white_plane;
 if (colorImage) invertToColorPlane(colorImage);
 else memset(g_inverted_plane, 0xFF, sizeof(g_inverted_plane));
 display.epd2.clearScreen(0xFF, 0xFF);
 display.writeImage(blackPlane, g_inverted_plane, 0, 0, W, H, false, false, false);
 display.refresh(false);
#else
 if (blackImage) {
  display.writeImage(blackImage, 0, 0, W, H, false, false, false);
 } else if (colorImage) {
  display.writeImage(colorImage, 0, 0, W, H, false, false, false);
 }
 display.refresh(false);
#endif
}

void epdDisplayDualCombo(const uint8_t *blackImage, const uint8_t *colorImage) {
 ensureDisplayReady();
#if defined(WFT_CZ15)
 memset(g_combo_plane, 0xFF, sizeof(g_combo_plane));
 for (size_t i = 0; i < IMG_BUF_LEN; ++i) {
  uint8_t b = blackImage ? blackImage[i] : 0xFF;
  uint8_t c = colorImage ? colorImage[i] : 0xFF;
  // v23 experimental combo encoding: preserve black bits, mark color bits with opposite polarity
  g_combo_plane[i] = b & ~c;
 }
 display.epd2.clearScreen(0xFF, 0xFF);
 display.writeImage(g_combo_plane, 0, 0, W, H, false, false, false);
 display.refresh(false);
#else
 epdDisplayDual(blackImage, colorImage);
#endif
}

void epdDisplayFast(const uint8_t *image) { epdDisplay(image); }

void epdPartialDisplay(uint8_t *data, int xStart, int yStart, int xEnd, int yEnd) {
 ensureDisplayReady();
 const int width = xEnd - xStart;
 const int height = yEnd - yStart;
 if (width <= 0 || height <= 0) return;
#if defined(WFT_CZ15)
 ensureWhitePlane();
 invertToColorPlane(data);
 display.writeImagePart(g_white_plane, g_inverted_plane, xStart, yStart, W, H, xStart, yStart, width, height, false, false, false);
 display.refresh(xStart, yStart, width, height);
#else
 display.writeImage(data, xStart, yStart, width, height, false, false, false);
 display.refresh(xStart, yStart, width, height);
#endif
}

void epdSleep() {
 if (!g_epd_initialized) return;
 display.hibernate();
 g_epd_initialized = false;
}

void epd_wft_4in2b_init() { epdInit(); }
void epd_wft_4in2b_clear(uint8_t color) {
 ensureDisplayReady();
 if (color == 0) display.clearScreen(0x00);
 else display.clearScreen(0xFF);
 display.refresh(false);
}
void epd_wft_4in2b_display(const uint8_t* image, size_t len) { (void)len; epdDisplay(image); }
void epd_wft_4in2b_dual_plane_test(const uint8_t* black, const uint8_t* color) {
#if defined(WFT_4IN2B) || defined(WFT_CZ15)
 ensureDisplayReady();
 Serial.println("[EPD TEST] dual_plane_test v13: practical mode, black plane forced white, content via color plane");
 display.epd2.clearScreen(0xFF, 0xFF);
 display.writeImage(nullptr, color, 0, 0, W, H, false, false, false);
 display.refresh(false);
#else
 (void)black; (void)color;
#endif
}
void epd_wft_4in2b_ram_channel_test(const uint8_t* plane10, const uint8_t* plane13) {
#if defined(WFT_4IN2B) || defined(WFT_CZ15)
 ensureDisplayReady();
 display.epd2.writeScreenBuffer(0xFF);
 display.epd2.writeImage(plane10, plane13, 0, 0, W, H, false, false, false);
 display.refresh(false);
#else
 (void)plane10; (void)plane13;
#endif
}
void epd_wft_4in2b_native_test(const uint8_t* data1, const uint8_t* data2) {
#if defined(WFT_4IN2B) || defined(WFT_CZ15)
 ensureDisplayReady();
 display.epd2.writeScreenBuffer(0xFF);
 display.epd2.writeNative(data1, data2, 0, 0, W, H, false, false, false);
 display.refresh(false);
#else
 (void)data1; (void)data2;
#endif
}
void epd_wft_4in2b_old_compat_test(const uint8_t* black, const uint8_t* color, bool doClear) {
#if defined(WFT_4IN2B) || defined(WFT_CZ15)
 ensureDisplayReady();
 if (doClear) {
  display.clearScreen(0xFF);
  display.refresh(false);
  delay(500);
 }
 display.writeImage(black, color, 0, 0, W, H, false, false, false);
 display.refresh(false);
#else
 (void)black; (void)color; (void)doClear;
#endif
}
void epd_wft_4in2b_sleep() { epdSleep(); }
