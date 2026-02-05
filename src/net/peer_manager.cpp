#include "net/peer_manager.h"
#include <iostream>

using namespace libp2p;
using namespace libp2p::host;
using namespace libp2p::transport;
using namespace libp2p::security;
using namespace libp2p::multi;

PeerManager::PeerManager(const std::string &listenAddr): listenAddress(listenAddr) {}

bool PeerManager::start() {
    try {
        auto noise = std::make_shared<noise::Noise>();
        auto tcp    = std::make_shared<tcp::TcpTransport>(HostConfig::defaultConfig().io);

        host = createHost(
            HostConfig{
                .transport = {tcp},
                .securityTransports = {noise}
            }
        );

        auto ma = Multiaddress::create(listenAddress).value();
        host->listen(ma);

        std::cout << "Listening at " << listenAddress << std::endl;
        return true;
    } catch(const std::exception &e) {
        std::cerr << "P2P start failed: " << e.what() << std::endl;
        return false;
    }
}

bool PeerManager::connectTo(const std::string &peerAddr) {
    try {
        auto ma = Multiaddress::create(peerAddr).value();
        host->network().connect(ma);
        return true;
    } catch (...) {
        return false;
    }
}

void PeerManager::broadcast(const std::string &topic, const std::string &msg) {
    for (auto &conn : host->getConnections()) {
        host->write(conn, topic, msg);
    }
}

void PeerManager::handleMessage(const std::string &topic,
                                const std::string &from,
                                const std::string &msg) {
    std::cout << "[" << topic << "] from " << from << ": " << msg << std::endl;
}
