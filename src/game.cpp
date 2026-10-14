#include "game.h"
#include "global.h"
#include "nob_addons.h"
#include <cmath>

void Game::setup_board() {
  board[0].type = Rook;
  board[0].color = PLAYER_WHITE;
  board[1].type = Knight;
  board[1].color = PLAYER_WHITE;
  board[2].type = Bishop;
  board[2].color = PLAYER_WHITE;
  board[3].type = Queen;
  board[3].color = PLAYER_WHITE;
  board[4].type = King;
  board[4].color = PLAYER_WHITE;
  board[5].type = Bishop;
  board[5].color = PLAYER_WHITE;
  board[6].type = Knight;
  board[6].color = PLAYER_WHITE;
  board[7].type = Rook;
  board[7].color = PLAYER_WHITE;

  for (int i = 8; i < 16; i++) {
    board[i].type = Pawn;
    board[i].color = PLAYER_WHITE;
  }

  for (int i = 16; i < 48; i++) {
    board[i].type = None;
    board[i].color = PLAYER_WHITE;
  }

  for (int i = 48; i < 56; i++) {
    board[i].type = Pawn;
    board[i].color = PLAYER_BLACK;
  }

  board[56].type = Rook;
  board[56].color = PLAYER_BLACK;
  board[57].type = Knight;
  board[57].color = PLAYER_BLACK;
  board[58].type = Bishop;
  board[58].color = PLAYER_BLACK;
  board[59].type = Queen;
  board[59].color = PLAYER_BLACK;
  board[60].type = King;
  board[60].color = PLAYER_BLACK;
  board[61].type = Bishop;
  board[61].color = PLAYER_BLACK;
  board[62].type = Knight;
  board[62].color = PLAYER_BLACK;
  board[63].type = Rook;
  board[63].color = PLAYER_BLACK;
}

void Game::doMove(Move move) {
  const int from = move.From();
  const int to = move.To();

  Piece     moving = board[from];
  Piece     captured = board[to];

  board[from] = {None, PLAYER_WHITE};

  if (move.Type() == MoveType::EnPassant) {
    const int capturedPawnIdx = (moving.color == PLAYER_WHITE)
                                    ? (to - DEFAULT_BOARD_DIMENSIONS)
                                    : (to + DEFAULT_BOARD_DIMENSIONS);
    board[capturedPawnIdx] = {None, PLAYER_WHITE};
  }

  if (move.Type() == MoveType::CastleKing) {
    const int rookFrom = (moving.color == PLAYER_WHITE) ? 7 : 63;
    const int rookTo = (moving.color == PLAYER_WHITE) ? 5 : 61;
    board[rookTo] = board[rookFrom];
    board[rookFrom] = {None, PLAYER_WHITE};
  } else if (move.Type() == MoveType::CastleQueen) {
    const int rookFrom = (moving.color == PLAYER_WHITE) ? 0 : 56;
    const int rookTo = (moving.color == PLAYER_WHITE) ? 3 : 59;
    board[rookTo] = board[rookFrom];
    board[rookFrom] = {None, PLAYER_WHITE};
  }

  if (move.Promo() != PieceType::None) { moving.type = move.Promo(); }

  board[to] = moving;

  history.push_back(move);

  turn = (turn == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE;
}

void Game::initialization(void) {
  SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

#ifdef _WIN32

  SetWindowIcon(
      LoadImageFromMemory(".png", Assets::icon_png, Assets::icon_png_size));

#endif

  chess_piece_image = LoadImageFromMemory(".png", Assets::All_pieces_png,
                                          Assets::All_pieces_png_size);
  chess_piece_textures = LoadTextureFromImage(chess_piece_image);

  float font_size = std::max(std::floor(screen_width * font_size_factor + 0.2),
                             std::floor(screen_height * 0.025));

  main_font = LoadFontFromMemory(
      ".ttf", Assets::CaskaydiaCoveNerdFontPropo_Regular_ttf,
      Assets::CaskaydiaCoveNerdFontPropo_Regular_ttf_size, font_size, NULL, 0);
  bold_font = LoadFontFromMemory(
      ".ttf", Assets::CaskaydiaCoveNerdFontPropo_Bold_ttf,
      Assets::CaskaydiaCoveNerdFontPropo_Bold_ttf_size, font_size, NULL, 0);
  italic_font = LoadFontFromMemory(
      ".ttf", Assets::CaskaydiaCoveNerdFontPropo_Italic_ttf,
      Assets::CaskaydiaCoveNerdFontPropo_Italic_ttf_size, font_size, NULL, 0);

  temp_mark = temp_save();
}
