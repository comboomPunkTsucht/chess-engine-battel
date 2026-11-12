mod bridge;
mod game;
mod moves;
mod nord;
mod piece;
mod server;
mod ui;

use std::net::SocketAddr;

use bevy::app::AppExit;
use bevy::ecs::message::MessageWriter;
use bevy::prelude::*;
use bevy::window::{MonitorSelection, PresentMode, WindowMode, WindowResolution};
use bevy_egui::{EguiContexts, EguiPlugin, EguiPrimaryContextPass, EguiPreUpdateSet};
use clap::Parser;
use crossbeam_channel::unbounded;

use crate::game::GameState;
use crate::server::ServerControl;

const WINDOW_WIDTH: f32 = 1280f32;
const WINDOW_HEIGHT: f32 = 720f32;
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

    let (tx, rx) = unbounded();
    let server = ServerControl::start(addr, tx);
    let controls = ui::controls::ControlsState {
        uri_text: format!("ws://{}", server.addr),
        ..Default::default()
    };

    App::new()
        .add_plugins(DefaultPlugins.set(WindowPlugin {
            primary_window: Some(Window {
                title: WINDOW_TITLE.into(),
                resolution: WindowResolution::new(WINDOW_WIDTH as u32, WINDOW_HEIGHT as u32),
                resize_constraints: WindowResizeConstraints {
                    min_width: WINDOW_WIDTH,
                    min_height: WINDOW_HEIGHT,
                    ..default()
                },
                ..default()
            }),
            ..default()
        }))
        .add_plugins(EguiPlugin::default())
        .insert_resource(ClearColor(nord::bevy(nord::NORD0)))
        .insert_resource(GameState::new())
        .insert_resource(ui::DockLayout(ui::initial_dock_state()))
        .insert_resource(bridge::Logs::default())
        .insert_resource(bridge::EventReceiver(rx))
        .insert_resource(server)
        .insert_resource(controls)
        .add_systems(Startup, spawn_camera)
        // Install the theme once the primary context exists (it is attached to
        // the camera in InitContexts) but strictly before BeginPass of the
        // same frame: `set_fonts` only becomes active at the start of the next
        // pass, and text styles referencing the new families would panic if
        // laid out with the still-default fonts.
        .add_systems(
            PreUpdate,
            apply_theme
                .after(EguiPreUpdateSet::InitContexts)
                .before(EguiPreUpdateSet::BeginPass),
        )
        .add_systems(Update, (bridge::receive_moves_system, window_shortcuts))
        .add_systems(EguiPrimaryContextPass, ui::render_dock_system)
        .run();
}

/// Install Nord visuals, all CaskaydiaCove fonts and the text styles.
fn apply_theme(mut contexts: EguiContexts, mut done: Local<bool>) -> Result {
    if *done {
        return Ok(());
    }
    // If the context is not attached yet, silently retry on the next frame;
    // the ordering above makes a same-frame BeginPass race impossible.
    let Ok(ctx) = contexts.ctx_mut() else {
        return Ok(());
    };
    nord::apply(ctx);
    *done = true;
    Ok(())
}

/// Esc / Ctrl-Cmd+Q quits, F11 toggles fullscreen, F10 toggles vsync.
fn window_shortcuts(
    keys: Res<ButtonInput<KeyCode>>,
    mut windows: Query<&mut Window>,
    mut exit: MessageWriter<AppExit>,
) {
    let modifier = keys.pressed(KeyCode::ControlLeft) || keys.pressed(KeyCode::SuperLeft);
    if keys.just_pressed(KeyCode::Escape) || (modifier && keys.just_pressed(KeyCode::KeyQ)) {
        exit.write(AppExit::Success);
        return;
    }

    let Ok(mut window) = windows.single_mut() else {
        return;
    };
    if keys.just_pressed(KeyCode::F11) {
        window.mode = match window.mode {
            WindowMode::Windowed => WindowMode::BorderlessFullscreen(MonitorSelection::Current),
            _ => WindowMode::Windowed,
        };
    }
    if keys.just_pressed(KeyCode::F10) {
        window.present_mode = match window.present_mode {
            PresentMode::Fifo | PresentMode::AutoVsync => PresentMode::AutoNoVsync,
            _ => PresentMode::Fifo,
        };
    }
}

/// bevy_egui attaches its primary context to the first camera it finds.
fn spawn_camera(mut commands: Commands) {
    commands.spawn(Camera2d);
}
