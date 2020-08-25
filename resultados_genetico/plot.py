import numpy as np
import math
from matplotlib import pyplot as plt
import seaborn as sns
from scipy.stats import pearsonr

class Datos:
	def __init__(self):
		self.alpha1 = []
		self.alpha2 = []
		self.beta1 = []
		self.beta2 = []
		self.gamma1 = []
		self.gamma2 = []
		self.num_iterations = 0
	
	def add_player(self, player):
		self.alpha1.append(player[0])
		self.alpha2.append(player[1])
		self.beta1.append(player[2])
		self.beta2.append(player[3])
		self.gamma1.append(player[4])
		self.gamma2.append(player[5])
		self.num_iterations += 1
		

def leer(path):
	d = Datos()
	file = open(path)
	for line in file:
		jugador = [float(x) for x in line.split()]
		d.add_player(jugador)
	
	return d

def plot(d):
	xs = [i for i in range(0, d.num_iterations)]
	
	plt.xlabel("Iteraciones")
	plt.ylabel("Valor de parametro del mejor jugador")
	
	plt.plot(xs, d.alpha1, '.', markersize = 1.2)
	plt.plot(xs, d.alpha2, '.', markersize = 1.2)
	plt.plot(xs, d.beta1, '.', markersize = 1.2)
	plt.plot(xs, d.beta2, '.', markersize = 1.2)
	plt.plot(xs, d.gamma1, '.', markersize = 1.2)
	plt.plot(xs, d.gamma2, '.', markersize = 1.2)
	
def legend():
	lg = plt.legend(["Alfa 1", "Alfa 2", "Beta 1", "Beta 2", "Gamma 1", "Gamma 2"])
	lg.legendHandles[0]._legmarker.set_markersize(10)
	lg.legendHandles[1]._legmarker.set_markersize(10)
	lg.legendHandles[2]._legmarker.set_markersize(10)
	lg.legendHandles[3]._legmarker.set_markersize(10)
	lg.legendHandles[4]._legmarker.set_markersize(10)
	lg.legendHandles[5]._legmarker.set_markersize(10)

def show_results(path, second_plot_ylim = (0,0)):
	print(path)
	d = leer(path)
	if second_plot_ylim != (0,0):
		plt.figure(figsize = (10,5))
		plt.subplot(1, 2, 1)
		plot(d)
		legend()
		plt.subplot(1, 2, 2)
		plot(d)
		plt.ylim(second_plot_ylim)
		plt.tight_layout()
	else:
		plt.figure(figsize=(8,6))
		plot(d)
		legend()
		
	plt.show()

sns.set()

show_results("tct_estr_crand.out")
show_results("tct_pond_crand.out", (-750, 750))
show_results("tct_pond_cfijo.out", (-700, 500))
show_results("ant_estr_crand.out")
show_results("ant_pond_crand.out", (-800, 400))
show_results("ant_pond_cfijo.out", (-1100, 650))
show_results("tct_pond_cfijo_redmut.out")
