import numpy as np 
import numpy.random as npr
import networkx as nx
import matplotlib as plt
import plotly.graph_objects as go
import pydtmc
import sys 
import plotly.express as plt
import graphviz
import pandas as pd
from mva import *

# class that describes all parameters for the model
class Params:
    alpha = 0.8
    beta = 0.2
    u1 = 15
    u2 = 75
    thinkTime = 5000
    timeSlice = 3
    Sio1 = 40
    Sio2 = 180
    qio1 = 0.65 #route to io1
    qio2 = 0.25 # route to io2
    qoutd = 0.04 #go to delay station
    qouts = 0.06 # renter the system
    numClients = 3
    pass



# this class represent a state in the chain
class State:
    def __init__(self,Ndelay, Ncpu, Nio1, Nio2,cpuStage = 0) -> None:
        self.Ndelay = Ndelay
        self.Ncpu = Ncpu
        self.cpuStage = cpuStage
        self.Nio1 = Nio1
        self.Nio2 = Nio2
        # this array serves as view for iterating through the stations
        self.state= [Ndelay,Ncpu,Nio1,Nio2,cpuStage]
        # this array serves as descriptor for the previous field
        self.descriptor =["Ndelay","Ncpu","Nio1","Nio2","cpuStage"]
        pass
    
    # a state is partially equal if the clients in all states are equal
    def partially_eq(self, state):
       return self.state[:-1] == state.state[:-1]

    # a state is valid when the sum of all the clients in station equals N
    def isValid(self)->bool:
            return (self.Ncpu + self.Nio1 + self.Nio2 + self.Ndelay) == Params.numClients
    
    # string representation for visualization in graphviz
    def __str__(self) -> str:
        return ("{},{},{},{}".format(
            self.Ndelay,
            self.Ncpu if self.Ncpu == 0 else "{}.{}".format(self.Ncpu,self.cpuStage),
            self.Nio1,
            self.Nio2
        ))
    
    def __call__(self, index : int) -> str:
       
       pass
    
    def __getitem__(self, index:int):
       return self.state[index]

    def __len__(self):
       return len(self.state) - 1
    
    def __eq__(self, __value: object) -> bool:
       return str(self) == str(__value)
    def __hash__(self) -> int:
       return hash(str(self))
    pass

