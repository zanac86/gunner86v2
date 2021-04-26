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
#ifdef ESP_USE_BUTTON
#include <GyverButton.h>
#endif
#include "fonts.h"
#include "FavoritesManager.h"


// --- ИНИЦИАЛИЗАЦИЯ ОБЪЕКТОВ ----------
CRGB leds[NUM_LEDS];

#if defined(USE_RANDOM_SETS_IN_APP) || defined(RANDOM_SETTINGS_IN_CYCLE_MODE)
uint8_t selectedSettings = 0U;
#endif //#if defined(USE_RANDOM_SETS_IN_APP) || defined(RANDOM_SETTINGS_IN_CYCLE_MODE)
#ifdef RANDOM_SETTINGS_IN_CYCLE_MODE
uint8_t random_on = RANDOM_SETTINGS_IN_CYCLE_MODE;
#endif //RANDOM_SETTINGS_IN_CYCLE_MODE

#ifdef ESP_USE_BUTTON
GButton touch(BTN_PIN, LOW_PULL, NORM_OPEN); // для физической (не сенсорной) кнопки нужно поменять LOW_PULL на HIGH_PULL. ну и кнопку нужно ставить без резистора между находящимися рядом пинами D2 и GND
#endif

// --- ИНИЦИАЛИЗАЦИЯ ПЕРЕМЕННЫХ -------
static const uint8_t maxDim = max(WIDTH, HEIGHT);

ModeType modes[MODE_AMOUNT];

uint8_t currentMode = EFF_FIRE; // 0;
bool loadingFlag = true;
bool ONflag = true;
bool settChanged = false;
bool buttonEnabled = true;

unsigned long autoplayDuration = 120;
unsigned long autoPlayTimeout = 0;

unsigned char matrixValue[8][16]; //это массив для эффекта Огонь

CRGB DriwingColor = CRGB(255, 255, 255);

void setup()
{
    Serial.begin(115200);
    Serial.println();
    ESP.wdtEnable(WDTO_8S);

    // КНОПКА
#if defined(ESP_USE_BUTTON)
    touch.setStepTimeout(BUTTON_STEP_TIMEOUT);
    touch.setClickTimeout(BUTTON_CLICK_TIMEOUT);
#endif


    // ЛЕНТА/МАТРИЦА
    FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)/*.setCorrection(TypicalLEDStrip)*/;
    //FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(0xFFB0F0); // по предложению @kostyamat добавлена такая цветокоррекция "теперь можно получить практически чистый желтый цвет" и получилось плохо
    FastLED.setBrightness(BRIGHTNESS);
    if (CURRENT_LIMIT > 0)
    {
        FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
    }

    FastLED.clear();

    fill_solid(leds, 10, CRGB::Red);
    FastLED.show();
    delay(500);
    fill_solid(leds, 10, CRGB::Green);
    FastLED.show();
    delay(500);
    fill_solid(leds, 10, CRGB::Blue);
    FastLED.show();
    delay(500);

    FastLED.clear();
    FastLED.show();

    for (uint8_t i = 0; i < MODE_AMOUNT; i++)
    {
        shuffleFavoriteModes[i] = i;
    }

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
        autoPlayTimeout = millis() + (autoplayDuration * 1000);
        settChanged = false;
    }

    // для случайного порядка
    // создать список номеров эффектов
    // перемешать его
    // пройти по индексу до конца
    // в конце опять его перемешать
    // можно две половины списка перемешивать

    ESP.wdtFeed();                                            // пнуть собаку
}
