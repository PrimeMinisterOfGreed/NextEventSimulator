use clap::{arg, Args, Parser};
use parameters::Params;

mod state;
mod parameters;
mod generator;
mod transition;

#[derive(Parser,Debug)]
struct Cli{
    #[arg(short,long)]
    numclients: u32,
    #[arg(short,long)]
    isomorphic : bool
}
fn main() {
    let cli = Cli::parse();
    let mut p = Params::instance();
    if cli.isomorphic{
        p.alpha = 0.5;
        p.beta = 0.5;
        p.u1 = 27;
        p.u2 = 27;
    }
    p.numclients = cli.numclients;
}
