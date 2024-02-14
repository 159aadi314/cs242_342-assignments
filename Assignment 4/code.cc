// /* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
// /*
//  * This program is free software; you can redistribute it and/or modify
//  * it under the terms of the GNU General Public License version 2 as
//  * published by the Free Software Foundation;
//  *
//  * This program is distributed in the hope that it will be useful,
//  * but WITHOUT ANY WARRANTY; without even the implied warranty of
//  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  * GNU General Public License for more details.
//  *
//  * You should have received a copy of the GNU General Public License
//  * along with this program; if not, write to the Free Software
//  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/tcp-westwood.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-stack-helper.h"     
#include <iostream>
#include <random>

/*
  network topology

  STA              AP              STA
  n0---------------n1--------------n2
  0               250              500
  10.1.3.2      10.1.3.1        10.1.3.3

*/

NS_LOG_COMPONENT_DEFINE ("Assignment4-App5");
using namespace ns3;

#define SERVER_STOPTIME 49.0               // set slightly lower than 50 to allow processing at receivers
#define START_LOWERBOUND 1.0
#define START_UPPERBOUND 5.0
#define CLIENT_STOPTIME 48.0
#define SERVER_STARTTIME 0.0
double simulation_time = 50.0;

Ptr<PacketSink> sink;                         /* Pointer to the packet sink application */

uint32_t segmentSize = 1000;
uint32_t RTSSize = 20;
uint32_t CTSSize = 14;
uint32_t ACKSize = 14;
uint32_t tcpAckFrameSize = 74;
uint32_t tcpSegmentFrameSize = segmentSize + 34;

uint32_t PhyTxDropCount = 0;
uint32_t PhyRxDropCount = 0;      // in bytes
uint32_t MacRxDropCount = 0;

void PhyTxDrop(Ptr<const Packet> p)
{
  NS_LOG_INFO("Packet Drop due to collision");
  PhyTxDropCount += p->GetSize();
}
void PhyRxDrop(Ptr<const Packet> p, WifiPhyRxfailureReason r)
{
  NS_LOG_INFO("Packet Drop due to collision");
  PhyRxDropCount += p->GetSize();
}

