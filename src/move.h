#pragma once
#include <cstdint>
#include <cassert>
#include "piece.h"

enum MoveType : uint8_t {
    Normal = 0,
    DoublePush = 1,
    Capture = 2,
    EnPassant = 3,
    CastleKing = 4,
    CastleQueen = 5,
    PromoKnight = 6,
    PromoBishop = 7,
    PromoRook = 8,
    PromoQueen = 9,
    PromoCaptureKnight = 10,
    PromoCaptureBishop = 11,
    PromoCaptureRook = 12,
    PromoCaptureQueen = 13,
};

struct Move {
    /**
     * 0-5   from square (0..63)
     * 6-11  to square   (0..63)
     * 12-15  MoveType
     * 16-19  captured piece type
     * 20-23  promotion piece type
     * 24-27  previous castling rights
     * 28-31  previous en passant file
     */
    uint32_t data;

    static constexpr Move None() { return Move{0}; }
    static constexpr bool IsNone(Move m) { return m.data == 0; }

    constexpr int From() const { return data & 0x3F; }
    constexpr int To() const { return (data >> 6) & 0x3F; }
    constexpr MoveType Type() const { return (MoveType)((data >> 12) & 0xF); }
    constexpr PieceType Captured() const { return (PieceType)((data >> 16) & 0xF); }
    constexpr PieceType Promo() const { return (PieceType)((data >> 20) & 0xF); }
    constexpr uint8_t PrevCastling() const { return (data >> 24) & 0xF; }
    constexpr uint8_t PrevEnPassantFile() const { return (data >> 28) & 0xF; }

    constexpr bool IsCapture() const {
        return Captured() != PieceType::None || Type() == MoveType::EnPassant;
    }
    constexpr bool IsPromotion() const {
        return Promo() != PieceType::None;
    }
    constexpr bool IsCastle() const {
        return Type() == MoveType::CastleKing || Type() == MoveType::CastleQueen;
    }

    constexpr bool operator==(Move o) const { return data == o.data; }
    constexpr bool operator!=(Move o) const { return data != o.data; }

    constexpr Move MakeMove(
        int from, int to,
        MoveType type,
        PieceType captured = PieceType::None,
        PieceType promo    = PieceType::None,
        uint8_t prevCastling = 0,
        uint8_t prevEnPassatFile   = 0)
    {
        assert(from >= 0 && from < 64);
        assert(to   >= 0 && to   < 64);
        return Move{
            (uint32_t)(from & 0x3F)
        | ((uint32_t)(to & 0x3F)          << 6)
        | ((uint32_t)type                 << 12)
        | ((uint32_t)captured             << 16)
        | ((uint32_t)promo                << 20)
        | ((uint32_t)(prevCastling & 0xF) << 24)
        | ((uint32_t)(prevEnPassatFile  & 0xF)  << 28)
        };
    }

    constexpr Move QuietMove(int from, int to) {
        return MakeMove(from, to, MoveType::Normal);
    }

    constexpr Move DoublePush(int from, int to) {
        return MakeMove(from, to, MoveType::DoublePush);
    }

    constexpr Move CaptureMove(int from, int to, PieceType captured) {
        return MakeMove(from, to, MoveType::Capture, captured);
    }

    constexpr Move EnPassantMove(int from, int to, PieceType capturedPawn) {
        return MakeMove(from, to, MoveType::EnPassant, capturedPawn);
    }

    constexpr Move CastleKingSide(int from, int to) {
        return MakeMove(from, to, MoveType::CastleKing);
    }

    constexpr Move CastleQueenSide(int from, int to) {
        return MakeMove(from, to, MoveType::CastleQueen);
    }

    constexpr Move PromotionMove(int from, int to, PieceType promo,
                                PieceType captured = PieceType::None)
    {
        MoveType t;
        switch (promo) {
            case PieceType::Knight:
                t = (captured == PieceType::None) ?
                    MoveType::PromoKnight : MoveType::PromoCaptureKnight; break;
            case PieceType::Bishop:
                t = (captured == PieceType::None) ?
                    MoveType::PromoBishop : MoveType::PromoCaptureBishop; break;
            case PieceType::Rook:
                t = (captured == PieceType::None) ?
                    MoveType::PromoRook : MoveType::PromoCaptureRook; break;
            case PieceType::Queen:
                t = (captured == PieceType::None) ?
                    MoveType::PromoQueen : MoveType::PromoCaptureQueen; break;
            default:
                t = MoveType::Normal; break;
        }
        return MakeMove(from, to, t, captured, promo);
    }
};