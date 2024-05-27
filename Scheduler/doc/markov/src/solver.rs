use std::borrow::{Borrow, BorrowMut};

use lstsq::{lstsq, Lstsq};
use nalgebra::{DMatrix, DVector, Dyn};

use crate::state::State;

pub struct Solver{
    mat:  DMatrix<f64>,
}

impl Solver {
    pub fn new(mat: DMatrix<f64>) -> Self {
        Solver { mat }
    }

    pub fn as_ctmc(&mut self) -> &mut Self {
        let mut mat = &mut self.mat;
        for i in 0..mat.nrows() {
            mat[(i, i)] = 0.0;
            mat[(i, i)] = -mat.row(i).sum();
        }
        self
    }

    pub fn add_normalization_condition(&mut self) -> &mut Self{
        let mut mat = &mut self.mat.transpose();
        let t = mat.clone().insert_row(mat.nrows(), 1.0);
        self.mat = t;
        self
    }

    pub fn solve(&mut self) -> Lstsq<f64,Dyn>{
        let mut mat = &mut self.mat;
        let mut b = DVector::<f64>::zeros(mat.nrows());
        b[mat.nrows() -1] = 1.0;
        let x = lstsq(mat, &b, 1e-14);
        x.ok().unwrap()
    }

}

#[derive(Clone,Debug)]
pub struct Solution{
    n_delay: f64,
    n_cpu: f64,
    n_io1: f64,
    n_io2: f64
}

impl Solution {
    fn new()->Self{
        Solution{n_delay: 0.0, n_cpu: 0.0, n_io1: 0.0, n_io2: 0.0}
    }
    pub fn from_solution_vector(solution_vector: &Lstsq<f64,Dyn>, node_vector: &Vec<State>)-> Self{
        let mut sol = Self::new();
        let vec = &solution_vector.solution;
        assert!(solution_vector.solution.len() == node_vector.len(),"Solution vector:{} and node vector:{} have different size",vec.len(),node_vector.len());
        for i in 0..vec.len(){
            sol.n_delay += vec[i] * node_vector[i].n_delay as f64;
            sol.n_cpu += vec[i] * node_vector[i].n_cpu as f64;
            sol.n_io1 += vec[i] * node_vector[i].n_io1 as f64;
            sol.n_io2 += vec[i] * node_vector[i].n_io2 as f64;
        }
        sol
    }
}

#[cfg(test)]
mod tests {
    use crate::{generator::{self, ChainGenerator}, parameters::Params, state::State};

    use super::*;

    #[test]
    fn test_matrix_solutor() {
        let mut generator = ChainGenerator::new();
        Params::instance().numclients = 1;
        let mut chain = generator.generate(State::new(1, 0, 0, 0, 0)).adj_matrix();
        let mut solver = Solver::new(chain).as_ctmc().add_normalization_condition().solve();
        println!("{:?}", solver.solution);

        let mut solution = Solution::from_solution_vector(&solver, generator.ordered());
        println!("{:?}",solution);

    }
}