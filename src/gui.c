#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pieces.h"
#include "board.h"
#include "moves.h"
#include "rules.h"
#include "controller.h"
#include "bots.h"
#include "gui.h"
#include "main_funct.h"

#define SQ 80

static GameController *controller = NULL;
static Color *g_player_color = NULL;
static OpponentType *g_opponent_type = NULL;
static GtkWidget *g_history_text = NULL;

void set_controller(GameController *c)
{
    controller = c;
}

void set_opponent_type(OpponentType *opponent)
{
    g_opponent_type = opponent;
}

void set_player_color(Color *player_color)
{
    g_player_color = player_color;
}

static const char *piece_image(Piece p)
{
    if (p.pieceType == EMPTY)
        return NULL;

    if (p.color == WHITE)
    {
        switch (p.pieceType)
        {
        case PAWN:
            return "pieces/wP.png";
        case ROOK:
            return "pieces/wR.png";
        case KNIGHT:
            return "pieces/wN.png";
        case BISHOP:
            return "pieces/wB.png";
        case QUEEN:
            return "pieces/wQ.png";
        case KING:
            return "pieces/wK.png";
        case ANTEATER:
            return "pieces/wA.png";
        default:
            return NULL;
        }
    }
    else
    {
        switch (p.pieceType)
        {
        case PAWN:
            return "pieces/bP.png";
        case ROOK:
            return "pieces/bR.png";
        case KNIGHT:
            return "pieces/bN.png";
        case BISHOP:
            return "pieces/bB.png";
        case QUEEN:
            return "pieces/bQ.png";
        case KING:
            return "pieces/bK.png";
        case ANTEATER:
            return "pieces/bA.png";
        default:
            return NULL;
        }
    }
}

typedef struct
{
    GtkWidget *event_box;
    GtkWidget *overlay;
    GtkWidget *sq_img;   // board tile
    GtkWidget *pc_img;   // piece image
    GtkWidget *hint_img; // legal move marker
    GtkWidget *hi_img;   // highlighted piece when clicked
} Cell;

static Cell g_cells[ROWS][COLS];
static GtkWidget *g_status_label = NULL;
static GtkWidget *btn_end_turn = NULL;
static GtkWidget* g_timer_label = NULL;
static guint g_timer_id = 0;

static void set_highlight(int r, int c, int highlighted)
{
    if (highlighted)
    {
        gtk_image_set_from_file(GTK_IMAGE(g_cells[r][c].hi_img), "pieces/border.png");
    }
    else
    {
        // Clear the image when the square is not selected
        gtk_image_clear(GTK_IMAGE(g_cells[r][c].hi_img));
    }
}

static void set_hint(int r, int c, int show_hint)
{
    if (show_hint)
    {
        gtk_image_set_from_file(GTK_IMAGE(g_cells[r][c].hint_img), "pieces/hint.png");
    }
    else
    {
        gtk_image_clear(GTK_IMAGE(g_cells[r][c].hint_img));
    }
}

static void refresh_cell(int r, int c)
{
    Piece p = controller_get_piece_at(controller, r, c);
    const char *img = piece_image(p);
    if (img)
    {
        gtk_image_set_from_file(GTK_IMAGE(g_cells[r][c].pc_img), img);
    }
    else
    {
        gtk_image_clear(GTK_IMAGE(g_cells[r][c].pc_img));
    }
}

static void refresh_all(void)
{
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {

            refresh_cell(r, c);

            if (is_square_selected(controller) && is_hint_square(controller, r, c))
            {
                set_hint(r, c, 1);
            }
            else
            {
                set_hint(r, c, 0);
            }

            if (is_square_selected(controller) && r == get_selected_row(controller) && c == get_selected_col(controller))
            {
                set_highlight(r, c, 1);
            }
            else
            {
                set_highlight(r, c, 0);
            }
        }
    }
}

static void refresh_timer(void) {
	if (!g_timer_label) return;

    int minutes = controller_get_time_elapsed(controller) / 60;
    int seconds = controller_get_time_elapsed(controller) % 60;
    char buf[32];
    snprintf(buf, sizeof buf, "Time: %02d:%02d", minutes, seconds);
	gtk_label_set_text(GTK_LABEL(g_timer_label), buf);
}