# class that describes all possible transitions
class Transition:
   
   # mark for transition type
   class TransitionType:
      UNKNOWN = -1
      CPU_TO_IO1 = 1
      CPU_TO_IO2 = 2
      IO2_TO_CPU = 4
      IO1_TO_CPU = 5
      DELAY_TO_CPU = 6
      CPU_TO_DELAY = 7
      CPU_TO_SELF = 8
      pass
   
   def __init__(self, head: State, tail : State) -> None:
      self.head = head
      self.tail = tail
      self.type = Transition.TransitionType.UNKNOWN
      self.detectType()
      pass
   
   # check if a transition is valid. A transition is valid only if the head and the tail 
   # had an Si - Sj == 1 and an Si - Sj == -1 and all other Si - Sj == 0
   def transitionIsValid(self):
      #check cpu stage validity
      if self.head.cpuStage != self.tail.cpuStage:
         if self.head.Ncpu < self.tail.Ncpu and self.tail.Ncpu > 0 and self.head.Ncpu > 0: return False
         pass
      # return diff without last item that is cpu stage
      diff = np.array(self.head.state[:-1]) - np.array(self.tail.state[:-1])
      counter = 0
      for i in diff:
         if abs(i) > 0:
            counter+=1
         pass
      if counter > 2: return False
      return diff.max() == 1 and diff.min() == -1 and diff.sum() == 0 
   
   # search for increment / decrement in the states, discarding ones not valid
   def detectMovement(self) -> tuple[str,str]:
      diff = np.array(self.head.state[:-1]) - np.array(self.tail.state[:-1])
      if self.head.partially_eq(self.tail):
         return ("","")
      return (self.head.descriptor[diff.argmax()], self.head.descriptor[diff.argmin()])

   # detect the type of transition from the decrement, increment values
   def detectType(self):
      if not self.transitionIsValid() and not self.head.partially_eq(self.tail) : return
      (decrement,increment)= self.detectMovement()
      if decrement == "ERROR" or increment == "ERROR" : return 
      elif decrement == "Ncpu" and increment == "Nio1": self.type = Transition.TransitionType.CPU_TO_IO1
      elif decrement == "Ncpu" and increment == "Nio2" : self.type = Transition.TransitionType.CPU_TO_IO2
      elif decrement == "Nio1" and increment == "Ncpu" : self.type = Transition.TransitionType.IO1_TO_CPU
      elif decrement == "Nio2" and increment == "Ncpu" : self.type = Transition.TransitionType.IO2_TO_CPU
      elif decrement == "Ndelay" and increment == "Ncpu" : self.type = Transition.TransitionType.DELAY_TO_CPU
      elif decrement == "Ncpu" and increment == "Ndelay" : self.type = Transition.TransitionType.CPU_TO_DELAY
      elif decrement == "" and increment == "" and self.head.partially_eq(self.tail) and self.head.Ncpu > 0 and self.head.cpuStage != self.tail.cpuStage: self.type = Transition.TransitionType.CPU_TO_SELF
      pass
   
   # calculate the P of the transition using the type
   def p(self):
      assert(self.type != Transition.TransitionType.UNKNOWN)
      if self.type == Transition.TransitionType.CPU_TO_IO1 or self.type == Transition.TransitionType.CPU_TO_IO2: return self.CpuToIo()
      elif self.type == Transition.TransitionType.IO1_TO_CPU or self.type ==  Transition.TransitionType.IO2_TO_CPU: return self.IoToCpu()
      elif self.type == Transition.TransitionType.DELAY_TO_CPU: return self.DelayToCpu()
      elif self.type == Transition.TransitionType.CPU_TO_DELAY : return self.CpuToDelay()
      elif self.type == Transition.TransitionType.CPU_TO_SELF: return self.CpuToSelf()
      pass



   
   def cpu_on_leave_stage_selector(self):
     
      #return (1 if self.tail.Ncpu == 0 else (Params.alpha if self.tail.cpuStage == 1 else Params.beta))
      if self.tail.Ncpu == 0:
         return 1
      elif self.tail.cpuStage == 1:
         return Params.alpha
      elif self.tail.cpuStage == 2:
         return Params.beta
      raise "Invalid stage specifier"
      pass
   
   def cpu_on_arrive_stage_selector(self):
  
      #return (1 if self.head.Ncpu > 0 else Params.alpha if self.tail.cpuStage == 1 else Params.beta)
      if self.head.Ncpu > 0:
         return 1
      elif self.tail.cpuStage == 1:
         return Params.alpha
      elif self.tail.cpuStage == 2:
         return Params.beta 
      raise "invalid stage specifier"
      pass

   # define the CPU leave function 
   def cpu_leave(self):
      #return Params.u1 if self.head.cpuStage == 1 else Params.u2
      assert self.head.Ncpu != 0
      if self.head.cpuStage == 1:
         return Params.u1
      elif self.head.cpuStage == 2:
         return Params.u2
      raise "Invalid stage specifier"
      pass
      
   

   def DelayToCpu(self):
      return (1/Params.thinkTime)*self.head.Ndelay*self.cpu_on_arrive_stage_selector()

      

   def CpuToIo(self): 
      a = 1
      if self.type == Transition.TransitionType.CPU_TO_IO1:
         a = Params.qio1
         pass
      elif self.type == Transition.TransitionType.CPU_TO_IO2:
         a = Params.qio2
         pass
      else:
         raise "Invalid transition" 
      return 1/self.cpu_leave() * self.cpu_on_leave_stage_selector() * a
   
   def CpuToDelay(self):
      return 1/self.cpu_leave() * self.cpu_on_leave_stage_selector() * Params.qoutd
   
   def CpuToSelf(self):
      return 1/self.cpu_leave() * self.cpu_on_leave_stage_selector() *Params.qouts + 1/Params.timeSlice*self.cpu_on_leave_stage_selector()*self.cpu_on_arrive_stage_selector()
      
   
   def IoToCpu(self):
      #return (1/(Params.Sio1 if self.type == Transition.TransitionType.CPU_TO_IO1 else Params.Sio2))*self.CpuA()
      a = 1
      if self.type == Transition.TransitionType.IO1_TO_CPU:
         a = 1/Params.Sio1
         pass
      elif self.type == Transition.TransitionType.IO2_TO_CPU:
         a=  1/Params.Sio2
         pass
      else:
         raise "Invalid Transition"
      return a  * self.cpu_on_arrive_stage_selector()
      pass


