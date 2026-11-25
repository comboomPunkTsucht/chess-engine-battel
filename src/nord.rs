#![allow(dead_code)]

use std::borrow::Cow;

use gpui_kit::{App, Hsla, rgba};

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

/// UI / proportional family used across the app.
pub const FONT_FAMILY: &str = "CaskaydiaCove Nerd Font Propo";
/// Monospace family for engine logs.
pub const FONT_FAMILY_MONO: &str = "CaskaydiaCove Nerd Font Mono";

/// Convert an `0xRRGGBBAA` palette entry into an `Hsla`.
pub fn hex(color: u32) -> Hsla {
    rgba(color).into()
}

/// Convert a palette entry by index (0..16) into an `Hsla`.
pub fn nord(index: usize) -> Hsla {
    hex(NORD_COLORS[index])
}

// All three CaskaydiaCove variants (Code, Mono, Propo) with every weight and
// style, embedded into the binary.
const FONTS: [(&str, &[u8]); 36] = [
    (
        "CaskaydiaCove-Bold",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFont-Bold.ttf"),
    ),
    (
        "CaskaydiaCove-BoldItalic",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFont-BoldItalic.ttf"),
    ),
    (
        "CaskaydiaCove-ExtraLight",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFont-ExtraLight.ttf"),
    ),
    (
        "CaskaydiaCove-ExtraLightItalic",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFont-ExtraLightItalic.ttf"),
    ),
    (
        "CaskaydiaCove-Italic",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFont-Italic.ttf"),
    ),
    (
        "CaskaydiaCove-Light",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFont-Light.ttf"),
    ),
    (
        "CaskaydiaCove-LightItalic",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFont-LightItalic.ttf"),
    ),
    (
        "CaskaydiaCove-Regular",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFont-Regular.ttf"),
    ),
    (
        "CaskaydiaCove-SemiBold",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFont-SemiBold.ttf"),
    ),
    (
        "CaskaydiaCove-SemiBoldItalic",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFont-SemiBoldItalic.ttf"),
    ),
    (
        "CaskaydiaCove-SemiLight",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFont-SemiLight.ttf"),
    ),
    (
        "CaskaydiaCove-SemiLightItalic",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFont-SemiLightItalic.ttf"),
    ),
    (
        "CaskaydiaCoveMono-Bold",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFontMono-Bold.ttf"),
    ),
    (
        "CaskaydiaCoveMono-BoldItalic",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFontMono-BoldItalic.ttf"),
    ),
    (
        "CaskaydiaCoveMono-ExtraLight",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFontMono-ExtraLight.ttf"),
    ),
    (
        "CaskaydiaCoveMono-ExtraLightItalic",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFontMono-ExtraLightItalic.ttf"),
    ),
    (
        "CaskaydiaCoveMono-Italic",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFontMono-Italic.ttf"),
    ),
    (
        "CaskaydiaCoveMono-Light",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFontMono-Light.ttf"),
    ),
    (
        "CaskaydiaCoveMono-LightItalic",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFontMono-LightItalic.ttf"),
    ),
    (
        "CaskaydiaCoveMono-Regular",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFontMono-Regular.ttf"),
    ),
    (
        "CaskaydiaCoveMono-SemiBold",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFontMono-SemiBold.ttf"),
    ),
    (
        "CaskaydiaCoveMono-SemiBoldItalic",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFontMono-SemiBoldItalic.ttf"),
    ),
    (
        "CaskaydiaCoveMono-SemiLight",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFontMono-SemiLight.ttf"),
    ),
    (
        "CaskaydiaCoveMono-SemiLightItalic",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFontMono-SemiLightItalic.ttf"),
    ),
    (
        "CaskaydiaCovePropo-Bold",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFontPropo-Bold.ttf"),
    ),
    (
        "CaskaydiaCovePropo-BoldItalic",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFontPropo-BoldItalic.ttf"),
    ),
    (
        "CaskaydiaCovePropo-ExtraLight",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFontPropo-ExtraLight.ttf"),
    ),
    (
        "CaskaydiaCovePropo-ExtraLightItalic",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFontPropo-ExtraLightItalic.ttf"),
    ),
    (
        "CaskaydiaCovePropo-Italic",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFontPropo-Italic.ttf"),
    ),
    (
        "CaskaydiaCovePropo-Light",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFontPropo-Light.ttf"),
    ),
    (
        "CaskaydiaCovePropo-LightItalic",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFontPropo-LightItalic.ttf"),
    ),
    (
        "CaskaydiaCovePropo-Regular",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFontPropo-Regular.ttf"),
    ),
    (
        "CaskaydiaCovePropo-SemiBold",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFontPropo-SemiBold.ttf"),
    ),
    (
        "CaskaydiaCovePropo-SemiBoldItalic",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFontPropo-SemiBoldItalic.ttf"),
    ),
    (
        "CaskaydiaCovePropo-SemiLight",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFontPropo-SemiLight.ttf"),
    ),
    (
        "CaskaydiaCovePropo-SemiLightItalic",
        include_bytes!("../assets/fonts/CaskaydiaCoveNerdFontPropo-SemiLightItalic.ttf"),
    ),
];

/// Embed every CaskaydiaCove face into the GPUI text system.
pub fn register_fonts(cx: &mut App) {
    let fonts: Vec<Cow<'static, [u8]>> = FONTS.iter().map(|(_, bytes)| Cow::Borrowed(*bytes)).collect();
    if let Err(err) = cx.text_system().add_fonts(fonts) {
        eprintln!("[ui] failed to register fonts: {err}");
    }
}