static void stop_timer(void) {
    if(g_timer_id != 0) {
        g_source_remove(g_timer_id);
        g_timer_id = 0;
	}
}


static void update_status(void)
{
    Color c = get_current_turn(controller);
    if (controller_in_checkmate(controller))
        gtk_label_set_text(GTK_LABEL(g_status_label),
                           c == WHITE ? "Checkmate! Black wins!" : "Checkmate! White wins!");
    else if (controller_in_stalemate(controller))
        gtk_label_set_text(GTK_LABEL(g_status_label), "Stalemate! Draw.");
	else if (controller_get_warning_status(controller))
    {
        gtk_label_set_text(GTK_LABEL(g_status_label),
                           c == WHITE ? "White to move. Warning: Time is over 1:00" : "Black to move. Warning: Time is over 1:00");
    }
    else
        gtk_label_set_text(GTK_LABEL(g_status_label),
                           c == WHITE ? "White to move." : "Black to move.");
}

static gboolean on_timer_tick(gpointer data) {
    (void)data;

    controller_increment_timer(controller);
	refresh_timer();
    update_status();

	return TRUE; // Continue calling the timer
}

static void start_timer(void) {
    stop_timer(); // Ensure any existing timer is stopped
    refresh_timer();
    g_timer_id = g_timeout_add_seconds(1, on_timer_tick, NULL);

	update_status();
}

void log_move_to_sidebar(Piece p, int fromR, int fromC, int toR, int toC) {
    if (!g_history_text) return;

    char move_str[64];
    char colorChar = (p.color == WHITE) ? 'w' : 'b';
    char typeChar;

    switch(p.pieceType) {
        case PAWN:     typeChar = 'P'; break;
        case ROOK:     typeChar = 'R'; break;
        case KNIGHT:   typeChar = 'N'; break;
        case BISHOP:   typeChar = 'B'; break;
        case QUEEN:    typeChar = 'Q'; break;
        case KING:     typeChar = 'K'; break;
        case ANTEATER: typeChar = 'A'; break;
        default:       typeChar = '?';
    }

    snprintf(move_str, sizeof(move_str), "%d. %c%c: %c%d -> %c%d\n",
             controller_get_move_count(controller) + 1, colorChar, typeChar,
             colToFile(fromC), rowToRank(fromR),
             colToFile(toC), rowToRank(toR));

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(g_history_text));
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);
    gtk_text_buffer_insert(buffer, &end, move_str, -1);

    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(g_history_text), &end, 0.0, FALSE, 0, 0);
}

static gboolean on_cell_click(GtkWidget *w, GdkEventButton *ev, gpointer ud)
{
    (void)w;
    (void)ev;
    int row = GPOINTER_TO_INT(ud) / 100;
    int col = GPOINTER_TO_INT(ud) % 100;

    bool turn_played = process_cell_click(controller, row, col);

    if (controller_in_anteating(controller) == true)
    {
        gtk_widget_show(btn_end_turn);
    }
    else
    {
        gtk_widget_hide(btn_end_turn);
    }

    if (turn_played)
    {
        start_timer();
		if (g_opponent_type == OPPONENT_COMPUTER && get_current_turn(&controller) != g_player_color &&
            !controller_in_checkmate(&controller) && !controller_in_stalemate(&controller)) {
            Move bot_move = getBotMove(&controller.board);
 
            // Only apply if a valid move was returned
            if (bot_move.fromRow != -1)
            {
                Piece bot_piece = controller_get_piece_at(&controller, bot_move.fromRow, bot_move.fromCol);
                log_move_to_sidebar(bot_piece, bot_move.fromRow, bot_move.fromCol, bot_move.toRow, bot_move.toCol);
                applyMove(&controller.board, bot_move);
                switch_turn(&controller);
                clear_selection(&controller);
                start_timer();
            }
        }
    }
    refresh_all();
    refresh_timer();
    update_status();

    return TRUE;
}

static void on_new_game(GtkButton *b, gpointer d)
{
    (void)b;
    (void)d;
    init_game(controller);
	start_timer();
    refresh_all();
    update_status();
}

static void end_turn(GtkButton *b, gpointer d)
{
    (void)b;
    (void)d;
    process_end_turn(controller);
    gtk_widget_hide(btn_end_turn);
    start_timer();
    refresh_all();
}

