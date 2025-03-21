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
    thread::sleep,
    time::Duration,
};

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

struct App {
    cli: Cli,
    fish_api: FishApi<TcpClient>,
}

impl App {
    pub fn handle_packet(&mut self, packet: ServerPacket) {
        match packet {
            network::protocol::ServerPacket::Pong => {
                println!("Pong!")
            }
            _ => unimplemented!(),
        }
    }
}

fn main() {
    // Start loop of the app
    command_loop();
    // view::display::display();

    let mut app = App {
        fish_api: FishApi::new(TcpClient::new(SocketAddrV4::new(
            Ipv4Addr::from_str("127.0.0.1").expect("Couldn't parse IPv4 address"),
            12345,
        ))),
        cli: Cli::parse(),
    };

    let config = Config::new(&app.cli.config).unwrap();
    // Print config file
    println!("Config used :\x1b[34m{}\x1b[0m", config);

    loop {
        match app.fish_api.try_receive() {
            Ok(Some(res)) => app.handle_packet(res),
            Ok(None) => {}
            Err(e) => {
                eprintln!("Error while treating Fish API response {:?}", e);
            }
        };

        // Additional actions

        sleep(Duration::from_millis(100));
    }
}
