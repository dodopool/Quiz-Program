#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <err.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <string.h>

using namespace std;

int string_to_num(string s)	//will return 0 if some ill-formatted is passed to it. Caution, the number must be STRICTLY POSITIVE
{
	int x = 0;
	int iterator = 0;
	while(iterator < s.size() && s[iterator] != '.')
	{
		x = x * 10;
		x = x + s[iterator] - 48;
		iterator ++;
	}
	return x;
}

int main()
{
	int last_q_id = -1;
	char client_id[10] = "\0";
	string str_client_id = "";
	int client_socket;
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(8080);
	server_address.sin_addr.s_addr = INADDR_ANY;
	int connection_status = connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address));
	if(connection_status == 0)
	{
		cout<<"Connection to server was successful :)\n";
		fflush(stdout);
		recv(client_socket, client_id, sizeof(client_id), 0);
		for(int phi = 0 ; phi < strlen(client_id); phi++)
		{
			str_client_id.push_back(client_id[phi]);
		}
		cout<<"You connection ID is : "<<str_client_id<<"\n";
		fflush(stdout);
		cout<<"\n\n -------------RULES------------\n\n";
		cout<<"1. There is a time limit of 10 seconds to press the buzzer\n";
		cout<<"2. To press buzzer, enter character 'p', and press the enter key\n";
		cout<<"3. After pressing buzzer, you have 10 seconds to answer question\n";
		cout<<"ASSUMING YOU ARE THE FIRST ONE TO PRESS THE BUZZER\n";
		cout<<"4. If you given correct answer, you will get 1.0 points\n";
		cout<<"5. If you are give wrong answer, 0.5 points will be deducted from your score\n";
		cout<<"6. If you give do not attempt the question after pressing buzzer, 0.5 points will be deducted from your score\n";
		cout<<"7. The quiz ends when one of the participants scores points greater than or equal to 5\n";
		cout<<"\n---------------------------------\n\n";
		fflush(stdout);
	}
	else
	{
		cout<<"There was an error connecting to the server :( \n";
		fflush(stdout);
		return 0;
	}
	start:
	if(connection_status != -1)
	{
		char buffer[1000] = "\0";
		int bytes_rec = 0;
		if((bytes_rec = recv(client_socket, buffer, sizeof(buffer), 0) < 0))
		{
			cout<<"ERROR \n";
		}
		else
		{
			string to_check = "";
			for(int phi = 0  ; phi < strlen(buffer) ; phi++)
			{
				to_check.push_back(buffer[phi]);
			}

			if(buffer[0] == '#' && buffer[1] == '#' && buffer[2] == '#')
			{
				cout<<"The quiz has ended ! \n";
				cout<<buffer<<"\n";
				fflush(stdout);
				return 0;
			}
			if(last_q_id != -1)
			{
				if(string_to_num(to_check) == last_q_id)
				{
					goto start;
				}
			}
			last_q_id = string_to_num(to_check);
			cout<<buffer<<"\n";
			fflush(stdout);
			cout<<"Press 'p' to press buzzer : ";
			fflush(stdout);

			string value = "";
			struct timeval tmo;
			fd_set readfds;

			FD_ZERO(&readfds);
			FD_SET(0, &readfds);
			tmo.tv_sec = 10;
			tmo.tv_usec = 0;

			switch(select(1, &readfds, NULL, NULL, &tmo))
			{
				case -1:
					err(1, "select");
					break;
				case 0:
					cout<<"\nYou did not answer this question !\n";
					goto end;
			}

			cin>>value;

			if((value == "p" || value == "P") && value.size() != 0) 			//buzzer has been pressed !
			{
				char ch;
				ch = value[0];
				send(client_socket, &ch, sizeof(ch),0);
				cout<<"The buzzer was pressed ! \nEnter your answer within 10 seconds : ";
				fflush(stdout);
				string answer = "";
				struct timeval tmoa;
				fd_set readfdsa;

				FD_ZERO(&readfdsa);
				FD_SET(0, &readfdsa);
				tmoa.tv_sec = 10;
				tmoa.tv_usec = 0;

				switch(select(1, &readfdsa, NULL, NULL, &tmoa))
				{
					case -1:
						err(1, "select");
						break;
					case 0:
						cout<<"\nYou pressed the buzzer but failed to answer question ! \n";
						goto end;	//this should not be return 1, as it will end the client program -- SEE THIS
				}
				getline(cin>>ws, answer);//cin>>answer;
				if(answer.size() != 0)
				{
					char *to_send = (char *)malloc(sizeof(char) * (answer.size() + 1));
					for(int j = 0 ; j < answer.size(); j++)
					{
						to_send[j] = answer[j];
					}
					to_send[answer.size()] = '\0';
					send(client_socket, to_send, answer.size(), 0);

					//send the answer to server
				}
				else
				{
						cout<<"\nYou pressed the buzzer but failed to answer question ! \n";
						goto end;	//this should not be return 1, as it will end the client program -- SEE THIS
				}

			}
			else							//what to do if buzzer is not pressed ?
			{
				cout<<"\n\n\nYou did not answer this question ! \n";
				fflush(stdout);
				goto end;
			}
		}
	}
	end:
	goto start;

	return 0;
}