//menu window
static const char *MENU_CSS =
    //window background
	"#menu_window {"
	" background-color: #1e2228;"
    "}"

    //title
    "#menu_title {"
    "  color: #dce3ec;"
    "  font-size: 18px;"
    "  font-weight: bold;"
    "  letter-spacing: 2px;"
    "}"

   //subtitle
    "#menu_subtitle {"
    "  color: #3d5060;"
    "  font-size: 10px;"
    "  letter-spacing: 2px;"
    "}"

    //section labels
    "#menu_section_label {"
    "  color: #2d6a9f;"
    "  font-size: 9px;"
    "  font-weight: bold;"
    "  letter-spacing: 2px;"
    "}"

    //buttons
    "#menu_button {"
    "  color: #5a7080;"
    "  background-color: #252c34;"
    "  border-radius: 6px;"
    "  border: 1px solid #2a3540;"
    "  padding: 6px 12px;"
    "  font-size: 13px;"
    "}"
    "#menu_button:checked {"
    "  background-color: #1a3650;"
    "  border-color: #2d6a9f;"
    "  color: #7ab8e8;"
    "}"
    "#menu_button:hover {"
    "  background-color: #2a3844;"
    "  border-color: #2d5a7a;"
    "  color: #8aafc8;"
    "}"

    //seperator
    "#menu_sep {"
    "  background-color: #252c34;"
    "  min-height: 1px;"
    "  margin: 4px 0;"
    "}"

    //play button
    "#menu_play {"
    "  background-color: #1a3a5c;"
    "  color: #7ab8e8;"
    "  font-size: 13px;"
    "  font-weight: bold;"
    "  letter-spacing: 3px;"
    "  border-radius: 6px;"
    "  border: 1px solid #2d6a9f;"
    "  padding: 10px 0;"
    "}"
    "#menu_play:hover {"
    "  background-color: #1f4a75;"
    "  border-color: #3d8abf;"
    "}"
    "#menu_play:active {"
    "  background-color: #122a42;"
    "}"

    //quit button
    "#menu_quit {"
    "  color: #303a45;"
    "  font-size: 11px;"
    "  background: none;"
    "  border: none;"
    "  padding: 4px;"
    "}"
    "#menu_quit:hover {"
    "  color: #4a6a80;"
    "}"
    
    //in game window
    "#game_window{"
    "  background-color: #efefef;"
    "}"
    
    //in game side box
    "#game_side_box {"
    "  background-color: #252c34;"
    "  border: 1px solid #2a3540;"
    "  border-radius: 12px;"
    "}"

    //side box labels
    "#game_side_label {"
    "  color: #5b9cf0;"
    "  font-size: 10px;"
    "  font-weight: bold;"
    "  letter-spacing: 3px;"
    "}"

    //box for clock and status
    "#side_box {"
    "  background-color: #1e2228;"
    "  border: 1px solid #2d6a9f;"
    "  border-radius: 8px;"
    "  padding: 18px;"
    "}"
    "#side_box > border {"
    "  border: none;"
    "}"

    //clock text
    "#clock_text {"
    "  color: #dce3ec;"
    "  font-size: 28px;"
    "  font-weight: bold;"
    "}"

    //status text
    "#status_text {"
    "  color: #dce3ec;"
    "  font-size: 15px;"
    "  font-weight: bold;"
    "}"

    //sidebar buttons
    "#game_side_button {"
    "  background-color: #1a3650;"
    "  color: #7ab8e8;"
    "  border-radius: 8px;"
    "  border: 1px solid #2d6a9f;"
    "  padding: 12px 14px;"
    "  font-size: 13px;"
    "  font-weight: bold;"
    "  letter-spacing: 2px;"
    "}"

    "#game_side_button:hover {"
    "  background-color: #1f4a75;"
    "  border-color: #3d8abf;"
    "}"
    
    "#history_view {"
    "  background-color: #1e2228;"
    "  color: #7ab8e8;"
    "  font-family: monospace;"
    "  font-size: 12px;"
    "}";

static void launch_game_window(void);

typedef struct {
    GtkWidget *window;
    GtkWidget *select_color;
    GtkWidget *select_opp;
} MenuData;

