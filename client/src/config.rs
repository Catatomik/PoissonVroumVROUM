use std::path::PathBuf;
use std::fs::File;
use std::io::{self, BufRead};
use std::collections::HashMap;

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

        if let Ok(lines) = read_lines(config_path) {
            for line in lines.map_while(Result::ok)  {
                if line.is_empty() || line.starts_with('#') {
                    continue;
                }
                if let Some((key, value)) = line.split_once('=') {
                    config_map.insert(key.trim().to_string(), value.trim().to_string());
                }
            }
        }

        // Ugly but works
        Ok ( 
            Config {
                controller_address: (config_map.get("controller-address").ok_or("controller-address miss".to_string())?.clone()),
                id: (config_map.get("id").ok_or("id miss".to_string())?.clone()),
                controller_port: (config_map.get("controller-port").ok_or("controller-port miss".to_string())?.parse::<u32>().map_err(|_|"controller-port has to be an Interger")?), 
                display_timeout_value: (config_map.get("display-timeout-value").ok_or("display-timeout-value miss".to_string())?.parse::<u32>().map_err(|_|"display-timeout-value has to be an Interger")?), 
                resources: (PathBuf::from(config_map.get("resources").ok_or("resources miss".to_string())?))
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
}

fn read_lines(filename: PathBuf) -> io::Result<io::Lines<io::BufReader<File>>> {
    let file = File::open(filename)?;
    Ok(io::BufReader::new(file).lines())
}