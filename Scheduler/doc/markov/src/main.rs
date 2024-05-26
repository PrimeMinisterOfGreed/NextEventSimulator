use clap::{arg, Args, Parser};
use generator::ChainGenerator;
use lstsq::Lstsq;
use nalgebra::Dyn;
use parameters::Params;
use solver::Solver;
use state::State;

mod state;
mod parameters;
mod generator;
mod transition;
mod solver;

#[derive(Parser,Debug)]
struct Cli{
    #[arg(short,long)]
    numclients: u8,
    #[arg(short,long)]
    isomorphic : bool
}




fn main() {
    let cli = Cli::parse();
    let mut p = Params::instance();
    if cli.isomorphic{
        p.alpha = 0.5;
        p.beta = 0.5;
        p.u1 = 27.0;
        p.u2 = 27.0;
    }
    p.numclients = cli.numclients;
    Params::instance().numclients = 3;
    let mut generator = ChainGenerator::new();
    let mut chain = generator.generate(State::new(3, 0, 0, 0, 0)).adj_matrix();
    let mut solver = Solver::new(chain).as_ctmc().add_normalization_condition().solve();

    

}
