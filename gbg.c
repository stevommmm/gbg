#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <Imlib2.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct rgb {
    unsigned int r;
    unsigned int g;
    unsigned int b;
} rgb;

void random_rgb(rgb *c) {
    c->r = rand() % 255;
    c->g = rand() % 255;
    c->b = rand() % 255;
}
void rotate_rgb(rgb *from, rgb *to) {
    to->r = (from->r + 128) % 255;
    to->g = (from->g + 128) % 255;
    to->b = (from->b + 128) % 255;
}


void render_for_screen(Display *disp, int screen_index) {
    Colormap colormap = DefaultColormap(disp, screen_index);
    Visual *vis = DefaultVisual(disp, screen_index);
    Window root = RootWindow(disp, screen_index);
    int depth = DefaultDepth(disp, screen_index);
    int height = XDisplayHeight(disp, screen_index);
    int width = XDisplayWidth(disp, screen_index);

    // Don't operate on a full size image, gradient production is slow
    unsigned int img_height = height / 4;
    unsigned int img_width = width / 4;

    rgb color1;
    rgb color2;
    random_rgb(&color1);
    rotate_rgb(&color1, &color2);

    Imlib_Image img;
    Pixmap pixmap = XCreatePixmap(disp, root, width, height, depth);

    imlib_context_set_display(disp);
    imlib_context_set_visual(vis);
    imlib_context_set_colormap(colormap);
    imlib_context_set_drawable(pixmap);

    img = imlib_create_image(img_width, img_height);
    imlib_context_set_image(img);
    imlib_image_set_format("png");
    imlib_context_set_dither(1);
    imlib_context_set_blend(1);
    imlib_context_set_color_range(imlib_create_color_range());

    // Set default background color
    imlib_context_set_color(255, 255, 255, 255);
    imlib_image_fill_rectangle(0, 0, img_width, img_height);

    // First color
    imlib_context_set_color(color1.r, color1.g, color1.b, 255);
    imlib_add_color_to_color_range(0);

    // Second
    imlib_context_set_color(color2.r, color2.g, color2.b, 255);
    imlib_add_color_to_color_range(img_height);
    imlib_image_fill_color_range_rectangle(0, 0, img_width, img_height, -45.0);

    // Render our smaller image to the size of the screen
    imlib_render_image_on_drawable_at_size(0, 0, width, height);

    // let go of everything
    imlib_free_color_range();
    imlib_free_image();

    // Set props for WM
    Atom prop_root, prop_esetroot;
    prop_root = XInternAtom(disp, "_XROOTPMAP_ID", False);
    prop_esetroot = XInternAtom(disp, "ESETROOT_PMAP_ID", False);
    XChangeProperty(disp, root, prop_root, XA_PIXMAP, 32, PropModeReplace, (unsigned char *) &pixmap, 1);
    XChangeProperty(disp, root, prop_esetroot, XA_PIXMAP, 32, PropModeReplace, (unsigned char *) &pixmap, 1);

    XSetWindowBackgroundPixmap(disp, root, pixmap);

    XClearWindow(disp, root);
    XFlush(disp);
    XSync(disp, False);

#ifndef NO_FINAL_FREE
    XFreePixmap(disp, pixmap);
#endif
}


int main(int argc, char *argv[]) {
    Display *disp;
    int i;

    srand(time(NULL));

    disp = XOpenDisplay(NULL);
    for (i = 0; i < ScreenCount(disp); i++) {
        render_for_screen(disp, i);

    }
    XKillClient(disp, AllTemporary);
    XSetCloseDownMode(disp, RetainTemporary);
    XCloseDisplay(disp);
    return EXIT_SUCCESS;
}
