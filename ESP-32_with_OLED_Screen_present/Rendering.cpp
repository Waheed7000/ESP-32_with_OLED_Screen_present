#include "Rendering.h"

static Adafruit_SH1106G* disp = nullptr;

void Rendering_init(Adafruit_SH1106G* displayPtr) {
  disp = displayPtr;
}

static void drawCenteredText(const Screen* s) {
  disp->setTextSize(s->textSize > 0 ? s->textSize : 1);
  disp->setTextColor(SH110X_WHITE);

  int16_t x1, y1;
  uint16_t w, h;
  uint16_t lineHeight = 0;

  // measure total block height first
  for (uint8_t i = 0; i < s->textLineCount; i++) {
    disp->getTextBounds(s->textLines[i], 0, 0, &x1, &y1, &w, &h);
    if (h > lineHeight) lineHeight = h;
  }
  uint16_t gap = 4;
  uint16_t totalHeight = (lineHeight * s->textLineCount) + (gap * (s->textLineCount - 1));
  uint16_t startY = (disp->height() - totalHeight) / 2;

  for (uint8_t i = 0; i < s->textLineCount; i++) {
    disp->getTextBounds(s->textLines[i], 0, 0, &x1, &y1, &w, &h);
    uint16_t x = (disp->width() - w) / 2;
    uint16_t y = startY + i * (lineHeight + gap);
    disp->setCursor(x, y);
    disp->print(s->textLines[i]);
  }
}

static void drawFrame(const ScreenFrame* f) {
  int16_t x = (disp->width() - f->width) / 2;
  int16_t y = (disp->height() - f->height) / 2;
  disp->drawBitmap(x, y, f->data, f->width, f->height, SH110X_WHITE);
}

void Rendering_draw(const Screen* screen, uint32_t holdMs) {
  if (disp == nullptr || screen == nullptr) return;

  switch (screen->type) {

    case SCREEN_TYPE_TEXT: {
      disp->clearDisplay();
      drawCenteredText(screen);
      disp->display();
      break;
    }

    case SCREEN_TYPE_IMAGE: {
      disp->clearDisplay();
      if (screen->frameCount > 0) drawFrame(&screen->frames[0]);
      disp->display();
      break;
    }

    case SCREEN_TYPE_GIF: {
      if (screen->frameCount == 0) break;
      uint32_t elapsed = 0;
      uint8_t frameIndex = 0;
      while (elapsed < holdMs) {
        disp->clearDisplay();
        drawFrame(&screen->frames[frameIndex]);
        disp->display();
        delay(screen->frameDelayMs);
        elapsed += screen->frameDelayMs;
        frameIndex = (frameIndex + 1) % screen->frameCount;
      }
      break;
    }
  }
}