#pragma once

namespace rigkit {

class rigProject;

/** @brief Register portable POD codecs owned by rigComponent onto a document pack. */
void registerComponentSerializers(rigProject& doc);

} // namespace rigkit
