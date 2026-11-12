# Chess Engine Battle — Rust (bevy + bevy_egui + axum)

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

# --- quality ---
check:
    cargo check

test:
    cargo test

clippy:
    cargo clippy --all-targets

# --- utility ---
kill:
    pkill -f "target/.*/chess-engine-battel" || true
    lsof -ti :14318 | xargs kill -9 2>/dev/null || true

clean:
    cargo clean
    rm -rf target/debug/bundle target/release/bundle