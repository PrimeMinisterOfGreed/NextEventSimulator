use crate::state::State;



enum TransitionType{
    Unknown = -1,
    CpuToIo1= 1,
    CpuToIo2 = 2,
    CpuToSelf = 3,
    CpuToDelay = 4,
    DelayToCpu = 5,
    Io1ToCpu = 6,
    Io2ToCpu = 7
}

struct Transition{
    t_type: TransitionType,
    head:State,
    tail:State
}

impl Transition {
    pub fn new(head:State, tail:State) -> Self{
        Transition{
            head,
            t_type: TransitionType::Unknown,
            tail
        }
    }

    pub fn detect(&mut self) -> TransitionType{
        
    }


}