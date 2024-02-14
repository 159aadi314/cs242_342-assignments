Running instructions
1. Please make sure the latest release of ns3 is installed (ns 3.36.1) from official website
2. After successful installation, running tests, then put the code.cc file and analysis-graph-plotting.py file within the ns-3.36.1/scratch directory.
3. Configure the ns3 build in debug mode by this command in terminal in ns-3.36.1 directory -  ./ns3 configure --build-profile=debug --enable-examples --enable-tests
4. ./ns3 run code

After this, some results are displayed on terminal, and the useful output is extracted into the output.txt file. This is then analysed by the python script, finding mean and standard deviation of the various values calculated. 4 graphs corresponding to these values are plotted using matplotlib and displayed. The graphs are saved as some png files, analysed data is saved in analysis.txt and the flow monitor output is put into Flowmon.xml

Assumptions and points to note - 
1. In bandwidth calculation, we consider frame sizes as follows - 
    a. data frame with tcp segment has a size of 1000 (tcp segment size) + 20 (ip header) + 14 (frame header) = 1034B.
    b. data frame with tcp ack has a size of 40 (tcp ack size) + 20 (ip header) + 14 (frame header) = 74B.
    c. RTS frame has size of 20 bytes, CTS is of 14 bytes, ACK frames are also of 14 bytes.
2. Units in results - Kbps not KBps
3. Collision is a physical layer phenomenon, but mac layer is tightly coupled with physical layer. Hence collision detection is composed of the following trace sources - phytxdrop (packet dropped during transmission), phyrxdrop (packet dropped during reception), mactxdrop (packet dropped in mac layer before it was transmitted) and macrxdrop (packet dropped in mac layer of receiver). The bandwidth is not wasted if mactxdrop is called, as it is called before transmission actually begins, so that is ignored here. Therefore, we consider it a collision (and a wastage of bandwidth because of it), if and only if - 
    a. Packet dropped during transmission (with wasted bytes = total packet length even though only some bytes of packet were put on link and hence wasted, not the whole packet) - phytxdrop
    b. Packet dropped during/after reception (wasted bytes = full packet length, even though more wasted because frame header and phy header are also there), at mac or phy layer - phyrxdrop, macrxdrop
    c. Collision is not considered if packet is dropped on some intermediate router, because no intermediate routers are there in this case. 
    d. Trace collection for collision is done by hooking PhyTxDrop, PhyRxDrop and MacRxDrop trace sources to callback functions where number of bytes dropped is updated.
4. We assume that the number of packets received as shown in the flow monitor (i->second.txpackets for flow i) includes retransmissions
5. Since RTS CTS and ACK frames are at the data link layer, flow monitor cannot detect them and hence we can't calculate their exact bandwidth without pcap file (flow monitor works at the network layer). To get around this, we have made the following assumptions - 
    a. If RTS-CTS threshold is larger than tcpSegmentframesize (ie size of the largest data segments sent, 1034B), then we set #RTS = #CTS = 0
    b. Else if RTS-CTS threshold is 0 or very small (73 or lesser, as 74 is length of tcpack, the smallest segment size), then we assume that RTS and CTS are required for tcpack as well, even though it is a small segment 
    c. Else #RTS = #CTS = #tcpsegments_sent, which is easily calculated from flow monitor as i->second.txPackets (txpackets is no. of tcp segments that sender sent), where i is for the flows viz. node 0 to node 1 and node 2 to node 1. Further, #ACK = #tcpsegments_sent + #tcpack, and #tcpack is easily calculated from flow monitor as i->second.rxpackets for the flows viz. node 1 to node 0 and node 1 to node 2
6. Tcp throughput at node 1 is assumed as 0 as it doesn't send any tcp data, while tcp throughput at node 0 and 2 are calculated using rxpackets on the flows of node 1 to node 0 and node 1 to node 2, as this rxpacket tells number of successfully acked sent pkts; then we are assuming all sent packets were of length 1000 bytes