/// Install Nord as the dark theme: mode first (loads the registered dark
/// config), then explicit colors and font families. `resolve_default_font`
/// only rewrites `.SystemUIFont`, so an explicit family is kept as-is.
pub fn apply(cx: &mut App) {
    use gpui_kit::component::{Theme, ThemeMode};

    Theme::change(ThemeMode::Dark, None, cx);
    Theme::update(cx, |theme| {
        theme.font_family = FONT_FAMILY.into();
        theme.mono_font_family = FONT_FAMILY_MONO.into();

        theme.background = hex(NORD0);
        theme.foreground = hex(NORD6);
        theme.border = hex(NORD2);
        theme.muted = hex(NORD1);
        theme.muted_foreground = hex(NORD4);

        theme.primary = hex(NORD14);
        theme.primary_hover = hex(NORD13);
        theme.primary_active = hex(NORD15);
        theme.primary_foreground = hex(NORD0);

        theme.secondary = hex(NORD2);
        theme.secondary_hover = hex(NORD3);
        theme.secondary_active = hex(NORD1);
        theme.secondary_foreground = hex(NORD6);

        theme.button = hex(NORD2);
        theme.button_hover = hex(NORD3);
        theme.button_active = hex(NORD1);
        theme.button_foreground = hex(NORD6);

        theme.button_primary = hex(NORD14);
        theme.button_primary_hover = hex(NORD13);
        theme.button_primary_active = hex(NORD15);
        theme.button_primary_foreground = hex(NORD0);

        theme.button_secondary = hex(NORD2);
        theme.button_secondary_hover = hex(NORD3);
        theme.button_secondary_active = hex(NORD1);
        theme.button_secondary_foreground = hex(NORD6);

        theme.button_danger = hex(NORD11);
        theme.button_danger_hover = hex(NORD12);
        theme.button_danger_active = hex(NORD13);
        theme.button_danger_foreground = hex(NORD0);

        theme.input = hex(NORD1);
        theme.caret = hex(NORD8);
        theme.selection = hex(NORD9);
        theme.ring = hex(NORD8);

        theme.link = hex(NORD8);
        theme.link_hover = hex(NORD7);
        theme.link_active = hex(NORD9);

        theme.danger = hex(NORD11);
        theme.danger_hover = hex(NORD12);
        theme.danger_active = hex(NORD13);
        theme.danger_foreground = hex(NORD0);

        theme.success = hex(NORD14);
        theme.success_hover = hex(NORD13);
        theme.success_active = hex(NORD15);
        theme.success_foreground = hex(NORD0);

        theme.warning = hex(NORD13);
        theme.warning_hover = hex(NORD12);
        theme.warning_active = hex(NORD15);
        theme.warning_foreground = hex(NORD0);

        theme.info = hex(NORD8);
        theme.info_hover = hex(NORD7);
        theme.info_active = hex(NORD9);
        theme.info_foreground = hex(NORD0);

        theme.accent = hex(NORD14);
        theme.accent_foreground = hex(NORD0);

        theme.tab = hex(NORD0);
        theme.tab_foreground = hex(NORD4);
        theme.tab_active = hex(NORD1);
        theme.tab_active_foreground = hex(NORD6);
        theme.tab_bar = hex(NORD0);
        theme.tab_bar_segmented = hex(NORD1);

        theme.title_bar = hex(NORD0);
        theme.title_bar_border = hex(NORD2);
        theme.status_bar = hex(NORD0);
        theme.status_bar_border = hex(NORD2);
        theme.window_border = hex(NORD2);

        theme.popover = hex(NORD1);
        theme.popover_foreground = hex(NORD6);
        theme.overlay = hex(NORD0);

        theme.scrollbar = hex(NORD1);
        theme.scrollbar_thumb = hex(NORD3);
        theme.scrollbar_thumb_hover = hex(NORD4);

        theme.progress_bar = hex(NORD14);
        theme.slider_bar = hex(NORD2);
        theme.slider_thumb = hex(NORD8);

        theme.list_even = hex(NORD1);
        theme.list_hover = hex(NORD2);
        theme.list_head = hex(NORD1);
        theme.list_active = hex(NORD2);
        theme.list_active_border = hex(NORD8);
        theme.list_highlight_foreground = hex(NORD6);

        theme.table = hex(NORD0);
        theme.table_even = hex(NORD1);
        theme.table_head = hex(NORD1);
        theme.table_head_foreground = hex(NORD6);
        theme.table_hover = hex(NORD2);
        theme.table_active = hex(NORD2);
        theme.table_active_border = hex(NORD8);
        theme.table_row_border = hex(NORD2);

        theme.sidebar = hex(NORD0);
        theme.sidebar_foreground = hex(NORD6);
        theme.sidebar_border = hex(NORD2);
        theme.sidebar_accent = hex(NORD1);
        theme.sidebar_accent_foreground = hex(NORD6);
        theme.sidebar_primary = hex(NORD14);
        theme.sidebar_primary_foreground = hex(NORD0);

        theme.group_box = hex(NORD1);
        theme.group_box_foreground = hex(NORD6);
        theme.description_list_label = hex(NORD4);
        theme.description_list_label_foreground = hex(NORD6);

        theme.skeleton = hex(NORD2);
        theme.drag_border = hex(NORD8);
        theme.drop_target = hex(NORD9);

        theme.red = hex(NORD11);
        theme.red_light = hex(NORD12);
        theme.green = hex(NORD14);
        theme.green_light = hex(NORD13);
        theme.blue = hex(NORD8);
        theme.blue_light = hex(NORD7);
        theme.yellow = hex(NORD13);
        theme.yellow_light = hex(NORD12);
        theme.magenta = hex(NORD15);
        theme.magenta_light = hex(NORD12);
        theme.cyan = hex(NORD7);
        theme.cyan_light = hex(NORD8);
    });
}
