use std::fmt::Formatter;

use ndarray::{Array, AsArray, Dim, Ix1};

use crate::parameters::Params;

const STATE_DESCRIPTOR : [&str;5]= ["ndelay","ncpu","nio1","nio2","cpustage"];

#[derive(Clone, Copy, Debug,PartialEq)]
pub struct State {
    pub n_delay: u8,
    pub n_cpu: u8,
    pub n_io1: u8,
    pub n_io2: u8,
    pub cpu_stage: u8,
}

impl State {
    pub fn new(n_delay: u8, n_cpu: u8, n_io1: u8, n_io2: u8, cpu_stage: u8) -> Self {
        State {
            n_delay,
            n_cpu,
            n_io1,
            n_io2,
            cpu_stage,
        }
    }

    pub fn as_array(&self) -> Array<u8, Ix1> {
        ndarray::Array::<u8, Ix1>::from_vec(vec![self.n_delay, self.n_cpu, self.n_io1, self.n_io2])
    }

    pub fn is_valid(&self) -> bool {
        return (self.as_array().sum() as u32) == Params::instance().numclients;
    }

    pub fn partial_eq(&self, other : &State) -> bool{
        self.as_array() == other.as_array()
    }
}


