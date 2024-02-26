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
from markov import *


class SystemParameters:
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
    qoutd = 0.1*0.4 #go to delay station
    qouts = 0.1*0.6 # renter the system
    numClients = 3
    pass




class State:
    def __init__(self,Ndelay, Ncpu, Nio1, Nio2,cpuStage = 0) -> None:
        self.Ndelay = Ndelay
        self.Ncpu = Ncpu
        self.cpuStage = cpuStage
        self.Nio1 = Nio1
        self.Nio2 = Nio2
        self.state= [Ndelay,Ncpu,Nio1,Nio2,cpuStage]
        self.descriptor =["Ndelay","Ncpu","Nio1","Nio2","cpuStage"]
        pass

    def isValid(self)->bool:
        if self.Ncpu > 0:
            return (self.cpuStage == 1 or self.cpuStage == 2) and (self.Ncpu + self.Nio1 + self.Nio2 + self.Ndelay) == 3
        else:
            return (self.Ncpu + self.Nio1 + self.Nio2 + self.Ndelay) == 3
    
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


class Transition:
   
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
   
   def transitionIsValid(self):
      #check cpu stage
      if self.head.cpuStage != self.tail.cpuStage:
         if self.head.Ncpu < self.tail.Ncpu and self.tail.Ncpu > 0 and self.head.Ncpu > 0: return False
         pass
      changed = 0
      def validate_var(a , b):
         nonlocal changed 
         if abs(a - b ) > 1 : return False 
         if abs(a-b) == 1 and changed > 1: return False
         if abs(a-b) == 0 : return True
         else: changed += 1         
         return True
      for i in range(len(self.head)):
         if not validate_var(self.head[i],self.tail[i]): return False
         pass
      return True
   
   def detectMovement(self) -> tuple[str,str]:
      decremented = ""
      incremented = ""
      for i in range(len(self.head)):
         if self.head[i] != self.tail[i]:
            if self.head[i] < self.tail[i]:
               incremented = self.head.descriptor[i] if incremented == "" else "ERROR"               
               pass
            if self.head[i] > self.tail[i]:
               decremented = self.head.descriptor[i] if decremented == "" else "ERROR"
               pass
         pass
      return (decremented,incremented)

   def detectType(self):
      if not self.transitionIsValid() : return
      (decrement,increment)= self.detectMovement()
      if decrement == "ERROR" or increment == "ERROR" : return 
      elif decrement == "Ncpu" and increment == "Nio1": self.type = Transition.TransitionType.CPU_TO_IO1
      elif decrement == "Ncpu" and increment == "Nio2" : self.type = Transition.TransitionType.CPU_TO_IO2
      elif decrement == "Nio1" and increment == "Ncpu" : self.type = Transition.TransitionType.IO1_TO_CPU
      elif decrement == "Nio2" and increment == "Ncpu" : self.type = Transition.TransitionType.IO2_TO_CPU
      elif decrement == "Ndelay" and increment == "Ncpu" : self.type = Transition.TransitionType.DELAY_TO_CPU
      elif decrement == "Ncpu" and increment == "Ndelay" : self.type = Transition.TransitionType.CPU_TO_DELAY
      pass
   
   def p(self):
      assert(self.type != Transition.TransitionType.UNKNOWN)
      if self.type == Transition.TransitionType.CPU_TO_IO1 or self.type == Transition.TransitionType.CPU_TO_IO2: return self.CpuToIo()
      elif self.type == Transition.TransitionType.IO1_TO_CPU or self.type ==  Transition.TransitionType.IO2_TO_CPU: return self.IoToCpu()
      elif self.type == Transition.TransitionType.DELAY_TO_CPU: return self.DelayToCpu()
      elif self.type == Transition.TransitionType.CPU_TO_DELAY : return self.CpuToDelay()
      elif self.type == Transition.TransitionType.CPU_TO_SELF: return self.CpuToSelf()
      pass
   
   def DelayToCpu(self):
      l = self.head.Ndelay*(1/SystemParameters.thinkTime)
      if self.head.Ncpu == 0:
         l *= SystemParameters.alpha if self.tail.cpuStage == 1 else  SystemParameters.beta
      return l
   
   def CpuL(self):
      l = 1/SystemParameters.u1 if self.head.cpuStage == 1 else 1/SystemParameters.u2
      if self.tail.Ncpu > 0:
         l *=  SystemParameters.alpha if self.tail.cpuStage == 1 else SystemParameters.beta
         pass
      return l
      

   def CpuToIo(self):      
      return self.CpuL()*SystemParameters.qio1 if self.type == Transition.TransitionType.CPU_TO_IO1 else self.CpuL()*SystemParameters.qio2
   
   def CpuToDelay(self):
      return self.CpuL()*SystemParameters.qoutd
   
   def CpuToSelf(self):
      return self.CpuL()*SystemParameters.qouts
   
   def IoToCpu(self):
      l = 1
      if self.head.Ncpu == 0:
         l = SystemParameters.alpha if self.tail.cpuStage == 1 else SystemParameters.beta
         pass
      return l * (1/SystemParameters.Sio1) if self.type == Transition.TransitionType.IO1_TO_CPU else l* (1/SystemParameters.Sio2)

   pass