static void on_play(GtkButton* btn, gpointer user_data) {
    (void)btn;
    MenuData* md = (MenuData*)user_data;

    *g_player_color = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(md->select_color)) ? WHITE : BLACK;
    *g_opponent_type = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(md->select_opp)) ? OPPONENT_HUMAN : OPPONENT_COMPUTER;

    launch_game_window();
    gtk_widget_destroy(md->window);
    g_free(md);
}

static void on_app_quit(GtkButton *w, gpointer d) {
    (void)w;
    (void)d;
	stop_timer();
    gtk_main_quit();
}

static void show_rules_window(GtkButton *btn, gpointer data)
{
    (void)btn;
    (void)data;

    GtkWidget *rules_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(rules_win), "How to Play / Rules");
    gtk_window_set_default_size(GTK_WINDOW(rules_win), 500, 400);
    gtk_window_set_resizable(GTK_WINDOW(rules_win), FALSE);
    gtk_window_set_position(GTK_WINDOW(rules_win), GTK_WIN_POS_CENTER);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(box), 16);
    gtk_container_add(GTK_CONTAINER(rules_win), box);

    GtkWidget *title = gtk_label_new("HOW TO PLAY ANTEATER CHESS");
    gtk_box_pack_start(GTK_BOX(box), title, FALSE, FALSE, 5);

    GtkWidget *rules_label = gtk_label_new(
        "1. Anteater Chess is played on an 8 x 10 board.\n"
        "2. White moves first.\n"
        "3. Standard chess pieces move like regular chess.\n"
        "4. The goal is to checkmate the opponent's king.\n"
        "5. The Anteater is a special piece.\n"
        "6. The Anteater moves one square in any direction.\n"
        "7. The Anteater can only capture pawns.\n"
        "8. After capturing a pawn, the Anteater may continue capturing nearby pawns.\n"
        "9. Click a piece to select it, then click a highlighted square to move.\n"
        "10. Use END TURN after the Anteater is done capturing."
    );

    gtk_label_set_line_wrap(GTK_LABEL(rules_label), TRUE);
    gtk_label_set_xalign(GTK_LABEL(rules_label), 0.0);
    gtk_box_pack_start(GTK_BOX(box), rules_label, TRUE, TRUE, 5);

    GtkWidget *close_btn = gtk_button_new_with_label("Close");
    g_signal_connect_swapped(close_btn, "clicked",
                             G_CALLBACK(gtk_widget_destroy), rules_win);
    gtk_box_pack_start(GTK_BOX(box), close_btn, FALSE, FALSE, 5);

    gtk_widget_show_all(rules_win);
}

