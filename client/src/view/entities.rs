#[derive(Debug, Clone)]
pub struct Fish {
    pub name: String,
    pub target_x: f32,
    pub target_y: f32,
    pub timestamp: std::time::Duration,
    pub target_time: f64,
    pub size_w: f32,
    pub size_h: f32,
}

impl Fish {
    pub fn new(
        name: impl Into<String>,
        target_x: f32,
        target_y: f32,
        size_w: f32,
        size_h: f32,
        target_time: f64,
        timestamp: std::time::Duration,
    ) -> Self {
        Self {
            name: name.into(),
            target_x,
            target_y,
            size_w,
            size_h,
            timestamp: timestamp,
            target_time,
        }
    }
}
