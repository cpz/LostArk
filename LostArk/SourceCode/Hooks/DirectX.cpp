#include <kiero/kiero.h>

#if KIERO_INCLUDE_D3D9
# include <impl/d3d9_impl.h>
#endif

#if KIERO_INCLUDE_D3D10
# include <impl/d3d10_impl.h>
#endif

#if KIERO_INCLUDE_D3D11
# include <impl/d3d11_impl.h>
#endif

#if KIERO_INCLUDE_D3D12
#endif

#if KIERO_INCLUDE_OPENGL
#endif

#if KIERO_INCLUDE_VULKAN
#endif

#include <Windows.h>

int kierohook()
{
    if (kiero::init(kiero::RenderType::Auto) == kiero::Status::Success)
    {
        switch (kiero::getRenderType())
        {
#if KIERO_INCLUDE_D3D9
        case kiero::RenderType::D3D9:
            impl::d3d9::init();
            break;
#endif
#if KIERO_INCLUDE_D3D10
        case kiero::RenderType::D3D10:
            impl::d3d10::init();
            break;
#endif
#if KIERO_INCLUDE_D3D11
        case kiero::RenderType::D3D11:
            impl::d3d11::init();
            break;
#endif
        default:
            break;
        }

        return 1;
    }

	return 0;
}