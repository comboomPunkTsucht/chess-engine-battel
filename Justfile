# Chess Engine Battle — Rust (bevy + bevy_egui + axum)

default: build

# tauri CLI: prefer standalone `tauri` (npm), fall back to `cargo tauri`
tauri := `command -v tauri >/dev/null 2>&1 && echo tauri || echo "cargo tauri"`

# --- build / run ---

build:
    cargo build

release:
    cargo build --release

run *args:
    cargo run -- {{args}}

run-release *args:
    cargo run --release -- {{args}}

# --- package (tauri-cli: macOS .app/.dmg, Linux .deb/.AppImage, Windows .msi/.nsis) ---

# Build + bundle (debug profile, host platform defaults)
bundle:
    cargo build
    {{tauri}} bundle --ci --no-binary-patching

# Build + bundle (release profile, host platform defaults)
bundle-release:
    cargo build --release
    {{tauri}} bundle --ci --no-binary-patching

# Explicit formats (pass after -- e.g. `just bundle-fmt -- -b deb`)
bundle-fmt *args:
    cargo build --release
    {{tauri}} bundle --ci --no-binary-patching {{args}}

# Open the packaged app (debug, macOS only)
open: bundle
    open "target/debug/bundle/macos/Chess Engine Battle.app"

# Open the packaged app (release, macOS only)
open-release: bundle-release
    open "target/release/bundle/macos/Chess Engine Battle.app"

# --- quality ---

check:
    cargo check

test:
    cargo test

clippy:
    cargo clippy --all-targets

# --- utility ---

# Kill any running instance and free the default WS port
kill:
    pkill -f "target/.*/chess-engine-battel" || true
    lsof -ti :14318 | xargs kill -9 2>/dev/null || true

clean:
    cargo clean
    rm -rf target/debug/bundle target/release/bundle
