//! General API to communicate trough a [transport] using [protocol]

use super::protocol::{ClientPacket, ServerPacket, ServerPacketParsingError};

pub trait Transport<Req, Res> {
    type RequestError;
    type ResponseError;

    fn try_send(&mut self, data: Req) -> Result<(), Self::RequestError>;
    fn try_receive(&mut self) -> Result<Option<Res>, Self::ResponseError>;
}

#[derive(Debug)]
pub enum FishApiError<ReqE, ResE> {
    RequestError(ReqE),
    ResponseError(ResE),
    ParsingError(ServerPacketParsingError),
    MismatchingResponse,
}

pub struct FishApi<T: Transport<ClientPacket, ServerPacket>> {
    transport: T,
}

impl<T: Transport<ClientPacket, ServerPacket>> FishApi<T> {
    pub fn new(transport: T) -> Self {
        FishApi { transport }
    }

    pub fn ping(&mut self) -> Result<(), FishApiError<T::RequestError, T::ResponseError>> {
        self.transport
            .try_send(ClientPacket::Ping)
            .map_err(FishApiError::RequestError)
    }

    pub fn try_receive(
        &mut self,
    ) -> Result<Option<ServerPacket>, FishApiError<T::RequestError, T::ResponseError>> {
        self.transport
            .try_receive()
            .map_err(FishApiError::ResponseError)
    }
}
