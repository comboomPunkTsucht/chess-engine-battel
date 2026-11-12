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

# --- package ---

# Build + produce Chess Engine Battle.app (debug profile)
bundle:
    cargo bundle

# Build + produce .app (release profile)
bundle-release:
    cargo bundle --release

# Open the packaged app (debug bundle)
open: bundle
    open "target/debug/bundle/dmg/Chess Engine Battle.app"

# Open the packaged app (release bundle)
open-release: bundle-release
    open "target/release/bundle/dmg/Chess Engine Battle.app"

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
    rm -rf "target/debug/bundle" "target/release/bundle"