# support class that prints graph in to a viewable object in graphviz
class Printer:
   def nx_to_graphviz(nxgraph : nx.DiGraph):
      graph = graphviz.Digraph()
      for node in nxgraph.graph.nodes:
        graph.node(node)
        pass
      for edge in nxgraph.graph.edges:
       graph.edge(edge[0],edge[1],str(round(nxgraph.graph.get_edge_data(edge[0],edge[1])["weight"],6)))
       pass
      return graph
   pass


# function that enumerates all possible nodes with a given CPU stage
def stage_enumerator(stage: int) -> list[State]:
    Ndelay = 0
    Ncpu = 1
    Nio1 = 2
    Nio2 = 3
    cpuStage = stage
    stages = [0,0,0,0]
    result = []
    while stages[Ndelay] <= Params.numClients:
        for i in reversed(range(4)):
            stages[i] += 1
            if stages[i] <= Params.numClients: break
            elif i > 0 : stages[i] = 0
            pass
        state= State(stages[Ndelay], stages[Ncpu], stages[Nio1],stages[Nio2],stage)
        if state.isValid(): result.append(state)
        pass
    return result

#enumerate all possible nodes in the grapg
def node_enumerator() -> list[State]:
    #enumerate all possible nodes with stage 1 then remove the duplicated one generated with stage 2
    nodes = stage_enumerator(1)
    for newnode in stage_enumerator(2):
        if newnode not in nodes:
            nodes.append(newnode)
            pass
        pass
    return nodes


# enumerate all possible transition in the chain
def edge_enumerator() -> list[tuple[State,State]]:
    nodes = node_enumerator()
    result = []
    for head in nodes:
        for tail in nodes:
            if Transition(head,tail).type != Transition.TransitionType.UNKNOWN and not (head,tail) in result:
                result.append((head,tail))
                pass
            pass
        pass
    return result

# wrapper for networkx digraph class
class DiGraph():
   
   def __init__(self):
     self.graph = nx.digraph.DiGraph()
     self.lastHead = ""
     self.lastTail = ""
     self.calls = 0
     pass

   def gen(self,headLabel, tailLabel, p):
    if (headLabel,tailLabel) in self.graph.edges:
      #print("Warning redundant edge {} , {} , call N {}, inW {}, newW{}".format(headLabel,tailLabel,self.calls,self.graph.get_edge_data(headLabel,tailLabel)["weight"],p))
      pass
    if headLabel not in self.graph.nodes:
     # print("Warning head {} not listed".format(headLabel))
      pass
    if tailLabel not in self.graph.nodes:
     # print("Warning tail {} not listed".format(tailLabel))
      pass
    self.lastHead = headLabel
    self.lastTail = tailLabel 
    self.graph.add_edge(headLabel,tailLabel,weight=p)
    return (headLabel,tailLabel)

   def Graph(self):
     return self.graph
   

   def __and__(self,arg:tuple[str,str,float]):
     return self.gen(arg[0],arg[1],arg[2])
  
   
   def __getitem__(self,index):
     return self.graph.edges()[index]
   
   def add_edge(self,head,tail,p):
     self.gen(head() if callable(head) else head, tail() if callable(tail) else tail, p)
     pass


   
   def __call__(self, head,tail,pout,pin =0):
     if callable(head): 
       head = head()
       pass
     if callable(tail):
       tail = tail()
       pass
     self.add_edge(head,tail,pout)
     if pin > 0:
      self.add_edge(tail,head,pin)
      pass
     self.calls += 1
     return self
   
   def add_edges(self,edges: list[tuple]):
     for edge in edges:
       head = ""
       tail = ""
       if callable(edge[0]): head = edge[0]()
       else: head=edge[0]
       if callable(edge[1]): tail = edge[1]()
       else: tail = edge[1]
       self.gen(head,tail,edge[2])
       pass
     pass
   
   def add_node(self,node:str):
     self.graph.add_node(node)
     pass
   
   
   def last_tail(self): return self.lastTail
   def last_head(self) : return self.lastHead

   
   
   def fig(self):
     pos = nx.draw_planar(self.graph)
     fig = nx.draw_networkx_nodes(self.graph,pos,node_size=1000)
     fig = nx.draw_networkx_labels(self.graph,pos)
     fig = nx.draw_networkx_edges(self.graph,pos)
     fig = nx.draw_networkx_edge_labels(self.graph,pos)
     pass
   pass

