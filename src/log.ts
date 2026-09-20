import { z } from "zod";

const LogLevelSchema = z.enum(["INFO", "WARN", "ERROR", "DEBUG"]);

type LogLevel = z.infer<typeof LogLevelSchema>;

export const LogMessageSchema = z.object({
  level: z.enum(LogLevelSchema.options),
  message: z.union([z.string(), z.json(), z.object(z.any())]),
  timestamp: z.date(),
});

export type LogMessage = z.infer<typeof LogMessageSchema>;

export function log(level: LogLevel, message: string | JSON | object) {
  let timestamp = new Date();

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

  window.ipcRenderer?.send("log", { level, message, timestamp } as LogMessage);
}
