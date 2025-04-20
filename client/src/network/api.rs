//! General API to communicate trough a [transport] using [protocol]

use super::protocol::{ClientPacket, Fish, ServerPacket};

use std::{
    fmt,
    fmt::Debug,
    marker::PhantomData,
    sync::mpsc::{Receiver, RecvError, SendError, Sender, TryRecvError, channel},
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
pub enum FishApiError {
    // Temp
    #[allow(dead_code)]
    RequestError(SendError<ClientPacket>),
    ResponseError(RecvError),
}

pub enum CommandResult {
    Ok,
    NOk,
}

impl fmt::Display for CommandResult {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> fmt::Result {
        match self {
            CommandResult::Ok => write!(f, "OK"),
            CommandResult::NOk => write!(f, "NOK"),
        }
    }
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

// Temp
#[allow(dead_code)]
pub struct ViewerConfig {
    pub x: usize,
    pub y: usize,
    pub width: usize,
    pub height: usize,
}

pub struct FishApi<T: Transport<ClientPacket, ServerPacket> + Send + 'static> {
    _p: PhantomData<T>,
    // Request channel transmission end
    request_tx: Sender<ClientPacket>,
    // Response channel receive end
    response_rx: Receiver<CommandResult>,
}

impl<T: Transport<ClientPacket, ServerPacket> + Send + 'static> FishApi<T> {
    pub fn new<F: FnMut(ServerPacket) + Send + 'static>(
        mut transport: T,
        mut response_handler: F,
    ) -> (Self, ViewerConfig) {
        let (request_tx, request_rx) = channel::<ClientPacket>();
        let (response_tx, response_rx) = channel::<CommandResult>();

        // Handshake, blocking
        transport
            .try_send(ClientPacket::Hello(None))
            .expect("Unable to send hello");
        // Wait for greeting from servers
        let viewer_config = loop {
            match transport.try_receive() {
                Ok(Some(ServerPacket::Greeting(_, x, y, width, height))) => {
                    // Got greeting!
                    // Continue initialization
                    break ViewerConfig {
                        x,
                        y,
                        width,
                        height,
                    };
                }
                Ok(Some(res)) => {
                    panic!("Unexpected answer while waiting for handshake: {:?}", res);
                }
                Ok(None) => {
                    // Got no response to read
                }
                Err(e) => {
                    panic!("Error while waiting for handshake: {:?}", e);
                }
            };

            sleep(Duration::from_millis(100));
        };

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

        (
            FishApi {
                _p: PhantomData,
                request_tx,
                response_rx,
            },
            viewer_config,
        )
    }

    // Ping must be non-blocking, hence treated in `response_handler` of FishAPI
    pub fn ping(&mut self) -> Result<(), FishApiError> {
        self.request_tx
            .send(ClientPacket::Ping)
            .map_err(FishApiError::RequestError)
    }

    pub fn add_fish(&mut self, fish: Fish) -> Result<CommandResult, FishApiError> {
        self.request_tx
            .send(ClientPacket::AddFish(fish))
            .map_err(FishApiError::RequestError)?;
        self.response_rx.recv().map_err(FishApiError::ResponseError)
    }

    pub fn del_fish(&mut self, name: String) -> Result<CommandResult, FishApiError> {
        self.request_tx
            .send(ClientPacket::DelFish(name))
            .map_err(FishApiError::RequestError)?;
        self.response_rx.recv().map_err(FishApiError::ResponseError)
    }

    pub fn start_fish(&mut self, name: String) -> Result<CommandResult, FishApiError> {
        self.request_tx
            .send(ClientPacket::StartFish(name))
            .map_err(FishApiError::RequestError)?;
        self.response_rx.recv().map_err(FishApiError::ResponseError)
    }
}
