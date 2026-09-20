import { app, BrowserWindow } from "electron";
//import { createRequire } from "node:module";
import { fileURLToPath } from "node:url";
import path from "node:path";

import { type LogMessage } from "../src/log";

//const require = createRequire(import.meta.url);
const __dirname = path.dirname(fileURLToPath(import.meta.url));

// The built directory structure
//
// ├─┬─┬ dist
// │ │ └── index.html
// │ │
// │ ├─┬ dist-electron
// │ │ ├── main.js
// │ │ └── preload.mjs
// │
process.env.APP_ROOT = path.join(__dirname, "..");

// 🚧 Use ['ENV_NAME'] avoid vite:define plugin - Vite@2.x
export const VITE_DEV_SERVER_URL = process.env["VITE_DEV_SERVER_URL"];
export const MAIN_DIST = path.join(process.env.APP_ROOT, "dist-electron");
export const RENDERER_DIST = path.join(process.env.APP_ROOT, "dist");

process.env.VITE_PUBLIC = VITE_DEV_SERVER_URL
  ? path.join(process.env.APP_ROOT, "public")
  : RENDERER_DIST;

let win: BrowserWindow | null;

function createWindow() {
  win = new BrowserWindow({
    icon: path.join(process.env.VITE_PUBLIC, "icon.png"),
    webPreferences: {
      preload: path.join(__dirname, "preload.mjs"),
    },
  });

  // Test active push message to Renderer-process.
  win.webContents.on("did-finish-load", () => {
    if (win) {
      win.webContents.send("main-process-message", new Date().toLocaleString());
    }
  });

  if (VITE_DEV_SERVER_URL) {
    if (win) {
      win.loadURL(VITE_DEV_SERVER_URL);
    }
  } else {
    // win.loadFile('dist/index.html')
    if (win) {
      win.loadFile(path.join(RENDERER_DIST, "index.html"));
    }
  }

  win.webContents.on("devtools-opened", () => {
    if (!VITE_DEV_SERVER_URL) {
      // Schließt die DevTools sofort wieder, falls jemand sie im Production-Build öffnet
      if (win) {
        win.webContents.closeDevTools();
      }
    } else {
      // Erzwingt z. B. den abgedockten Modus, falls sie jemand im Dev-Build manuell öffnet
      if (win) {
        win.webContents.openDevTools({ mode: "detach" });
      }
    }
  });

  win.webContents.ipc.on("ping", (e) => {
    if (win) {
      if (e.processId === win.webContents.getProcessId()) {
        win.webContents.send("ping", "[Ping] pong");
      }
    }
  });
  win.webContents.ipc.on("log", (_, logMessage: LogMessage) => {
    let { level, message, timestamp } = logMessage;

    if (typeof message === "object" && message !== null) {
      message = JSON.stringify(message);
    } else if (typeof message !== "string") {
      message = String(message);
    }

    // Sicheres Parsen des Datums, falls es über IPC als String ankommt
    const timeString = new Date(timestamp).toLocaleString();

    // Einheitlicher Log-String mittels Template Literals
    const formattedLog = `[${timeString}][${level}] ${message}`;

    // Log in der Main-Prozess Konsole ausgeben
    console.log(formattedLog);
  });
}

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on("window-all-closed", () => {
  if (process.platform !== "darwin") {
    app.quit();
    win = null;
  }
});

app.setAboutPanelOptions({
  applicationName: "Chess Engine Battle",
  iconPath: path.join(
    process.env.VITE_PUBLIC,
    process.platform === "darwin" ? "AppIcon.icon" : "icon.png",
  ),
});

app.on("activate", () => {
  // On OS X it's common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (BrowserWindow.getAllWindows().length === 0) {
    createWindow();
  }
});

app.on("ready", () => {
  if (win) {
    win.setAppDetails({
      appId: "app.comboompunktsucht.chess-engine-battel",
      appIconPath: path.join(
        process.env.VITE_PUBLIC,
        process.platform === "darwin" ? "AppIcon.icon" : "icon.png",
      ),
    });
  }
});

app.enableSandbox();

app.whenReady().then(createWindow);
