use nalgebra::SMatrix;
use num_traits::Num;

#[derive(PartialEq)]
pub enum StationType {
    Delay = 0,
    LoadIndipendent = 1,
}


#[derive(Debug, Clone)]
pub struct Mva<const Clients: usize, const NumOfStations: usize> {
    mean_clients: SMatrix<f64, NumOfStations, Clients>,
    mean_waits: SMatrix<f64, NumOfStations, Clients>,
    throughputs: SMatrix<f64, NumOfStations, Clients>,
    utilizations: SMatrix<f64, NumOfStations, Clients>,
}

impl<const Clients: usize, const NumOfStations: usize> Mva<Clients, NumOfStations> {
    pub fn new() -> Self {
        Mva {
            mean_clients: SMatrix::zeros(),
            mean_waits: SMatrix::zeros(),
            throughputs: SMatrix::zeros(),
            utilizations: SMatrix::zeros(),
        }
    }

    pub fn run(&mut self,
        stations: [StationType; NumOfStations],
        visits: [f64; NumOfStations],
        service_time: [f64; NumOfStations],
    ) -> &mut Self {

        for k in 1..Clients{
            for i in 0..NumOfStations{
                if stations[i] == StationType::Delay{
                    self.mean_waits[(i,k)] = service_time[i];
                }
                else{
                    self.mean_waits[(i,k)] = service_time[i] * (1.0 + self.mean_waits[(i,k-1)]);
                }
            }

            let mut sum = 0.0;
            for i in 0..NumOfStations{
                sum += visits[i]*self.mean_waits[(i,k)];
            }
            let xref = k as f64/sum;

            for i in 0..NumOfStations{
                self.throughputs[(i,k)] = xref*visits[i];
                if stations[i] == StationType::Delay{
                    self.mean_clients[(i,k)] = service_time[i]*self.throughputs[(i,k)];
                    self.utilizations[(i,k)] = self.mean_clients[(i,k)]/k as f64;
                }
                else {
                    self.utilizations[(i,k)] = service_time[i]*self.throughputs[(i,k)];
                    self.mean_clients[(i,k)]= self.utilizations[(i,k)]*(1.0+ self.mean_clients[(i,k-1)]);
                }
            }
        }
        self
    }
}




#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_on_standard() {
        
    }
}