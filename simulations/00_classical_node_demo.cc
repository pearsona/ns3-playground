#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

Ptr<NetDevice> senderDevice;
Address receiverAddress;
uint16_t protocolNumber = 0x0800; // IPv4
uint32_t packetSize = 1024; // bytes
uint32_t numPackets = 10;
Time packetInterval = MilliSeconds(5); // 5ms between packets

void SendPacket()
{
    Ptr<Packet> packet = Create<Packet> (packetSize);
    uint32_t uid = packet->GetUid();
    senderDevice->Send(packet, receiverAddress, protocolNumber);
    NS_LOG_UNCOND ("Packet UID " << uid << " scheduled and sent at " << Simulator::Now ().GetSeconds ());
}

void TxCallback (Ptr<const Packet> packet)
{
    NS_LOG_UNCOND ("Packet UID " << packet->GetUid() << " physically transmitted at " << Simulator::Now ().GetSeconds ());
}

void RxCallback (Ptr<const Packet> packet)
{
    NS_LOG_UNCOND ("Packet UID " << packet->GetUid() << " physically received at " << Simulator::Now ().GetSeconds ());
}


void SchedulePackets()
{
    for (uint32_t i = 0; i < numPackets; ++i)
    {
        Simulator::Schedule (i * packetInterval, &SendPacket);
    }
}

void ChangeDataRate(uint32_t time, float dataRate)
{
    Simulator::Schedule (MilliSeconds (time), [dataRate] {
        NS_LOG_UNCOND ("[Throttle] Changing DataRate to " << dataRate << " Mbps at " << Simulator::Now ().GetSeconds () << " seconds");

        Config::Set ("/NodeList/0/DeviceList/0/$ns3::PointToPointNetDevice/DataRate", StringValue (std::to_string(dataRate) + "Mbps"));
        Config::Set ("/NodeList/1/DeviceList/0/$ns3::PointToPointNetDevice/DataRate", StringValue (std::to_string(dataRate) + "Mbps"));
    });
}


int main (int argc, char *argv[])
{
    Time::SetResolution (Time::NS);

    NodeContainer nodes;
    nodes.Create (2);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

    NetDeviceContainer devices;
    devices = pointToPoint.Install (nodes);

    senderDevice = devices.Get(0);
    receiverAddress = devices.Get(1)->GetAddress();

    devices.Get (0)->TraceConnectWithoutContext ("PhyTxEnd", MakeCallback (&TxCallback));
    devices.Get (1)->TraceConnectWithoutContext ("PhyRxEnd", MakeCallback (&RxCallback));

    // Schedule multiple packets
    SchedulePackets();

    // Schedule throttling at 10ms
    ChangeDataRate(10, 1.0);
    ChangeDataRate(20, 0.05);
    ChangeDataRate(30, 50);

    Simulator::Run ();
    Simulator::Destroy ();

    return 0;
}
