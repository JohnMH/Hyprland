#include "DMABuffer.hpp"
#include "WLBuffer.hpp"
#include "../../render/Renderer.hpp"
#include "../ToplevelExportWlrFuncs.hpp"

CDMABuffer::CDMABuffer(uint32_t id, wl_client* client, SDMABUFAttrs attrs_) : attrs(attrs_) {
    g_pHyprRenderer->makeEGLCurrent();

    Debug::log(LOG, "DMABuffer: Creating buffer {:x}", (uintptr_t)this);

    size     = attrs.size;
    resource = CWLBufferResource::create(makeShared<CWlBuffer>(client, 1, id));

    eglImage = g_pHyprOpenGL->createEGLImage(attrs);

    if (!eglImage)
        return;

    texture = makeShared<CTexture>(attrs, eglImage);
    opaque  = pixel_format_is_opaque(attrs.format);
    success = texture->m_iTexID;

    listeners.resourceDestroy = events.destroy.registerListener([this](std::any d) {
        listeners.resourceDestroy.reset();
        destroyData();
    });

    if (!success)
        Debug::log(ERR, "Failed to create a dmabuf: texture is null");
}

CDMABuffer::~CDMABuffer() {
    destroyData();
}

eBufferCapability CDMABuffer::caps() {
    return BUFFER_CAPABILITY_DATAPTR;
}

eBufferType CDMABuffer::type() {
    return BUFFER_TYPE_DMABUF;
}

void CDMABuffer::update(const CRegion& damage) {
    ;
}

SDMABUFAttrs CDMABuffer::dmabuf() {
    return attrs;
}

std::tuple<uint8_t*, uint32_t, size_t> CDMABuffer::beginDataPtr(uint32_t flags) {
    // FIXME:
    return {nullptr, 0, 0};
}

void CDMABuffer::endDataPtr() {
    // FIXME:
}

bool CDMABuffer::good() {
    return success;
}

void CDMABuffer::updateTexture() {
    ;
}

void CDMABuffer::destroyData() {
    Debug::log(LOG, "DMABuffer: Destroying buffer {:x}", (uintptr_t)this);

    closeFDs();

    // we do not destroy this as CTexture has taken ownership of the image
    // TODO: wrap eglImage
    eglImage = nullptr;
}

void CDMABuffer::closeFDs() {
    for (int i = 0; i < attrs.planes; ++i) {
        if (attrs.fds[i] == -1)
            continue;
        close(attrs.fds[i]);
        attrs.fds[i] = -1;
    }
}