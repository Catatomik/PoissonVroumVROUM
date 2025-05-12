mod config;
mod network;
mod repl;
mod view;

use clap::Parser;
use config::Config;
use network::{api::FishApi, protocol::ServerPacket, tcp::TcpClient};
use repl::cli::command_loop;
use std::{
    net::SocketAddrV4,
    path::PathBuf,
    sync::{Arc, Mutex},
    thread,
};
use view::entities;

#[derive(Parser)]
#[command(name = "PoissonVroumVROUM")]
#[command(version = "0.1")]
#[command(about = "Here we start fishes", long_about = None)]
struct Cli {
    #[arg(
        value_name = "CONFIG",
        help = "File of configuration for the client",
        required = true
    )]
    config: PathBuf,
}

pub fn handle_packet(fishes: &mut Arc<Mutex<Vec<entities::Fish>>>, packet: ServerPacket) {
    match packet {
        ServerPacket::Pong => {
            #[cfg(debug_assertions)]
            println!("Pong!");
        }
        ServerPacket::FishesList(list) => {
            let mut fishes_lock = fishes.lock().unwrap();
            let old_length = fishes_lock.len();
            let new_length = list.len();
            fishes_lock.splice(0..(1.max(new_length) - 1).min(1.max(old_length) - 1), list);
            fishes_lock.truncate(new_length);
        }
        _ => eprintln!("Unexpected packet to handle"),
    }
}

fn main() {
    let cli = Cli::parse();

    let config = Config::new(&cli.config).unwrap();
    // Print config file
    println!("Config used :\x1b[34m{}\x1b[0m", config);

    let fishes = Arc::new(Mutex::new(Vec::new()));

    let (mut fish_api, viewer_config) = FishApi::new(
        TcpClient::new(SocketAddrV4::new(config.get_address(), config.get_port())),
        &config,
        {
            let mut fishes = fishes.clone();
            move |p| handle_packet(&mut fishes, p)
        },
    );
    // Temp ping to check API connection
    fish_api.start().unwrap();

    // Start loop of the app
    thread::spawn(move || command_loop(fish_api));

    // Start display
    view::display::display(fishes, viewer_config, config.get_resources());
}
