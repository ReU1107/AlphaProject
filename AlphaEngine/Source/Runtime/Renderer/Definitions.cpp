#include "Definitions.h"
#include "RenderCore/Texture.h"

namespace Alpha::Renderer
{
    TextureResource::~TextureResource() = default;

    TextureResource::TextureResource(
        TextureResource&&) noexcept = default;

    TextureResource& TextureResource::operator=(
        TextureResource&&) noexcept = default;
}