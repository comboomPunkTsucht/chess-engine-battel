# Chess Engine Battle — Rust (gpui + axum)

default: build

# --- build / run ---

build:
    cargo build

release:
    cargo build --release

run *args:
    cargo run -- {{args}}

run-release *args:
    cargo run --release -- {{args}}

# --- package (cargo-bundle: macOS .app/.dmg, Linux .deb/.AppImage, Windows .msi) ---

# Build + bundle (debug profile, host platform defaults)
bundle:
    cargo bundle

# Build + bundle (release profile, host platform defaults)
bundle-release:
    cargo bundle --release

# Explicit formats (pass after -- e.g. `just bundle-fmt -- -f deb`)
bundle-fmt *args:
    cargo bundle {{args}}

# Open the packaged app (debug, macOS only)
open: bundle
    open "target/debug/bundle/osx/Chess Engine Battle.app"

# Open the packaged app (release, macOS only)
open-release: bundle-release
    open "target/release/bundle/osx/Chess Engine Battle.app"

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
