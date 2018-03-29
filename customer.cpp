//this is a customer .cpp
#include<iostream>
using namespace std;
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/select.h>
#include<fcntl.h>
#include<errno.h>
#include<vector>
#include<fstream>

#define SERV_PORT 22223

int main()
{
	vector<string>vec_service;
	ifstream fip("servrip");
	string servip;
	while(fip>>servip)
	{
		cout<<servip<<endl;
		vec_service.push_back(servip);
	}
	vector<int>vec_socketconnect(vec_service.size(),socket(AF_INET,SOCK_STREAM,0));
	struct sockaddr_in servaddr;
	memset(&servaddr,'\0',sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET,"10.0.2.15",&servaddr.sin_addr);
	
	while(true)
	{
		
		string strcmd;
		cin>>strcmd;
		if(strcmd != "r" && strcmd != "k" && strcmd != "f" && strcmd != "s")
		{	
			continue;
		}
		for(int i = 0; i<vec_service.size(); i++)
		{
			if(0 == connect(vec_socketconnect[i],(sockaddr *)&servaddr,sizeof(servaddr)))
			{
				send(vec_socketconnect[i],strcmd.c_str(),1,0);
			}
				
		}
	}
	return 0;
}
