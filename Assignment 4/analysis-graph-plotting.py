from cProfile import label
from statistics import mean
import matplotlib.pyplot as plt 
import math
import statistics
from tabulate import tabulate

file1 = open("output.txt","r")

x = [0,256,512,1000]
y = [[],[],[],[],[],[],[],[],[]]

for i in range(0,4):
	line = file1.readline()
	data = line.split(':')
	for j in range(0,9):
		y[j].append(float(data[j]))


head = ['RTS-CTS Threshold (in Bytes) ->', 0, 256, 512, 1000, 'Mean', 'Standard deviation']
table = [
    ['RTS Bandwidth', y[0][0], y[0][1], y[0][2], y[0][3], mean(y[0]), statistics.stdev(y[0])],
    ['CTS Bandwidth', y[1][0], y[1][1], y[1][2], y[1][3], mean(y[1]), statistics.stdev(y[1])],
    ['ACK Bandwidth', y[2][0], y[2][1], y[2][2], y[2][3], mean(y[2]), statistics.stdev(y[2])],
    ['TCP Segment Bandwidth', y[3][0], y[3][1], y[3][2], y[3][3], mean(y[3]), statistics.stdev(y[3])],
    ['TCP Ack Bandwidth', y[4][0], y[4][1], y[4][2], y[4][3], mean(y[4]), statistics.stdev(y[4])],
    ['Bandwidth wasted due to collisions', y[5][0], y[5][1], y[5][2], y[5][3], mean(y[5]), statistics.stdev(y[5])],
    ['TCP Throughput Node 0', y[6][0], y[6][1], y[6][2], y[6][3], mean(y[6]), statistics.stdev(y[6])],
    ['TCP Throughput Node 1', y[7][0], y[7][1], y[7][2], y[7][3], mean(y[7]), statistics.stdev(y[7])],
    ['TCP Throughput Node 2', y[8][0], y[8][1], y[8][2], y[8][3], mean(y[8]), statistics.stdev(y[8])], 
]
with open('analysis.txt', 'w') as f:
    f.write(tabulate(table, headers=head, tablefmt='orgtbl'))

# plotting Average bandwidth spent in transmitting 
# RTS, CTS and ACK frames (control frames)
plt.figure()
plt.plot(x, y[0], color='green', linestyle='solid', linewidth = 2, 
         marker='o', markerfacecolor='black', markersize=4,label="RTS")
plt.plot(x, y[1], color='orange', linestyle='solid', linewidth = 2, 
         marker='o', markerfacecolor='black', markersize=4,label="CTS")
plt.plot(x, y[2], color='blue', linestyle='solid', linewidth = 2, 
         marker='o', markerfacecolor='black', markersize=4,label="ACK")
plt.xlabel('RTS/CTS Threshold (in Bytes)')
plt.ylabel("Control Frame Bandwidths (in Kbps)")
plt.title('Bandwidth spent in transmission of RTS, CTS & ACK vs RTS Threshold')
plt.legend()
plt.savefig('ControlFrameBandwidths.png', bbox_inches='tight')

# plotting Average bandwidth spent in transmitting 
# TCP segments and TCP ACK packets(data frames)
plt.figure()
plt.plot(x, y[3], color='red', linestyle='solid', linewidth = 2, 
         marker='o', markerfacecolor='black', markersize=4,label="TCP Segments")
plt.plot(x, y[4], color='violet', linestyle='solid', linewidth = 2, 
         marker='o', markerfacecolor='black', markersize=4,label="TCP Acks")
plt.xlabel('RTS Threshold (in Bytes)')
plt.ylabel('Data Frame Bandwidths (in Kbps)')
plt.title('Bandwidth spent in transmission of TCP ACK and TCP segments vs RTS Threshold')
plt.legend()
plt.savefig('DataFrameBandwidths.png', bbox_inches='tight')

# plot bandwidth wasted due to collisions 
plt.figure()
plt.plot(x, y[5], color='blue', linestyle='solid', linewidth = 2, 
         marker='o', markerfacecolor='black', markersize=4, label="Collisions") 
plt.xlabel('RTS Threshold')
plt.ylabel('Bandwidth wasted due to collisions (in Kbps)')
plt.title('Bandwidth wastage due to collisions vs RTS threshold')
plt.ylim(0, 0.5)
plt.savefig('CollisionWastage.png', bbox_inches='tight')

# plotting TCP throughput at each node
plt.figure()
plt.plot(x, y[6], color='yellow', linestyle='solid', linewidth = 2, 
         marker='o', markerfacecolor='black', markersize=4, label="Node 0") 
plt.plot(x, y[7], color='violet', linestyle='solid', linewidth = 2, 
         marker='o', markerfacecolor='black', markersize=4, label="Node 1") 
plt.plot(x, y[8], color='orange', linestyle='solid', linewidth = 2, 
         marker='o', markerfacecolor='black', markersize=4, label="Node 2")          
plt.xlabel('RTS Threshold (in Bytes)')
plt.ylabel('TCP throughput at each node (in Kbps)')
plt.title('Plot of TCP throughputs vs RTS threshold')
plt.legend()
plt.savefig('TCPThroughputs.png', bbox_inches='tight')

#Show all plots
plt.show() 