class Printer:

   def nx_to_graphviz(nxgraph : nx.DiGraph):
      graph = graphviz.Digraph()
      for node in nxgraph.graph.nodes:
        graph.node(node)
        pass
      for edge in nxgraph.graph.edges:
       graph.edge(edge[0],edge[1],str(nxgraph.graph.get_edge_data(edge[0],edge[1])["weight"]))
       pass
      return graph
   
   

   pass


def stage_enumerator(stage: int) -> list[State]:
    Ndelay = 0
    Ncpu = 1
    Nio1 = 2
    Nio2 = 3
    cpuStage = stage
    stages = [0,0,0,0]
    result = []
    while stages[Ndelay] <= 3:
        for i in reversed(range(4)):
            stages[i] += 1
            if stages[i] <= 3: break
            elif i > 0 : stages[i] = 0
            pass
        state= State(stages[Ndelay], stages[Ncpu], stages[Nio1],stages[Nio2],stage)
        if state.isValid(): result.append(state)
        pass
    return result

def node_enumerator() -> list[State]:
    nodes = stage_enumerator(1)
    for newnode in stage_enumerator(2):
        if newnode not in nodes:
            nodes.append(newnode)
            pass
        pass
    return nodes


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
    self.graph.add_edge(headLabel,tailLabel,weight=round(p,5))
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


class ChainGenerator:
   
   def __init__(self, nodes : list[State]) -> None:
      self.nodes = nodes
      self.edges : list[Transition] = []
      self.queue = []
      self.ordered = []   
      pass

   def __call__(self, firstNode : State):
      self.queue.append(firstNode)
      while len(self.queue) > 0:
         self.compute_next()
         pass
      pass

   def compute_next(self):
      ref = self.queue[0]
      self.queue.remove(ref)
      #print("Computing nodes for: {}".format(str(ref)))
      if ref in self.ordered: return
      self.ordered.append(ref)
      for node in self.nodes:
         tr = Transition(ref,node)
         if tr.type != Transition.TransitionType.UNKNOWN:
            self.edges.append(tr)
            if not node in self.ordered:
               #print("Discovered {}".format(str(node)))
               self.queue.append(node)
               pass
            pass
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


def balance_ctmc(mat : np.ndarray[np.ndarray]) -> np.ndarray:
    copy = mat.copy()
    for i in range(len(mat)):
        
        copy[i][i] = -copy[i].sum()
        pass
    return copy
    pass


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

