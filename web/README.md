# Chess Web (WASM)

A minimal web frontend for the chess engine, compiled to WebAssembly via Emscripten.

## Prerequisites

Install [Emscripten](https://emscripten.org/docs/getting_started/downloads.html):

```bash
# Clone and install EMSDK
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

## Build

```bash
cd web
make
```

This produces `chess.js` and `chess.wasm` in the `web` directory.

**Important:** You must serve the app over HTTP (not by opening the HTML file directly). Browsers block loading .wasm from `file://`:

```bash
cd web
# Python
python3 -m http.server 8000

# Or Node
npx serve -p 8000
```

Then open http://localhost:8000

## GitHub Pages

The repo includes a GitHub Actions workflow that builds and deploys to GitHub Pages on push to `main`/`master`. Enable GitHub Pages in repo Settings → Pages → Source: GitHub Actions.
