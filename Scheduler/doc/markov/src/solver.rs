use core::fmt;
use std::{borrow::{Borrow, BorrowMut}, fmt::Pointer};

use indicatif::ProgressBar;
use lstsq::{lstsq, Lstsq};
use nalgebra::{DMatrix, DVector, Dyn};

use crate::{parameters::Params, state::State};

pub struct Solver {
    mat: DMatrix<f64>,
}

impl Solver {
    pub fn new(mat: DMatrix<f64>) -> Self {
        Solver { mat }
    }

    pub fn as_ctmc(&mut self) -> &mut Self {
        let mut mat = &mut self.mat;
        let mut progress = ProgressBar::new(mat.nrows() as u64).with_message("Balancing matrix");
        for i in 0..mat.nrows() {
            mat[(i, i)] = 0.0;
            mat[(i, i)] = -mat.row(i).sum();
            progress.inc(1);
        }
        self
    }

    pub fn add_normalization_condition(&mut self) -> &mut Self {
        let mut mat = &mut self.mat.transpose();
        let t = mat.clone().insert_row(mat.nrows(), 1.0);
        self.mat = t;
        self
    }

    pub fn mat(&self)-> &DMatrix<f64>{
        &self.mat
    }

    pub fn solve(&mut self) -> Lstsq<f64, Dyn> {
        let mut mat = &mut self.mat;
        let mut b = DVector::<f64>::zeros(mat.nrows());
        b[mat.nrows() - 1] = 1.0;
        let x = lstsq(mat, &b, 1e-14);
        x.ok().unwrap()
    }
}

#[derive(Clone, Debug)]
pub struct Solution {
    n_delay: f64,
    n_cpu: f64,
    n_io1: f64,
    n_io2: f64,
    stage_1 : f64,
    stage_2 : f64,
    n_delay_distrib: Vec<f64>,
    n_cpu_distrib: Vec<f64>,
    n_io1_distrib: Vec<f64>,
    n_io2_distrib: Vec<f64>,
    stage_1_distrib: Vec<f64>,
    stage_2_distrib: Vec<f64>,
}

impl Solution {
    fn new() -> Self {
        let mut array = vec![0.0;Params::instance().numclients as usize + 1];
        Solution {
            n_delay: 0.0,
            n_cpu: 0.0,
            n_io1: 0.0,
            n_io2: 0.0,
            stage_1: 0.0,
            stage_2 : 0.0,
            n_delay_distrib: array.clone(),
            n_cpu_distrib: array.clone(),
            n_io1_distrib: array.clone(),
            n_io2_distrib: array.clone(),
            stage_1_distrib: array.clone(),
            stage_2_distrib: array,
        }
    }
    pub fn from_solution_vector(
        solution_vector: &Lstsq<f64, Dyn>,
        node_vector: &Vec<State>,
    ) -> Self {
        let mut sol = Self::new();
        let vec = &solution_vector.solution;
        let mut progress = ProgressBar::new(vec.len() as u64).with_message("Computing solution");
        assert!(
            solution_vector.solution.len() == node_vector.len(),
            "Solution vector:{} and node vector:{} have different size",
            vec.len(),
            node_vector.len()
        );
        for i in 0..vec.len(){
            sol.n_delay_distrib[node_vector[i].n_delay as usize] += vec[i];
            sol.n_cpu_distrib[node_vector[i].n_cpu as usize] += vec[i];
            sol.n_io1_distrib[node_vector[i].n_io1 as usize] += vec[i];
            sol.n_io2_distrib[node_vector[i].n_io2 as usize] += vec[i];
            if node_vector[i].cpu_stage == 1 {
                sol.stage_1_distrib[node_vector[i].n_delay as usize] += vec[i];
                sol.stage_1 += vec[i];
            } else {
                sol.stage_2_distrib[node_vector[i].n_delay as usize] += vec[i];
                sol.stage_2+= vec[i];
            }
            sol.n_delay += vec[i] * node_vector[i].n_delay as f64;
            sol.n_cpu += vec[i] * node_vector[i].n_cpu as f64;
            sol.n_io1 += vec[i] * node_vector[i].n_io1 as f64;
            sol.n_io2 += vec[i] * node_vector[i].n_io2 as f64;
            progress.inc(1);
        }
        sol
    }
}


impl fmt::Display for Solution{
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        writeln!(f, "Delay: {:.6}, CPU: {:.6}, IO1: {:.6}, IO2: {:.6}, Stage1:{:.6}, Stage2:{:.6}", self.n_delay, self.n_cpu, self.n_io1, self.n_io2,self.stage_1,self.stage_2)?;
        writeln!(f, "Delay D: {:?}", self.n_delay_distrib)?;
        writeln!(f, "Cpu D: {:?}", self.n_cpu_distrib)?;
        writeln!(f, "Io1 D: {:?}", self.n_io1_distrib)?;
        writeln!(f, "Io2 D: {:?}", self.n_io2_distrib)?;
        writeln!(f, "Stage1 D: {:?}", self.stage_1_distrib)?;
        writeln!(f, "Stage2 D: {:?}", self.stage_2_distrib)
        
    }
}

#[cfg(test)]
mod tests {
    use crate::{
        generator::{self, ChainGenerator},
        parameters::Params,
        state::State,
        transition::TransitionType,
    };

    use super::*;

    #[test]
    fn test_matrix_solutor() {
        let mut generator = ChainGenerator::new();
        Params::instance().numclients = 1;
        let mut chain = generator.generate(State::new(1, 0, 0, 0, 0));
        let mut graph = chain.graph();
        let mut binding = Solver::new(graph.adj_matrix());
        let mut solver = binding.as_ctmc().add_normalization_condition();
        let mut resolved = solver.solve();
        println!("{:?}", resolved.solution);

        let mut solution = Solution::from_solution_vector(&resolved, generator.ordered());
        println!("{:?}", solution);

        let node_ref = graph.nodes()[1];
        let mut tail_example = 0;
        for i in 2..graph.nodes().len() {
            let transition = graph.get_transition(&node_ref, &generator.ordered()[i]);
            if transition.is_none() {
                continue;
            }
            let t = transition.unwrap();
            if *t.t_type() == TransitionType::CpuToSelf
                && t.head().n_cpu > 0
                && t.tail().n_cpu > 0
                && t.tail() != t.head()
            {
                tail_example = i;
            }
        }

        assert!(solver.mat[(1, tail_example)] > 0.0);
    }
}
