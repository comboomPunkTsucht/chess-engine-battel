use super::super::{WINDOW_TITLE,Quit, ToggleFullscreen};
use gpui_kit::*;

pub(super) fn build_app_menus() -> Vec<Menu> {
    vec![
        Menu::new(WINDOW_TITLE).items([
            MenuItem::os_submenu("Services", SystemMenuType::Services),
            MenuItem::separator(),
            MenuItem::action(format!("Quit {}",WINDOW_TITLE), Quit),
        ]),
        Menu::new("View").items([
          MenuItem::action("Toggle Fullscreen",ToggleFullscreen)
        ])
    ]
}

pub(super) fn install_menus(cx: &mut App) {
    cx.set_menus(build_app_menus());
    gpui_kit::base::GlobalState::global_mut(cx)
        .set_app_menus(build_app_menus().into_iter().map(|m| m.owned()).collect());
}
