use clap::Parser;
use std::path::PathBuf;

use repl::cli::command_loop;
pub mod repl;

use config::Config;
pub mod config;

mod view;

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
    println!("{}", config_stc);
    print!("\x1b[0m");

    // Start loop of the app
    command_loop();
    // view::display::display();
}