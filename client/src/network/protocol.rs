use crate::view::entities;
use std::{error::Error, fmt::Display};

pub enum NetworkPacket {
    ClientPacket(ClientPacket),
    ServerPacket(ServerPacket),
}

pub enum ServerPacket {
    // < greeting <ID>
    Greeting(Option<usize>),
    Pong,
    // < no greeting
    // Requested ID doesn't exist or is already affected
    NoGreeting,
    // List of fishes to manage. Contains
    // - position to go to
    // - size (hitbox)
    // - time to move
    FishesList(Vec<entities::Fish>),
    Ok,
    NOk,
    Bye,
}

pub enum ClientPacket {
    // > hello [as in N<ID>]
    Hello(Option<usize>),
    Ping,
    // Periodically ask for fishes
    GetFishes,
    // Continuously ask for fishes
    // Optionally takes a number of lists to receive
    LsFishes(Option<usize>),
    AddFish(entities::Fish),
    DelFish,
    // VroumVROUM
    StartFish,
    LogOut,
}
