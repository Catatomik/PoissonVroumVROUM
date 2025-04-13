use std::io;

use crate::network::api::{FishApi, FishApiError, Transport};
use crate::network::protocol::{ClientPacket, Fish, ServerPacket};

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
            "status" => {
                if let None = command_parts.next() {
                    status();
                } else {
                    println!("Usage: status");
                }
            }
            "help" => {
                if let None = command_parts.next() {
                    help();
                } else {
                    println!("Usage: help");
                }
            }
            "addFish" => {
                if let (Some(name), Some("at"), Some(position), Some(size), Some(behavior), None) = (
                    command_parts.next(),
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
                            let Ok(position_x) = position_x.parse::<u32>() else {
                                println!("x position must be an integer");
                                continue;
                            };
                            let Ok(position_y) = position_y.parse::<u32>() else {
                                println!("y position must be an integer");
                                continue;
                            };
                            add_fish(
                                &mut api_fish,
                                name,
                                position_x,
                                position_y,
                                lenght,
                                height,
                                behavior,
                            );
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
                if let (Some(name), None) = (command_parts.next(), command_parts.next()) {
                    del_fish(&mut api_fish, name);
                } else {
                    println!("Usage: delFish <name>");
                }
            }
            "startFish" => {
                if let (Some(name), None) = (command_parts.next(), command_parts.next()) {
                    start_fish(&mut api_fish, name);
                } else {
                    println!("Usage: startFish <name>");
                }
            }
            "quit" => {
                if let None = command_parts.next() {
                    break;
                } else {
                    println!("Usage: quit");
                }
            }
            _ => {
                println!("Invalid command !");
                println!("Please use '$ help'");
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

fn add_fish<T: Transport<ClientPacket, ServerPacket> + Send + 'static>(
    fish_api: &mut FishApi<T>,
    name: &str,
    x: u32,
    y: u32,
    length: u32,
    height: u32,
    behavior: &str,
) {
    let new_fish = Fish {
        name: String::from(name),
        position_x: x,
        position_y: y,
        length,
        height,
        behavior: String::from(behavior),
    };
    match fish_api.add_fish(new_fish) {
        Ok(res) => println!("    => {}", res.to_string()),
        Err(FishApiError::ResponseError(_)) => {
            eprintln!("Error while getting response for addFish request")
        }
        Err(FishApiError::RequestError(_)) => eprintln!("Error while sending addFish request"),
    }
}

fn del_fish<T: Transport<ClientPacket, ServerPacket> + Send + 'static>(
    fish_api: &mut FishApi<T>,
    name: &str,
) {
    match fish_api.del_fish(String::from(name)) {
        Ok(res) => println!("    => {}", res.to_string()),
        Err(FishApiError::ResponseError(_)) => {
            eprintln!("Error while getting response for delFish request")
        }
        Err(FishApiError::RequestError(_)) => eprintln!("Error while sending delFish request"),
    }
}

fn start_fish<T: Transport<ClientPacket, ServerPacket> + Send + 'static>(
    fish_api: &mut FishApi<T>,
    name: &str,
) {
    match fish_api.start_fish(String::from(name)) {
        Ok(res) => println!("    => {}", res.to_string()),
        Err(FishApiError::ResponseError(_)) => {
            eprintln!("Error while getting response for startFish request")
        }
        Err(FishApiError::RequestError(_)) => eprintln!("Error while sending startFish request"),
    }
}
