#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pieces.h"
#include "board.h"
#include "moves.h"
#include "rules.h"

#define SQ 80

//oponent type and color selection
typedef enum { OPPONENT_HUMAN, OPPONENT_COMPUTER } OpponentType;

static Color g_player_color = WHITE;
static OpponentType g_opponent_type = OPPONENT_HUMAN;

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

static Board g_board;

// Selection state
static int g_selected = 0;
static int g_sel_row = 0;
static int g_sel_col = 0;

// legal moves for currently selected piece
static MoveList g_hints;

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

static int is_hint_square(int r, int c)
{
    for (int i = 0; i < g_hints.index; i++)
    {
        if (g_hints.list[i].toRow == r && g_hints.list[i].toCol == c)
        {
            return 1;
        }
    }
    return 0;
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
    Piece p = getPiece(&g_board, r, c);
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

            if (g_selected && is_hint_square(r, c))
            {
                set_hint(r, c, 1);
            }
            else
            {
                set_hint(r, c, 0);
            }

            if (g_selected && r == g_sel_row && c == g_sel_col)
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

static void update_status(void)
{
    Color c = g_board.currentTurn;
    if (isCheckmate(&g_board, c))
        gtk_label_set_text(GTK_LABEL(g_status_label),
                           c == WHITE ? "Checkmate! Black wins!" : "Checkmate! White wins!");
    else if (isStalemate(&g_board, c))
        gtk_label_set_text(GTK_LABEL(g_status_label), "Stalemate! Draw.");
    else
        gtk_label_set_text(GTK_LABEL(g_status_label),
                           c == WHITE ? "White to move." : "Black to move.");
}

static int is_legal_target(int r, int c)
{
    for (int i = 0; i < g_hints.index; i++)
        if (g_hints.list[i].toRow == r && g_hints.list[i].toCol == c)
            return 1;
    return 0;
}

// Only use when move is known to be legal
static Move get_played_move(int r, int c)
{
    for (int i = 0; i < g_hints.index; i++)
        if (g_hints.list[i].toRow == r && g_hints.list[i].toCol == c)
            return g_hints.list[i];
    
    return createMove(-1, -1, -1, -1);
}

static gboolean on_cell_click(GtkWidget *w, GdkEventButton *ev, gpointer ud)
{
    (void)w;
    (void)ev;
    int row = GPOINTER_TO_INT(ud) / 100;
    int col = GPOINTER_TO_INT(ud) % 100;

    Piece clicked = getPiece(&g_board, row, col);
    int own = isOwnPiece(clicked, g_board.currentTurn);

    if (!g_selected)
    {
        if (own)
        {
            g_selected = 1;
            g_sel_row = row;
            g_sel_col = col;
            g_hints.index = 0;
            legalMovesForPiece(&g_board, row, col, &g_hints);
            refresh_all();
        }
    }
    else
    {
        if (is_legal_target(row, col))
        {
            Move playedMove = get_played_move(row, col);
            Piece selected = getPiece(&g_board, g_sel_row, g_sel_col);
            Piece target = getPiece(&g_board, row, col);
            if (selected.pieceType == ANTEATER && target.pieceType == PAWN)
            {
                g_board.isAntEating = true;
                gtk_widget_show(btn_end_turn);
            }
            applyMove(&g_board, playedMove);

            if (g_board.isAntEating == false)
            {
                g_board.currentTurn = (g_board.currentTurn == WHITE) ? BLACK : WHITE;
                g_selected = 0;
                g_hints.index = 0;
                refresh_all();
                update_status();
                refresh_all();
            }
            else
            {
                g_selected = 0;
                g_hints.index = 0;
                legalMovesForPiece(&g_board, row, col, &g_hints);
                if (g_hints.index == 0)
                {
                    g_board.isAntEating = false;
                    gtk_widget_hide(btn_end_turn);
                    g_board.currentTurn = (g_board.currentTurn == WHITE) ? BLACK : WHITE;
                }
                refresh_all();
                update_status();
                refresh_all();
            }
        }
        else if (own)
        {
            g_sel_row = row;
            g_sel_col = col;
            g_hints.index = 0;
            legalMovesForPiece(&g_board, row, col, &g_hints);
            refresh_all();
        }
        else
        {
            g_selected = 0;
            g_hints.index = 0;
            refresh_all();
        }
    }

    return TRUE;
}

static void on_new_game(GtkButton *b, gpointer d)
{
    (void)b;
    (void)d;
    setupBoard(&g_board);
    g_selected = 0;
    g_hints.index = 0;
    refresh_all();
    update_status();
}

static void end_turn(GtkButton *b, gpointer d)
{
    (void)b;
    (void)d;
    g_board.isAntEating = false;
    gtk_widget_hide(btn_end_turn);
    g_board.currentTurn = (g_board.currentTurn == WHITE) ? BLACK : WHITE;
    update_status();
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

    g_player_color = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(md->select_color)) ? WHITE : BLACK;
    g_opponent_type = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(md->select_opp)) ? OPPONENT_HUMAN : OPPONENT_COMPUTER;

    gtk_widget_destroy(md->window);
    g_free(md);
    launch_game_window();
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
    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);

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
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(select_white), TRUE);
	gtk_box_pack_start(GTK_BOX(color_box), select_white, TRUE, TRUE, 0);

	GtkWidget* select_black = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(select_white), "Black");
	gtk_widget_set_name(select_black, "menu_button");
	gtk_box_pack_start(GTK_BOX(color_box), select_black, TRUE, TRUE, 0);

	//opponent
	GtkWidget* opp_label = gtk_label_new("OPPONENT");
	gtk_widget_set_name(opp_label, "menu_section_label");
	gtk_label_set_xalign(GTK_LABEL(opp_label), 0.0);
	gtk_widget_set_margin_bottom(opp_label, 6);
	gtk_box_pack_start(GTK_BOX(vbox), opp_label, FALSE, FALSE, 0);

	GtkWidget* opp_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_widget_set_margin_bottom(opp_box, 24);
	gtk_box_pack_start(GTK_BOX(vbox), opp_box, FALSE, FALSE, 0);

	GtkWidget* select_human = gtk_radio_button_new_with_label(NULL, "Human");
	gtk_widget_set_name(select_human, "menu_button");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(select_human), TRUE);
	gtk_box_pack_start(GTK_BOX(opp_box), select_human, TRUE, TRUE, 0);

	GtkWidget* select_computer = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(select_human), "Computer");
	gtk_widget_set_name(select_computer, "menu_button");
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

    //quit button
	GtkWidget* quit_btn = gtk_button_new_with_label("Quit");
	gtk_widget_set_name(quit_btn, "menu_quit");
	gtk_button_set_relief(GTK_BUTTON(quit_btn), GTK_RELIEF_NONE);
	gtk_widget_set_halign(quit_btn, GTK_ALIGN_CENTER);
	gtk_box_pack_start(GTK_BOX(vbox), quit_btn, FALSE, FALSE, 0);

	g_signal_connect(quit_btn, "clicked", G_CALLBACK(gtk_main_quit), NULL);

	MenuData* md = g_new(MenuData, 1);
	md->window = win;
	md->select_color = select_white;
	md->select_opp = select_human;
	g_signal_connect(play_btn, "clicked", G_CALLBACK(on_play), md);

	gtk_widget_show_all(win);

}

