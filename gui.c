#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pieces.h"
#include "board.h"
#include "moves.h"
#include "rules.h"

#define SQ 80

static const char *piece_image(Piece p)
{
    if (p.pieceType == EMPTY) return NULL;
 
    if (p.color == WHITE) {
        switch (p.pieceType) {
        case PAWN:     return "pieces/wP.png";
        case ROOK:     return "pieces/wR.png";
        case KNIGHT:   return "pieces/wN.png";
        case BISHOP:   return "pieces/wB.png";
        case QUEEN:    return "pieces/wQ.png";
        case KING:     return "pieces/wK.png";
        case ANTEATER: return "pieces/wA.png";
        default:       return NULL;
        }
    } else {
        switch (p.pieceType) {
        case PAWN:     return "pieces/bP.png";
        case ROOK:     return "pieces/bR.png";
        case KNIGHT:   return "pieces/bN.png";
        case BISHOP:   return "pieces/bB.png";
        case QUEEN:    return "pieces/bQ.png";
        case KING:     return "pieces/bK.png";
        case ANTEATER: return "pieces/bA.png";
        default:       return NULL;
        }
    }
}

static Board g_board;
 
//Selection state
static int g_selected   = 0;
static int g_sel_row    = 0;
static int g_sel_col    = 0;
 
//legal moves for currently selected piece
static MoveList g_hints;

typedef struct {
    GtkWidget *event_box;
    GtkWidget *overlay;
    GtkWidget *sq_img;   //board tile
    GtkWidget *pc_img;   //piece image
} Cell;
 
static Cell           g_cells[ROWS][COLS];
static GtkWidget     *g_status_label = NULL;

static void refresh_cell(int r, int c){
    Piece p = getPiece(&g_board, r, c);
    const char *img = piece_image(p);
    if (img) {
        gtk_image_set_from_file(GTK_IMAGE(g_cells[r][c].pc_img), img);
    } else {
        gtk_image_clear(GTK_IMAGE(g_cells[r][c].pc_img));
    }
}
 
static void refresh_all(void){
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            refresh_cell(r, c);
}

