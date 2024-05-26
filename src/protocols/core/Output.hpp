#pragma once

#include <memory>
#include <vector>
#include <cstdint>
#include "../WaylandProtocol.hpp"
#include "wayland.hpp"
#include "../../helpers/signal/Listener.hpp"

class CMonitor;

class CWLOutputResource {
  public:
    CWLOutputResource(SP<CWlOutput> resource_, SP<CMonitor> pMonitor);
    static SP<CWLOutputResource> fromResource(wl_resource*);

    bool                         good();
    wl_client*                   client();
    SP<CWlOutput>                getResource();
    void                         updateState();

    WP<CMonitor>                 monitor;

    WP<CWLOutputResource>        self;

  private:
    SP<CWlOutput> resource;
    wl_client*    pClient = nullptr;
};

class CWLOutputProtocol : public IWaylandProtocol {
  public:
    CWLOutputProtocol(const wl_interface* iface, const int& ver, const std::string& name, SP<CMonitor> pMonitor);

    virtual void          bindManager(wl_client* client, void* data, uint32_t ver, uint32_t id);

    SP<CWLOutputResource> outputResourceFrom(wl_client* client);

    WP<CMonitor>          monitor;

  private:
    void destroyResource(CWLOutputResource* resource);

    //
    std::vector<SP<CWLOutputResource>> m_vOutputs;

    struct {
        CHyprSignalListener modeChanged;
    } listeners;

    friend class CWLOutputResource;
};

namespace PROTO {
    inline std::unordered_map<std::string, UP<CWLOutputProtocol>> outputs;
};
