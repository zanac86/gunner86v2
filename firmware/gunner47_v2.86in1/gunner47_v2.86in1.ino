//
// ======================= ВНИМАНИЕ !!! =============================
//  Все настройки делаются на вкладке Constants.h
//  Почитайте там то, что на русском языке написано.
//  Либо ничего не трогайте, если собирали, как в оригинальном видео.
//
//  решение проблем можно поискать тут под спойлерами:
//  https://community.alexgyver.ru/goto/post?id=33652
// ==================================================================


// Ссылка для менеджера плат:
// https://arduino.esp8266.com/stable/package_esp8266com_index.json

#define FASTLED_USE_PROGMEM 1 // просим библиотеку FASTLED экономить память контроллера на свои палитры

#include "pgmspace.h"
#include "Constants.h"
#include <FastLED.h>
#include "Types.h"
#include "timerMinim.h"
#include <GyverButton.h>
#include "fonts.h"
#include "FavoritesManager.h"


// --- ИНИЦИАЛИЗАЦИЯ ОБЪЕКТОВ ----------
CRGB leds[NUM_LEDS];

#if defined(USE_RANDOM_SETS_IN_APP) || defined(RANDOM_SETTINGS_IN_CYCLE_MODE)
uint8_t selectedSettings = 0U;
#endif //#if defined(USE_RANDOM_SETS_IN_APP) || defined(RANDOM_SETTINGS_IN_CYCLE_MODE)

// для физической (не сенсорной) кнопки нужно поменять LOW_PULL на HIGH_PULL
// ну и кнопку нужно ставить без резистора между находящимися рядом пинами D2 и GND
GButton touch(BTN_PIN, LOW_PULL, NORM_OPEN);

// --- ИНИЦИАЛИЗАЦИЯ ПЕРЕМЕННЫХ -------
static const uint8_t maxDim = max(WIDTH, HEIGHT);

ModeType modes[MODE_AMOUNT];

uint8_t currentMode = EFF_FIRE; // 0;
// флаг установки новых параметров при смене эффекта
bool loadingFlag = true;
bool ONflag = true;
bool settChanged = false;
bool setFavoriteMode = false; // after 3/4 clicks switch to fire or rainbow


unsigned long autoplayDuration = 180;
unsigned long autoplayFavoritesDuration = 1800;
unsigned long autoPlayTimeout = 0;

unsigned char matrixValue[8][16]; //это массив для эффекта Огонь

void setup()
{
  //Serial.begin(115200);
  ESP.wdtEnable(WDTO_8S);

  // КНОПКА
  touch.setStepTimeout(100);
  touch.setClickTimeout(500);

  // ЛЕНТА/МАТРИЦА
  FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)/*.setCorrection(TypicalLEDStrip)*/;
  //FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(0xFFB0F0); // по предложению @kostyamat добавлена такая цветокоррекция "теперь можно получить практически чистый желтый цвет" и получилось плохо
  FastLED.setBrightness(BRIGHTNESS);
  if (CURRENT_LIMIT > 0)
  {
    FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
  }

  FastLED.clear();
  FastLED.show();

  delay(1000);

  demoMode = (digitalRead(BTN_PIN) == HIGH);
  if (demoMode)
  {
    selectedSettings = 0;
    autoplayDuration = 300;
  }

  CRGB cs[] = {CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Yellow, CRGB::Cyan};
  for (int i = 0; i < 3; i++)
  {
    fill_solid(leds, (demoMode) ? 1 : 10, cs[i]);
    FastLED.show();
    delay(500);
  }

  FastLED.clear();
  FastLED.show();

  restoreSettings();

  // ОСТАЛЬНОЕ
  memset(matrixValue, 0, sizeof(matrixValue)); //это массив для эффекта Огонь. странно, что его нужно залить нулями
  randomSeed(micros());
  changePower();
  favoritesInit();
  autoPlayTimeout = millis() + (autoplayDuration * 1000);
  loadingFlag = true;
  settChanged = true;
}

void nextEffect()
{
  currentMode = favoritesNext();
  FastLED.setBrightness(modes[currentMode].Brightness);
  loadingFlag = true;
  settChanged = true;
  selectedSettings = 1U; // внутри эффектов флаг сбросится внутри установки новых случайных параметров
}

void switchToEffect(uint8_t m)
{
  currentMode = m;
  FastLED.setBrightness(modes[currentMode].Brightness);
  loadingFlag = true;
  settChanged = true;
  selectedSettings = 1U; // внутри эффектов флаг сбросится внутри установки новых случайных параметров
  setFavoriteMode = true;
}

void loop()
{
  effectsTick();
  buttonTick();
  if (!settChanged)
  {
    if (millis() > autoPlayTimeout)
    {
      nextEffect();
    }
  }
  if (settChanged)
  {
    if (setFavoriteMode)
    {
      autoPlayTimeout = millis() + (autoplayFavoritesDuration * 1000);
    }
    else
    {
      autoPlayTimeout = millis() + (autoplayDuration * 1000);
    }
  }
  settChanged = false;
  setFavoriteMode = false;

  ESP.wdtFeed();                                            // покормить собаку :)
}
