use clap::Parser;

#[derive(Parser)]
#[command(name = "PoissonVroumVROUM")]
#[command(version = "0.1")]
#[command(about = "Here we start fishes", long_about = None)]
struct Cli {
    #[arg(long)]
    id: String,
    #[arg(default_value_t = 12345)]
    port: u16,
}


fn main() {
    let cli = Cli::parse();

    println!("id : {}", cli.id);
    println!("port : {}", cli.port);
    println!("Hello, world!");
}