void MacRxDrop(Ptr<const Packet> p){
  NS_LOG_INFO("Packet Drop due to collision");
  MacRxDropCount += p->GetSize();
}
void GetStats(FlowMonitorHelper &flowHelper, Ptr<FlowMonitor> &flowMonitor, int rtscts_threshold){
 	/* Check for lost packets */
	flowMonitor->CheckForLostPackets ();  
	/*Check flow stats */
	/* Get stats for transmitted, received packets */
	Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowHelper.GetClassifier ());
	FlowMonitor::FlowStatsContainer stats = flowMonitor->GetFlowStats ();

  int rtsCount = 0, ctsCount = 0, ackCount = 0, tcpSegmentCount = 0, tcpAckCount = 0;
  int acked0 = 0, acked2 = 0;       // no. of bytes sent by nodes 0 and 2, acknowledged at tpt layer by their receiver (here node 1)

	for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); i++)
	{
		Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
    if(t.sourceAddress == Ipv4Address("10.1.3.1"))          // ACK and tcpack, #ack = #tcpack. tx packets refer to pkts at transport level, so this is just number of tcp acks sent out
    {
      tcpAckCount += (int)i->second.txPackets * tcpAckFrameSize;      // tcpack
      ackCount += (int)i->second.rxPackets * ACKSize;           // ack
      if(rtscts_threshold < (int) tcpAckFrameSize) {
        rtsCount += (int)i->second.txPackets * RTSSize; // rts cts here also if threshold 0
        ctsCount += (int)i->second.txPackets * CTSSize;
      }
      if(t.destinationAddress == Ipv4Address("10.1.3.2")){
        acked0 += (int)i->second.rxPackets * segmentSize;         // segment_size because tpt layer data is not data_size, it is data_size - pkt header
      }else if(t.destinationAddress == Ipv4Address("10.1.3.3")){
        acked2 += (int)i->second.rxPackets * segmentSize;
      }
    }
    else{                                                   // rts, cts, tcp segments #tcp = #rts = #cts
      tcpSegmentCount += (int)i->second.txPackets * tcpSegmentFrameSize;   // tcp segment
      if(((int)segmentSize - rtscts_threshold) >= 0){
        rtsCount += (int)i->second.txPackets * RTSSize;      // rts
        ctsCount += (int)i->second.txPackets * CTSSize;       // cts, here because #cts = #rts. but non zero only if threshold < segment size, otherwise no rts cts
      }
      ackCount += (int)i->second.rxPackets * ACKSize;     // ack
    }
	}
  /*Calculation of various bandwidths with the data obtained from the file*/
  double rtsBandwidth = (rtsCount*8.0)/(simulation_time*3.0*1000.0);
  double ctsBandwidth = (ctsCount*8.0)/(simulation_time*3.0*1000.0);
  double ackBandwidth = (ackCount*8.0)/(simulation_time*3.0*1000.0);
  double tcpSegmentBandwidth = (tcpSegmentCount*8.0)/(simulation_time*3.0*1000.0);
  double tcpAckBandwidth = (tcpAckCount*8.0)/(simulation_time*3.0*1000.0);
  int bytesDropped = PhyTxDropCount+PhyRxDropCount+MacRxDropCount;      // bandwidth is only wasted due to collision if pkt dropped between phy layer of sender and app layer of receiver.
  double wasted_bw = (bytesDropped*8.0)/(simulation_time*3.0*1000.0);
	double averageThroughputNode0 = ((acked0*8.0)/(simulation_time*1000.0));
  double averageThroughputNode1 = 0.0;
	double averageThroughputNode2 = ((acked2*8.0)/(simulation_time*1000.0));

  std::cout << "\nAverage bandwidth (over all nodes, over whole simulation time) spent in transmitting RTS: " << rtsBandwidth << "Kbps\n";
  std::cout << "Average bandwidth (over all nodes, over whole simulation time) spent in transmitting CTS: " << ctsBandwidth << "Kbps\n";
  std::cout << "Average bandwidth (over all nodes, over whole simulation time) spent in transmitting ACK: " << ackBandwidth << "Kbps\n";
  std::cout << "Average bandwidth (over all nodes, over whole simulation time) spent in transmitting TCP Segments: " << tcpSegmentBandwidth << "Kbps\n";
  std::cout << "Average bandwidth (over all nodes, over whole simulation time) spent in transmitting TCP Acks: " << tcpAckBandwidth << "Kbps\n";
  std::cout << "Average bandwidth (over all nodes, over whole simulation time) wasted due to collisions: " << wasted_bw << " Kbps\n";
  std::cout << "Average TCP throughput obtained at node 0: " << averageThroughputNode0 << " Kbps" << std::endl;
	std::cout << "Average TCP throughput obtained at node 1: " << averageThroughputNode1 << " Kbps" << std::endl;
  std::cout << "Average TCP throughput obtained at node 2: " << averageThroughputNode2 << " Kbps" << std::endl;
  // node 0 and node 2 are only receiving acks so their tcp throughput is non zero. tcp throughput is number of data bytes (ie appl layer data) acked (ie successfully sent) per unit time. acks only go upto tpt layer. so node 1 has 0 tcp throughput, ie at sink app. this value is slightly lesser than total rxbytes at nodes 0 and 2 (see in debug) because rx bytes is at network layer level, headers are removed before sending to app layer

	FILE *FilePointer;
	FilePointer = fopen("output.txt","a");
  fprintf(FilePointer,"%lf:", rtsBandwidth);
  fprintf(FilePointer,"%lf:", ctsBandwidth);
  fprintf(FilePointer,"%lf:", ackBandwidth);
  fprintf(FilePointer,"%lf:", tcpSegmentBandwidth);
  fprintf(FilePointer,"%lf:", tcpAckBandwidth);
  fprintf(FilePointer,"%lf:", wasted_bw);
  fprintf(FilePointer,"%lf:", averageThroughputNode0);
  fprintf(FilePointer,"%lf:", averageThroughputNode1);
	fprintf(FilePointer,"%lf\n", averageThroughputNode2);
  fclose(FilePointer);
  std::cout << "Data loaded into output.txt" << "\n";
  PhyTxDropCount = 0;
  PhyRxDropCount = 0;

	flowMonitor->SerializeToXmlFile("Flowmon.xml", true, true);
  
  std::cout << "Flow monitor probe data loaded into Flowmon.xml" << "\n";
  std::cout << "---------------------------------------------------------------" << "\n";
}


