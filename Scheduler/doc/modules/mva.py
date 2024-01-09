from typing import Any
import numpy as np
from enum import Enum

class StationType(Enum):
    Delay = 1
    LoadIndependent = 2
    pass

class MVA:
    def __init__(self, matrix:np.ndarray[np.double] ,serviceTimes:np.ndarray[np.double], types:[StationType],N:int):
        self.meanclients = None
        self.meanwaits = None
        self.throughputs = None
        self.utilizations = None
        self.visits = None
        self.serviceTimes = serviceTimes
        self.types = types 
        self.matrix = matrix
        self.N = N
        pass

    def mvalid(self, visits:np.ndarray[np.double], serviceTimes:np.ndarray[np.double], types: [StationType], N: int):
        M = len(self.visits)
        self.meanclients = np.zeros((M,N))
        self.meanwaits = np.zeros((M,N))
        self.throughputs = np.zeros((M,N))
        self.utilizations = np.zeros((M,N))
        for k in range(1,N):
            for i in range(0,M):
                if self.types[i] == StationType.Delay:
                    self.meanwaits[i,k] = serviceTimes[i]
                    pass
                else:
                    self.meanwaits[i,k]=serviceTimes[i]*(1+self.meanclients[i,k-1])
                    pass
                pass
            sum = 0.0
            for i in range(0,M):
                sum += visits[i] * self.meanwaits[i,k]
                pass
            xref = k/sum 
            for i in range(0,M):
                self.throughputs[i,k] = self.visits[i]*xref
                if types[i] == StationType.Delay:
                    self.meanclients[i,k] = self.serviceTimes[i]*self.throughputs[i,k]
                    self.utilizations[i,k]= self.meanclients[i,k]/k 
                    pass
                else:
                    self.utilizations[i,k] = self.serviceTimes[i]*self.throughputs[i,k]
                    self.meanclients[i,k] = self.utilizations[i,k]*(1+self.meanclients[i,k-1])
                    pass
                pass
            pass
        return self
    
    def load_visits(self, matrix:np.ndarray, iterations: int):
        self.visits = np.array([0] * len(matrix))
        self.visits[0] = 1
        newmat = matrix.astype(np.double)
        old = newmat.astype(np.double)
        for i in range(iterations):
            old = newmat.astype(np.double)
            newmat = old.astype(np.double) @ old.astype(np.double)
            newmat = newmat.round(10) #needed because overflow may occur
            pass
        self.visits = self.visits @ newmat
        for i in range(1,len(self.visits)):
            self.visits[i] = self.visits[i]*1/self.visits[0]
            pass
        self.visits[0] = 1
        return self
    
    def __call__(self):
        self.load_visits(self.matrix,10000)
        self.mvalid(self.visits,self.serviceTimes,self.types,self.N)
        return self
    