# helper for generate the chain using DFS algorithm
class ChainGenerator:
   
   def __init__(self, nodes : list[State]) -> None:
      self.nodes = nodes
      self.edges : list[Transition] = []
      self.queue = []
      self.ordered = []   
      pass

   #extract a state from the queue and enumerate all possible transition from that state
   def compute_next(self):
      ref = self.queue[0]
      self.queue.remove(ref)
      if ref in self.ordered: return
      self.ordered.append(ref)
      for node in self.nodes:
         tr = Transition(ref,node)


         if tr.type != Transition.TransitionType.UNKNOWN:
            self.edges.append(tr)
            if not node in self.ordered:
               self.queue.append(node)
               pass
            pass
         pass
      pass

   # setup the chain and compute transition unless the frontier is empty
   def __call__(self, firstNode : State):
      self.queue.append(firstNode)
      while len(self.queue) > 0:
         self.compute_next()
         pass
      pass


   def chain(self) -> DiGraph:
      chain = DiGraph()
      for node in self.ordered:
         chain.add_node(str(node))
         pass
      for edge in self.edges:
         chain.add_edge(str(edge.head),str(edge.tail),edge.p())
         pass
      return chain
   
   def get_edges(self, refNode: State) -> tuple[list[Transition],list[Transition],list[State]]:
      inner= []
      outer = []
      nodes = []
      for edge in self.edges:
         if str(edge.head) == str(refNode):
            outer.append(edge)
            if edge.tail not in nodes:
               nodes.append(edge.tail)
            pass
         elif str(edge.tail) == str(refNode):
            inner.append(edge)
            if edge.head not in nodes:
               nodes.append(edge.head)
            pass
         pass
      return(inner,outer,nodes)

   # return only the refNode with all the transition associated
   def subgraph(self, refNode:State) -> DiGraph:
      (inner,outer,nodes) = self.get_edges(refNode)
      graph = DiGraph()
      for node in nodes:
         graph.add_node(str(node))
         pass
      for edge in inner + outer:
         graph.add_edge(str(edge.head), str(edge.tail),edge.p())
         pass
      return graph
   pass


# balance the CTMC adding the qii = -sum(qij) condition
def balance_ctmc(mat : np.ndarray[np.ndarray]) -> np.ndarray:
    copy = mat.copy()
    for i in range(len(mat)):
        copy[i][i] = 0
        copy[i][i] = -copy[i].sum()
        pass
    return copy
    pass

