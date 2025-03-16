use clap::Parser;
use std::fs;
use std::path::PathBuf;

use cli::cli::command_loop;
pub mod cli;

#[derive(Parser)]
#[command(name = "PoissonVroumVROUM")]
#[command(version = "0.1")]
#[command(about = "Here we start fishes", long_about = None)]
struct Cli {
    #[arg(value_name = "CONFIG", help = "File of configuration for the client", required = true)]
    config: PathBuf,
}

fn main() {
    let cli = Cli::parse();

    match fs::read_to_string(&cli.config) {
        Ok(content) => println!("Content :\n{}", content),
        Err(e) => println!("Error :\n{}", e),
    }
    
    command_loop();
}