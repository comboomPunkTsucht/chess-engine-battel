#![allow(dead_code)]

use gpui::{Hsla, rgba};

// Polar Night
pub const NORD0: u32 = 0x2e3440ff;
pub const NORD1: u32 = 0x3b4252ff;
pub const NORD2: u32 = 0x434c5eff;
pub const NORD3: u32 = 0x4c566aff;
// Snow Storm
pub const NORD4: u32 = 0xd8dee9ff;
pub const NORD5: u32 = 0xe5e9f0ff;
pub const NORD6: u32 = 0xeceff4ff;
// Frost
pub const NORD7: u32 = 0x8fbcbbff;
pub const NORD8: u32 = 0x88c0d0ff;
pub const NORD9: u32 = 0x81a1c1ff;
pub const NORD10: u32 = 0x5e81acff;
// Aurora
pub const NORD11: u32 = 0xbf616aff;
pub const NORD12: u32 = 0xd08770ff;
pub const NORD13: u32 = 0xebcb8bff;
pub const NORD14: u32 = 0xa3be8cff;
pub const NORD15: u32 = 0xb48eadff;

pub const NORD_COLORS: [u32; 16] = [
    NORD0, NORD1, NORD2, NORD3, NORD4, NORD5, NORD6, NORD7, NORD8, NORD9, NORD10, NORD11, NORD12,
    NORD13, NORD14, NORD15,
];

// Semantic aliases (personal additions, not part of the original palette)
pub const NORD_BACKGROUND: u32 = NORD0;
pub const NORD_FOREGROUND: u32 = NORD6;
pub const NORD_TEXT: u32 = NORD_FOREGROUND;
pub const NORD_PRIMARY: u32 = NORD14;
pub const NORD_ACCENT: u32 = NORD11;
pub const NORD_HIGHLIGHT: u32 = NORD_ACCENT;
pub const NORD_ERROR: u32 = NORD11;
pub const NORD_WARNING: u32 = NORD12;
pub const NORD_INFO: u32 = NORD6;
pub const NORD_TRACE: u32 = NORD13;
pub const NORD_DEBUG: u32 = NORD15;
pub const NORD_FATAL: u32 = NORD11;
pub const NORD_SUCCESS: u32 = NORD14;

/// Convert an `0xRRGGBBAA` palette entry into a gpui color.
pub fn hex(color: u32) -> Hsla {
    rgba(color).into()
}

/// Convert a palette entry by index (0..16) into a gpui color.
pub fn nord(index: usize) -> Hsla {
    hex(NORD_COLORS[index])
}