# convert a a ctmc to its inner dtmc
def convert_to_dtmc(mat: np.ndarray[np.ndarray]) -> np.ndarray:
    maxq = 0
    ctmc = mat.copy()
    for i in range(len(ctmc)):
        if abs(ctmc[i][i]) > abs(maxq):
            maxq = ctmc[i][i]
            pass
        pass
    id = np.identity(len(ctmc))
    for i in range(len(ctmc)):
        for j in range(len(ctmc[i])):
            ctmc[i][j] = id[i][j] - (ctmc[i][j]/maxq)
            pass
        if ctmc[i].sum() <1:
            for k in range(len(ctmc[i])):
                if ctmc[i][k] >0:
                    ctmc[i][k] += (1-ctmc[i].sum())
                    pass
                pass
            pass
        pass
    return ctmc

#convert a graph from the chainGenerator to an adjacency matrix
def get_adj_matrix(generator: ChainGenerator):
   nodes = generator.ordered
   mat = np.zeros((len(nodes),len(nodes)),dtype=np.double)
   adj :dict = generator.chain().graph.adj
   for i in range(len(nodes)):
       ref = nodes[i]
       for j in range(len(nodes)):
           to = nodes[j]
           if to in adj[ref]:
               mat[i][j] = adj[ref][to]["weight"]
           pass
       pass
   return mat



def print_state_distribution(states, probabilities):
   Ndelay = [0]*(Params.numClients+1)
   Ncpu = [0]*(Params.numClients+1)
   Nio1 = [0]*(Params.numClients+1)
   Nio2 = [0]*(Params.numClients+1)
   for state in states:
      p = probabilities[states.index(state)]
      Ndelay[state.Ndelay] += p
      Ncpu[state.Ncpu] += p 
      Nio1[state.Nio1] += p
      Nio2[state.Nio2] += p
      pass
   print("State distribution for {} : {}".format("Ndelay",Ndelay))
   print("State distribution for {} : {}".format("Ncpu",Ncpu))
   print("State distribution for {} : {}".format("Nio1",Nio1))
   print("State distribution for {} : {}".format("Nio2",Nio2))
   pass



# calculate the model and compare it to MVA
def execute_markov(print_graph = False):
   # MVA description
   matrix = np.array([
    [0,1,0,0,0],
    [0,0,1,0,0],
    [0.004,0.006,0.9,0.065,0.025],
    [0,0,1,0,0],
    [0,0,1,0,0]],np.dtype('d'))

   # calculate Mean values
   mva = MVA(matrix,[5000,0,2.7,40,180],[StationType.Delay,StationType.LoadIndependent,StationType.LoadIndependent,StationType.LoadIndependent,StationType.LoadIndependent],30)
   mva()
   utilization = mvaToDataframe(mva.utilizations)
   throughputs = mvaToDataframe(mva.throughputs)
   meanWaits = mvaToDataframe(mva.meanwaits)
   meanClients = mvaToDataframe(mva.meanclients)
   
   # generate the markov chain starting from state (N,0,0,0)
   nodes = node_enumerator()
   def start():
      for i in nodes:
         if i.Ndelay == Params.numClients : return i
      pass

   generator = ChainGenerator(nodes)
   generator(start())
   if print_graph:
      print(Printer.nx_to_graphviz(generator.chain()))
      pass

   adj = get_adj_matrix(generator)
   if print_graph:
      
      pass
   q = balance_ctmc(adj)
   
   # Useful to catch change in matrix that don't reflect in the resolution
   print("Frobenius Norm: " + str(np.linalg.norm(q,'fro')))
   # transpose again since pi Q =0 must be transformed in Q pi = b
   m = q.copy().transpose()

   # add normalization condition sum(pi) = 1
   norm = np.ones(len(m))
   m =np.vstack((m,norm))

   # vector of coefficient 1 
   b= np.ones(len(m))
   
   #resolve for least squared
   (x,c,k,m) = np.linalg.lstsq(m,b,rcond=1e-15)

   # verify that solution approx 0
   print("Min norm of solution: ",c.min())
   print("Sum of PI: ",sum(x))
   print("Rank of solution: ", k)
   print("Ordered states: " , len(generator.ordered))
   #print([str(x) for x in generator.ordered])   
   ordered = generator.ordered
   Ndelay = 0
   Ncpu = 0
   Nio1 = 0
   Nio2 = 0

   print_state_distribution(ordered,x)
   # sum probability densities* numclients= mean clients
   for state in ordered:
       p = x[ordered.index(state)]
       Ndelay += (state.Ndelay * p)
       Ncpu += (state.Ncpu * p)
       Nio1 += (state.Nio1 * p)
       Nio2 += (state.Nio2 * p)
       pass

   
   print("Ndelay {} Expected {}".format(Ndelay,meanClients["DELAY"][Params.numClients]))
   print("Ncpu {} Expected {}".format(Ncpu,meanClients["CPU"][Params.numClients]))
   print("Nio1 {} Expected {}".format(Nio1,meanClients["IO1"][Params.numClients]))
   print("Nio2 {} Expected {}".format(Nio2,meanClients["IO2"][Params.numClients]))
   print("Mean execution time {}".format((Params.u1*Params.alpha) + (Params.u2*Params.beta)))
   if Params.u1 != Params.u2:
      print("Exponential are different so the chain is not isomorphic, ignore expected")
   return (m)
   pass


