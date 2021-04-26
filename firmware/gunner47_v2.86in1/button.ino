#ifdef ESP_USE_BUTTON

bool brightDirection;
static bool startButtonHolding = false;                     // флаг: кнопка удерживается для изменения яркости/скорости/масштаба лампы кнопкой


void buttonTick()
{
    if (!buttonEnabled)                                       // события кнопки не обрабатываются, если она заблокирована
    {
        return;
    }

    touch.tick();
    uint8_t clickCount = touch.hasClicks() ? touch.getClicks() : 0U;


    // однократное нажатие
    if (clickCount == 1U)
    {
        nextEffect();
        //        ONflag = !ONflag;
        //        changePower();
        //        settChanged = true;
        //        loadingFlag = true;
    }


    // двухкратное нажатие
    if (ONflag && clickCount == 2U)
    {
        if (++currentMode >= MODE_AMOUNT)
        {
            currentMode = 0;
        }
        FastLED.setBrightness(modes[currentMode].Brightness);
        loadingFlag = true;
        settChanged = true;
    }


    // трёхкратное нажатие
    if (ONflag && clickCount == 3U)
    {
        if (--currentMode >= MODE_AMOUNT)
        {
            currentMode = MODE_AMOUNT - 1;
        }
        FastLED.setBrightness(modes[currentMode].Brightness);
        loadingFlag = true;
        settChanged = true;
    }


    // четырёхкратное нажатие
    if (clickCount == 4U)
    {
    }


    // пятикратное нажатие
    if (clickCount == 5U)                                     // вывод IP на лампу
    {
        loadingFlag = true;

        while (!fillString("Have a nice day!", CRGB::White, false))
        {
            delay(1);
            ESP.wdtFeed();
        }

        loadingFlag = true;
    }

    // кнопка только начала удерживаться
    if (ONflag && touch.isHolded())
    {
        brightDirection = !brightDirection;
        startButtonHolding = true;
    }


    // кнопка нажата и удерживается
    if (ONflag && touch.isStep())
    {
        switch (touch.getHoldClicks())
        {
            case 0U:                                              // просто удержание (до удержания кнопки кликов не было) - изменение яркости
                {
                    uint8_t delta = modes[currentMode].Brightness < 10U // определение шага изменения яркости: при яркости [1..10] шаг = 1, при [11..16] шаг = 3, при [17..255] шаг = 15
                                    ? 1U
                                    : 5U;
                    modes[currentMode].Brightness =
                        constrain(brightDirection
                                  ? modes[currentMode].Brightness + delta
                                  : modes[currentMode].Brightness - delta,
                                  1, 255);
                    FastLED.setBrightness(modes[currentMode].Brightness);
                    break;
                }

            case 1U:                                              // удержание после одного клика - изменение скорости
                {
                    modes[currentMode].Speed = constrain(brightDirection ? modes[currentMode].Speed + 1 : modes[currentMode].Speed - 1, 1, 255);
                    loadingFlag = true; // без перезапуска эффекта ничего и не увидишь
                    break;
                }

            case 2U:                                              // удержание после двух кликов - изменение масштаба
                {
                    modes[currentMode].Scale = constrain(brightDirection ? modes[currentMode].Scale + 1 : modes[currentMode].Scale - 1, 1, 100);
                    loadingFlag = true; // без перезапуска эффекта ничего и не увидишь
                    break;
                }

            default:
                break;
        }

        settChanged = true;
    }


    // кнопка отпущена после удерживания
    if (ONflag && !touch.isHold() && startButtonHolding)      // кнопка отпущена после удерживания
    {
        startButtonHolding = false;
        loadingFlag = true;
    }
}
#endif
