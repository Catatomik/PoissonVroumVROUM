use std::collections::HashMap;
use raylib::prelude::*;

pub fn display() {
    let screen_width = 600;
    let screen_height = 600;
    let (mut rl, thread) = raylib::init().size(screen_width, screen_height).title("Aquarium").build();

    let bg_image_path = "aqua.png"; 
    let bg_texture = rl
        .load_texture(&thread, bg_image_path)
        .expect(&format!("Impossible de charger l'image de background de l'aquarium : {}", bg_image_path));

    let fish_names = ["fish1.png", "fish2.png", "default.png"];
    
    let mut map_fish_texture: HashMap<String, Texture2D> = HashMap::new();

    for &fish in &fish_names {
        let texture = rl
            .load_texture(&thread, fish)
            .expect(&format!("Impossible de charger le poisson : {}", fish));
        
        map_fish_texture.insert(fish.to_string(), texture);
    }

    let bg_source = Rectangle::new(0.0, 0.0, bg_texture.width() as f32, bg_texture.height() as f32);
    let bg_dest = Rectangle::new(0.0, 0.0, screen_width as f32, screen_height as f32);
    let origin = Vector2::new(0.0, 0.0);

    while !rl.window_should_close() {
        let mut d = rl.begin_drawing(&thread); 
        d.draw_texture_pro(&bg_texture, bg_source, bg_dest, origin, 0.0, Color::WHITE);
        display_fish(&mut d, &map_fish_texture, "fish1", 100.0, 150.0, 80, 40, 0.0);
        display_fish(&mut d, &map_fish_texture, "fish2", 300.0, 200.0, 100, 50, 45.0); 
        display_fish(&mut d, &map_fish_texture, "fish3", 500.0, 100.0, 90, 45, 90.0);

    }
}

fn display_fish(
    d: &mut RaylibDrawHandle,
    map_fish_texture: &HashMap<String, Texture2D>, 
    fish_name: &str, 
    x: f32, 
    y: f32, 
    w: i32, 
    h: i32, 
    rotation: f32
) {
    let fish_path = format!("{}.png", fish_name); 
    
    // si pas de correspondance ds Hashmap, prendre "default.png", 
    let texture = map_fish_texture.get(&fish_path)
        .or_else(|| {
            map_fish_texture.get("default.png")
        });

    let texture = match texture {
        Some(tex) => tex,
        None => {
            println!("Aucune texture par défaut disponible, impossible d'afficher {}", fish_path);
            return;
        }
    };

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
