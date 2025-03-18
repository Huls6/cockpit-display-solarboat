//
// Created by pi on 2/25/25.
//

#ifndef BITMAPDISPLAY_H
#define BITMAPDISPLAY_H

#include <stdint.h>
#include <stdlib.h>

const uint8_t* getLetterBitmap(const char letter, size_t* size);
const uint8_t* getEmojiBitmap(const char emoji, size_t* size);


#endif //BITMAPDISPLAY_H
