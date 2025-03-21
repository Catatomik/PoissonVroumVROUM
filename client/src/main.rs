mod network;
mod view;

use network::{api::FishApi, protocol::ServerPacket, tcp::TcpClient};
use std::{
    net::{Ipv4Addr, SocketAddrV4},
    str::FromStr as _,
    thread::sleep,
    time::Duration,
};

struct App {
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
    let mut app = App {
        fish_api: FishApi::new(TcpClient::new(SocketAddrV4::new(
            Ipv4Addr::from_str("127.0.0.1").expect("Couldn't parse IPv4 address"),
            12345,
        ))),
    };

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
