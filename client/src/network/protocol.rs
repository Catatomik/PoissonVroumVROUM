//! Internal communication protocol

use crate::view::entities;
use std::str::FromStr;

/// A server packet, i.e. a packet sent by a server following internal communication protocol
#[derive(Debug)]
pub enum ServerPacket {
    /// < greeting <ID>
    Greeting(usize, usize, usize, usize, usize),
    Pong,
    /// < no greeting
    /// Requested ID doesn't exist or is already affected
    NoGreeting,
    /// List of fishes to manage. Contains
    /// - position to go to
    /// - size (hitbox)
    /// - time to move
    FishesList(Vec<entities::Fish>),
    Ok,
    NOk,
    Bye,
}

#[derive(Debug)]
pub enum ServerPacketParsingError {
    InvalidFormat,
    UnsupportedCommand(String),
}

impl FromStr for ServerPacket {
    type Err = ServerPacketParsingError;

    fn from_str(raw_packet: &str) -> Result<ServerPacket, ServerPacketParsingError> {
        use ServerPacketParsingError::*;

        let mut packet_parts = raw_packet.split_whitespace();
        let first_word = packet_parts.next().ok_or(InvalidFormat)?;
        match first_word {
            "pong" => Ok(ServerPacket::Pong),
            "greeting" => {
                let packet_parts_plus = packet_parts.clone().flat_map(|s| s.split('+'));
                if packet_parts_plus.clone().count() != 4 {
                    Err(InvalidFormat)?
                };
                let packet_parts_x = packet_parts_plus.flat_map(|s| s.split('x'));
                if packet_parts_x.clone().count() != 6 {
                    Err(InvalidFormat)?
                };
                if let (Some(id), Some(x), Some(y), Some(w), Some(h), None) = (
                    packet_parts.next(),
                    packet_parts.next(),
                    packet_parts.next(),
                    packet_parts.next(),
                    packet_parts.next(),
                    packet_parts.next(),
                ) {
                    Ok(ServerPacket::Greeting(
                        id.strip_prefix("N")
                            .ok_or(InvalidFormat)?
                            .parse()
                            .map_err(|_| InvalidFormat)?,
                        x.parse().map_err(|_| InvalidFormat)?,
                        y.parse().map_err(|_| InvalidFormat)?,
                        w.parse().map_err(|_| InvalidFormat)?,
                        h.parse().map_err(|_| InvalidFormat)?,
                    ))
                } else {
                    Err(InvalidFormat)
                }
            }
            other => Err(UnsupportedCommand(other.to_string())),
        }
    }
}

/// A client packet, i.e. a packet sent by a client following internal communication protocol
#[derive(Debug)]
pub enum ClientPacket {
    /// > hello [as in N<ID>]
    Hello(Option<usize>),
    Ping,
    /// Periodically ask for fishes
    GetFishes,
    /// Continuously ask for fishes, stacking answers
    /// Optionally takes a number of lists to receive
    LsFishes(Option<usize>),
    /// Continuously ask for fishes
    GetFishesContinuously,
    AddFish(entities::Fish),
    DelFish,
    /// VroumVROUM
    StartFish,
    LogOut,
}

impl ToString for ClientPacket {
    fn to_string(&self) -> String {
        match self {
            Self::Ping => String::from("ping"),
            Self::Hello(None) => String::from("hello"),
            Self::Hello(Some(id)) => format!("hello as in {}", id),
            _ => unimplemented!(),
        }
    }
}