if __name__ == "__main__":
   Params.u1 = 15
   Params.u2 = 75
   Params.alpha  = 0.8
   Params.beta  = 0.2
   Params.numClients = 10
   execute_markov(True)
   pass






def __tests__():
    def assert_p(tr: Transition, p:float):
      assert tr.p() == p, "Error p()={} while required {}".format(tr.p(),p)
      pass
    
    def test_p(state1: State, state2 : State, expected):
       tr = Transition(state1,state2)
       assert_p(tr,expected)
       print("{}->{} p:{}, e:{}".format(str(state1),str(state2),tr.p(),expected))
       pass

    s1 = State(3,0,0,0)
    s2 = State(2,1,0,0,1)
    tr = Transition(s1,s2)
    assert_p(tr,(1/Params.thinkTime)*Params.alpha*3)
    s2 = State(2,1,0,0,2)
    tr = Transition(s1,s2)
    assert_p(tr,(1/Params.thinkTime)*Params.beta*3)
    s1 = State(0,3,0,0,2)
    s2 = State(0,2,0,1,1)
    tr = Transition(s1,s2)
    assert_p(tr, (1/Params.u2)*Params.alpha*Params.qio2)

    s1= State(0,2,1,0,2)
    s2 = State(0,1,1,1,1)
    tr = Transition(s1,s2)
    assert_p(tr, (1/Params.u2)*Params.alpha*Params.qio2)
    test_p(State(0,3,0,0,2), State(0,2,0,1,2),(1/Params.u2)*Params.beta*Params.qio2)
    test_p(State(0,2,1,0,1),State(0,3,0,0,1),(1/Params.Sio1))
    test_p(State(0,1,1,1,1),State(0,0,1,2),(1/Params.u1)*Params.qio2)
    test_p(State(0,2,1,0,2),State(0,1,1,1,1),(1/Params.u2)*Params.alpha*Params.qio2)
    test_p(State(1,0,2,0),State(0,1,2,0,2),(1/Params.thinkTime)*Params.beta)
    test_p(State(0,3,0,0,1),State(0,2,1,0,2),(1/Params.u1)*Params.beta*Params.qio1)

    generator = ChainGenerator(node_enumerator())
    generator(State(3,0,0,0))
    for state in generator.ordered:
       assert generator.ordered.count(state) == 1, "Error state {} found {} times".format(str(state),generator.ordered.count(state))
       pass
    print(list(map(lambda x: str(x),generator.ordered)))
    graph = generator.subgraph(State(3,0,0,0))
    print(Printer.nx_to_graphviz(graph))

    p = State(3,0,0,0,1)
    q = State(3,0,0,0,2)
    assert(p.partially_eq(q) and not p == q)

    p = State(1,2,0,0,1)
    q = State(1,2,0,0,2)
    assert(p.partially_eq(q) and not p == q)
    print("All tests passed")
    pass
pass
