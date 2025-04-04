//! General API to communicate trough a [transport] using [protocol]

use super::protocol::{ClientPacket, ServerPacket, ServerPacketParsingError};
use std::{
    fmt::Debug,
    marker::PhantomData,
    sync::mpsc::{Receiver, Sender, TryRecvError, channel},
    thread::{sleep, spawn},
    time::Duration,
};

/// A non-blocking transport
/// i.e., send and receive methods
pub trait Transport<Req, Res> {
    type RequestError: Debug;
    type ResponseError: Debug;

    /// Non-blocking send
    fn try_send(&mut self, data: Req) -> Result<(), Self::RequestError>;
    /// Non-blocking receive
    fn try_receive(&mut self) -> Result<Option<Res>, Self::ResponseError>;
}

#[derive(Debug)]
pub enum FishApiError<ReqE, ResE> {
    RequestError(ReqE),
    ResponseError(ResE),
    ParsingError(ServerPacketParsingError),
}

pub enum CommandResult {
    Ok,
    NOk,
}

impl TryFrom<ServerPacket> for CommandResult {
    type Error = ();

    fn try_from(value: ServerPacket) -> Result<Self, Self::Error> {
        match value {
            ServerPacket::Ok => Ok(CommandResult::Ok),
            ServerPacket::NOk => Ok(CommandResult::NOk),
            // If ServerPacket is not Ok nor NOk, unable to cast into CommandResult
            _ => Err(()),
        }
    }
}

pub struct FishApi<T: Transport<ClientPacket, ServerPacket> + Send + 'static> {
    _p: PhantomData<T>,
    // Request channel transmission end
    request_tx: Sender<ClientPacket>,
    // Response channel receive end
    response_rx: Receiver<CommandResult>,
}

impl<T: Transport<ClientPacket, ServerPacket> + Send + 'static> FishApi<T> {
    pub fn new<F: FnMut(ServerPacket) -> () + Send + 'static>(
        mut transport: T,
        mut response_handler: F,
    ) -> Self {
        let (request_tx, request_rx) = channel::<ClientPacket>();
        let (response_tx, response_rx) = channel::<CommandResult>();

        spawn(move || {
            loop {
                // Treat packet to send through transport, if any
                if let Some(req) = match request_rx.try_recv() {
                    Ok(req) => Some(req),
                    Err(TryRecvError::Disconnected) => {
                        // Forced to quit, can't continue if channel is dead
                        panic!("Transport thread channel disconnected")
                    }
                    Err(TryRecvError::Empty) => None,
                } {
                    if let Err(e) = transport.try_send(req) {
                        // Error on transport send
                        // Do not break or panic, just to keep going
                        eprintln!("{:?}", e);
                    }
                }

                // Treat received packet through transport, if any
                match transport.try_receive() {
                    Ok(Some(p @ (ServerPacket::Ok | ServerPacket::NOk))) => {
                        if let Err(e) = response_tx.send(CommandResult::try_from(p).unwrap()) {
                            eprintln!("{:?}", e);
                        }
                    }
                    Ok(Some(p)) => response_handler(p),
                    Ok(None) => {
                        // Got no response to read
                        // We do nothing, just ignore this iteration and wait for further receive
                    }
                    Err(e) => {
                        // Error transport on receive
                        // Do not break or panic, just to keep going
                        eprintln!("{:?}", e);
                    }
                };

                // Anyway, sleep to prevent consuming too much CPU
                sleep(Duration::from_millis(100));
            }
        });

        FishApi {
            _p: PhantomData,
            request_tx,
            response_rx,
        }
    }

    pub fn ping(&mut self) -> () {
        self.request_tx
            .send(ClientPacket::Ping)
            .expect("Cannot send through transport thread channel");
    }
}
