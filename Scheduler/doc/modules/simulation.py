import os
import subprocess
scheduler_command = "../../build/Scheduler/scheduler"

class SimulatorCommander():

    def __init__(self) -> None:
        self.result = ""
        self.stationToRecord = ["CPU","IO1","IO2","SWAP_IN"]
        self.measureToRecord = ["ActiveTime"]
        self.scenario = "Default"
        self.seed = 123456789
        self.enable_log = False
        self.processes = [subprocess.Popen]
        pass
    
    def enable_logger(self,value):
        self.enable_log = value
        pass

    def generate_command(self):
        script = "seed {}; scenario {}; init; nr -1;".format(self.seed,self.scenario)
        for station in self.stationToRecord:
            script += "queryMeasure {} N; queryMeasure {} W;".format(station,station)
            pass
        for measure in self.measureToRecord:
            script += "queryMeasure {};".format(measure)
            pass
        return script

    def set_seed(self,seed):
        self.seed = seed
        pass
    
    def run(self) -> subprocess.Popen:
        script = self.generate_command()
        cmd=scheduler_command +  " {} --program \'{}\'".format( "--no-log" if not self.enable_log else "",script)
        process = subprocess.Popen(cmd,shell=True)
        self.processes.append(process)
        return process
    
    def collect_and_wait(self):
        for process in self.processes:
            self.result += str(process.stdout)
            pass
        pass
    pass


if __name__ == "__main__":
    comm = SimulatorCommander()
    comm.enable_logger(True)
    p = comm.run()
    p.wait()
    print(p.stdout)
    pass