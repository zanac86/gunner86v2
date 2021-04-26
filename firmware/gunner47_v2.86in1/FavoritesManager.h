#pragma once
#include "Constants.h"

/*
 * Есть список всех режимов
 * Перемешиваем его и выдаем по очереди
 * пока не дойдем до конца
 * Теперь снова его перемешиваем.
 *
 */
uint8_t shuffleFavoriteModes[MODE_AMOUNT];
uint8_t shuffleCurrentIndex = 0;

void favoritesShuffle()
{
    for (int i = 0; i < MODE_AMOUNT; i++)
    {
        int x = random(0, MODE_AMOUNT);
        uint8_t tmp = shuffleFavoriteModes[i];
        shuffleFavoriteModes[i] = shuffleFavoriteModes[x];
        shuffleFavoriteModes[x] = tmp;
    }
}

uint8_t favoritesNext()
{
    shuffleCurrentIndex++;
    if (shuffleCurrentIndex >= MODE_AMOUNT)
    {
        favoritesShuffle();
        shuffleCurrentIndex = 0;
    }
    return shuffleFavoriteModes[shuffleCurrentIndex];
}

void favoritesInit()
{
    shuffleCurrentIndex = 0;
    for (int i = 0; i < MODE_AMOUNT; i++)
    {
        shuffleFavoriteModes[i] = i;
    }
    favoritesShuffle();

}