def get_adj_matrix(generator: ChainGenerator):
   nodes = generator.ordered
   mat = np.zeros((len(nodes),len(nodes)))
   adj :dict = generator.chain().graph.adj
   for i in range(len(nodes)):
       ref = nodes[i]
       for j in range(len(nodes)):
           to = nodes[j]
           if to in adj[ref]:
               mat[j][i] = round(adj[ref][to]["weight"],10)
           pass
       pass
   return mat




if __name__ == "__main__":
    
   generator = ChainGenerator(node_enumerator())
   generator(State(3,0,0,0))
   q = balance_ctmc(get_adj_matrix(generator).transpose())
   print(q)
   dtmc = pydtmc.MarkovChain(convert_to_dtmc(q))
   dtmc.pi

   m = q.copy().transpose()

   norm = np.ones(len(m))
   m =np.vstack((m,norm))
   b= np.zeros(len(m))
   b[-1] = 1
   x = np.linalg.lstsq(m,b,rcond=None)[0]

   print(np.linalg.norm( (m@x) - b ,2).min())
   print(sum(x))
   print(dtmc.pi[0].sum())
   for node in generator.ordered:
      print("State:{} , P:{}".format(node,x[generator.ordered.index(node)]))
      pass
   print("Len ",len(generator.ordered))

   ordered = generator.ordered
   Ndelay = 0
   Ncpu = 0
   Nio1 = 0
   Nio2 = 0


   for state in ordered:
       p = x[ordered.index(state)]
       Ndelay += (state.Ndelay * p)
       Ncpu += (state.Ncpu * p)
       Nio1 += (state.Nio1 * p)
       Nio2 += (state.Nio2 * p)
       pass

   print("Ndelay {}".format(Ndelay))
   print("Ncpu {}".format(Ncpu))
   print("Nio1 {}".format(Nio1))
   print("Nio2 {}".format(Nio2))
   pass
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
    assert_p(tr,(1/SystemParameters.thinkTime)*SystemParameters.alpha*3)
    s2 = State(2,1,0,0,2)
    tr = Transition(s1,s2)
    assert_p(tr,(1/SystemParameters.thinkTime)*SystemParameters.beta*3)
    s1 = State(0,3,0,0,2)
    s2 = State(0,2,0,1,1)
    tr = Transition(s1,s2)
    assert_p(tr, (1/SystemParameters.u2)*(1/SystemParameters.timeSlice)*SystemParameters.alpha*SystemParameters.qio2)

    s1= State(0,2,1,0,2)
    s2 = State(0,1,1,1,1)
    tr = Transition(s1,s2)
    assert_p(tr, (1/SystemParameters.u2)*(1/SystemParameters.timeSlice)*SystemParameters.alpha*SystemParameters.qio2)
    test_p(State(0,3,0,0,2), State(0,2,0,1,2),(1/SystemParameters.u2)*SystemParameters.beta*SystemParameters.qio2)
    test_p(State(0,2,1,0,1),State(0,3,0,0,1),(1/SystemParameters.Sio1))
    test_p(State(0,1,1,1,1),State(0,0,1,2),(1/SystemParameters.u1)*SystemParameters.qio2)
    test_p(State(0,2,1,0,2),State(0,1,1,1,1),(1/SystemParameters.u2)*SystemParameters.alpha*SystemParameters.qio2)
    test_p(State(1,0,2,0),State(0,1,2,0,2),(1/SystemParameters.thinkTime)*SystemParameters.beta)
    test_p(State(0,3,0,0,1),State(0,2,1,0,2),(1/SystemParameters.u1)*SystemParameters.beta*SystemParameters.qio1)

    generator = ChainGenerator(node_enumerator())
    generator(State(3,0,0,0))
    for state in generator.ordered:
       assert generator.ordered.count(state) == 1, "Error state {} found {} times".format(str(state),generator.ordered.count(state))
       pass
    print(list(map(lambda x: str(x),generator.ordered)))
    graph = generator.subgraph(State(3,0,0,0))
    print(Printer.nx_to_graphviz(graph))

    print("All tests passed")
    pass
pass
