//! Internal communication protocol

use crate::view::entities;
use std::str::FromStr;

/// A server packet, i.e. a packet sent by a server following internal communication protocol
#[derive(Debug)]
pub enum ServerPacket {
    /// < greeting <ID> <x> <y> <width> <height>
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

        let first_word = raw_packet.split_whitespace().next().ok_or(InvalidFormat)?;
        match first_word {
            "pong" => Ok(ServerPacket::Pong),
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
            _ => unimplemented!(),
        }
    }
}
