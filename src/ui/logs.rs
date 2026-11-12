use bevy_egui::egui;

use crate::bridge::Logs;

pub fn show_logs(ui: &mut egui::Ui, logs: &Logs) {
    egui::ScrollArea::vertical()
        .stick_to_bottom(true)
        .auto_shrink([false, false])
        .show(ui, |ui| {
            if logs.0.is_empty() {
                ui.label("No log entries yet.");
            }
            for line in &logs.0 {
                ui.monospace(line);
            }
        });
}
