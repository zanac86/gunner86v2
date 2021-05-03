#pragma once
#include "Constants.h"

/*
   Есть список всех режимов
   Перемешиваем его и выдаем по очереди
   пока не дойдем до конца
   Теперь снова его перемешиваем.

*/
uint8_t shuffleFavoriteModes[MODE_AMOUNT];
uint8_t shuffleCurrentIndex = 0;

/*
  Короткий список эффектов
*/
uint8_t demoListModes[] =
{
    EFF_PLASMA,
    EFF_PRISMATA,
    EFF_SPIDER,
    EFF_MATRIX,
    EFF_FIRE,
    EFF_FOUNTAIN,
    EFF_TWINKLES,
    EFF_RAIN,
    EFF_RAINBOW_VER
};
uint8_t demoCurrentIndex = 0;
uint8_t demoModesCount = ARRAY_SIZE(demoListModes);

bool demoMode = false;

void favoritesShuffle()
{
    for (int i = 0; i < MODE_AMOUNT; i++)
    {
        int x = random(0, MODE_AMOUNT);
        uint8_t tmp = shuffleFavoriteModes[i];
        shuffleFavoriteModes[i] = shuffleFavoriteModes[x];
        shuffleFavoriteModes[x] = tmp;
    }
    /*
    for (int i = 0; i < MODE_AMOUNT; i++)
    {
        shuffleFavoriteModes[i] = i; // EFF_FOUNTAIN;
    }
    */
}

uint8_t favoritesNext()
{
    if (demoMode)
    {
        demoCurrentIndex++;
        if (demoCurrentIndex >= demoModesCount)
        {
            demoCurrentIndex = 0;
        }
        return demoListModes[demoCurrentIndex];
    }
    else
    {
        shuffleCurrentIndex++;
        if (shuffleCurrentIndex >= MODE_AMOUNT)
        {
            favoritesShuffle();
            shuffleCurrentIndex = 0;
        }
        return shuffleFavoriteModes[shuffleCurrentIndex];
    }
}

void favoritesInit()
{
    if (demoMode)
    {
        demoCurrentIndex = 0;
    }
    else
    {
        shuffleCurrentIndex = 0;
        for (int i = 0; i < MODE_AMOUNT; i++)
        {
            shuffleFavoriteModes[i] = i;
        }
        favoritesShuffle();
    }
}
