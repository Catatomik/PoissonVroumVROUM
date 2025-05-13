use std::fmt;

#[derive(Debug, Clone)]
pub struct Fish {
    pub x: f32,
    pub y: f32,
    pub width: f32,
    pub height: f32,
    pub is_started: bool,
}

impl Fish {
    pub fn new(target_x: f32, target_y: f32, width: f32, height: f32, is_started: bool) -> Self {
        Self {
            // Default position : if it was not already displayed, spawn it at its target
            x: target_x,
            y: target_y,
            width,
            height,
            is_started,
        }
    }
}

impl fmt::Display for Fish {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> fmt::Result {
        write!(
            f,
            "at {}x{},{}x{} {}",
            self.x,
            self.y,
            self.width,
            self.height,
            if self.is_started {
                "started"
            } else {
                "notStarted"
            }
        )
    }
}
