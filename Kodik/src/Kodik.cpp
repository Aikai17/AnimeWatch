#include <libxml/HTMLparser.h>
#include <curl/curl.h>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include "../include/Base64.h"
#include "../../AnimeGO/include/cJSON.h"
#include "../include/Kodik_API.h"

using namespace std;
using namespace kodik;

struct memory{
	char* response=nullptr;
	int size=0;
	~memory(){
		if(response)
			free(response);
	}
};
size_t write_callbackfunc(char* data,size_t size,size_t nmemb,void* userdata){
	int realsize=nmemb*size;
	memory* mem=(memory*)userdata;
	mem->response=(char*)realloc(mem->response,mem->size+realsize+1);
	if(mem->response==nullptr){
		printf("NO MEMORY");
		exit(-1);
		free(mem->response);
		return 0;
	}
	memcpy((&mem->response[mem->size]),data,realsize);
	mem->size+=realsize;
	mem->response[mem->size]=0;
	return realsize;
}
struct params{
	string hash;
	string id;
	string type;
	string d;
	string d_sign;
	string pd;
	string pd_sign;
	string ref="";
	string ref_sign;
	string bad_user="true";
	string cdn_is_working="true";
};
string rot_13_encode(string& src);
string get_dash_link(params& url_params);
void traverse_seria_page(params& url_params,char* data);
void traverse_sources_page(string& source,char* data);

string kodik::kodik_get_dash_link(string& link){
	string source;
	memory chunk;

	CURL* curl;
	CURLcode code;

	curl_global_init(0);
	curl=curl_easy_init();
	
	curl_easy_setopt(curl,CURLOPT_URL,link.c_str());
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&chunk);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,write_callbackfunc);

	code=curl_easy_perform(curl);
	
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	if(code!=0){
		return "Kodik: wrong URL";
	}	

	params url_params;
	traverse_seria_page(url_params,chunk.response);
	source=get_dash_link(url_params);
	
	return source;
}
string get_dash_link(params& url_params){
	string source;
	memory chunk;

	CURL* curl;
	CURLcode code;

	curl_global_init(0);
	curl=curl_easy_init();

	string post_params="hash="+url_params.hash+"&id="+url_params.id+"&type="+url_params.type+"&d="+url_params.d+"&d_sign="+url_params.d_sign+"&pd="+url_params.pd+"&pd_sign="+url_params.pd_sign+"&ref="+url_params.ref+"&ref_sign="+url_params.ref_sign+"&bad_user="+url_params.bad_user+"&cdn_is_working="+url_params.cdn_is_working;
	
	curl_easy_setopt(curl,CURLOPT_URL,"https://kodik.info/ftor");
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&chunk);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,write_callbackfunc);
	curl_easy_setopt(curl,CURLOPT_POSTFIELDS,post_params.c_str());
	
	code=curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	if(code!=0){
		return "URL error";
	}

	traverse_sources_page(source,chunk.response);

	return source;
}
void traverse_sources_page(string& source,char* data){
	cJSON* json=cJSON_Parse(data);
	cJSON* links=cJSON_GetObjectItemCaseSensitive(json,"links");
	links=links->child;
	string raw_source;
	while(cJSON* item=links->next){
		raw_source=item->child->child->valuestring;
		links=links->next;
	}
	cJSON_Delete(json);
	raw_source=rot_13_encode(raw_source);
	raw_source=base64_decode(raw_source);
	bool protocol_needed=raw_source.find("https:")==string::npos?true:false;
	if(protocol_needed)
		source="https:"+raw_source;
	else
		source=raw_source;
}
void traverse_seria_page(params& url_params,char* data){
	char* params_begin=strstr(data,"var urlParams = '");
	char* params_end=strstr(params_begin,"}");
	params_begin=params_begin+strlen("var urlParams = '");
	params_end[1]='\0';
	
	cJSON* json=cJSON_Parse(params_begin);
	cJSON* d=cJSON_GetObjectItemCaseSensitive(json,"d");
	cJSON* d_sign=cJSON_GetObjectItemCaseSensitive(json,"d_sign");
	cJSON* pd=cJSON_GetObjectItemCaseSensitive(json,"pd");
	cJSON* pd_sign=cJSON_GetObjectItemCaseSensitive(json,"pd_sign");
	cJSON* ref_sign=cJSON_GetObjectItemCaseSensitive(json,"ref_sign");
	
	url_params.d=d->valuestring;
	url_params.d_sign=d_sign->valuestring;
	url_params.pd=pd->valuestring;
	url_params.pd_sign=pd_sign->valuestring;
	url_params.ref_sign=ref_sign->valuestring;
	
	cJSON_Delete(json);

	char* video_params_begin=strstr(params_end+2,".type = '");
	char* type=video_params_begin+strlen(".type = '");
	char* type_end=strstr(type,"';");
	type_end[0]='\0';
	
	char* hash=strstr(type_end+1,".hash = '");
	hash=hash+strlen(".hash = '");
	char* hash_end=strstr(hash,"';");
	hash_end[0]='\0';

	char* id=strstr(hash_end+1,".id = '");
	id=id+strlen(".id = '");
	char* id_end=strstr(id,"';");
	id_end[0]='\0';

	url_params.type=type;
	url_params.hash=hash;
	url_params.id=id;
}
string rot_13_encode(string& src){
	string alphabet="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	string new_src;
	for(size_t i = 0;i<src.size();i++){
		char symbol=src[i];
		bool is_lower=islower(symbol);
		symbol=toupper(symbol);
		size_t index=string::npos;
		if((index=alphabet.find(symbol))!=string::npos){
			symbol=alphabet[(index+13)%alphabet.size()];
			if(is_lower)
				symbol=tolower(symbol);
		}
		new_src+=symbol;
	}
	return new_src;
}
