use raylib::prelude::*;


pub fn display() {
    let screen_width = 600;
    let screen_height = 600;
    let (mut rl, thread) = raylib::init().size(screen_width, screen_height).title("Aquarium").build();

    let bg_image_path = "aqua.png"; 

    let bg_texture: Texture2D = rl
        .load_texture(&thread, bg_image_path)
        .expect(&format!("Impossible de charger l'image de background : {}", bg_image_path));

    let bg_source = Rectangle::new(0.0, 0.0, bg_texture.width() as f32, bg_texture.height() as f32);
    let bg_dest = Rectangle::new(0.0, 0.0, screen_width as f32, screen_height as f32);
    let origin = Vector2::new(0.0, 0.0);

    while !rl.window_should_close() {
        let mut d = rl.begin_drawing(&thread); 
        d.draw_texture_pro(&bg_texture, bg_source, bg_dest, origin, 0.0, Color::WHITE);
        display_fish(&mut d, &thread, "logo", 100.0, 150.0, 80, 40, 0.0);
        display_fish(&mut d, &thread, "logo", 300.0, 200.0, 100, 50, 45.0); 
        display_fish(&mut d, &thread, "logo", 500.0, 100.0, 90, 45, 90.0);
    }
}

/*
* @param: d, le handle de dessin Raylib
* @param: thread, le thread Raylib
* @param: fish_name, le nom du fichier de poisson (ex: "fish1", "fish2", "fish3")
* @param: x, coordonnée x
* @param: y, coordonnée y
* @param: w, largeur du poisson
* @param: h, hauteur du poisson
* @param: rotation, angle de rotation du poisson
*/
fn display_fish(d: &mut RaylibDrawHandle, thread: &RaylibThread, fish_name: &str, x: f32, y: f32, w: i32, h: i32, rotation: f32) {
    let fish_path = format!("{}.png", fish_name);  

    let texture: Texture2D = d.load_texture(thread, &fish_path)
        .expect(&format!("Impossible de charger l'image du poisson : {}", fish_path));

    let scale_x = w as f32 / texture.width() as f32;
    let scale_y = h as f32 / texture.height() as f32;

    d.draw_texture_ex(
        &texture,
        Vector2::new(x, y),
        rotation,
        scale_x.min(scale_y),
        Color::WHITE,          
    );
}
