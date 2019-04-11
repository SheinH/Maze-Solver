/* compiles with command line  gcc test.c -lX11 -lm -L/usr/X11R6/lib */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

Display *display_ptr;
Screen *screen_ptr;
int screen_num;
char *display_name = NULL;
unsigned int display_width, display_height;

Window win;
int border_width;
unsigned int win_width, win_height;
int win_x, win_y;

XWMHints *wm_hints;
XClassHint *class_hints;
XSizeHints *size_hints;
XTextProperty win_name, icon_name;
char *win_name_string = "Example Window";
char *icon_name_string = "Icon for Example Window";

XEvent report;

GC gc, gc_green, gc_blue, gc_red, gc_grey;
unsigned long valuemask = 0;
XGCValues gc_values, gc_green_values, gc_blue_values, gc_red_values, gc_grey_values;
Colormap color_map;
XColor tmp_color1, tmp_color2;

void BFS(int n, int *maze);

int main(int argc, char **argv)
{
    int maze[200][200];
    int i, j, randinit = 0;
    if (argc == 2)
        sscanf(argv[1], "%d", &randinit);
    else
        randinit = (int) time(NULL);
    printf("%d\n", randinit);
    srand(randinit);
    for (i = 0; i < 200; i++)
        for (j = 0; j < 200; j++)
            maze[i][j] = 0;
    for (i = 1; i < 196; i++)
        for (j = 1; j < 199; j++)
        {
            if (rand() % 100 < 8)
            { maze[i][j] = maze[i + 1][j] = maze[i + 2][j] = maze[i + 3][j] = 1; }
        }
    for (i = 1; i < 199; i++)
        for (j = 1; j < 196; j++)
        {
            if (rand() % 100 < 8)
            { maze[i][j] = maze[i][j + 1] = maze[i][j + 2] = maze[i][j + 3] = 1; }
        }
    for (i = 1; i < 199; i++)
        for (j = 1; j < 199; j++)
        {
            if (rand() % 100 < 5 && (maze[i - 1][j] + maze[i][j + 1] + maze[i + 1][j] + maze[i][j - 1]) >= 1)
            { maze[i][j] = 1; }
        }
    for (i = 1; i < 199; i++)
        for (j = 1; j < 199; j++)
        {
            if (rand() % 100 < 25 && (maze[i - 1][j] + maze[i][j + 1] + maze[i + 1][j] + maze[i][j - 1]) >= 2)
            { maze[i][j] = 1; }
        }
    /* start point */
    i = (rand() % 190) + 5;
    j = (rand() % 190) + 5;
    maze[i][j] = 2;
    maze[i + 1][j] = maze[i + 2][j] = maze[i + 3][j] = 1;
    maze[i - 1][j] = maze[i - 2][j] = maze[i - 3][j] = 1;
    maze[i][j + 1] = maze[i][j + 2] = maze[i][j + 3] = 1;
    maze[i][j - 1] = maze[i][j - 2] = maze[i][j - 3] = 1;
    /* target point */
    i = (rand() % 190) + 5;
    j = (rand() % 190) + 5;
    maze[i][j] = 3;
    maze[i + 1][j] = maze[i + 2][j] = maze[i + 3][j] = 1;
    maze[i - 1][j] = maze[i - 2][j] = maze[i - 3][j] = 1;
    maze[i][j + 1] = maze[i][j + 2] = maze[i][j + 3] = 1;
    maze[i][j - 1] = maze[i][j - 2] = maze[i][j - 3] = 1;

    /* opening display: basic connection to X Server */
    if ((display_ptr = XOpenDisplay(display_name)) == NULL)
    {
        printf("Could not open display. \n");
        exit(-1);
    }
    printf("Connected to X server  %s\n", XDisplayName(display_name));
    screen_num = DefaultScreen(display_ptr);
    screen_ptr = DefaultScreenOfDisplay(display_ptr);
    color_map = XDefaultColormap(display_ptr, screen_num);
    display_width = DisplayWidth(display_ptr, screen_num);
    display_height = DisplayHeight(display_ptr, screen_num);

    printf("Width %d, Height %d, Screen Number %d\n",
           display_width, display_height, screen_num);
    /* creating the window */
    border_width = 10;
    win_x = 0;
    win_y = 0;
    win_width = 800;
    win_height = 800; /*rectangular window*/

    win = XCreateSimpleWindow(display_ptr, RootWindow(display_ptr, screen_num),
                              win_x, win_y, win_width, win_height, border_width,
                              BlackPixel(display_ptr, screen_num),
                              WhitePixel(display_ptr, screen_num));
    /* now try to put it on screen, this needs cooperation of window manager */
    size_hints = XAllocSizeHints();
    wm_hints = XAllocWMHints();
    class_hints = XAllocClassHint();
    if (size_hints == NULL || wm_hints == NULL || class_hints == NULL)
    {
        printf("Error allocating memory for hints. \n");
        exit(-1);
    }

    size_hints->flags = PPosition | PSize | PMinSize;
    size_hints->min_width = 60;
    size_hints->min_height = 60;

    XStringListToTextProperty(&win_name_string, 1, &win_name);
    XStringListToTextProperty(&icon_name_string, 1, &icon_name);

    wm_hints->flags = StateHint | InputHint;
    wm_hints->initial_state = NormalState;
    wm_hints->input = False;

    class_hints->res_name = "x_use_example";
    class_hints->res_class = "examples";

    XSetWMProperties(display_ptr, win, &win_name, &icon_name, argv, argc,
                     size_hints, wm_hints, class_hints);

    /* what events do we want to receive */
    XSelectInput(display_ptr, win,
                 ExposureMask | StructureNotifyMask | ButtonPressMask);

    /* finally: put window on screen */
    XMapWindow(display_ptr, win);

    XFlush(display_ptr);

    /* create graphics context, so that we may draw in this window */
    gc = XCreateGC(display_ptr, win, valuemask, &gc_values);
    XSetForeground(display_ptr, gc, BlackPixel(display_ptr, screen_num));
    XSetLineAttributes(display_ptr, gc, 4, LineSolid, CapRound, JoinRound);
    /* and four other graphics contexts, to draw in green, blue, red and grey*/
    gc_green = XCreateGC(display_ptr, win, valuemask, &gc_green_values);
    XSetLineAttributes(display_ptr, gc_green, 6, LineSolid, CapRound, JoinRound);
    if (XAllocNamedColor(display_ptr, color_map, "green",
                         &tmp_color1, &tmp_color2) == 0)
    {
        printf("failed to get color green\n");
        exit(-1);
    }
    else
        XSetForeground(display_ptr, gc_green, tmp_color1.pixel);
    gc_blue = XCreateGC(display_ptr, win, valuemask, &gc_blue_values);
    XSetLineAttributes(display_ptr, gc_blue, 6, LineSolid, CapRound, JoinRound);
    if (XAllocNamedColor(display_ptr, color_map, "blue",
                         &tmp_color1, &tmp_color2) == 0)
    {
        printf("failed to get color blue\n");
        exit(-1);
    }
    else
        XSetForeground(display_ptr, gc_blue, tmp_color1.pixel);
    gc_red = XCreateGC(display_ptr, win, valuemask, &gc_red_values);
    XSetLineAttributes(display_ptr, gc_red, 6, LineSolid, CapRound, JoinRound);
    if (XAllocNamedColor(display_ptr, color_map, "red",
                         &tmp_color1, &tmp_color2) == 0)
    {
        printf("failed to get color red\n");
        exit(-1);
    }
    else
        XSetForeground(display_ptr, gc_red, tmp_color1.pixel);
    gc_grey = XCreateGC(display_ptr, win, valuemask, &gc_grey_values);
    if (XAllocNamedColor(display_ptr, color_map, "light grey",
                         &tmp_color1, &tmp_color2) == 0)
    {
        printf("failed to get color grey\n");
        exit(-1);
    }
    else
        XSetForeground(display_ptr, gc_grey, tmp_color1.pixel);

    /* and now it starts: the event loop */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (1)
    {
        XNextEvent(display_ptr, &report);
        switch (report.type)
        {
            case Expose:
                /* (re-)draw the example figure. This event happens
                   each time some part ofthe window gets exposed (becomes visible) */
                for (i = 0; i < 200; i++)
                {
                    for (j = 0; j < 200; j++)
                    {
                        switch (maze[i][j])
                        {
                            case 0:
                                XFillRectangle(display_ptr, win, gc, 4 * i, 4 * j, 4, 4);
                                break;
                            case 1:
                                XFillRectangle(display_ptr, win, gc_grey, 4 * i, 4 * j, 4, 4);
                                break;
                            case 2:
                                XFillRectangle(display_ptr, win, gc_red, 4 * i, 4 * j, 4, 4);
                                break;
                            case 3:
                                XFillRectangle(display_ptr, win, gc_green, 4 * i, 4 * j, 4, 4);
                                break;
                            case 4:
                                XFillRectangle(display_ptr, win, gc_blue, 4 * i, 4 * j, 4, 4);
                                break;
                            default: /*should not happen*/ break;
                        }
                    }
                }
                break;
            case ConfigureNotify:
                /* This event happens when the user changes the size of the window*/
                win_width = report.xconfigure.width;
                win_height = report.xconfigure.height;
                break;
            case ButtonPress:
                /* This event happens when the user pushes a mouse button. I draw
                  a circle to show the point where it happened, but do not save
                  the position; so when the next redraw event comes, these circles
              disappear again. */
            {
                BFS(200, &(maze[0][0]));
                for (i = 0; i < 200; i++)
                {
                    for (j = 0; j < 200; j++)
                    {
                        switch (maze[i][j])
                        {
                            case 0:
                                XFillRectangle(display_ptr, win, gc, 4 * i, 4 * j, 4, 4);
                                break;
                            case 1:
                                XFillRectangle(display_ptr, win, gc_grey, 4 * i, 4 * j, 4, 4);
                                break;
                            case 2:
                                XFillRectangle(display_ptr, win, gc_red, 4 * i, 4 * j, 4, 4);
                                break;
                            case 3:
                                XFillRectangle(display_ptr, win, gc_green, 4 * i, 4 * j, 4, 4);
                                break;
                            case 4:
                                XFillRectangle(display_ptr, win, gc_blue, 4 * i, 4 * j, 4, 4);
                                break;
                            default: /*should not happen*/ break;
                        }
                    }
                }
            }
                break;
            default:
                /* this is a catch-all for other events; it does not do anything.
                       One could look at the report type to see what the event was */
                break;
        }

    }
#pragma clang diagnostic pop
    exit(0);
}
