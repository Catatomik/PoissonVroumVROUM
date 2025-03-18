use std::path::PathBuf;
use std::fs::File;
use std::io::{self, BufRead};
use std::collections::HashMap;

use std::ffi::OsStr;

pub struct Config {
    controller_address: String,
    id: String,
    controller_port: u32,
    display_timeout_value: u32,
    resources: PathBuf
}

impl Config {
    pub fn new(config_path:PathBuf) -> Result<Config, String> {
        let mut config_map = HashMap::new();

        let file = File::open(config_path).map_err(|_|"can't open the file".to_string())?;
        let lines = io::BufReader::new(file).lines();

        for line in lines.map_while(Result::ok) {
            if line.is_empty() || line.starts_with('#') {
                continue;
            }
            if let Some((key, value)) = line.split_once('=') {
                config_map.insert(key.trim().to_string(), value.trim().to_string());
            }
        }

        Ok ( 
            Config {
                controller_address: (hasmap_get(&config_map, "controller-address")?),
                id: (hasmap_get(&config_map, "id")?),
                controller_port: (hasmap_get(&config_map, "controller-port")?.parse::<u32>().map_err(|_|"controller-port has to be an Interger")?), 
                display_timeout_value: (hasmap_get(&config_map, "display-timeout-value")?.parse::<u32>().map_err(|_|"display-timeout-value has to be an Interger")?), 
                resources: (PathBuf::from(hasmap_get(&config_map, "resources")?))
            }
        )
    }

    pub fn get_address(&self) -> &str {
        self.controller_address.as_str()
    }

    pub fn get_port(&self) -> u32 {
        self.controller_port
    }

    pub fn get_id(&self) -> &str {
        self.id.as_str()
    }

    pub fn get_timeout(&self) -> u32 {
        self.display_timeout_value
    }

    pub fn get_resources(&self) -> PathBuf {
        self.resources.to_path_buf()
    }

    pub fn display(&self) {
        println!("Address: {}", self.get_address());
        println!("Id: {}", self.get_id());
        println!("Port: {}", self.get_port());
        println!("Timeout: {}", self.get_timeout());
        println!("Resources name: {:?}", match self.get_resources().file_name() {
            Some(name) => name,
            None => OsStr::new("No file selected"),
        })
    }
}

fn hasmap_get(map:&HashMap<String, String>, key:&str) -> Result<String, String> {
    map.get(key).ok_or(format!("{key} miss")).cloned()
}