static void show_menu_window(void) {
    GtkCssProvider* css = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css, MENU_CSS, -1, NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(css),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(css);

    //window
    GtkWidget* win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_name(win, "menu_window");
    gtk_window_set_title(GTK_WINDOW(win), "Anteater Chess");
    gtk_window_set_resizable(GTK_WINDOW(win), FALSE);
    gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(win), 32);
    gtk_window_set_default_size(GTK_WINDOW(win), 480, -1);

    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(win), vbox);

    //anteater image
    GtkWidget* piece_img = gtk_image_new_from_file("pieces/wA.png");
    gtk_widget_set_halign(piece_img, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_bottom(piece_img, 14);
    gtk_box_pack_start(GTK_BOX(vbox), piece_img, FALSE, FALSE, 0);

    //title
    GtkWidget* title = gtk_label_new("ANTEATER CHESS");
    gtk_widget_set_name(title, "menu_title");
    gtk_widget_set_margin_bottom(title, 4);
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);

    //subtitle
    GtkWidget* subtitle = gtk_label_new("En Passant Police");
	gtk_widget_set_name(subtitle, "menu_subtitle");
	gtk_widget_set_margin_bottom(subtitle, 24);
	gtk_box_pack_start(GTK_BOX(vbox), subtitle, FALSE, FALSE, 0);

    //play as
	GtkWidget* play_as_label = gtk_label_new("PLAY AS");
	gtk_widget_set_name(play_as_label, "menu_section_label");
	gtk_label_set_xalign(GTK_LABEL(play_as_label), 0.0);
	gtk_widget_set_margin_bottom(play_as_label, 6);
	gtk_box_pack_start(GTK_BOX(vbox), play_as_label, FALSE, FALSE, 0);

	GtkWidget* color_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_widget_set_margin_bottom(color_box, 20);
	gtk_box_pack_start(GTK_BOX(vbox), color_box, FALSE, FALSE, 0);

	GtkWidget* select_white = gtk_radio_button_new_with_label(NULL, "White");
	gtk_widget_set_name(select_white, "menu_button");
	gtk_widget_set_size_request(select_white, 190, 40);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(select_white), TRUE);
	gtk_box_pack_start(GTK_BOX(color_box), select_white, TRUE, TRUE, 0);

	GtkWidget* select_black = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(select_white), "Black");
	gtk_widget_set_name(select_black, "menu_button");
    gtk_widget_set_size_request(select_black, 190, 40);
	gtk_box_pack_start(GTK_BOX(color_box), select_black, TRUE, TRUE, 0);

	//opponent
	GtkWidget* opp_label = gtk_label_new("OPPONENT");
	gtk_widget_set_name(opp_label, "menu_section_label");
	gtk_label_set_xalign(GTK_LABEL(opp_label), 0.0);
	gtk_widget_set_margin_bottom(opp_label, 6);
	gtk_box_pack_start(GTK_BOX(vbox), opp_label, FALSE, FALSE, 0);

	GtkWidget* opp_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_size_request(opp_box, 400, -1);
	gtk_widget_set_margin_bottom(opp_box, 24);
	gtk_box_pack_start(GTK_BOX(vbox), opp_box, FALSE, FALSE, 0);

	GtkWidget* select_human = gtk_radio_button_new_with_label(NULL, "Human");
	gtk_widget_set_name(select_human, "menu_button");
    gtk_widget_set_size_request(select_human, 190, 40);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(select_human), TRUE);
	gtk_box_pack_start(GTK_BOX(opp_box), select_human, TRUE, TRUE, 0);

	GtkWidget* select_computer = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(select_human), "Computer");
	gtk_widget_set_name(select_computer, "menu_button");
    gtk_widget_set_size_request(select_computer, 190, 40);
	gtk_box_pack_start(GTK_BOX(opp_box), select_computer, TRUE, TRUE, 0);

    //seperator
	GtkWidget* sep = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_widget_set_name(sep, "menu_sep");
	gtk_widget_set_margin_bottom(sep, 20);
	gtk_box_pack_start(GTK_BOX(vbox), sep, FALSE, FALSE, 0);

    //play button
	GtkWidget* play_btn = gtk_button_new_with_label("PLAY");
	gtk_widget_set_name(play_btn, "menu_play");
	gtk_widget_set_margin_bottom(play_btn, 10);
	gtk_box_pack_start(GTK_BOX(vbox), play_btn, FALSE, FALSE, 0);

	//rules button
	GtkWidget* rules_btn = gtk_button_new_with_label("HOW TO PLAY / RULES");
	gtk_widget_set_name(rules_btn, "menu_play");
	gtk_widget_set_margin_bottom(rules_btn, 10);
	gtk_box_pack_start(GTK_BOX(vbox), rules_btn, FALSE, FALSE, 0);

    //quit button
	GtkWidget* quit_btn = gtk_button_new_with_label("Quit");
	gtk_widget_set_name(quit_btn, "menu_quit");
	gtk_button_set_relief(GTK_BUTTON(quit_btn), GTK_RELIEF_NONE);
	gtk_widget_set_halign(quit_btn, GTK_ALIGN_CENTER);
	gtk_box_pack_start(GTK_BOX(vbox), quit_btn, FALSE, FALSE, 0);

	g_signal_connect(quit_btn, "clicked", G_CALLBACK(on_app_quit), NULL);
	
	g_signal_connect(rules_btn, "clicked", G_CALLBACK(show_rules_window), NULL);

	MenuData* md = g_new(MenuData, 1);
	md->window = win;
	md->select_color = select_white;
	md->select_opp = select_human;
	g_signal_connect(play_btn, "clicked", G_CALLBACK(on_play), md);

	gtk_widget_show_all(win);

}

