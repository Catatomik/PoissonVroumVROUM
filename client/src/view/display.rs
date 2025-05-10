use super::entities::Fish;
use crate::network::api::ViewerConfig;
use raylib::prelude::*;
use std::{
    collections::HashMap,
    sync::{Arc, Mutex},
    time::Instant,
};

use std::path::PathBuf;

pub fn display(new_fish_list: Arc<Mutex<Vec<Fish>>>, viewer_config: ViewerConfig, path: PathBuf) {
    let (mut rl, thread) = raylib::init()
        .size(viewer_config.width as i32, viewer_config.height as i32)
        .title("Aquarium")
        .log_level(TraceLogLevel::LOG_WARNING)
        .build();
    let path_ressources = path.to_str().expect("Chemin invalide");
    let bg_image_path = format!("{}/aqua.png", path_ressources);
    let bg_texture = rl
        .load_texture(&thread, &bg_image_path)
        .unwrap_or_else(|_| {
            panic!(
                "Impossible de charger l'image de background de l'aquarium : {}",
                bg_image_path
            )
        });

    let fish_names = ["fish1.png", "fish2.png", "default.png"];

    let mut map_fish_texture: HashMap<String, Texture2D> = HashMap::new();
    let mut current_fish_list: HashMap<String, Fish> = HashMap::new();

    for &fish in &fish_names {
        let fish_path = format!("{}/{}", path_ressources, fish);
        let texture = rl
            .load_texture(&thread, &fish_path)
            .unwrap_or_else(|_| panic!("Impossible de charger le poisson : {}", fish));
        map_fish_texture.insert(fish.to_string(), texture);
    }
    let default_path: String = format!("{}/default.png", path_ressources);

    let texture_default = rl
        .load_texture(&thread, &default_path)
        .unwrap_or_else(|_| panic!("Impossible de charger le poisson : {}", "default"));

    let bg_source: Rectangle = Rectangle::new(
        0.0,
        0.0,
        bg_texture.width() as f32,
        bg_texture.height() as f32,
    );
    let bg_dest = Rectangle::new(
        0.0,
        0.0,
        viewer_config.width as f32,
        viewer_config.height as f32,
    );
    let origin = Vector2::new(0.0, 0.0);

    while !rl.window_should_close() {
        let dt = rl.get_frame_time();
        rl.set_target_fps(60);
        let mut d = rl.begin_drawing(&thread);
        d.draw_texture_pro(&bg_texture, bg_source, bg_dest, origin, 0.0, Color::WHITE);
        find_next_current_positions(&mut current_fish_list, &new_fish_list, dt, &viewer_config);
        for (name, fish) in current_fish_list.iter() {
            let texture = find_right_texture(name, &map_fish_texture, &texture_default);
            display_fish(&mut d, texture, fish, 0.0);
        }
    }
}

// function which takes the current version of the fish and the new version of the fish and the dt(delays of a frame)
//calculate the new position of the fish
fn find_right_position(
    current_fish: &mut Fish,
    new_fish: &Fish,
    dt: f32,
    viewer_config: &ViewerConfig,
) {
    let now = Instant::now();

    if new_fish.timestamp <= now {
        current_fish.target_x = (new_fish.target_x * viewer_config.width as f32) / 100.0;
        current_fish.target_y = (new_fish.target_y * viewer_config.height as f32) / 100.0;
    } else {
        let time_remain = new_fish.timestamp.duration_since(now);
        let duration = time_remain.as_secs_f64() * 1.0;
        let v_x = ((new_fish.target_x * viewer_config.width as f32) / 100.0 - current_fish.target_x)
            as f64
            / duration;
        let v_y = ((new_fish.target_y * viewer_config.height as f32) / 100.0
            - current_fish.target_y) as f64
            / duration;

        let x =
            current_fish.target_x as f64 + (v_x * dt as f64) / 100.0 * viewer_config.width as f64;
        let y =
            current_fish.target_y as f64 + (v_y * dt as f64) / 100.0 * viewer_config.width as f64;
        current_fish.target_x = x as f32;
        current_fish.target_y = y as f32;

        current_fish.size_h = (new_fish.size_h * viewer_config.height as f32) / 100.0;
        current_fish.size_w = (new_fish.size_w * viewer_config.width as f32) / 100.0;
    }
}

// function which takes the list of the current fishes and one of the new one and calculate the new current positions of the fishes
fn find_next_current_positions(
    current_fish_list: &mut HashMap<String, Fish>,
    new_fish_list: &Arc<Mutex<Vec<Fish>>>,
    dt: f32,
    viewer_config: &ViewerConfig,
) {
    let new_fish_list_guard = new_fish_list.lock().unwrap();

    for new_fish in new_fish_list_guard.iter() {
        if !current_fish_list.contains_key(&new_fish.name) {
            let mut fish = new_fish.clone();
            fish.size_h = (new_fish.size_h * viewer_config.height as f32) / 100.0;
            fish.size_w = (new_fish.size_w * viewer_config.width as f32) / 100.0;
            fish.target_x = (new_fish.target_x * viewer_config.width as f32) / 100.0;
            fish.target_y = (new_fish.target_y * viewer_config.height as f32) / 100.0;
            current_fish_list.insert(new_fish.name.clone(), fish);
        } else if let Some(current_fish) = current_fish_list.get_mut(&new_fish.name) {
            find_right_position(current_fish, new_fish, dt, viewer_config);
        }
    }

    // on checke les fish qui ne sont plus envoyés par le serveur ie ont été supprimés
    current_fish_list.retain(|_, fish| {
        // Keep it if it's found in the new list
        new_fish_list_guard
            .iter()
            .any(|new_fish| new_fish.name == fish.name)
    });
}

//display a fish with a texture, a fish and the rotation
fn display_fish(d: &mut RaylibDrawHandle, texture: &Texture2D, fish: &Fish, rotation: f32) {
    let x = fish.target_x;
    let y = fish.target_y;
    let h = fish.size_h;
    let w = fish.size_w;
    let scale_x = w / texture.width() as f32;
    let scale_y = h / texture.height() as f32;

    d.draw_texture_ex(
        texture,
        Vector2::new(x, y),
        rotation,
        scale_x.min(scale_y),
        Color::WHITE,
    );
}

fn find_right_texture<'a>(
    name_fish: &str,
    map_fish_texture: &'a HashMap<String, Texture2D>,
    default: &'a Texture2D,
) -> &'a Texture2D {
    let name_f = name_fish.split('_').next().unwrap_or("default");
    let fish_path = format!("{}.png", name_f);
    map_fish_texture.get(&fish_path).unwrap_or(default)
}
