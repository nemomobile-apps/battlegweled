/**
 * SDL 1.2 Porting Helper
 * Copyright (C) 2013 Jolla Ltd.
 * Contact: Thomas Perl <thomas.perl@jollamobile.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **/


#ifndef SDL12_COMPAT_H
#define SDL12_COMPAT_H

#include <SDL.h>

SDL_Surface *
SDL_SetVideoMode(int width, int height, int depth, int flags);

void
SDL_UpdateRect(SDL_Surface *screen, int x, int y, int w, int h);

void
SDL_Flip(SDL_Surface *screen);

void
SDL_SetAlpha(SDL_Surface *screen, int flags, int alpha);

void
SDL_WM_SetCaption(const char *, const char *);

#define SDL_DOUBLEBUF 0x100000
#define SDL_FULLSCREEN 0x400000

#define SDL_HWSURFACE 0
#define SDL_SRCCOLORKEY SDL_TRUE

#endif /* SDL12_COMPAT_H */
