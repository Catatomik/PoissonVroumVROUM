//! # Simple TCP client
//!
//! Simple sync TCP client

use super::api::Transport;
use std::{
    fmt::Debug,
    io::{self, Read, Write as _},
    net::{SocketAddrV4, TcpStream},
    str::{FromStr, Utf8Error, from_utf8},
};

#[derive(Debug)]
pub enum TcpReceiveError<E> {
    ParsingError(E),
    SocketError(io::Error),
    Utf8Error(Utf8Error),
}

pub struct TcpClient {
    stream: TcpStream,
    buf: Vec<u8>,
}

impl TcpClient {
    pub fn new(addr: SocketAddrV4) -> Self {
        let stream = TcpStream::connect(addr).expect("Unable to connect TCP socket");
        stream
            .set_nonblocking(true)
            .expect("Unable to set TCP socket non-blocking");

        TcpClient {
            stream,
            buf: Vec::new(),
        }
    }
}

impl<Req: ToString, Res: FromStr> Transport<Req, Res> for TcpClient
where
    <Res as FromStr>::Err: Debug,
{
    type RequestError = io::Error;

    type ResponseError = TcpReceiveError<Res::Err>;

    fn try_send(&mut self, data: Req) -> Result<(), Self::RequestError> {
        writeln!(self.stream, "{}", &data.to_string())?;

        Ok(())
    }

    fn try_receive(&mut self) -> Result<Option<Res>, Self::ResponseError> {
        const BUF_SIZE: usize = 512;

        let mut buf = [0u8; BUF_SIZE];

        let read_cnt = match self.stream.read(&mut buf) {
            Ok(read) => read,
            Err(e) => {
                return if let io::ErrorKind::WouldBlock = e.kind() {
                    Ok(None)
                } else {
                    Err(Self::ResponseError::SocketError(e))
                };
            }
        };
        if read_cnt == 0 {
            return Ok(None);
        }

        self.buf.extend_from_slice(&buf[..read_cnt]);

        let newline_idx = match buf.iter().position(|c| *c == b'\n') {
            Some(idx) => idx,
            None => return Ok(None),
        };

        let res = Res::from_str(
            from_utf8(&self.buf[..newline_idx]).map_err(Self::ResponseError::Utf8Error)?,
        )
        .map(Some)
        .map_err(Self::ResponseError::ParsingError);

        // Remove read line
        self.buf.splice(..=newline_idx, []);

        res
    }
}
