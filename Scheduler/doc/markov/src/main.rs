use clap::{arg, Args, Parser};
use generator::ChainGenerator;
use lstsq::Lstsq;
use nalgebra::Dyn;
use parameters::Params;
use solver::{Solution, Solver};
use state::State;

mod state;
mod mva;
mod parameters;
mod generator;
mod transition;
mod solver;

#[derive(Parser,Debug)]
struct Cli{
    #[arg(short,long,default_value="3")]
    numclients: u8,
    #[arg(short,long, default_value="true")]
    isomorphic : bool,
    #[arg(long,default_value="false")]
    print_graph: bool,
    #[arg(long,default_value="true")]
    print_distributions: bool,
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
    p.numclients = 10;
    let mut generator = ChainGenerator::new();
    let mut chain = generator.generate(State::new(Params::instance().numclients, 0, 0, 0, 0)).adj_matrix();
    let mut solver = Solver::new(chain).as_ctmc().add_normalization_condition().solve();

    let mut solution = Solution::from_solution_vector(&solver, &generator.ordered());

    println!("{}",solution);
    
    if cli.print_graph{
        println!("///////\n{}",generator.to_dot());
    }

}
