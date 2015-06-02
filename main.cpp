/*************************************************************************
    > File Name: ChinaUnicom.cpp
    > Author: ic2y
    > Mail: ic2y@qq.com
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#define DEBUG false
CURL *curl;
char *findPosition;
bool onlineFlag = false;
int sleepTime = 10;
int idOffset = 9;
int idLen = 15;
int passwordOffset = 34;
int passwordLen = 16;
char targetURL[] = "http://www.baidu.com";
char keyword[] = "Content-Length";
int minLen = 10000;//低于10000，说明不是百度的首页 
char loginURL[] = "http://210.77.16.29/cgi-bin/do_login";
char postData[] = "username=000000000000000&password=0000000000000000&drop=0&type=1&n=100";



void autoLogin();
void initCurl();
void debug(char *msg);
void log(char *msg);
void chinaUnicomIsGood();
bool loadConfig(int argc,char *argv[]);


void debug(char *msg){
    if(DEBUG){
        log(msg);
    }
}

void log(char *msg){
    time_t now;
    time(&now);
    printf("[TIME]:%s[DEBUG]:%s\n",ctime(&now),msg);
}

void autoLogin()
{
	log("autologin"); 
    CURL *loginCURL = curl_easy_init();
    if(loginCURL){
        curl_easy_setopt(loginCURL, CURLOPT_READFUNCTION,NULL);
        curl_easy_setopt(loginCURL,CURLOPT_URL,loginURL);
        curl_easy_setopt(loginCURL, CURLOPT_POSTFIELDS, postData);
        curl_easy_perform(loginCURL);
        curl_easy_cleanup(loginCURL);
        log("send login done");
    }else{
        log("send login error");
    }
}
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata){
    findPosition = strstr(ptr,keyword);
    if(findPosition){
    	int len = atoi(ptr + 16);
		if(len > minLen){
	        onlineFlag = true;
	        return 0;			
		} 
    }
    return nmemb * size;
}


bool loadConfig(int argc,char *argv[])
{
    if(argc < 3){
        log("input argv...like: *.out id password");
        return false;
    }
    strncpy(postData + idOffset,argv[1],idLen);
    strncpy(postData + passwordOffset,argv[2],passwordLen);
    if(argc > 3){
        sleepTime = atoi(argv[3]);
    }
    return true;
}


void UCASisGood()
{
    curl_easy_perform(curl);
}

void initCurl(){
    curl = curl_easy_init();
    if(!curl){
        debug("init curl error");
    }else{
        curl_easy_setopt(curl, CURLOPT_URL, targetURL);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_callback);
    }
}

int main(int argc,char *argv[]){

    if(!loadConfig(argc,argv)){
        return -1;
    }
    initCurl();

    while(true){
        onlineFlag = false;
        //test ChinaUnicom is good?If disconnect,auto login
        UCASisGood();
        if(!onlineFlag)
        {
        	log("try log in ");
            autoLogin();
        }else{
            log("net ok");
        }
        sleep(sleepTime);
    }

    return 0;
}
