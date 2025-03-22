use raylib::prelude::*;
use std::collections::HashMap;

pub fn display() {
    let screen_width = 600;
    let screen_height = 600;
    let (mut rl, thread) = raylib::init()
        .size(screen_width, screen_height)
        .title("Aquarium")
        .build();

    let bg_image_path = "assets/aqua.png";
    let bg_texture = rl.load_texture(&thread, bg_image_path).expect(&format!(
        "Impossible de charger l'image de background de l'aquarium : {}",
        bg_image_path
    ));

    let fish_names = ["fish1.png", "fish2.png", "default.png"];

    let mut map_fish_texture: HashMap<String, Texture2D> = HashMap::new();

    for &fish in &fish_names {
        let texture = rl
            .load_texture(&thread, &format!("assets/{}", fish))
            .expect(&format!("Impossible de charger le poisson : {}", fish));
        map_fish_texture.insert(fish.to_string(), texture);
    }
    let texture_default = rl
        .load_texture(&thread, "assets/default.png")
        .expect(&format!("Impossible de charger le poisson : {}", "default"));

    let bg_source = Rectangle::new(
        0.0,
        0.0,
        bg_texture.width() as f32,
        bg_texture.height() as f32,
    );
    let bg_dest = Rectangle::new(0.0, 0.0, screen_width as f32, screen_height as f32);
    let origin = Vector2::new(0.0, 0.0);

    while !rl.window_should_close() {
        let mut d = rl.begin_drawing(&thread);
        let texture = find_right_texture("fish2".to_owned(), &map_fish_texture, &texture_default);
        d.draw_texture_pro(&bg_texture, bg_source, bg_dest, origin, 0.0, Color::WHITE);
        display_fish(&mut d, texture, 100.0, 150.0, 80, 40, 0.0);
        display_fish(&mut d, texture, 300.0, 200.0, 100, 50, 45.0);
        display_fish(&mut d, texture, 500.0, 100.0, 90, 45, 90.0);
    }
}

fn display_fish(
    d: &mut RaylibDrawHandle,
    texture: &Texture2D,
    x: f32,
    y: f32,
    w: i32,
    h: i32,
    rotation: f32,
) {
    let scale_x = w as f32 / texture.width() as f32;
    let scale_y = h as f32 / texture.height() as f32;

    d.draw_texture_ex(
        texture,
        Vector2::new(x, y),
        rotation,
        scale_x.min(scale_y),
        Color::WHITE,
    );
}

fn find_right_texture<'a>(
    name_fish: String,
    map_fish_texture: &'a HashMap<String, Texture2D>,
    default: &'a Texture2D,
) -> &'a Texture2D {
    let fish_path = format!("{}.png", name_fish);
    map_fish_texture.get(&fish_path).unwrap_or(default)
}
