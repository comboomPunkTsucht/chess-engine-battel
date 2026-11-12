#![allow(dead_code)]

use std::sync::Arc;

use bevy::color::Color;
use egui::{Color32, FontData, FontFamily, FontId, Stroke, TextStyle};

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

/// Convert an `0xRRGGBBAA` palette entry into an egui color.
pub fn hex(color: u32) -> Color32 {
    let r = ((color >> 24) & 0xff) as u8;
    let g = ((color >> 16) & 0xff) as u8;
    let b = ((color >> 8) & 0xff) as u8;
    let a = (color & 0xff) as u8;
    Color32::from_rgba_unmultiplied(r, g, b, a)
}

/// Convert a palette entry by index (0..16) into an egui color.
pub fn nord(index: usize) -> Color32 {
    hex(NORD_COLORS[index])
}

/// Convert a palette entry into a Bevy color (e.g. the clear color).
pub fn bevy(color: u32) -> Color {
    Color::srgba_u8(
        ((color >> 24) & 0xff) as u8,
        ((color >> 16) & 0xff) as u8,
        ((color >> 8) & 0xff) as u8,
        (color & 0xff) as u8,
    )
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

/// Apply the complete Nord theme to an egui context: visuals, fonts and
/// text styles (run once per context).
pub fn apply(ctx: &egui::Context) {
    set_visuals(ctx);
    set_fonts(ctx);
    set_text_styles(ctx);
}

fn set_visuals(ctx: &egui::Context) {
    let mut v = egui::Visuals::dark();
    v.dark_mode = true;
    v.override_text_color = Some(hex(NORD6));
    v.panel_fill = hex(NORD0);
    v.window_fill = hex(NORD0);
    v.window_stroke = Stroke::new(1.0, hex(NORD3));
    v.extreme_bg_color = hex(NORD1);
    v.faint_bg_color = hex(NORD1);
    v.code_bg_color = hex(NORD1);
    v.hyperlink_color = hex(NORD8);
    v.warn_fg_color = hex(NORD13);
    v.error_fg_color = hex(NORD11);
    v.selection.bg_fill = hex(NORD9);
    v.selection.stroke = Stroke::new(1.0, hex(NORD6));
    v.text_cursor.stroke = Stroke::new(1.5, hex(NORD8));
    v.button_frame = true;

    v.widgets.noninteractive.bg_fill = hex(NORD0);
    v.widgets.noninteractive.weak_bg_fill = hex(NORD0);
    v.widgets.noninteractive.bg_stroke = Stroke::new(1.0, hex(NORD2));
    v.widgets.noninteractive.fg_stroke = Stroke::new(1.0, hex(NORD6));

    v.widgets.inactive.bg_fill = hex(NORD2);
    v.widgets.inactive.weak_bg_fill = hex(NORD2);
    v.widgets.inactive.bg_stroke = Stroke::new(1.0, hex(NORD3));
    v.widgets.inactive.fg_stroke = Stroke::new(1.0, hex(NORD6));

    v.widgets.hovered.bg_fill = hex(NORD3);
    v.widgets.hovered.weak_bg_fill = hex(NORD3);
    v.widgets.hovered.bg_stroke = Stroke::new(1.0, hex(NORD9));
    v.widgets.hovered.fg_stroke = Stroke::new(1.0, hex(NORD6));

    v.widgets.active.bg_fill = hex(NORD1);
    v.widgets.active.weak_bg_fill = hex(NORD1);
    v.widgets.active.bg_stroke = Stroke::new(1.0, hex(NORD9));
    v.widgets.active.fg_stroke = Stroke::new(1.0, hex(NORD6));

    v.widgets.open.bg_fill = hex(NORD3);
    v.widgets.open.weak_bg_fill = hex(NORD3);
    v.widgets.open.bg_stroke = Stroke::new(1.0, hex(NORD9));
    v.widgets.open.fg_stroke = Stroke::new(1.0, hex(NORD6));

    ctx.set_visuals(v);
}

fn set_fonts(ctx: &egui::Context) {
    let mut fonts = egui::FontDefinitions::default();
    for (name, bytes) in FONTS {
        fonts
            .font_data
            .insert((*name).to_owned(), Arc::new(FontData::from_static(bytes)));
    }

    // Glyph-fallback order per family: Regular first, then the other weights.
    // The first face holding a glyph wins, so body text stays Regular while
    // every other weight remains available behind its own family.
    const WEIGHTS: [&str; 12] = [
        "Regular",
        "SemiBold",
        "Bold",
        "Italic",
        "SemiBoldItalic",
        "BoldItalic",
        "SemiLight",
        "SemiLightItalic",
        "Light",
        "LightItalic",
        "ExtraLight",
        "ExtraLightItalic",
    ];
    let ordered = |prefix: &str| -> Vec<String> {
        WEIGHTS
            .iter()
            .map(|weight| format!("{prefix}{weight}"))
            .collect()
    };

    // Proportional UI text uses the Propo variant; logs/code use Mono.
    // egui's emoji fonts stay as final glyph fallback.
    if let Some(list) = fonts.families.get_mut(&FontFamily::Proportional) {
        let mut faces = ordered("CaskaydiaCovePropo-");
        faces.append(list);
        *list = faces;
    }
    if let Some(list) = fonts.families.get_mut(&FontFamily::Monospace) {
        let mut faces = ordered("CaskaydiaCoveMono-");
        faces.append(list);
        *list = faces;
    }

    // Every single face is also selectable on its own via FontId (each one
    // falls back to the Regular of its own variant for missing glyphs).
    for (name, _) in FONTS {
        let regular = if name.contains("Mono") {
            "CaskaydiaCoveMono-Regular"
        } else if name.contains("Propo") {
            "CaskaydiaCovePropo-Regular"
        } else {
            "CaskaydiaCove-Regular"
        };
        let mut faces = vec![name.to_owned()];
        if name != regular {
            faces.push(regular.to_owned());
        }
        fonts
            .families
            .insert(FontFamily::Name(name.into()), faces);
    }

    ctx.set_fonts(fonts);
}

fn set_text_styles(ctx: &egui::Context) {
    ctx.global_style_mut(|style| {
        // Headings render in the real Bold face, buttons in SemiBold.
        let heading_size = style
            .text_styles
            .get(&TextStyle::Heading)
            .map_or(22.0, |font_id| font_id.size);
        let button_size = style
            .text_styles
            .get(&TextStyle::Button)
            .map_or(14.0, |font_id| font_id.size);
        style.text_styles.insert(
            TextStyle::Heading,
            FontId::new(
                heading_size,
                FontFamily::Name("CaskaydiaCovePropo-Bold".into()),
            ),
        );
        style.text_styles.insert(
            TextStyle::Button,
            FontId::new(
                button_size,
                FontFamily::Name("CaskaydiaCovePropo-SemiBold".into()),
            ),
        );
        // Body / Small / Monospace already resolve through the Proportional /
        // Monospace families set up in `set_fonts`.
    });
}