static void update_status(void){
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

static int is_legal_target(int r, int c){
    for (int i = 0; i < g_hints.index; i++)
        if (g_hints.list[i].toRow == r && g_hints.list[i].toCol == c)
            return 1;
    return 0;
}

static gboolean on_cell_click(GtkWidget *w, GdkEventButton *ev, gpointer ud){
    (void)w; (void)ev;
    int row = GPOINTER_TO_INT(ud) / 100;
    int col = GPOINTER_TO_INT(ud) % 100;
 
    Piece clicked = getPiece(&g_board, row, col);
    int own = (clicked.pieceType != EMPTY &&
               clicked.color == g_board.currentTurn);
 
    if (!g_selected) {
        if (own) {
            g_selected = 1;
            g_sel_row = row;
            g_sel_col = col;
            g_hints.index = 0;
            legalMovesForPiece(&g_board, row, col, &g_hints);
        }
    } else {
        if (is_legal_target(row, col)) {
            movePiece(&g_board, g_sel_row, g_sel_col, row, col);
            g_board.moveCount++;
            g_board.currentTurn =
                (g_board.currentTurn == WHITE) ? BLACK : WHITE;
            g_selected    = 0;
            g_hints.index = 0;
            refresh_all();
            update_status();
        } else if (own) {
            g_sel_row     = row;
            g_sel_col     = col;
            g_hints.index = 0;
            legalMovesForPiece(&g_board, row, col, &g_hints);
        } else {
            g_selected    = 0;
            g_hints.index = 0;
        }
    }
 
    return TRUE;
}

static void on_new_game(GtkButton *b, gpointer d)
{
    (void)b; (void)d;
    setupBoard(&g_board);
    g_selected    = 0;
    g_hints.index = 0;
    refresh_all();
    update_status();
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
    setupBoard(&g_board);
    g_hints.index = 0;
 
    //Window
    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win), "Anteater Chess");
    gtk_window_set_resizable(GTK_WINDOW(win), FALSE);
    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);
 
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 8);
    gtk_container_add(GTK_CONTAINER(win), vbox);
 
    //board row
    GtkWidget *board_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_box_pack_start(GTK_BOX(vbox), board_row, FALSE, FALSE, 0);
 
    //rank labels
    GtkWidget *rank_col = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(board_row), rank_col, FALSE, FALSE, 0);
    for (int r = 0; r < ROWS; r++) {
        char buf[4];
        snprintf(buf, sizeof buf, "%d", rowToRank(r));
        GtkWidget *lbl = gtk_label_new(buf);
        gtk_widget_set_size_request(lbl, 20, SQ);
        gtk_box_pack_start(GTK_BOX(rank_col), lbl, FALSE, FALSE, 0);
    }
 
    //grid
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 0);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 0);
    gtk_box_pack_start(GTK_BOX(board_row), grid, FALSE, FALSE, 0);
 
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            Cell *cell = &g_cells[r][c];
 
            char path[128];
            snprintf(path, sizeof path, "pieces/squares/sq_%d_%d.png", r, c);
            cell->sq_img = gtk_image_new_from_file(path);
            gtk_widget_set_size_request(cell->sq_img, SQ, SQ);
 
            cell->pc_img = gtk_image_new();
            gtk_widget_set_size_request(cell->pc_img, SQ, SQ);
            gtk_widget_set_halign(cell->pc_img, GTK_ALIGN_FILL);
            gtk_widget_set_valign(cell->pc_img, GTK_ALIGN_FILL);
 
            cell->overlay = gtk_overlay_new();
            gtk_container_add(GTK_CONTAINER(cell->overlay), cell->sq_img);
            gtk_overlay_add_overlay(GTK_OVERLAY(cell->overlay), cell->pc_img);
 
            cell->event_box = gtk_event_box_new();
            gtk_container_add(GTK_CONTAINER(cell->event_box), cell->overlay);
            gtk_widget_add_events(cell->event_box, GDK_BUTTON_PRESS_MASK);
            g_signal_connect(cell->event_box, "button-press-event",
                             G_CALLBACK(on_cell_click),
                             GINT_TO_POINTER(r * 100 + c));
 
            gtk_grid_attach(GTK_GRID(grid), cell->event_box, c, r, 1, 1);
        }
    }
 
    //file labels
    GtkWidget *file_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(vbox), file_row, FALSE, FALSE, 0);
    GtkWidget *spacer = gtk_label_new("");
    gtk_widget_set_size_request(spacer, 28, 16);
    gtk_box_pack_start(GTK_BOX(file_row), spacer, FALSE, FALSE, 0);
    for (int c = 0; c < COLS; c++) {
        char buf[4];
        snprintf(buf, sizeof buf, "%c", colToFile(c));
        GtkWidget *lbl = gtk_label_new(buf);
        gtk_widget_set_size_request(lbl, SQ, 16);
        gtk_box_pack_start(GTK_BOX(file_row), lbl, FALSE, FALSE, 0);
    }
 
    //status
    g_status_label = gtk_label_new("White to move.");
    gtk_label_set_xalign(GTK_LABEL(g_status_label), 0.0);
    gtk_box_pack_start(GTK_BOX(vbox), g_status_label, FALSE, FALSE, 2);
 
    //buttons
    GtkWidget *btn_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_pack_start(GTK_BOX(vbox), btn_row, FALSE, FALSE, 4);
 
    GtkWidget *btn_new  = gtk_button_new_with_label("New Game");
    GtkWidget *btn_quit = gtk_button_new_with_label("Quit");
    g_signal_connect(btn_new,  "clicked", G_CALLBACK(on_new_game),   NULL);
    g_signal_connect(btn_quit, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    gtk_box_pack_start(GTK_BOX(btn_row), btn_new,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(btn_row), btn_quit, FALSE, FALSE, 0);
 
    refresh_all();
    gtk_widget_show_all(win);
    gtk_main();
    return 0;
}
 
