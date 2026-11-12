mod game;
mod moves;
mod nord;
mod piece;
mod server;
mod ui;

use std::borrow::Cow;
use std::net::SocketAddr;

use clap::Parser;
use gpui::{App, AppContext, Application, TitlebarOptions, WindowBounds, WindowOptions, px, size};
use tokio::sync::mpsc::unbounded_channel;

use ui::BattleView;

const WINDOW_WIDTH: f32 = 1280.0;
const WINDOW_HEIGHT: f32 = 720.0;
const WINDOW_TITLE: &str = "Chess Engine Battle";

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

    let server_uri = format!("ws://{addr}");
    eprintln!("[server] Websocket server URI: {server_uri}");

    let (move_tx, move_rx) = unbounded_channel();
    let _server = server::spawn_engine_server(addr, move_tx);

    Application::new().run(move |cx: &mut App| {
        let fonts: Vec<Cow<'static, [u8]>> = vec![
            Cow::Borrowed(
                include_bytes!("../assets/CaskaydiaCoveNerdFontPropo-Regular.ttf").as_slice(),
            ),
            Cow::Borrowed(
                include_bytes!("../assets/CaskaydiaCoveNerdFontPropo-Bold.ttf").as_slice(),
            ),
            Cow::Borrowed(
                include_bytes!("../assets/CaskaydiaCoveNerdFontPropo-Italic.ttf").as_slice(),
            ),
        ];
        if let Err(err) = cx.text_system().add_fonts(fonts) {
            eprintln!("[ui] failed to register fonts: {err}");
        }

        cx.on_window_closed(|cx| {
            if cx.windows().is_empty() {
                cx.quit();
            }
        })
        .detach();

        cx.open_window(
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
            move |_, cx| cx.new(|cx| BattleView::new(move_rx, server_uri, cx)),
        )
        .expect("failed to open window");

        cx.activate(true);
    });
}
