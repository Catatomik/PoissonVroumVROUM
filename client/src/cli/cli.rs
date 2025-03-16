use std::io;

pub fn command_loop() {
    println!("\nWelcome to PoissonVroumVROUM");
    loop {
        let mut full_command = String::new();

        eprint!("\x1b[32m$ ");
        io::stdin()
            .read_line(&mut full_command)
            .expect("Failed to read line");
        eprint!("\x1b[0m");
        // println!("Your command : {}", cmd);
        let full_command = full_command.trim();
        let mut command_parts = full_command.split_whitespace();
        let main_command = command_parts.next();

        // TODO : find a better way (enum? / static string?)
        match main_command {
            Some("status") => status(),
            Some("help") => help(),
            // TODO : Opti and check type of position and size
            Some("addFish") => {
                if let (Some(name), Some("at"), Some(position), Some(size), Some(behavior)) =
                    (command_parts.next(), command_parts.next(), command_parts.next(), command_parts.next(), command_parts.next())
                {
                    add_fish(name, position, size, behavior);
                } else {
                    println!("Usage: addFish <name> at <position> <size> <behavior>");
                }
            }
            Some("delFish") => {
                if let Some(name) = command_parts.next() {
                    del_fish(name);
                } else {
                    println!("Usage: delFish <name>");
                }
            }
            Some("startFish") => {
                if let Some(name) = command_parts.next() {
                    start_fish(name);
                } else {
                    println!("Usage: startFish <name>");
                }
            }
            Some("quit") => break,
            _ => {
                println!("Invalid command !");
                println!("use help");
            },
        }
    }
    println!("bye bye");
}

fn help() {
    println!("valid command :");
    println!("\tstatus : to know if tou are connected to the controller&é");
    println!("\tquit : to quit the app");
    println!("\taddFish <name> at <position> <size> <behavior> : to add a fish");
    println!("\tdelFish <name> : to delete a fish");
    println!("\tstartFish <name> : to VroumVroum a fish");
    println!("\thelp : you just used it...");
}

fn status() {
    println!("Status controller");
}

fn add_fish(name:&str, position:&str, size:&str, behavior:&str) {
    println!("Add fish named {name} at {position} with size {size} and behavior {behavior}");
}

fn del_fish(name:&str) {
    println!("Fish named {name} was deleted");
}

fn start_fish(name:&str) {
    println!("Fish named {name} was VroumVroum");
}