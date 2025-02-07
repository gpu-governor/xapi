#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    Display *display;
    int screen;
    Window window;
    GC gc;
    Colormap colormap;
    XFontStruct *font;
} Context;

typedef enum { FILLED, OUTLINE } DrawMode;

// Initialize window and context
Context *init(const char *title, int width, int height) {
    Context *ctx = (Context *)malloc(sizeof(Context));
    ctx->display = XOpenDisplay(NULL);

    if (ctx->display == NULL) {
        fprintf(stderr, "Unable to open X display\n");
        exit(1);
    }

    ctx->screen = DefaultScreen(ctx->display);
    ctx->colormap = DefaultColormap(ctx->display, ctx->screen);

    ctx->window = XCreateSimpleWindow(
        ctx->display,
        RootWindow(ctx->display, ctx->screen),
        0, 0, width, height, 1,
        BlackPixel(ctx->display, ctx->screen),
        WhitePixel(ctx->display, ctx->screen)
    );

    XSetStandardProperties(ctx->display, ctx->window, title, title, None, NULL, 0, NULL);
    XSelectInput(ctx->display, ctx->window, ExposureMask | KeyPressMask);

    ctx->gc = XCreateGC(ctx->display, ctx->window, 0, 0);

    // Load default font
    ctx->font = XLoadQueryFont(ctx->display, "fixed");
    if (ctx->font) {
        XSetFont(ctx->display, ctx->gc, ctx->font->fid);
    }

    XMapWindow(ctx->display, ctx->window);
    return ctx;
}

// Close X11 window and free resources
void destroy(Context *ctx) {
    XFreeGC(ctx->display, ctx->gc);
    XDestroyWindow(ctx->display, ctx->window);
    XCloseDisplay(ctx->display);
    free(ctx);
}

// Set color by RGB values
void set_color(Context *ctx, int r, int g, int b) {
    XColor color;
    color.red = r * 256;
    color.green = g * 256;
    color.blue = b * 256;
    color.flags = DoRed | DoGreen | DoBlue;
    XAllocColor(ctx->display, ctx->colormap, &color);
    XSetForeground(ctx->display, ctx->gc, color.pixel);
}

// Set font size (attempts to load a matching font)
void set_font_size(Context *ctx, int size) {
    char font_name[32];
    snprintf(font_name, sizeof(font_name), "*-fixed-*-*-*-*-%d-*-*-*-*-*-*-*", size);
    XFontStruct *new_font = XLoadQueryFont(ctx->display, font_name);
    if (new_font) {
        XSetFont(ctx->display, ctx->gc, new_font->fid);
        XFreeFont(ctx->display, ctx->font);
        ctx->font = new_font;
    } else {
        fprintf(stderr, "Failed to load font size %d, using default\n", size);
    }
}

// Draw a rectangle
void draw_rect(Context *ctx, int x, int y, int width, int height, DrawMode mode) {
    if (mode == FILLED) {
        XFillRectangle(ctx->display, ctx->window, ctx->gc, x, y, width, height);
    } else {
        XDrawRectangle(ctx->display, ctx->window, ctx->gc, x, y, width, height);
    }
}

// Draw a circle (approximation using arcs)
void draw_circle(Context *ctx, int x, int y, int radius, DrawMode mode) {
    if (mode == FILLED) {
        XFillArc(ctx->display, ctx->window, ctx->gc, x - radius, y - radius, 2 * radius, 2 * radius, 0, 360 * 64);
    } else {
        XDrawArc(ctx->display, ctx->window, ctx->gc, x - radius, y - radius, 2 * radius, 2 * radius, 0, 360 * 64);
    }
}

// Draw a line
void draw_line(Context *ctx, int x1, int y1, int x2, int y2) {
    XDrawLine(ctx->display, ctx->window, ctx->gc, x1, y1, x2, y2);
}

// Draw a triangle
void draw_triangle(Context *ctx, int x1, int y1, int x2, int y2, int x3, int y3, DrawMode mode) {
    XPoint points[4] = {{x1, y1}, {x2, y2}, {x3, y3}, {x1, y1}};
    if (mode == FILLED) {
        XFillPolygon(ctx->display, ctx->window, ctx->gc, points, 3, Convex, CoordModeOrigin);
    } else {
        XDrawLines(ctx->display, ctx->window, ctx->gc, points, 4, CoordModeOrigin);
    }
}

// Clear the screen
void clear_screen(Context *ctx) {
    XClearWindow(ctx->display, ctx->window);
}

// Draw text at a specific position
void draw_text(Context *ctx, int x, int y, const char *text) {
    XDrawString(ctx->display, ctx->window, ctx->gc, x, y, text, strlen(text));
}

// Simple main example to test the functions
int main() {
    Context *ctx = init("X11 Abstraction Example", 800, 600);

    XEvent event;
    int running = 1;

    while (running) {
        XNextEvent(ctx->display, &event);

        if (event.type == Expose) {
            // Clear window and draw shapes/text
            clear_screen(ctx);
            
            set_color(ctx, 255, 0, 0);
            draw_rect(ctx, 50, 50, 200, 100, FILLED);

            set_color(ctx, 0, 255, 0);
            draw_circle(ctx, 400, 300, 100, OUTLINE);

            set_color(ctx, 0, 0, 255);
            set_font_size(ctx, 30);
            draw_text(ctx, 100, 500, "Hello, X11 Graphics with Enhanced Features!");

            set_color(ctx, 255, 255, 0);
            draw_line(ctx, 50, 50, 400, 300);

            set_color(ctx, 255, 0, 255);
            draw_triangle(ctx, 500, 100, 600, 300, 400, 300, OUTLINE);
        }

        if (event.type == KeyPress) {
            running = 0;
        }
    }

    destroy(ctx);
    return 0;
}
