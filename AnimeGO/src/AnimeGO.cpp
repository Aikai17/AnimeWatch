#include <libxml/HTMLparser.h>
#include <curl/curl.h>
#include <cstdlib>
#include <string.h>
#include <map>

#include "../include/AnimeGO_API.h"
#include "../include/cJSON.h"


using namespace std;
using namespace animego;

struct memory{
	char* response=nullptr;
	int size=0;
	~memory(){
		if(response)
			free(response);
	}
};
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata){
	int realsize=size*nmemb;
	memory* mem=(memory*)userdata;
	mem->response=(char*)realloc(mem->response,mem->size+realsize+1);
	if(mem->response==nullptr){
		exit(-1);
		return 0;
	}
	memcpy(&(mem->response[mem->size]),ptr,realsize);
	mem->size+=realsize;
	mem->response[mem->size]=0;
	return realsize;
}

void traverse_embed_page(htmlNodePtr a_node,vector<dub>& dubs,dub& cur_dub,vector<player>& players,player& cur_player);
void traverse_info_page(htmlNodePtr a_node,vector<episode>& episodes,episode& cur_episode); 
void traverse_search(htmlNodePtr a_node,vector<title>& titles,title& cur_title);

vector<title> animego::search(string name){
	CURL* curl;
	CURLcode code;
	memory chunk{0};
	vector<title> titles;
	
	curl_global_init(0);
	curl=curl_easy_init();

	char* validTitle=curl_easy_escape(curl,name.c_str(),name.size());
	string URL="https://animego.org/search/anime?q=";
	URL.append(validTitle);
	delete[] validTitle;
	
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,write_callback);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&chunk);
	curl_easy_setopt(curl,CURLOPT_URL,URL.c_str());
	code=curl_easy_perform(curl);

	char* check=strstr(chunk.response,"По запросу");

	curl_easy_cleanup(curl);
	curl_global_cleanup();
	if(check||code!=0)
		return titles;

	LIBXML_TEST_VERSION
	htmlDocPtr doc=htmlReadMemory(chunk.response,chunk.size,NULL,NULL,
			HTML_PARSE_NOBLANKS|HTML_PARSE_NOERROR|HTML_PARSE_NOWARNING|HTML_PARSE_NONET);

	htmlNodePtr root=xmlDocGetRootElement(doc);
	
	title last_title;
	traverse_search(root,titles,last_title);
	
	xmlFreeDoc(doc);
	xmlCleanupParser();

	return titles;
}

