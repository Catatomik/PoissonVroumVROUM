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
        let mut command_parts = full_command.split_whitespace():
        let main_command = command_parts.next();
        // TODO : find a better way (enum? / static string?)
        match main_command {
            Some("status") => status(),
            Some("help") => help(),
            Some("quit") => break,
            _ => {
                println!("Invalid command !");
                println!("use help");
            },
        }
    }
    println!("bye bye");
}

fn status() {
    println!("Status Fish !!");
}

fn help() {
    println!("valid command :");
    println!("\tstatus : to know if tou are connected to the controller&é");
    println!("\tquit : to quit the app");
    println!("\thelp : you just used it...");
}