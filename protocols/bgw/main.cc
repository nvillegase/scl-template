#include <scl/net/config.h>
#include <scl/net/network.h>
#include <scl/net/tcp_channel.h>
#include <scl/protocol/base.h>
#include <scl/util/cmdline.h>

#include "bgw.cpp"

using namespace scl;

void BGWExecution(const util::ProgramOptions& opts) {

  // Get the ID of this party and the network config filename that was passed to
  // the program on the commandline.
  auto id = opts.Get<std::size_t>("id");
  auto conf = opts.Get("conf");

  // Create a NetworkConfig object from the file.
  auto network_conf = net::NetworkConfig::Load(id, std::string(conf));

  // Create a network. This takes care of connecting all the parties to each
  // other, using the information in the network config.
  auto network = net::Network::Create<net::TcpChannel<>>(network_conf);

  // This lambda will be called whenever our protocol generates any output.
  auto output_cb = [](std::any output) {
    auto xy = std::any_cast<Mersenne61>(output);
    std::cout << "xy = " << xy << endl;
  };

  // Evaluate the protocol for 5 rounds.
  proto::Evaluate(BGWProtocol::Create((int)id, 2, 5), network, output_cb);
}

int main(int argc, char** argv) {
  // This adds some command line arguments to our program, of which there are
  // only two :)
  const auto opts =
      util::ProgramOptions::Parser{}
          .Add(util::ProgramArg::Required("id", "int", "ID of this party"))
          .Add(util::ProgramArg::Required("conf", "string", "network config"))
          .Parse(argc, argv);

  BGWExecution(opts);
}