vector<episode> animego::get_title_info(string link){
	vector<episode> episodes;
	link+="?type=episodeSchedule&episodeNumber=9999";
	CURL* curl;
	CURLcode code;
	curl_slist* slist=nullptr;
	memory chunk{0};

	curl_global_init(0);
	curl=curl_easy_init();

	slist=curl_slist_append(slist,"Accept: \"application/json, text/javascript, */*; q=0.01\"");
	slist=curl_slist_append(slist,"X-Requested-With: XMLHttpRequest");
	curl_easy_setopt(curl,CURLOPT_HTTPHEADER,slist);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,write_callback);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&chunk);
	curl_easy_setopt(curl,CURLOPT_URL,link.c_str());
	
	code=curl_easy_perform(curl);
	curl_slist_free_all(slist);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	if(code!=0)
		return episodes;
	
	cJSON* json=cJSON_Parse(chunk.response);
	cJSON* status=cJSON_GetObjectItemCaseSensitive(json,"status");
	cJSON* content=cJSON_GetObjectItemCaseSensitive(json,"content");
	bool isFilm=false;

	if(strcmp(status->valuestring,"success")!=0){
		cJSON_Delete(json);
		return episodes;
	}
	if(content->valuestring[0]=='\0')
		isFilm=true;
	
	if(!isFilm){
		htmlDocPtr doc=htmlReadMemory(content->valuestring,strlen(content->valuestring),NULL,"UTF-8",HTML_PARSE_NOBLANKS|HTML_PARSE_NOERROR|HTML_PARSE_NOWARNING|HTML_PARSE_NONET);
		if(!doc)
			return episodes;
		htmlNodePtr root=xmlDocGetRootElement(doc);
		if(!root)
			return episodes;
		
		episode last_episode;
		traverse_info_page(root,episodes,last_episode);

		xmlFreeDoc(doc);
		xmlCleanupParser();
	}
	cJSON_Delete(json);
	
	return episodes;
}
vector<embed> animego::get_embed_link(string episode_id,string episode){
	CURL* curl;
	CURLcode code;
	vector<embed> embeds;
	memory chunk{0};
	curl_slist* slist=nullptr;
	string URL="https://animego.org/anime/series?episode="+episode+"&id="+episode_id;

	curl_global_init(0);
	curl=curl_easy_init();
	slist=curl_slist_append(slist,"Referer: https://animego.org");
	slist=curl_slist_append(slist,"X-Requested-With: XMLHttpRequest");

	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,write_callback);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&chunk);
	curl_easy_setopt(curl,CURLOPT_HTTPHEADER,slist);
	curl_easy_setopt(curl,CURLOPT_URL,URL.c_str());

	code=curl_easy_perform(curl);

	curl_slist_free_all(slist);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	if(code!=0){
		return embeds;
	}

	cJSON* json=cJSON_Parse(chunk.response);
	cJSON* status=cJSON_GetObjectItemCaseSensitive(json,"status");
	cJSON* content=cJSON_GetObjectItemCaseSensitive(json,"content");
	if(strcmp(status->valuestring,"success")!=0){
		cJSON_Delete(json);
		return embeds;
	}

	htmlDocPtr doc=htmlReadMemory(content->valuestring,strlen(content->valuestring),NULL,"UTF-8",
			HTML_PARSE_NOBLANKS|HTML_PARSE_NOERROR|HTML_PARSE_NOWARNING|HTML_PARSE_NONET);
	if(!doc)
		return embeds;
	htmlNodePtr root=xmlDocGetRootElement(doc);
	if(!root)
		return embeds;
	
	vector<dub> dubs;
	vector<player> players;
	player last_player;
	dub last_dub;
	traverse_embed_page(root,dubs,last_dub,players,last_player);

	cJSON_Delete(json);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	
	map<string,string> id_to_name;
	embed cur_embed;
	for(size_t i = 0;i<dubs.size();i++){
		id_to_name[dubs[i].dub_id]=dubs[i].dub_name;
	}
	for(size_t i = 0;i<players.size();i++){
		cur_embed.player_name=players[i].player_name;
		cur_embed.dub_name=id_to_name[players[i].player_id];
		cur_embed.link=players[i].link;
		embeds.push_back(cur_embed);
	}
	return embeds;
}

string animego::aniboom_get_media(string embed_link){
	memory chunk;
	CURL* curl;
	CURLcode code;
	curl_slist* slist=nullptr;
	
	slist=curl_slist_append(slist,"Referer: https://animego.org/");
	curl_global_init(0);
	curl=curl_easy_init();

	curl_easy_setopt(curl,CURLOPT_HTTPHEADER,slist);
	curl_easy_setopt(curl,CURLOPT_URL,embed_link.c_str());
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,write_callback);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&chunk);
	
	code=curl_easy_perform(curl);

	curl_slist_free_all(slist);
	curl_easy_cleanup(curl);
	curl_global_cleanup();	
	if(code!=0){
		return "aniboom error";	
	}
	
	string source=aniboom_get_dash_link(chunk.response);
	
	return source;
}
void traverse_search(htmlNodePtr a_node,vector<title>& titles,title& cur_title){
	htmlNodePtr cur_node;
	if(a_node==nullptr)
		return;

	for(cur_node=a_node;cur_node;cur_node=cur_node->next){
		xmlChar* value=xmlGetProp(cur_node,(xmlChar*)"class");
		if(value){
			if(strstr((char*)value,"animes-grid-item-body-info")){
				htmlNodePtr infoNode=cur_node->children;
				xmlChar* type=xmlNodeGetContent(infoNode->children);
				if(xmlStrcmp(type,(xmlChar*)"Фильм")==0){
					continue;
				}
				infoNode=infoNode->next;
				infoNode=infoNode->next;
				xmlChar* date=xmlNodeGetContent(infoNode->children);
				cur_title.date=(char*)date;
				cur_title.type=(char*)type;
				titles.push_back(cur_title);
				xmlFree(type);
				xmlFree(date);
			}
			else if(strstr((char*)value,"card-title")){
				htmlNodePtr cardNode=cur_node->children;
				xmlChar* episode_info=xmlGetProp(cardNode,(xmlChar*)"href");
				xmlChar* title=xmlNodeGetContent(cardNode);
				cur_title.name=(char*)title;
				cur_title.link=(char*)episode_info;
				xmlFree(episode_info);
				xmlFree(title);
			}
			xmlFree(value);	
		}
		traverse_search(cur_node->children,titles,cur_title);
	}
}

