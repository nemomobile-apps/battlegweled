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


#include "sdl12compat.h"

#include <GLES/gl.h>
#include <assert.h>

static SDL_Surface *
surface = NULL;

static SDL_Window *
window = NULL;

static SDL_GLContext
context;


SDL_Surface *
SDL_SetVideoMode(int width, int height, int depth, int flags)
{
    if (surface == NULL) {
        int r, g, b;
        if (depth == 16) {
            r = 0xF800;
            g = 0x07E0;
            b = 0x001F;
        } else {
            assert(0/* Depth not handled yet */);
        }

        surface = SDL_CreateRGBSurface(0, width, height, depth, r, g, b, 0);
        window = SDL_CreateWindow("SDL12Compat",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                width, height,
                SDL_WINDOW_FULLSCREEN | SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
        context = SDL_GL_CreateContext(window);
    }

    return surface;
}

static void
blit_to_screen()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);

    GLuint texture;
    glGenTextures(1, &texture);

    float vtxcoords[] = {
        -1, -1,
        -1, +1,
        +1, -1,
        +1, +1,
    };

    float texcoords[] = {
        0, 1,
        0, 0,
        1, 1,
        1, 0,
    };

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    SDL_PixelFormat fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.format = SDL_PIXELFORMAT_RGB888;
    fmt.BitsPerPixel = 24;
    fmt.BytesPerPixel = 3;
    fmt.Rmask = 0x0000ff;
    fmt.Gmask = 0x00ff00;
    fmt.Bmask = 0xff0000;
    fmt.Amask = 0x000000;

    SDL_Surface *tmp = SDL_ConvertSurface(surface, &fmt, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tmp->w, tmp->h, 0,
            GL_RGB, GL_UNSIGNED_BYTE, tmp->pixels);
    SDL_FreeSurface(tmp);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, vtxcoords);
    glTexCoordPointer(2, GL_FLOAT, 0, texcoords);

    glColor4f(1.0, 1.0, 1.0, 1.0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &texture);

    glDisable(GL_TEXTURE_2D);
}

void
SDL_UpdateRect(SDL_Surface *screen, int x, int y, int w, int h)
{
    static Uint32 last = 0;
    Uint32 now = SDL_GetTicks();

    static const Uint32 FPS_LIMIT = 60;

    if (now - last > 1000 / FPS_LIMIT) {
        // Rate-limit updates
        blit_to_screen();
        SDL_GL_SwapWindow(window);
        last = now;
    }
}

void
SDL_Flip(SDL_Surface *screen)
{
    SDL_UpdateRect(screen, 0, 0, screen->w, screen->h);
}

void
SDL_SetAlpha(SDL_Surface *screen, int flags, int alpha)
{
    SDL_SetSurfaceAlphaMod(screen, alpha);
}

void
SDL_WM_SetCaption(const char *a, const char *b)
{
    // TODO
}

