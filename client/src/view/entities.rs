use std::fmt;

#[derive(Debug, Clone)]
pub struct Fish {
    pub name: String,
    pub target_x: f32,
    pub target_y: f32,
    pub timestamp: std::time::Instant,
    pub size_w: f32,
    pub size_h: f32,
    pub is_started: bool,
}

impl Fish {
    pub fn new(
        name: impl Into<String>,
        target_x: f32,
        target_y: f32,
        size_w: f32,
        size_h: f32,
        timestamp: std::time::Instant,
        is_started: bool,
    ) -> Self {
        Self {
            name: name.into(),
            target_x,
            target_y,
            size_w,
            size_h,
            timestamp,
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
