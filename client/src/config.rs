pub mod config;

use std::path::PathBuf;

struct Config {
    controller_address: String,
    id: String,
    controller_port: u32,
    display_timeout_value: u32,
    resources: PathBuf
}

impl Config {
    fn new(config_path:PathBuf) {
        // TODO : all
        // lines.filter(|l| !starts_with("#")).map(|l| split("="))
    }
}