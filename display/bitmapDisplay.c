
#include "bitmapDisplay.h"

#include <stdint.h>

#include <stdint.h>
#include <stdlib.h>

const uint8_t* getLetterBitmap(const char letter, size_t* size) {
    // Capital letters
    static uint8_t A[] = { 0x7E, 0x11, 0x11, 0x11, 0x7E, 0x00 };
    static uint8_t B[] = { 0x7F, 0x49, 0x49, 0x49, 0x36, 0x00 };
    static uint8_t C[] = { 0x3E, 0x41, 0x41, 0x41, 0x22, 0x00 };
    static uint8_t D[] = { 0x7F, 0x41, 0x41, 0x41, 0x3E, 0x00 };
    static uint8_t E[] = { 0x7F, 0x49, 0x49, 0x49, 0x41, 0x00 };
    static uint8_t F[] = { 0x7F, 0x48, 0x48, 0x48, 0x40, 0x00 };
    static uint8_t G[] = { 0x3E, 0x41, 0x49, 0x49, 0x2E, 0x00 };
    static uint8_t H[] = { 0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00 };
    static uint8_t I[] = { 0x00, 0x41, 0x7F, 0x41, 0x00 };
    static uint8_t J[] = { 0x02, 0x01, 0x01, 0x01, 0x7E, 0x00 };
    static uint8_t K[] = { 0x7F, 0x08, 0x14, 0x22, 0x41, 0x00 };
    static uint8_t L[] = { 0x7F, 0x01, 0x01, 0x01, 0x01, 0x00 };
    static uint8_t M[] = { 0x7F, 0x20, 0x10, 0x20, 0x7F, 0x00 };
    static uint8_t N[] = { 0x7F, 0x20, 0x10, 0x08, 0x7F, 0x00 };
    static uint8_t O[] = { 0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00 };
    static uint8_t P[] = { 0x7F, 0x09, 0x09, 0x09, 0x06, 0x00 };
    static uint8_t Q[] = { 0x3E, 0x41, 0x41, 0x21, 0x3E, 0x00 };
    static uint8_t R[] = { 0x7F, 0x48, 0x48, 0x48, 0x78, 0x00 };
    static uint8_t S[] = { 0x46, 0x49, 0x49, 0x49, 0x31, 0x00 };
    static uint8_t T[] = { 0x01, 0x01, 0x7F, 0x01, 0x01, 0x00 };
    static uint8_t U[] = { 0x3F, 0x01, 0x01, 0x01, 0x3F, 0x00 };
    static uint8_t V[] = { 0x1F, 0x20, 0x40, 0x20, 0x1F, 0x00 };
    static uint8_t W[] = { 0x7F, 0x01, 0x1E, 0x01, 0x7F, 0x00 };
    static uint8_t X[] = { 0x63, 0x14, 0x08, 0x14, 0x63, 0x00 };
    static uint8_t Y[] = { 0x30, 0x0C, 0x03, 0x0C, 0x30, 0x00 };
    static uint8_t Z[] = { 0x61, 0x51, 0x49, 0x45, 0x43, 0x00 };

    //Non-capital letters
    static const uint8_t a[] = { 0x20, 0x54, 0x54, 0x54, 0x78, 0x00 };
    static const uint8_t b[] = { 0x7F, 0x48, 0x48, 0x48, 0x30, 0x00 };
    static const uint8_t c[] = { 0x38, 0x44, 0x44, 0x44, 0x20, 0x00 };
    static const uint8_t d[] = { 0x30, 0x48, 0x48, 0x48, 0x7F, 0x00 };
    static const uint8_t e[] = { 0x38, 0x54, 0x54, 0x54, 0x18, 0x00 };
    static const uint8_t f[] = { 0x08, 0x7E, 0x09, 0x01, 0x02, 0x00 };
    static const uint8_t g[] = { 0x0C, 0x52, 0x52, 0x52, 0x3E, 0x00 };
    static const uint8_t h[] = { 0x7F, 0x08, 0x08, 0x08, 0x70, 0x00 };
    static const uint8_t i[] = { 0x48, 0x7D, 0x40, 0x00 };
    static const uint8_t j[] = { 0x20, 0x40, 0x40, 0x3D, 0x00 };
    static const uint8_t k[] = { 0x7F, 0x10, 0x28, 0x44, 0x00 };
    static const uint8_t l[] = { 0x41, 0x7F, 0x40, 0x00 };
    static const uint8_t m[] = { 0x7C, 0x04, 0x18, 0x04, 0x78, 0x00 };
    static const uint8_t n[] = { 0x7C, 0x08, 0x04, 0x04, 0x78, 0x00 };
    static const uint8_t o[] = { 0x38, 0x44, 0x44, 0x44, 0x38, 0x00 };
    static const uint8_t p[] = { 0x7C, 0x14, 0x14, 0x14, 0x08, 0x00 };
    static const uint8_t q[] = { 0x08, 0x14, 0x14, 0x14, 0x7C, 0x00 };
    static const uint8_t r[] = { 0x7C, 0x08, 0x04, 0x04, 0x08, 0x00 };
    static const uint8_t s[] = { 0x48, 0x54, 0x54, 0x54, 0x20, 0x00 };
    static const uint8_t t[] = { 0x04, 0x3F, 0x44, 0x40, 0x20, 0x00 };
    static const uint8_t u[] = { 0x3C, 0x40, 0x40, 0x20, 0x7C, 0x00 };
    static const uint8_t v[] = { 0x1C, 0x20, 0x40, 0x20, 0x1C, 0x00 };
    static const uint8_t w[] = { 0x3C, 0x40, 0x30, 0x40, 0x3C, 0x00 };
    static const uint8_t x[] = { 0x44, 0x28, 0x10, 0x28, 0x44, 0x00 };
    static const uint8_t y[] = { 0x0C, 0x50, 0x50, 0x50, 0x3C, 0x00 };
    static const uint8_t z[] = { 0x44, 0x64, 0x54, 0x4C, 0x44, 0x00 };

    //Special characters
    static const uint8_t zero[] = { 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00 };
    static const uint8_t one[] = { 0x00, 0x42, 0x7F, 0x40, 0x00 };
    static const uint8_t two[] = { 0x62, 0x51, 0x49, 0x49, 0x46, 0x00 };
    static const uint8_t three[] = { 0x22, 0x41, 0x49, 0x49, 0x36, 0x00 };
    static const uint8_t four[] = { 0x18, 0x14, 0x12, 0x7F, 0x10, 0x00 };
    static const uint8_t five[] = { 0x2F, 0x49, 0x49, 0x49, 0x31, 0x00 };
    static const uint8_t six[] = { 0x3E, 0x49, 0x49, 0x49, 0x32, 0x00 };
    static const uint8_t seven[] = { 0x01, 0x71, 0x09, 0x05, 0x03, 0x00 };
    static const uint8_t eight[] = { 0x36, 0x49, 0x49, 0x49, 0x36, 0x00 };
    static const uint8_t nine[] = { 0x26, 0x49, 0x49, 0x49, 0x3E, 0x00 };

    static const uint8_t space[] = { 0x00, 0x00, 0x00, 0x00 };
    static const uint8_t exclamation[] = { 0x00, 0x5F, 0x00 };
    static const uint8_t question[] = { 0x20, 0x40, 0x4D, 0x48, 0x30, 0x00 };
    static const uint8_t period[] = { 0x00, 0x40, 0x00 };
    static const uint8_t colon[] = { 0x00, 0x42, 0x00 };
    static const uint8_t percent[] = { 0x62, 0x64, 0x08, 0x13, 0x23, 0x00 };
    static const uint8_t celsius[] = { 0x06, 0x09, 0x09, 0x06, 0x00 };
    static const uint8_t vertical_bar[] = { 0xFF };

    static uint8_t unknown[] = { 0x00, 0x00, 0x00, 0x00 };  // Unknown character

    // Determine size and return corresponding bitmap
    switch (letter) {
        case 'A': *size = sizeof(A); return A;
        case 'B': *size = sizeof(B); return B;
        case 'C': *size = sizeof(C); return C;
        case 'D': *size = sizeof(D); return D;
        case 'E': *size = sizeof(E); return E;
        case 'F': *size = sizeof(F); return F;
        case 'G': *size = sizeof(G); return G;
        case 'H': *size = sizeof(H); return H;
        case 'I': *size = sizeof(I); return I;
        case 'J': *size = sizeof(J); return J;
        case 'K': *size = sizeof(K); return K;
        case 'L': *size = sizeof(L); return L;
        case 'M': *size = sizeof(M); return M;
        case 'N': *size = sizeof(N); return N;
        case 'O': *size = sizeof(O); return O;
        case 'P': *size = sizeof(P); return P;
        case 'Q': *size = sizeof(Q); return Q;
        case 'R': *size = sizeof(R); return R;
        case 'S': *size = sizeof(S); return S;
        case 'T': *size = sizeof(T); return T;
        case 'U': *size = sizeof(U); return U;
        case 'V': *size = sizeof(V); return V;
        case 'W': *size = sizeof(W); return W;
        case 'X': *size = sizeof(X); return X;
        case 'Y': *size = sizeof(Y); return Y;
        case 'Z': *size = sizeof(Z); return Z;

        case 'a': *size = sizeof(a); return a;
        case 'b': *size = sizeof(b); return b;
        case 'c': *size = sizeof(c); return c;
        case 'd': *size = sizeof(d); return d;
        case 'e': *size = sizeof(e); return e;
        case 'f': *size = sizeof(f); return f;
        case 'g': *size = sizeof(g); return g;
        case 'h': *size = sizeof(h); return h;
        case 'i': *size = sizeof(i); return i;
        case 'j': *size = sizeof(j); return j;
        case 'k': *size = sizeof(k); return k;
        case 'l': *size = sizeof(l); return l;
        case 'm': *size = sizeof(m); return m;
        case 'n': *size = sizeof(n); return n;
        case 'o': *size = sizeof(o); return o;
        case 'p': *size = sizeof(p); return p;
        case 'q': *size = sizeof(q); return q;
        case 'r': *size = sizeof(r); return r;
        case 's': *size = sizeof(s); return s;
        case 't': *size = sizeof(t); return t;
        case 'u': *size = sizeof(u); return u;
        case 'v': *size = sizeof(v); return v;
        case 'w': *size = sizeof(w); return w;
        case 'x': *size = sizeof(x); return x;
        case 'y': *size = sizeof(y); return y;
        case 'z': *size = sizeof(z); return z;

        case '0': *size = sizeof(zero); return zero;
        case '1': *size = sizeof(one); return one;
        case '2': *size = sizeof(two); return two;
        case '3': *size = sizeof(three); return three;
        case '4': *size = sizeof(four); return four;
        case '5': *size = sizeof(five); return five;
        case '6': *size = sizeof(six); return six;
        case '7': *size = sizeof(seven); return seven;
        case '8': *size = sizeof(eight); return eight;
        case '9': *size = sizeof(nine); return nine;

        case ' ': *size = sizeof(space); return space;
        case '!': *size = sizeof(exclamation); return exclamation;
        case '?': *size = sizeof(question); return question;
        case '.': *size = sizeof(period); return period;
        case ':': *size = sizeof(colon); return colon;
        case '%': *size = sizeof(percent); return percent;
        case '*': *size = sizeof(celsius); return celsius; // Celsius symbol
        case '|': *size = sizeof(vertical_bar); return vertical_bar;

        default: *size = sizeof(unknown); return unknown;
    }
}

// Function that returns a pointer to the bitmap for emojis
const uint8_t* getEmojiBitmap(const char emoji, size_t* size) {
    static const uint8_t batteryEmpty[] = { 0x7E, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x66, 0x24, 0x3C };
    static const uint8_t batteryHalf[] = { 0x7E, 0x7E, 0x7E, 0x7E, 0x42, 0x42, 0x42, 0x66, 0x24, 0x3C };
    static const uint8_t batteryFull[] = { 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x3C, 0x3C };
    static const uint8_t defaultEmoji[] = { 0x00, 0x00, 0x00, 0x00 };  // Default unknown emoji

    switch (emoji) {
        case 'E': *size = sizeof(batteryEmpty); return batteryEmpty; // Empty battery
        case 'H': *size = sizeof(batteryHalf); return batteryHalf;   // Half battery
        case 'F': *size = sizeof(batteryFull); return batteryFull;   // Full battery
        default: *size = sizeof(defaultEmoji); return defaultEmoji;  // Unknown
    }
}