export enum LogLevel {
  INFO = "INFO",
  WARN = "WARN",
  ERROR = "ERROR",
  DEBUG = "DEBUG",
}
export interface LogMessage {
  level: LogLevel;
  message: string | JSON | object;
  timestamp: Date;
}

export function log(level: LogLevel, message: string | JSON | object) {
  window.ipcRenderer?.send("log", {
    level,
    message,
    timestamp: new Date(),
  });
}
