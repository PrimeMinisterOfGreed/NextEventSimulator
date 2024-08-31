use std::alloc::System;

use crate::state;

pub struct Params {
    pub alpha: f64,
    pub beta: f64,
    pub u1: f64,
    pub u2: f64,
    pub thinktime: f64,
    pub timeslice: f64,
    pub sio1: f64,
    pub sio2: f64,
    pub qio1: f64,
    pub qio2: f64,
    pub qoutd: f64,
    pub qouts: f64,
    pub numclients: u8,
}

impl Params {
    pub  fn instance() -> &'static mut Self {
        unsafe {
            static mut parameters: Params = Params {
                alpha: 0.8,
                beta: 0.2,
                u1: 15.0,
                u2: 75.0,
                thinktime: 5000.0,
                timeslice: 3.0,
                sio1: 40.0,
                sio2: 180.0,
                qio1: 0.65,
                qio2: 0.25,
                qoutd: 0.1 * 0.4,
                qouts: 0.1 * 0.6,
                numclients: 3,
            };
            &mut parameters
        }
    }
}
