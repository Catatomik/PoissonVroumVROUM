mod config;
mod network;
mod repl;
mod view;

use clap::Parser;
use config::Config;
use network::{api::FishApi, protocol::ServerPacket, tcp::TcpClient};
use repl::cli::command_loop;
use std::{
    net::{Ipv4Addr, SocketAddrV4},
    path::PathBuf,
    str::FromStr as _,
    sync::{Arc, Mutex},
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
        network::protocol::ServerPacket::Pong => {
            println!("Pong!")
        }
        _ => unimplemented!(),
    }
}

fn main() {
    let cli = Cli::parse();

    let config = Config::new(&cli.config).unwrap();
    // Print config file
    println!("Config used :\x1b[34m{}\x1b[0m", config);

    let fishes = Arc::new(Mutex::new(Vec::new()));

    let mut fish_api = FishApi::new(
        TcpClient::new(SocketAddrV4::new(
            Ipv4Addr::from_str("127.0.0.1").expect("Couldn't parse IPv4 address"),
            12345,
        )),
        {
            let mut fishes = fishes.clone();
            move |p| handle_packet(&mut fishes, p)
        },
    );

    fish_api.ping();

    // Start loop of the app
    command_loop();
    // view::display::display();
}
