use crate::parameters::Params;
use nalgebra::{SVector, Vector, Vector1};
use num_derive::FromPrimitive;
use std::{
    fmt::{self, format, Display, Formatter},
    iter::Sum,
    ops::Sub,
};

#[derive(FromPrimitive, PartialEq)]
pub enum StateDescriptor {
    Ndelay = 0,
    Ncpu = 1,
    Nio1 = 2,
    Nio2 = 3,
}

#[derive(Clone, Copy)]
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

    pub fn as_array(&self) -> SVector<i8, 4> {
        SVector::<i8, 4>::from_row_slice(&[
            self.n_delay as i8,
            self.n_cpu as i8,
            self.n_io1 as i8,
            self.n_io2 as i8,
        ])
    }

    pub fn is_valid(&self) -> bool {
        return self.as_array().sum() as u8 == Params::instance().numclients
            && if self.n_cpu > 0 {
                self.cpu_stage == 1 || self.cpu_stage == 2
            } else {
                true
            };
    }

    pub fn partial_eq(&self, other: &State) -> bool {
        self.as_array() == other.as_array()
    }
}

pub trait GraphElement {
    fn to_graph_element(&self)-> String;
}

impl GraphElement for State {
    
    fn to_graph_element(&self) -> String {
        format!(
            "{}-{}-{}-{}",
            self.n_delay,
            if self.n_cpu > 0 {
                format!("{}.{}", self.n_cpu, self.cpu_stage)
            } else {
                "0".to_string()
            },
            self.n_io1,
            self.n_io2
        )
    }
}

impl PartialEq for State {
    fn eq(&self, other: &Self) -> bool {
        self.n_delay == other.n_delay
            && self.n_cpu == other.n_cpu
            && self.n_io1 == other.n_io1
            && self.n_io2 == other.n_io2
            && if self.n_cpu > 0 {
                self.cpu_stage == other.cpu_stage
            } else {
                true
            }
    }
}

impl Sub for State {
    fn sub(self, rhs: Self) -> Self::Output {
        return self.as_array() - rhs.as_array();
    }

    type Output = SVector<i8, 4>;
}

impl Display for State {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        f.write_fmt(format_args!(
            "Delay:{},Cpu:{},Io1:{},Io2:{},Stage:{}",
            self.n_delay, self.n_cpu, self.n_io1, self.n_io2, self.cpu_stage
        ))
    }
}

#[cfg(test)]
mod tests {

    use super::*;

    #[test]
    fn test_instance() {
        let state = State::new(3, 0, 0, 0, 1);
        Params::instance().numclients = 3;
        assert!(state.is_valid());
    }

    #[test]
    fn test_partial_eq() {
        let s1 = State::new(1, 1, 1, 0, 1);
        let s2 = State::new(1, 1, 1, 0, 2);
        let mut sum = s1.as_array().sum();
        assert!(s1.is_valid());
        assert!(s2.is_valid());
        assert!(s1.partial_eq(&s2));
        assert!(s1 != s2);
    }

    #[test]
    fn test_full_eq() {
        let s1 = State::new(3, 0, 0, 0, 1);
        let s2 = State::new(3, 0, 0, 0, 2);
        assert!(s1.partial_eq(&s2) && s1 == s2)
    }

    #[test]
    fn test_sub_state() {
        let s1 = State::new(3, 0, 0, 0, 1);
        let s2 = State::new(3, 0, 0, 0, 2);
        assert!((s1 - s2) == SVector::<i8, 4>::zeros());
    }
}
