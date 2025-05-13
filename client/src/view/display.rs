use super::entities;
use crate::network::{api::ViewerConfig, protocol};
use raylib::prelude::*;
use std::{
    collections::HashMap,
    sync::{Arc, Mutex},
    time::Instant,
};

use std::path::PathBuf;

static EXIT_REQUESTED: Mutex<bool> = Mutex::new(false);
static EXITED: Mutex<()> = Mutex::new(());

/// Starts displaying
pub fn display(
    target_fish_list: Arc<Mutex<Vec<protocol::Fish>>>,
    displayed_fish_map: Arc<Mutex<HashMap<String, entities::Fish>>>,
    viewer_config: ViewerConfig,
    path: PathBuf,
) {
    let _unused = EXITED.lock().expect("Exit lock acquire on display startup");

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

    let mut fish_texture_map: HashMap<String, Texture2D> = HashMap::new();

    for &fish in &fish_names {
        let fish_path = format!("{}/{}", path_ressources, fish);
        let texture = rl
            .load_texture(&thread, &fish_path)
            .unwrap_or_else(|_| panic!("Impossible de charger le poisson : {}", fish));
        fish_texture_map.insert(fish.to_string(), texture);
    }

    let default_path: String = format!("{}/default.png", path_ressources);
    let fish_texture_default = rl
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

    rl.set_target_fps(60);
    while !rl.window_should_close() && !*EXIT_REQUESTED.lock().unwrap() {
        let dt = rl.get_frame_time();
        let mut d = rl.begin_drawing(&thread);
        d.draw_texture_pro(&bg_texture, bg_source, bg_dest, origin, 0.0, Color::WHITE);

        let mut displayed_fish_map_lock = displayed_fish_map.lock().unwrap();

        refresh_fishes(
            &mut displayed_fish_map_lock,
            &target_fish_list,
            dt,
            &viewer_config,
        );
        for (fish_name, fish) in displayed_fish_map_lock.iter() {
            display_fish(
                &mut d,
                (&fish_texture_map, &fish_texture_default),
                fish_name,
                fish,
                0.0,
            );
        }
    }

    println!("Exiting display");
}

// function which takes the current version of the fish and the new version of the fish and the dt(delays of a frame)
//calculate the new position of the fish
fn move_fish(
    current_fish: &mut entities::Fish,
    target_fish: &protocol::Fish,
    dt: f32,
    viewer_config: &ViewerConfig,
) {
    let now = Instant::now();

    if target_fish.arriving_at <= now {
        // It has reached its destination
        current_fish.x = target_fish.target_x / 100.0 * viewer_config.width as f32;
        current_fish.y = target_fish.target_y / 100.0 * viewer_config.height as f32;
    } else {
        // It's still going to its destination, so move it
        let duration = target_fish.arriving_at.duration_since(now).as_secs_f32();

        let v_x =
            (target_fish.target_x / 100.0 * viewer_config.width as f32 - current_fish.x) / duration;
        let v_y = (target_fish.target_y / 100.0 * viewer_config.height as f32 - current_fish.y)
            / duration;

        println!("speed {}+{}={} ; dt {}", v_x, v_y, v_x + v_y, dt);

        current_fish.x += v_x * dt;
        current_fish.y += v_y * dt;
    }
}

// function which takes the list of the current fishes and one of the new one and calculate the new current positions of the fishes
fn refresh_fishes(
    displayed_fish_map: &mut HashMap<String, entities::Fish>,
    target_fish_list: &Arc<Mutex<Vec<protocol::Fish>>>,
    dt: f32,
    viewer_config: &ViewerConfig,
) {
    let target_fish_list_guard = target_fish_list.lock().unwrap();

    for target_fish in target_fish_list_guard.iter() {
        if !displayed_fish_map.contains_key(&target_fish.name) {
            // New fish received, add it to our fishes
            displayed_fish_map.insert(
                target_fish.name.clone(),
                entities::Fish::new(
                    (target_fish.target_x * viewer_config.width as f32) / 100.0,
                    (target_fish.target_y * viewer_config.height as f32) / 100.0,
                    (target_fish.width * viewer_config.width as f32) / 100.0,
                    (target_fish.height * viewer_config.height as f32) / 100.0,
                    false,
                ),
            );
        } else if let Some(current_fish) = displayed_fish_map.get_mut(&target_fish.name) {
            move_fish(current_fish, target_fish, dt, viewer_config);
        }
    }

    // on checke les fish qui ne sont plus envoyés par le serveur ie ont été supprimés
    displayed_fish_map.retain(|current_fish_name, _| {
        // Keep it if it's found in the new list
        target_fish_list_guard
            .iter()
            .any(|target_fish| target_fish.name == *current_fish_name)
    });
}

//display a fish with a texture, a fish and the rotation
fn display_fish(
    d: &mut RaylibDrawHandle,
    fish_textures: (&HashMap<String, Texture2D>, &Texture2D),
    fish_name: &str,
    fish: &entities::Fish,
    rotation: f32,
) {
    let texture = find_right_texture(fish_name, fish_textures);
    let scale_x = fish.width / texture.width() as f32;
    let scale_y = fish.height / texture.height() as f32;

    d.draw_texture_ex(
        texture,
        Vector2::new(fish.x, fish.y),
        rotation,
        scale_x.min(scale_y),
        Color::WHITE,
    );
}

fn find_right_texture<'a>(
    name_fish: &str,
    fish_textures: (&'a HashMap<String, Texture2D>, &'a Texture2D),
) -> &'a Texture2D {
    let name_f = name_fish.split('_').next().unwrap_or("default");
    let fish_path = format!("{}.png", name_f);
    fish_textures.0.get(&fish_path).unwrap_or(fish_textures.1)
}

pub fn exit() {
    {
        let mut exit_requested_lock = EXIT_REQUESTED.lock().unwrap();
        *exit_requested_lock = true;
    }
    // Wait for EXITED lock to be unlocked
    let _unused = EXITED.lock();
}
