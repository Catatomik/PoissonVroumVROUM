use std::io;

use crate::network::api::{FishApi, Transport};
use crate::network::protocol::{ClientPacket, ServerPacket};

pub fn command_loop<T: Transport<ClientPacket, ServerPacket> + Send + 'static>(
    mut api_fish: FishApi<T>,
) {
    println!("\nWelcome to PoissonVroumVROUM");
    loop {
        let mut full_command = String::new();

        eprint!("\x1b[32m$ ");
        io::stdin()
            .read_line(&mut full_command)
            .expect("Failed to read line");
        eprint!("\x1b[0m");

        let full_command = full_command.trim();
        let mut command_parts = full_command.split_whitespace();
        let main_command = match command_parts.next() {
            Some(content) => content,
            _ => continue,
        };

        match main_command {
            "status" => status(),
            "help" => help(),
            "addFish" => {
                if let (Some(name), Some("at"), Some(position), Some(size), Some(behavior)) = (
                    command_parts.next(),
                    command_parts.next(),
                    command_parts.next(),
                    command_parts.next(),
                    command_parts.next(),
                ) {
                    if let Some((lenght, height)) = size.split_once('x') {
                        let lenght = match lenght.parse::<u32>() {
                            Ok(c) => c,
                            Err(_) => {
                                println!("size_x position must be an integer");
                                continue;
                            }
                        };
                        let height = match height.parse::<u32>() {
                            Ok(c) => c,
                            Err(_) => {
                                println!("size_y position must be an integer");
                                continue;
                            }
                        };
                        if let Some((position_x, position_y)) = position.split_once('x') {
                            let position_x = match position_x.parse::<u32>() {
                                Ok(c) => c,
                                Err(_) => {
                                    println!("x position must be an integer");
                                    continue;
                                }
                            };
                            let position_y = match position_y.parse::<u32>() {
                                Ok(c) => c,
                                Err(_) => {
                                    println!("y position must be an integer");
                                    continue;
                                }
                            };
                            api_fish.add_fish(add_fish(
                                name, position_x, position_y, lenght, height, behavior,
                            ));
                        } else {
                            println!("position should be like 10x10");
                            continue;
                        }
                    } else {
                        println!("size should be like 10x4");
                        continue;
                    }
                } else {
                    println!("Usage: addFish <name> at <position> <size> <behavior>");
                }
            }
            "delFish" => {
                if let Some(name) = command_parts.next() {
                    api_fish.del_fish(String::from(name));
                    del_fish(name);
                } else {
                    println!("Usage: delFish <name>");
                }
            }
            "startFish" => {
                if let Some(name) = command_parts.next() {
                    api_fish.start_fish(String::from(name));
                    start_fish(name);
                } else {
                    println!("Usage: startFish <name>");
                }
            }
            "quit" => break,
            "ping" => api_fish.ping(),
            _ => {
                println!("Invalid command !");
                println!("use help");
            }
        }
    }
    println!("bye bye");
}

fn help() {
    println!("valid command :");
    println!("\tstatus : to know if tou are connected to the controller&é");
    println!("\tquit : to quit the app");
    println!("\taddFish <name> at <position(ex:12x85)> <size(ex:10x4)> <behavior> : to add a fish");
    println!("\tdelFish <name> : to delete a fish");
    println!("\tstartFish <name> : to VroumVroum a fish");
    println!("\thelp : you just used it...");
}

fn status() {
    println!("Status controller");
}

fn add_fish(name: &str, x: u32, y: u32, lenght: u32, height: u32, behavior: &str) -> Fish {
    println!(
        "Add fish named {name} at {x}x{y} with size {lenght}x{height} and behavior {behavior}"
    );
    Fish {
        name: String::from(name),
        position_x: x,
        position_y: y,
        lenght: lenght,
        height: height,
        behavior: String::from(behavior),
    }
}

fn del_fish(name: &str) {
    println!("Fish named {name} was deleted");
}

fn start_fish(name: &str) {
    println!("Fish named {name} was VroumVroum");
}

#[derive(Debug)]
pub struct Fish {
    pub name: String,
    pub position_x: u32,
    pub position_y: u32,
    pub lenght: u32,
    pub height: u32,
    pub behavior: String,
}
