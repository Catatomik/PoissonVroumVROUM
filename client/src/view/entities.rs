use raylib::texture::Texture2D;

use std::fmt;

#[derive(Debug, Clone)]
pub struct Fish<'a> {
    pub x: f32,
    pub y: f32,
    pub width: f32,
    pub height: f32,
    pub texture: &'a Texture2D,
    pub is_started: bool,
}

impl<'a> Fish<'a> {
    pub fn new(
        target_x: f32,
        target_y: f32,
        width: f32,
        height: f32,
        texture: &'a Texture2D,
        is_started: bool,
    ) -> Self {
        Self {
            // Default position : if it was not already displayed, spawn it at its target
            x: target_x,
            y: target_y,
            width,
            height,
            texture,
            is_started,
        }
    }
}

impl fmt::Display for Fish {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{} ", self.name)?;
        write!(f, "at {}x{}", self.target_x, self.target_y)?;
        writeln!(f, "{}x{} ", self.size_w, self.size_h)
    }
}
