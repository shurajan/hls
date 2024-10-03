//
// Created by sasha on 10/3/24.
//

#ifndef PLAYLIST_PARSER_SITE_CBR_H
#define PLAYLIST_PARSER_SITE_CBR_H

#include "PlaylistParserBase.h"

// Конкретная реализация для сайта CBR
class PlaylistParserSiteCbr : public PlaylistParserBase {
public:
   std::vector<std::string> parse_m3u8_playlist(const std::string& playlist, Resolution resolution) override;
};

#endif // PLAYLIST_PARSER_SITE_CBR_H