void Simulation(uint32_t rtscts_threshold){
  std::string dataRate = "500Kbps"; 
  std::string phyRate = "HtMcs7";

  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", UintegerValue (rtscts_threshold));
  Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue("1500"));
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (segmentSize));

  WifiHelper wifi;
  wifi.SetStandard(WIFI_STANDARD_80211n);
  
  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (2);
  NodeContainer wifiApNodes;
  wifiApNodes.Create  (1);
  NodeContainer networkNodes;
  networkNodes.Add(wifiStaNodes.Get(0));
  networkNodes.Add(wifiApNodes.Get(0));
  networkNodes.Add(wifiStaNodes.Get(1));

  YansWifiChannelHelper channel;
  channel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  channel.AddPropagationLoss ("ns3::RandomPropagationLossModel");

  YansWifiPhyHelper phy;
  phy.SetChannel (channel.Create ());
  phy.SetErrorRateModel ("ns3::YansErrorRateModel");
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                      "DataMode", StringValue (phyRate),
                                      "ControlMode", StringValue ("HtMcs0"));

  WifiMacHelper mac;
  Ssid ssid = Ssid ("ns-3-ssid");

  NetDeviceContainer staDevices;
  mac.SetType ("ns3::StaWifiMac", "Ssid", SsidValue (ssid));
  staDevices = wifi.Install (phy, mac, wifiStaNodes);

  NetDeviceContainer apDevices;
  mac.SetType ("ns3::ApWifiMac", "Ssid", SsidValue (ssid));
  apDevices = wifi.Install (phy, mac, wifiApNodes);


  /* setting up physical topology*/

  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (250.0),
                                 "DeltaY", DoubleValue (0.0),
                                 "GridWidth", UintegerValue (3),
                                 "Z", DoubleValue(0.0),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (networkNodes);

  InternetStackHelper stack;
  stack.Install(wifiApNodes);
  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId ()));
  Config::SetDefault ("ns3::TcpWestwood::ProtocolType", EnumValue (TcpWestwood::WESTWOODPLUS));
  stack.Install(wifiStaNodes.Get(0));
  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpHybla::GetTypeId ()));
  stack.Install(wifiStaNodes.Get(1));
  Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpCubic::GetTypeId ()));

  Ipv4AddressHelper address;
  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer apInterface = address.Assign (apDevices);      
  Ipv4InterfaceContainer staInterface = address.Assign(staDevices);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


  uint16_t port = 9;

  PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer serverApps = sinkHelper.Install (wifiApNodes);
  sink = StaticCast<PacketSink> (serverApps.Get (0));
  serverApps.Start(Seconds(SERVER_STARTTIME));
  serverApps.Stop (Seconds (SERVER_STOPTIME));

  OnOffHelper source ("ns3::TcpSocketFactory", (InetSocketAddress(apInterface.GetAddress(0), port)));
  source.SetAttribute ("PacketSize", UintegerValue (segmentSize));
  source.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  source.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  source.SetAttribute ("DataRate", DataRateValue (DataRate (dataRate)));
  ApplicationContainer clientApps = source.Install (wifiStaNodes);
  std::random_device rd;
  std::default_random_engine eng(rd());
  std::uniform_real_distribution<double> unif(START_LOWERBOUND, START_UPPERBOUND);
  double a = unif(eng);
  double b = unif(eng);
  clientApps.Get(0)->SetStartTime(Seconds(a));
  clientApps.Get(1)->SetStartTime(Seconds(b));
  clientApps.Stop (Seconds (CLIENT_STOPTIME));

  Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyRxDrop", MakeCallback(&PhyRxDrop));
  Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxDrop", MakeCallback(&PhyTxDrop));	
  Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRxDrop", MakeCallback(&MacRxDrop));	

  Ptr<FlowMonitor> flowMonitor;
  FlowMonitorHelper flowHelper;
  flowMonitor = flowHelper.InstallAll();

  Simulator::Stop(Seconds(simulation_time));
  Simulator::Run ();
  GetStats(flowHelper, flowMonitor, rtscts_threshold);
  Simulator::Destroy();
}

int main (int argc, char *argv[])
{
  bool verbose = true;

  FILE *FilePointer;
  FilePointer = fopen("output.txt","w");
	fclose(FilePointer);

  CommandLine cmd (__FILE__);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.Parse (argc,argv);

  std::vector<uint32_t> rts_threshold = {0, 256, 512, 1000};

  for(int i = 0; i < (int) rts_threshold.size(); i++){
    std::cout << "RTS/CTS Threshold: " << rts_threshold[i] << "\n";
    Simulation(rts_threshold[i]);
  }
  system("python3 ./scratch/analysis-graph-plotting.py");
  std::cout << "Data of output.txt analysed into analysis.txt and graphs plotted using analysis-graph-plotting.py. See ControlFrameBandwidths.png, DataFrameBandwidths.png, CollisionWastage.png, TCPThroughputs.png for the plots" << "\n";
  return 0;
}