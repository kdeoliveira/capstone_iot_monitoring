using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Channels;
using System.Threading.Tasks;
using Grpc.Core;
using IOT.Monitoring;

namespace GrpcClient
{
    public class Client
    {
        private Grpc.Core.Channel channel;
        private RemoteEndpoint.RemoteEndpointClient client;
        public Client(string address)
        {
            channel = new Grpc.Core.Channel(address, ChannelCredentials.Insecure);
            client = new RemoteEndpoint.RemoteEndpointClient(channel);
        }



        ~Client()
        {
            
        }

        public void CloseConnection()
        {
            channel.ShutdownAsync().Wait();
        }

        public string GetServerId()
        {
            string result = "";
            var t = Task.Run(async () =>
            {
                var reply = await client.GetServerInfoAsync(new Empty());

                result = String.Format("{0}-{1}.{2}", reply.Major, reply.Minor, reply.Rpc);
            });
            t.Wait();
            return result;
        }

        public async Task<string> GetServerIdAsync()
        {
            var reply = await client.GetServerInfoAsync(new Empty());
            return String.Format("{0}-{1}.{2}", reply.Major, reply.Minor, reply.Rpc);
        }

        public List<Device> GetListOfDevices()
        {
            
            var response = client.ListDevices(new Empty());

            return response.Device.ToList();
        }

        public async Task<List<Device>> GetListOfDevicesAsync()
        {

            var response = await client.ListDevicesAsync(new Empty());

            return response.Device.ToList();
        }

        public List<Packet> GetListOfPackets(int id)
        {
            var response = new List<Packet>();
            var t = Task.Run(async () =>
                {
                    var stream = client.ReadPacket(new PacketRequest { 
                        Id = id,
                    });

                    while (await stream.ResponseStream.MoveNext())
                    {
                        var cur = stream.ResponseStream.Current;
                        if (cur == null)
                            break;
                        response.Add(cur);
                    }

                    
                });

            t.Wait();

            return response;
        }

        public async Task ReadPackets(CancellationToken cancel, ChannelWriter<Packet> buffer)
        {
            var stream = client.ReadAll();
            var sender = stream.ResponseStream;
            var reader = stream.RequestStream;
            _ = Task.Run(async () =>
              {
                  while (!cancel.IsCancellationRequested)
                  {
                      await reader.WriteAsync(new ReadAllOn { Active = true });
                  }

              });
           
            
            while (await sender.MoveNext(cancel))
            {
                await buffer.WriteAsync(sender.Current);
            }
            await stream.RequestStream.CompleteAsync();
            buffer.Complete();
        }
    }
    //class Program
    //{
    //    static void Main(string[] args)
    //    {

    //        Client client = new Client("127.0.0.1:50051");

    //        string x = client.GetServerId();

    //        Console.WriteLine("Result from server: {0}", x);

    //        var y = client.GetListOfDevices();

    //        Console.ReadKey();
    //    }
    //}
}
