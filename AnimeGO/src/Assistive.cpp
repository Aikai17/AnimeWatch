#include <libxml/HTMLparser.h>
#include <cstring>
#include "../include/Assistive.h"
using namespace std;

string get_anime_id(string& link){
	string id;
	size_t index=0;
	for(index = link.size()-1;link[index]!='-';index--){}
	index+=1;//skip minus
	while(index<link.size()){
		id+=link[index];
		index++;
	}
	return id;
}

xmlChar* skip_whitespaces(xmlChar* cur){
	xmlChar* occurence=nullptr;
	int i = 0;
	int len=xmlStrlen(cur);
	while(cur[i]==' '||cur[i]=='\n'){
		i++;
	}
	occurence=cur+i;
	while(cur[i]!='\n'){
		if(i>=len)
			return occurence;
		i++;
	}
	cur[i]='\0';
	return occurence;
}
std::string aniboom_get_dash_link(char* chunk){
	char* str=strstr(chunk,"data-parameters");
	char* dash=strstr(str,"&quot;dash&quot;:");
	if(!dash)
		return "";
	char* dash_src=strstr(dash,"https:");
	char* border=strstr(dash_src,"}");
	border[1]='\0';
	char* dash_end=strstr(dash_src,".mpd");
	if(dash_end)
		dash_end[strlen(".mpd")]='\0';
	else{
		dash_end=strstr(dash_src,".m3u8");
		dash_end[strlen(".m3u8")]='\0';
	}	
	string result;
	for(int i = 0;dash_src[i]!='\0';i++){
		if(dash_src[i]=='\\')
			continue;
		result+=dash_src[i];
	}
	if(result[result.size()-1]==' ')
		result[result.size()-1]='\0';
	return result;
}