void traverse_info_page(htmlNodePtr a_node,vector<episode>& episodes,episode& cur_episode){
	htmlNodePtr cur_node=nullptr;
	if(a_node==nullptr)
		return;

	for(cur_node=a_node;cur_node;cur_node=cur_node->next){
		xmlChar* body_value=xmlGetProp(cur_node,(xmlChar*)"class");
		xmlChar* attr=(xmlChar*)"released-episodes-item";
		xmlChar* data_id=xmlGetProp(cur_node,(xmlChar*)"data-watched-id");

		if(data_id){
			cur_episode.id=(char*)data_id;
			episodes.push_back(cur_episode);
			xmlFree(data_id);
		}
		if(xmlStrncmp(body_value,attr,xmlStrlen(attr))==0){
			htmlNodePtr body_node=cur_node->children;
			xmlChar* node_value = xmlGetProp(body_node,(xmlChar*)"class");
			xmlChar* value=(xmlChar*)"row m-0";

			if(xmlStrcmp(node_value,value)==0){
				htmlNodePtr episode_node=body_node->children;
				xmlChar* episode=xmlNodeGetContent(episode_node->children);
				episode_node=episode_node->next;
				xmlChar* episode_name=xmlNodeGetContent(episode_node);
				episode_node=episode_node->next;
				xmlChar* episode_release=xmlNodeGetContent(episode_node->children->children);
				
				xmlChar* valid_name = skip_whitespaces(episode_name);//dont free
				cur_episode.date=(char*)episode_release;
				cur_episode.seria=(char*)episode;
				cur_episode.title=(char*)valid_name;
				

				xmlFree(episode_release);
				xmlFree(episode_name);
				xmlFree(episode);
			}
			if(node_value)
				xmlFree(node_value);
		}
		if(body_value)
			xmlFree(body_value);
		traverse_info_page(cur_node->children,episodes,cur_episode);
	}

}
void traverse_embed_page(htmlNodePtr a_node,vector<dub>& dubs,dub& cur_dub, vector<player>& players, player& cur_player){
	htmlNodePtr cur_node=nullptr;
	if(!a_node)
		return;

	for(cur_node=a_node;cur_node;cur_node=cur_node->next){
		xmlChar* id=xmlGetProp(cur_node,(xmlChar*)"id");
		if(id){
			if(xmlStrcmp(id,(xmlChar*)"video-dubbing")==0){
				htmlNodePtr dubbing_node=cur_node->children;
				while(dubbing_node){
					xmlChar* data_dub=xmlGetProp(dubbing_node,(xmlChar*)"data-dubbing");
					xmlChar* name_dub=xmlNodeGetContent(dubbing_node->children);			
					xmlChar* valid_data=skip_whitespaces(data_dub);
					xmlChar* valid_name=skip_whitespaces(name_dub);
					
					cur_dub.dub_name=(char*)valid_name;
					cur_dub.dub_id=(char*)valid_data;
					dubs.push_back(cur_dub);

					xmlFree(name_dub);
					xmlFree(data_dub);
					if(!dubbing_node->next)
						break;
					dubbing_node=dubbing_node->next;
					if(!dubbing_node->next)
						break;
					dubbing_node=dubbing_node->next;
				}
			}
			if(xmlStrcmp(id,(xmlChar*)"video-players")==0){
				htmlNodePtr playerNode=cur_node->children;

				while(playerNode){
					xmlChar* data_player=xmlGetProp(playerNode,(xmlChar*)"data-player");
					xmlChar* data_dub=xmlGetProp(playerNode,(xmlChar*)"data-provide-dubbing");
					xmlChar* player_name=xmlNodeGetContent(playerNode->children);
					
					string valid_link="https:";
					valid_link.append((char*)data_player);
					
					if(xmlStrcmp(player_name,(xmlChar*)"AniBoom")==0||xmlStrcmp(player_name,(xmlChar*)"Kodik")==0){
						cur_player.player_id=(char*)data_dub;
						cur_player.player_name=(char*)player_name;
						cur_player.link=valid_link;
						players.push_back(cur_player);
					}
					xmlFree(data_dub);
					xmlFree(data_player);
					xmlFree(player_name);

					if(!playerNode->next)
						break;
					playerNode=playerNode->next;
					if(!playerNode->next)
						break;
					playerNode=playerNode->next;
				}
			}
			
			xmlFree(id);	
		}

		traverse_embed_page(cur_node->children,dubs,cur_dub,players,cur_player);
	}
}
