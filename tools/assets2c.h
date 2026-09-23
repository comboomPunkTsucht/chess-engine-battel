#ifndef ASSETS2C_H_
#define ASSETS2C_H_
#include "raylib.h"
#include "raymath.h"
#define NOB_IMPLEMENTATION
#include "nob_addons.h"
#define HT_IMPLEMENTATION
#include "ht.h"

enum AssetType {
  ASSET_TYPE_UNKNOWN = 0,
  ASSET_TYPE_IMAGE,
  ASSET_TYPE_SOUND,
  ASSET_TYPE_MUSIC,
  ASSET_TYPE_SHADER,
  ASSET_TYPE_FONT,
  ASSET_TYPE_TEXT,
  ASSET_TYPE_BINARY
};

typedef struct {
    char          *name;
    enum AssetType type;

    union {
        Image  image;
        Sound  sound;
        Music  music;
        Shader shader;
        Font   font;
        char  *text;
        void  *binary;
    } data;
} Asset;

#endif