static void launch_game_window(void) {
    setupBoard(&g_board);
    g_hints.index = 0;

    GtkWidget* win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win), "Anteater Chess");
    gtk_window_set_resizable(GTK_WINDOW(win), FALSE);
    gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER);
    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 8);
    gtk_container_add(GTK_CONTAINER(win), vbox);
    
    // board row
    GtkWidget* board_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_box_pack_start(GTK_BOX(vbox), board_row, FALSE, FALSE, 0);

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
    gtk_box_pack_start(GTK_BOX(vbox), file_row, FALSE, FALSE, 0);
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

    // status
    g_status_label = gtk_label_new("White to move.");
    gtk_label_set_xalign(GTK_LABEL(g_status_label), 0.0);
    gtk_box_pack_start(GTK_BOX(vbox), g_status_label, FALSE, FALSE, 2);

    // buttons
    GtkWidget* btn_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_pack_start(GTK_BOX(vbox), btn_row, FALSE, FALSE, 4);

    GtkWidget* btn_new = gtk_button_new_with_label("New Game");
    GtkWidget* btn_quit = gtk_button_new_with_label("Quit");
    btn_end_turn = gtk_button_new_with_label("End Turn");
    g_signal_connect(btn_new, "clicked", G_CALLBACK(on_new_game), NULL);
    g_signal_connect(btn_quit, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(btn_end_turn, "clicked", G_CALLBACK(end_turn), NULL);
    gtk_box_pack_start(GTK_BOX(btn_row), btn_new, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(btn_row), btn_quit, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(btn_row), btn_end_turn, FALSE, FALSE, 0);

    refresh_all();
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
