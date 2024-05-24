
use nalgebra::SVector;
use rustworkx_core::petgraph::{graph::Node, Graph};

use crate::{parameters::Params, state::State, transition::Transition};



struct NodeGenerator{
    generated: Vec<State>
}


impl NodeGenerator {
    pub fn new()-> Self{
        return NodeGenerator{
            generated: Vec::new()
        };
    }

    pub fn add_state(&mut self,state: &State){
        if state.is_valid(){
            self.generated.push(state.clone());
        }
    }

    pub fn generate(&mut self){
        let mut cursor = SVector::<u8,4>::zeros();
        while cursor[0] != Params::instance().numclients{
            for i in (0..4).rev(){
                if cursor[i] < Params::instance().numclients{
                    cursor[i]+=1;
                    break;
                }
                else{
                    cursor[i] = 0;
                }
            }
            let mut state = State::new(cursor[0], cursor[1], cursor[2], cursor[3], 1);
            self.add_state(&state);
            if state.n_cpu > 0{
                state.cpu_stage = 2;
                self.add_state(&state)
            }   
        }
    }
}


pub struct ChainGenerator{
    node_generator: NodeGenerator,
    graph: Graph<State,Transition>,
    ordered: Vec<State>
}


impl ChainGenerator {
    pub fn new()-> Self{
        ChainGenerator{
            graph: Graph::new(),
            node_generator: NodeGenerator::new(),
            ordered: Vec::new()
        }
    }

    fn next(){
        
    }


}




#[cfg(test)]
mod tests {
    use std::fmt::Debug;

    use super::*;

    #[test]
    fn test_node_generator() {
        let mut generator = NodeGenerator::new();
        Params::instance().numclients = 3;
        generator.generate();
        assert!(generator.generated.len() == 30);
    }
}