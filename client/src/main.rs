use clap::Parser;
use std::path::PathBuf;

use repl::cli::command_loop;
pub mod repl;

use config::Config;
pub mod config;

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

    // Print config file
    println!("Config used :\x1b[34m");

    let config_stc = Config::new(cli.config).unwrap();
    println!("Address: {}", config_stc.get_address());
    println!("Id: {}", config_stc.get_id());
    println!("Port: {}", config_stc.get_port());
    println!("Timeout: {}", config_stc.get_timeout());
    println!("Resources: {:?}\x1b[0m", config_stc.get_resources().file_name());

    // Start loop of the app
    command_loop();
}