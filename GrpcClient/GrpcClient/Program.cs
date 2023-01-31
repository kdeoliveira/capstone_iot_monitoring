using Grpc.Net.Client;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using IOT.Monitoring;
using Grpc.Core;

namespace GrpcClient
{
    class Program
    {
        static void Main(string[] args)
        {
            var channel = new Channel("127.0.0.1:50051", ChannelCredentials.Insecure);
            {
                
                var client = new RemoteEndpoint.RemoteEndpointClient(channel);

                var state = channel.State;

                state = channel.State;

                var t = Task.Run(async () =>
                {
                    var reply = await client.GetServerInfoAsync(new Empty());

                    Console.WriteLine("{0}-{1}.{2}", reply.Major, reply.Minor, reply.Rpc);
                });

                t.Wait();
                
            }

            channel.ShutdownAsync().Wait();
            Console.ReadKey();

                
        }
    }
}
