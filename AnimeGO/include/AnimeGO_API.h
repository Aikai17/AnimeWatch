#pragma once
#include <vector>
#include "Assistive.h"


namespace animego{
	struct dub{
		std::string dub_id;
		std::string dub_name;
	};
	struct player{
		std::string player_id;
		std::string player_name;
		std::string link;
	};
	struct title{
		std::string name;
		std::string link;
		std::string type;
		std::string date;
	};
	struct episode{
		std::string seria;
		std::string title;
		std::string date;
		std::string id;
	};
	struct embed{
		std::string player_name;
		std::string link;
		std::string dub_name;
	};
	//Returns vector of titles (name,link,type,date)
	std::vector<title> search(std::string title);
	//Returns vector of title's episodes {titleLink} (seria,title,date,id)
	std::vector<episode> get_title_info(std::string titleLink);
	//Returns embeds vector title's episode {episode_id}
	std::vector<embed> get_embed_link(std::string id,std::string episode_id);
	//Returns media link (m3u8,mpd)
	std::string aniboom_get_media(std::string embed_link);

}

