//! Internal communication protocol

use crate::view::entities::Fish as ViewFish;
use std::{
    fmt,
    str::FromStr,
    time::{Duration, Instant},
};

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
    FishesList(Vec<ViewFish>),
    Ok,
    NOk,
    Bye,
}

#[derive(Debug)]
pub enum ServerPacketParsingError {
    InvalidFormat,
    UnsupportedCommand(String),
}

/// Server packet deserialization
impl FromStr for ServerPacket {
    type Err = ServerPacketParsingError;

    fn from_str(raw_packet: &str) -> Result<ServerPacket, ServerPacketParsingError> {
        use ServerPacketParsingError::*;

        let mut packet_parts = raw_packet.split_whitespace();
        let first_word = packet_parts.next().ok_or(InvalidFormat)?;
        match first_word {
            "pong" => {
                if packet_parts.next().is_none() {
                    Ok(ServerPacket::Pong)
                } else {
                    Err(InvalidFormat)
                }
            }
            "OK" => {
                if packet_parts.next().is_none() {
                    Ok(ServerPacket::Ok)
                } else {
                    Err(InvalidFormat)
                }
            }
            "NOK" => {
                if packet_parts.next().is_none() {
                    Ok(ServerPacket::NOk)
                } else {
                    Err(InvalidFormat)
                }
            }
            "greeting" => {
                let packet_parts_plus = packet_parts.flat_map(|s| s.split('+'));
                if packet_parts_plus.clone().count() != 4 {
                    Err(InvalidFormat)?;
                };
                let mut packet_parts_x = packet_parts_plus.flat_map(|s| s.split('x'));
                if packet_parts_x.clone().count() != 5 {
                    Err(InvalidFormat)?;
                };
                if let (Some(id), Some(x), Some(y), Some(w), Some(h), None) = (
                    packet_parts_x.next(),
                    packet_parts_x.next(),
                    packet_parts_x.next(),
                    packet_parts_x.next(),
                    packet_parts_x.next(),
                    packet_parts_x.next(),
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
            "no" => {
                if let (Some(second), None) = (packet_parts.next(), packet_parts.next()) {
                    // Chaining `if let` and `&&` is unstable
                    if second == "greeting" {
                        Ok(ServerPacket::NoGreeting)
                    } else {
                        Err(UnsupportedCommand(raw_packet.to_owned()))
                    }
                } else {
                    Err(UnsupportedCommand(raw_packet.to_owned()))
                }
            }
            "bye" => {
                if packet_parts.next().is_none() {
                    Ok(ServerPacket::Bye)
                } else {
                    Err(InvalidFormat)
                }
            }
            "list" => {
                let packet_data = raw_packet.strip_prefix("list").expect("Command was `list`");
                if packet_data.is_empty() {
                    Ok(ServerPacket::FishesList(vec![]))
                } else {
                    Ok(ServerPacket::FishesList(
                        packet_data
                            .strip_prefix(" [")
                            .ok_or(InvalidFormat)?
                            .strip_suffix(']')
                            .ok_or(InvalidFormat)?
                            .split("] [")
                            // all ] or [ are removed
                            .map(|fish_slice| {
                                // At this point, fish_slice is like "fish_name at XxY,WxH,delay"
                                let mut splitted = fish_slice.split(" at ");
                                // Separate fish name from props
                                let (fish_name, mut props) =
                                    if let (Some(fish_name), Some(props), None) =
                                        (splitted.next(), splitted.next(), splitted.next())
                                    {
                                        Ok((fish_name, props.split(',')))
                                    } else {
                                        Err(InvalidFormat)
                                    }?;

                                // Parse props parts
                                let (mut coords, mut size, time) =
                                    if let (Some(coords), Some(size), Some(time), None) =
                                        (props.next(), props.next(), props.next(), props.next())
                                    {
                                        Ok((coords.split('x'), size.split('x'), time))
                                    } else {
                                        Err(InvalidFormat)
                                    }?;

                                // Parse position parts
                                let (target_x, target_y) =
                                    if let (Some(target_x), Some(target_y), None) =
                                        (coords.next(), coords.next(), coords.next())
                                    {
                                        Ok((
                                            target_x.parse::<f32>().map_err(|_| InvalidFormat)?,
                                            target_y.parse::<f32>().map_err(|_| InvalidFormat)?,
                                        ))
                                    } else {
                                        Err(InvalidFormat)
                                    }?;

                                // Parse size parts
                                let (size_w, size_h) = if let (Some(size_w), Some(size_h), None) =
                                    (size.next(), size.next(), size.next())
                                {
                                    Ok((
                                        size_w.parse::<f32>().map_err(|_| InvalidFormat)?,
                                        size_h.parse::<f32>().map_err(|_| InvalidFormat)?,
                                    ))
                                } else {
                                    Err(InvalidFormat)
                                }?;

                                // Parse time
                                let timestamp = Instant::now()
                                    + Duration::from_secs(time.parse().map_err(|_| InvalidFormat)?);

                                Ok(ViewFish::new(
                                    fish_name, target_x, target_y, size_w, size_h, timestamp, false,
                                ))
                            })
                            // Collect into a Result because it implements FromIterator
                            // See https://doc.rust-lang.org/std/result/index.html#collecting-into-result
                            .collect::<Result<Vec<ViewFish>, ServerPacketParsingError>>()?,
                    ))
                }
            }
            other => Err(UnsupportedCommand(other.to_string())),
        }
    }
}

// Temp
#[allow(dead_code)]
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
    AddFish(Fish),
    DelFish(String),
    /// VroumVROUM
    StartFish(String),
    LogOut,
}

/// Client packet serialization
impl fmt::Display for ClientPacket {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> fmt::Result {
        match self {
            ClientPacket::Ping => write!(f, "ping"),
            ClientPacket::AddFish(fish) => write!(
                f,
                "addFish {} at {}x{},{}x{}, {}",
                fish.name,
                fish.position_x,
                fish.position_y,
                fish.length,
                fish.height,
                fish.behavior
            ),
            ClientPacket::DelFish(name) => write!(f, "delFish {}", name),
            ClientPacket::StartFish(name) => write!(f, "startFish {}", name),
            ClientPacket::Hello(None) => write!(f, "hello"),
            ClientPacket::Hello(Some(id)) => write!(f, "hello in as N{}", id),
            ClientPacket::GetFishes => write!(f, "getFishes"),
            ClientPacket::LsFishes(_) => write!(f, "ls"),
            ClientPacket::GetFishesContinuously => write!(f, "getFishesContinuously"),
            ClientPacket::LogOut => write!(f, "log out"),
        }
    }
}

#[derive(Debug)]
pub struct Fish {
    pub name: String,
    pub position_x: u32,
    pub position_y: u32,
    pub length: u32,
    pub height: u32,
    pub behavior: String,
}