static void launch_game_window(void) {
    init_game(controller);
    GtkWidget* win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win), "Anteater Chess");
    gtk_window_set_resizable(GTK_WINDOW(win), FALSE);
    gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER);
    g_signal_connect(win, "destroy", G_CALLBACK(on_app_quit), NULL);

    //screen layout
	GtkWidget* layout = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 18);
	gtk_container_set_border_width(GTK_CONTAINER(layout), 12);
	gtk_container_add(GTK_CONTAINER(win), layout);

    //left panel for board
	GtkWidget* board_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
	gtk_box_pack_start(GTK_BOX(layout), board_box, FALSE, FALSE, 0);

	//right panel for side bar
	GtkWidget* side_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_size_request(side_box, 260, -1);
	gtk_box_pack_start(GTK_BOX(layout), side_box, FALSE, FALSE, 0);
    
    // board row
    GtkWidget* board_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_box_pack_start(GTK_BOX(board_box), board_row, FALSE, FALSE, 0);

    // rank labels
    GtkWidget* rank_col = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(board_row), rank_col, FALSE, FALSE, 0);
    for (int r = 0; r < ROWS; r++)
    {
        char buf[4];
        snprintf(buf, sizeof buf, "%d", rowToRank(r));
        GtkWidget* lbl = gtk_label_new(buf);
        gtk_widget_set_size_request(lbl, 20, SQ);
        gtk_box_pack_start(GTK_BOX(rank_col), lbl, FALSE, FALSE, 0);
    }

    // grid
    GtkWidget* grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 0);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 0);
    gtk_box_pack_start(GTK_BOX(board_row), grid, FALSE, FALSE, 0);

    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            Cell* cell = &g_cells[r][c];

            char path[128];
            snprintf(path, sizeof path, "pieces/squares/sq_%d_%d.png", r, c);
            cell->sq_img = gtk_image_new_from_file(path);
            gtk_widget_set_size_request(cell->sq_img, SQ, SQ);

            cell->pc_img = gtk_image_new();
            gtk_widget_set_size_request(cell->pc_img, SQ, SQ);
            gtk_widget_set_halign(cell->pc_img, GTK_ALIGN_FILL);
            gtk_widget_set_valign(cell->pc_img, GTK_ALIGN_FILL);

            cell->hint_img = gtk_image_new();
            gtk_widget_set_size_request(cell->hint_img, SQ, SQ);
            gtk_widget_set_halign(cell->hint_img, GTK_ALIGN_CENTER);
            gtk_widget_set_valign(cell->hint_img, GTK_ALIGN_CENTER);

            cell->hi_img = gtk_image_new();
            gtk_widget_set_size_request(cell->hi_img, SQ, SQ);

            cell->overlay = gtk_overlay_new();
            gtk_container_add(GTK_CONTAINER(cell->overlay), cell->sq_img);
            gtk_overlay_add_overlay(GTK_OVERLAY(cell->overlay), cell->pc_img);
            gtk_overlay_add_overlay(GTK_OVERLAY(cell->overlay), cell->hint_img);
            gtk_overlay_add_overlay(GTK_OVERLAY(cell->overlay), cell->hi_img);

            cell->event_box = gtk_event_box_new();
            gtk_container_add(GTK_CONTAINER(cell->event_box), cell->overlay);
            gtk_widget_add_events(cell->event_box, GDK_BUTTON_PRESS_MASK);
            g_signal_connect(cell->event_box, "button-press-event",
                G_CALLBACK(on_cell_click),
                GINT_TO_POINTER(r * 100 + c));

            gtk_grid_attach(GTK_GRID(grid), cell->event_box, c, r, 1, 1);
        }
    }

    // file labels
    GtkWidget* file_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(board_box), file_row, FALSE, FALSE, 0);
    GtkWidget* spacer = gtk_label_new("");
    gtk_widget_set_size_request(spacer, 28, 16);
    gtk_box_pack_start(GTK_BOX(file_row), spacer, FALSE, FALSE, 0);
    for (int c = 0; c < COLS; c++)
    {
        char buf[4];
        snprintf(buf, sizeof buf, "%c", colToFile(c));
        GtkWidget* lbl = gtk_label_new(buf);
        gtk_widget_set_size_request(lbl, SQ, 16);
        gtk_box_pack_start(GTK_BOX(file_row), lbl, FALSE, FALSE, 0);
    }

    //side box frame
	GtkWidget* side_frame = gtk_frame_new(NULL);
	gtk_widget_set_name(side_frame, "game_side_box");
	gtk_box_pack_start(GTK_BOX(side_box), side_frame, FALSE, FALSE, 0);

	GtkWidget* side_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
	gtk_container_set_border_width(GTK_CONTAINER(side_vbox), 18);
	gtk_container_add(GTK_CONTAINER(side_frame), side_vbox);

    //turn clock
	GtkWidget* clock_label = gtk_label_new("TURN CLOCK");
	gtk_widget_set_name(clock_label, "game_side_label");
	gtk_label_set_xalign(GTK_LABEL(clock_label), 0.5);
	gtk_box_pack_start(GTK_BOX(side_vbox), clock_label, FALSE, FALSE, 4);

	GtkWidget* clock_box = gtk_frame_new(NULL);
	gtk_widget_set_name(clock_box, "side_box");
	gtk_box_pack_start(GTK_BOX(side_vbox), clock_box, FALSE, FALSE, 0);

	g_timer_label = gtk_label_new("Time: 00:00");
	gtk_widget_set_name(g_timer_label, "clock_text");
	gtk_container_add(GTK_CONTAINER(clock_box), g_timer_label);

	GtkWidget* sep2 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_start(GTK_BOX(side_vbox), sep2, FALSE, FALSE, 8);

    //side box buttons
	GtkWidget* side_btn = gtk_button_new_with_label("NEW GAME");
	gtk_widget_set_name(side_btn, "game_side_button");

	btn_end_turn = gtk_button_new_with_label("END TURN");
	gtk_widget_set_name(btn_end_turn, "game_side_button");

	GtkWidget* quit_btn = gtk_button_new_with_label("QUIT");
	gtk_widget_set_name(quit_btn, "game_side_button");

    g_signal_connect(side_btn, "clicked", G_CALLBACK(on_new_game), NULL);
    g_signal_connect(btn_end_turn, "clicked", G_CALLBACK(end_turn), NULL);
    g_signal_connect(quit_btn, "clicked", G_CALLBACK(on_app_quit), NULL);

    gtk_box_pack_start(GTK_BOX(side_vbox), side_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(side_vbox), btn_end_turn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(side_vbox), quit_btn, FALSE, FALSE, 0);

	GtkWidget* sep3 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_start(GTK_BOX(side_vbox), sep3, FALSE, FALSE, 8);

    // Move History Label
    GtkWidget* history_label = gtk_label_new("MOVE HISTORY");
    gtk_widget_set_name(history_label, "game_side_label");
    gtk_box_pack_start(GTK_BOX(side_vbox), history_label, FALSE, FALSE, 2);

    // Scrollable container for the text
    GtkWidget* scroll_win = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_win), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scroll_win, -1, 150); // Give it some height
    gtk_box_pack_start(GTK_BOX(side_vbox), scroll_win, TRUE, TRUE, 0);

    g_history_text = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(g_history_text), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(g_history_text), FALSE);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(g_history_text), 10);
    gtk_widget_set_name(g_history_text, "history_view"); // For CSS styling

    gtk_container_add(GTK_CONTAINER(scroll_win), g_history_text);

	//side box status
	GtkWidget* status_label = gtk_label_new("STATUS");
	gtk_widget_set_name(status_label, "game_side_label");
	gtk_label_set_xalign(GTK_LABEL(status_label), 0.5);
	gtk_box_pack_start(GTK_BOX(side_vbox), status_label, FALSE, FALSE, 2);

	GtkWidget* status_box = gtk_frame_new(NULL);
	gtk_widget_set_name(status_box, "side_box");
    gtk_box_pack_start(GTK_BOX(side_vbox), status_box, FALSE, FALSE, 0);

    // status
    g_status_label = gtk_label_new("White to move.");
	gtk_widget_set_name(g_status_label, "status_text");
	gtk_widget_set_halign(g_status_label, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(g_status_label, GTK_ALIGN_CENTER);
	gtk_container_add(GTK_CONTAINER(status_box), g_status_label);

    refresh_all();
    update_status();
    gtk_widget_show_all(win);
    gtk_widget_hide(btn_end_turn);
}

int run_gui(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
    show_menu_window();
    gtk_main();
    return 0;
}
