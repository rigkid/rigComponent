#pragma once

namespace rigkit {
namespace ecs {

/**
 * @brief Session-only entity tag - document IO skips tagged entities.
 * @details Preview scenes, caches, and other host scaffolding live in the same
 *          registry as document content. Tag them so serializers never write
 *          them into a document and content systems can ignore them. The tag
 *          itself has no serializer, so it can never round-trip through a file.
 */
struct CTransient {};

} // namespace ecs
} // namespace rigkit
