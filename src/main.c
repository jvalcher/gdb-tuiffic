#include <signal.h>
#include <ncurses.h>

#include "data.h"
#include "utilities.h"
#include "parse_config_file.h"
#include "render_layout.h"
#include "start_debugger.h"
#include "run_plugin.h"
#include "update_window_data/_update_window_data.h"
#include "plugins.h"
#include "persist_data.h"

static void  initialize_ncurses          (void);
static void  set_signals                 ();
static void  update_initial_window_data  (state_t*);

// TODO: implement changes, scripts, documentation to allow termfu to be debugged by termfu
// TODO: check timestamp, reload binary when changed
// TODO: re-render layout on terminal screen size change



int
main (void) 
{
    int        key;
    state_t    state;
    debugger_t debugger;

    state.debugger = &debugger;

    initialize_ncurses ();

    parse_config_file (&state);

    start_debugger (&state); 

    render_layout (FIRST_LAYOUT, &state);

    set_signals ();

    update_initial_window_data (&state);

    get_persisted_data (&state);

    while (debugger.running) {
        key = getch ();
        run_plugin (state.plugin_key_index[key], &state);
    }

    persist_data (&state);

    clean_up ();

    return 0;
}



static void
initialize_ncurses (void)
{
    initscr();

    if (has_colors()) {
        start_color();
        init_pair(RED_BLACK, COLOR_RED, COLOR_BLACK);           // RED_BLACK
        init_pair(GREEN_BLACK, COLOR_GREEN, COLOR_BLACK);       // GREEN_BLACK
        init_pair(YELLOW_BLACK, COLOR_YELLOW, COLOR_BLACK);     // YELLOW_BLACK
        init_pair(BLUE_BLACK, COLOR_BLUE, COLOR_BLACK);         // BLUE_BLACK
        init_pair(MAGENTA_BLACK, COLOR_MAGENTA, COLOR_BLACK);   // MAGENTA_BLACK
        init_pair(CYAN_BLACK, COLOR_CYAN, COLOR_BLACK);         // CYAN_BLACK
        init_pair(WHITE_BLACK, COLOR_WHITE, COLOR_BLACK);       // WHITE_BLACK
        init_pair(WHITE_BLUE, COLOR_WHITE, COLOR_BLUE);         // WHITE_BLUE
    }

    cbreak();
    noecho();
    curs_set (0);
    set_escdelay (0);
}



static void
update_initial_window_data (state_t *state)
{
    update_windows (state, 2, Src, Asm);
}



static void
sigint_handler (int sig_num)
{
    (void) sig_num;
    pfeme ("Program exited (SIGINT)\n");
}

static void
set_signals (void)
{
    signal (SIGINT, sigint_handler);    // Ctrl-C
}

