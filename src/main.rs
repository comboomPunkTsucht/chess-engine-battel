mod bridge;
mod game;
mod moves;
mod nord;
mod piece;
mod server;
mod ui;

use std::net::SocketAddr;

use clap::Parser;
use crossbeam_channel::unbounded;
use gpui_kit::{
    App, AppContext as _, KeyBinding, TitlebarOptions, WindowBounds, WindowOptions, open_window,
    px, size,
};
use gpui_kit::component::input::InputState;

use crate::server::ServerControl;

const WINDOW_WIDTH: f32 = 1280.0;
const WINDOW_HEIGHT: f32 = 720.0;
const WINDOW_TITLE: &str = "Chess Engine Battle";

gpui_kit::actions!(app, [Quit, ToggleFullscreen]);

#[derive(Parser)]
#[command(
    name = "chess-engine-battel",
    version,
    about = "Chess Engine Battle visualizer"
)]
struct Args {
    /// Address the websocket server listens on
    #[arg(long = "server-address", short = 'a', default_value = "0.0.0.0")]
    server_address: String,

    /// Port the websocket server listens on
    #[arg(long = "server-port", short = 'p', default_value_t = 14318)]
    server_port: u16,
}

fn main() {
    let args = Args::parse();

    let addr: SocketAddr = format!("{}:{}", args.server_address, args.server_port)
        .parse()
        .unwrap_or_else(|err| {
            eprintln!("invalid server address: {err}");
            std::process::exit(1);
        });

    let (tx, rx) = unbounded();
    let server = ServerControl::start(addr, tx);
    eprintln!("[server] Websocket server URI: ws://{}", server.addr);

    gpui_kit::application().run(move |cx: &mut App| {
        gpui_kit::init(cx);
        nord::register_fonts(cx);
        nord::apply(cx);

        cx.bind_keys([
            KeyBinding::new("escape", Quit, None),
            KeyBinding::new("cmd-q", Quit, None),
            KeyBinding::new("ctrl-q", Quit, None),
            KeyBinding::new("f11", ToggleFullscreen, None),
        ]);
        cx.on_action(|_: &Quit, cx| cx.quit());
        cx.on_action(|_: &ToggleFullscreen, cx| {
            if let Some(window) = cx.active_window() {
                let _ = window.update(cx, |_, window, _| window.toggle_fullscreen());
            }
        });
        cx.on_window_closed(|cx, _| {
            if cx.windows().is_empty() {
                cx.quit();
            }
        })
        .detach();

        let events = bridge::EventReceiver(rx);
        open_window(
            WindowOptions {
                window_bounds: Some(WindowBounds::centered(
                    size(px(WINDOW_WIDTH), px(WINDOW_HEIGHT)),
                    cx,
                )),
                titlebar: Some(TitlebarOptions {
                    title: Some(WINDOW_TITLE.into()),
                    ..Default::default()
                }),
                window_min_size: Some(size(px(WINDOW_WIDTH), px(WINDOW_HEIGHT))),
                ..Default::default()
            },
            cx,
            move |window, cx| {
                let uri = format!("ws://{}", server.addr);
                let uri_input =
                    cx.new(|cx| InputState::new(window, cx).default_value(uri));
                cx.new(|cx| ui::BattleView::new(events, server, uri_input, cx))
            },
        )
        .expect("failed to open window");

        cx.activate(true);
    });
}
