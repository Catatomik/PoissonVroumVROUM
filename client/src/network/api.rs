//! General API to communicate trough a [transport] using [protocol]

use super::protocol::{ClientPacket, ServerPacket, ServerPacketParsingError};
use std::{
    fmt::Debug,
    marker::PhantomData,
    sync::mpsc::{Receiver, Sender, TryRecvError, channel},
    thread::{sleep, spawn},
    time::Duration,
};

pub trait Transport<Req, Res> {
    type RequestError: Debug;
    type ResponseError: Debug;

    fn try_send(&mut self, data: Req) -> Result<(), Self::RequestError>;
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
            _ => Err(()),
        }
    }
}

pub struct FishApi<T: Transport<ClientPacket, ServerPacket> + Send + 'static> {
    _p: PhantomData<T>,
    request_tx: Sender<ClientPacket>,
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
                if let Some(req) = match request_rx.try_recv() {
                    Ok(req) => Some(req),
                    Err(TryRecvError::Disconnected) => {
                        panic!("Transport thread channel disconnected")
                    }
                    Err(TryRecvError::Empty) => None,
                } {
                    if let Err(e) = transport.try_send(req) {
                        eprintln!("{:?}", e);
                    }
                }

                let res = match transport.try_receive() {
                    Ok(Some(p @ (ServerPacket::Ok | ServerPacket::NOk))) => {
                        if let Err(e) = response_tx.send(CommandResult::try_from(p).unwrap()) {
                            eprintln!("{:?}", e);
                        }
                    }
                    Ok(Some(p)) => response_handler(p),
                    Ok(None) => {
                        // Got no response to read
                    }
                    Err(e) => {
                        eprintln!("{:?}", e);
                    }
                };

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
