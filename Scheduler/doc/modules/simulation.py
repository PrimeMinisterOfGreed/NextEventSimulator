import io
import os
import subprocess
import pandas as pd

class SimulatorCommander():

    def __init__(self,command) -> None:
        self.results = []
        self.command = command
        self.preamble = ""
        self.postexecution = ""
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
    
    def add_command(self,command, postexecution=False):
        if postexecution:
            self.preamble += "{};".format(command)
            pass
        else:
            self.postexecution += "{};".format(command)
            pass
        pass

    def run(self) -> subprocess.Popen:
        script = self.generate_command()[:-1] 
        self.preamble = ""
        self.postexecution = ""
        args = ["--no-log" if not self.enable_log else "","--program","{}".format(script)]
        cmd= "{} {} --program \'{}\'".format( self.command,"--no-log" if not self.enable_log else "",script)
        process = subprocess.Popen(executable=self.command,args=args,stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        self.processes.append(process)
        return process
    
    def collect(self):
        for process in self.processes:
            process.wait()
            out, err = process.communicate()
            self.results.append(process.communicate(out))
            pass
        pass


    def fetch_data(self,iterations: int, scenario: str, starting_seed = 123456789):
        seed = starting_seed
        self.scenario = scenario
        data = pd.DataFrame()
        if os.path.exists("./data/{}.csv".format(scenario)):
            data = pd.read_csv("./data/{}.csv".format(scenario),sep=";",index_col=0)
            return data
        for i in range(iterations):
            simdata = pd.DataFrame()
            self.seed = seed+i
            p=self.run()
            p.wait()
            out,err = p.communicate()
            simdata = pd.read_csv(io.BytesIO(b'Station;Measure;R;LB;HB;Samples;Precision;Expected\n' + out),encoding="utf8",sep=";")
            simdata["Simulation"] = [i+1]*len(simdata)
            simdata["Seed"] = [seed+i]*len(simdata)
            data = pd.concat([data,simdata],ignore_index=True)
            pass
        if not os.path.exists("./data/{}.csv".format(scenario)):
            data.to_csv("./data/{}.csv".format(scenario),sep=';')
            pass
        return data
    pass



if __name__ == "__main__":
    comm = SimulatorCommander("C:/Users/matteo.ielacqua/OneDrive - INPECO SPA/Desktop/Personal/NextEventSimulator/build/Scheduler/scheduler.exe")
    comm.enable_logger(False)
    comm.scenario = "Simplified"
    p = comm.run()
    p.wait()
    out,err = p.communicate()
    print(out)
    pass