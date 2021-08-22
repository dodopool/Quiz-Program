#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <pthread.h>
#include <iterator>

using namespace std;

#define NUM 3


typedef struct
{
    int sock;
    int client_id;

} connection_holder;

typedef struct
{
    int total_clients;

}server_holder;

void initialize_questions(vector <string> & questions)
{
	questions[0] = "Which was the first Pokemon to be created by the Pokemon team?";
	questions[1] = "What Pokemon does Pikachu evolve into?";
	questions[2] = "What type of Pokemon is Mewtwo?";
	questions[3] = "The Pokemon Eevee can have multiple evolutions. How many such evolutions exist?";
	questions[4] = "If you need to revive your fainted Pokemon to full health, where do you go?";
	questions[5] = "What is the full form of the word 'Pokemon'?";
	questions[6] = "How many Mega Evolutions does Charizard have?";
	questions[7] = "Which is the only Pokemon that can change its gender?";
	questions[8] = "Which is the first Pokemon that was able to talk in the anime?";
	questions[9] = "Which is the only Pokemon that is able to devolve?";
	questions[10] = "Who was the creator of Pokemon?";
	questions[11] = "Who is the God of all Pokemon?";
	questions[12] = "The Pokemon Hitmonchan got its name from which famous martial artist?";
	questions[13] = "Which Pokemon is considered to be a failed attempt of cloning Mew?";
	questions[14] = "In the anime, which food item does Ash's Pikachu love?";
	questions[15] = "Which Pokemon was originally supposed to be the Mascot for Pokemon Series?";
	questions[16] = "Where is Ash's hometown located?";
	questions[17] = "Which is the most effective ball in the Pokemon game?";
	questions[18] = "In which season of Pokemon Anime does Ash finally win the Pokemon League?";
	questions[19] = "What does Ash dream of becoming one day?";
}

void initialize_answers(vector <vector <string> > & answer)
{
	answer[0].push_back("rhydon");
	answer[0].push_back("rydon");

	answer[1].push_back("raichu");

	answer[2].push_back("psychic");

	answer[3].push_back("8");
	answer[3].push_back("eight");

	answer[4].push_back("pokemon center");
	answer[4].push_back("pokemon centre");
	answer[4].push_back("pokecenter");
	answer[4].push_back("pokecentre");
	answer[4].push_back("pokemoncentre");
	answer[4].push_back("pokemoncenter");
	answer[4].push_back("pokemon_center");
	answer[4].push_back("pokemon_centre");

	answer[5].push_back("pocket monster");
	answer[5].push_back("pocket monsters");
	answer[5].push_back("pocket_monster");
	answer[5].push_back("pocket_monsters");
	answer[5].push_back("pocketmonster");
	answer[5].push_back("pocketmonsters");

	answer[6].push_back("2");
	answer[6].push_back("two");

	answer[7].push_back("azurill");
	answer[7].push_back("azuril");

	answer[8].push_back("meowth");

	answer[9].push_back("slowbro");

	answer[10].push_back("satoshi tajiri");
	answer[10].push_back("satoshi_tajiri");

	answer[11].push_back("arceus");

	answer[12].push_back("jackie chan");
	answer[12].push_back("jackie_chan");

	answer[13].push_back("ditto");
	answer[13].push_back("dito");

	answer[14].push_back("ketchup");

	answer[15].push_back("clefairy");
	answer[15].push_back("clifairy");
	answer[15].push_back("klefairy");
	answer[15].push_back("klifairy");

	answer[16].push_back("kanto");
	answer[16].push_back("pallet");
	answer[16].push_back("pallet town");
	answer[16].push_back("kanto town");
	answer[16].push_back("pallet_town");
	answer[16].push_back("kanto_town");
	answer[16].push_back("town pallet");
	answer[16].push_back("town kanto");
	answer[16].push_back("town_pallet");
	answer[16].push_back("town_kanto");
	answer[16].push_back("townpallet");
	answer[16].push_back("townkanto");
	answer[16].push_back("pallettown");
	answer[16].push_back("kantotown");

	answer[17].push_back("master ball");
	answer[17].push_back("master_ball");
	answer[17].push_back("masterball");
	answer[17].push_back("master");

	answer[18].push_back("20");
	answer[18].push_back("twenty");

	answer[19].push_back("pokemon master");
	answer[19].push_back("pokemon_master");
	answer[19].push_back("pokemonmaster");
	answer[19].push_back("master");
}

void lower_it(string &to_lower)
{
	for(int i = 0 ; i < to_lower.size(); i++)
	{
		int k = to_lower[i];
		if(k >= 65 && k <= 90)
		{
			k = k + 32;
			to_lower[i] = k;
		}
		else
		{
			continue;
		}
	}
}

string number_to_string(int num)
{
    int is_negative = 0;
	string to_return;
    if(num < 0)
    {
        is_negative = 1;
        num = num * (-1);
    }
	while(num / 10 != 0)
	{
		int k = num % 10;
		num = num/10;
		to_return.push_back(k + 48);
	}
	to_return.push_back(num + 48);
    if(is_negative == 1)
    {
        fflush(stdout);
        to_return.push_back('-');
    }
	reverse(to_return.begin(), to_return.end());
	return to_return;
}

int is_corrects(int question_id, vector <vector <string> > &answer, string audience_answer)
{
    int is_ok = 0;
    lower_it(audience_answer);
    for(int j = 0 ; j < answer[question_id].size(); j++)
    {
        if(audience_answer == answer[question_id][j])
        {
            is_ok = 1;
            break;
        }
    }
    return is_ok;
}

int end_signal = 0 ;    //0 is quiz is to continue, else 1
int answer_got = 0;
string what_answer;
int signal_client = 0;
string to_send;
int correct_signal = 0; //1 if correct, -1 if wrong and 0 by default (no answer received)
string end_send_loss = ""; //this will be send when the quiz is ended to the non-winning clients
string rand_ans = "";
int buzz_press = 0; //tell the server_process if someone has pressed the buzzer or not (0 if no one has pressed the buzzer, else 1)
vector <int> answer_tracker;
int t_end_signal = 0;

pthread_mutex_t mtx;

void print(string s)
{
    pthread_mutex_lock(&mtx);
    cout<<s<<"\n";
    fflush(stdout);
    pthread_mutex_unlock(&mtx);
}

int is_corrects_rand(string audience_answer)
{
    int is_ok = 0;
    lower_it(audience_answer);
    if(audience_answer == rand_ans)
    {
        is_ok = 1;
    }
    return is_ok;
}

string num_to_string(int num)
{
    int is_negative = 0;
	string to_return;
    if(num < 0)
    {
        is_negative = 1;
        num = num * (-1);
    }
    while(num / 10 != 0)
    {
        int k = num % 10;
        num = num/10;
        to_return.push_back(k + 48);
    }
	to_return.push_back(num + 48);
    if(is_negative == 1)
    {
        fflush(stdout);
        to_return.push_back('-');
    }
	reverse(to_return.begin(), to_return.end());
	return to_return;
}

string double_to_string(double x)
{
    string to_return = "";
    if(x < 0)
    {
        to_return = to_return + "-";
        x = x * (-1.0);
    }
    int k = x;
    to_return = to_return + num_to_string(k);
    to_return = to_return + ".";
    x = (x - k)*10;
    to_return = to_return + num_to_string(x);
    return to_return;
}

string generator(int number_of_digits, string char_list)
{
    vector <int> indices;
    while(indices.size() != number_of_digits)
    {
        int x = rand() % (char_list.size());
        indices.push_back(x);
    }
    string to_return;
    vector <int>::iterator it;
    for(it = indices.begin(); it != indices.end(); it++)
    {
        to_return.push_back(char_list[*it]);
    }
    return to_return;
}

int string_to_num(string s)
{
    int x = 0;
    for(int i = 0 ; i < s.size(); i++)
    {
        x = x * 10;
        x = x + s[i] - 48;
    }
    return x;
}

int apply_operator(int left, int right, char op)
{
    if(op == '*')
    {
        return (left * right);
    }
    else if(op == '-')
    {
        return (left - right);
    }
    else
    {
        return (left + right);
    }
}

void generate_expression()
{
    rand_ans = "";
    srand(time(NULL));
    int dig_1 = rand() % 3;
    dig_1 = dig_1 + 1;
    int dig_2 = rand() % 3;
    dig_2 = dig_2 + 1;
    int dig_3 = rand() % 3;
    dig_3 = dig_3 + 1;
    //cout<<dig_1<<" "<<dig_2<<" "<<dig_3<<"\n";
    string num_1 = generator(dig_1, "123456789");
    string num_2 = generator(dig_2, "123456789");
    string num_3 = generator(dig_3, "123456789");
    string op_1 = generator(1, "+-*");
    string op_2 = generator(1, "+-*");
    string final_string = num_1 + op_1 + num_2 + op_2 + num_3;
    to_send = to_send + final_string;
    //cout<<final_string<<"\n";
    //cout<<string_to_num(num_1)<<" "<<string_to_num(num_2)<<" "<<string_to_num(num_3)<<" "<<op_1[0]<<" "<<op_2[0]<<"\n";
    int result = 0;
    if(op_1[0] == '*')
    {
        result = apply_operator(string_to_num(num_1), string_to_num(num_2), op_1[0]);
        if(op_2[0] == '*')
        {
        result = apply_operator(result, string_to_num(num_3), op_2[0]);
        }
        else
        {
        result = apply_operator(result, string_to_num(num_3), op_2[0]);
        }
    }
    else
    {
        if(op_2[0] == '*')
        {
            result = apply_operator(string_to_num(num_2), string_to_num(num_3), op_2[0]);
            result = apply_operator(string_to_num(num_1), result, op_1[0]);
        }
        else
        {
            result = apply_operator(string_to_num(num_1), string_to_num(num_2), op_1[0]);
            result = apply_operator(result, string_to_num(num_3), op_2[0]);
        }
    }
    rand_ans = num_to_string(result);
    //cout<<result<<"\n";
}


void * server_process(void * data_server)
{
	fflush(stdout);
    server_holder * data;
    data = (server_holder *) data_server;
	srand(time(NULL));
	unordered_set <int> question_order;
	fflush(stdout);
	while(question_order.size() != 20)
	{
		int x = rand() % 20;
		question_order.insert(x);
	}

	vector <int> question_order_dup;
	unordered_set <int>::iterator it;
	for(it = question_order.begin(); it != question_order.end(); it++)
	{
		question_order_dup.push_back(*it);
	}

	vector <double> scores(3);
	vector <string> questions(20);
	vector <vector <string> > answer(20);
	initialize_questions(questions);
	initialize_answers(answer);

	int total_clients = data->total_clients;
    int i = 0 ;
    while(!end_signal)
    {
        correct_signal = 0;
        answer_got = 0;
        what_answer = "";
        signal_client = 0;
        to_send = "";
        rand_ans = "";
        buzz_press = 0;
        usleep(21000000);
        correct_signal = 0;
        answer_got = 0;
        what_answer = "";
        signal_client = 0;
        to_send = "";
        rand_ans = "";
        buzz_press = 0;
        answer_tracker.push_back(0);
        if(i < 20)
        {
            int question_to_send = question_order_dup[i];
            to_send = number_to_string(i + 1);
            to_send = to_send + ". ";
            to_send = to_send + questions[question_to_send];
            signal_client = 1;
            print(to_send);
            fflush(stdout);
		}
		else
		{
            to_send = number_to_string(i + 1);
            to_send = to_send + ". Evaluate the expression : ";
            generate_expression();
            signal_client = 1;
            print(to_send);
		}

        while(1)
        {
            if(answer_got == 0) //no answer received by the client
            {
                continue;
            }
            else if(answer_got == -1)   //the buzzer was pressed, but the participant was unable to answer in the time
            {
                correct_signal = -1;
                break;
            }
            else if(answer_got == -2 && !buzz_press)   //timer of 10 seconds is over, nobody was able to answer the question (and no one even pressed the buzzer !)
            {
                break;
            }
            else  //the buzzer was pressed, and the answer was recieved;
            {
                if(i <= 19)
                {
                    //print("Client sent answer : " + what_answer);
                    //print(questions[question_order_dup[i]]);//print()
                    //print(what_answer);
                    int is_correct = is_corrects(question_order_dup[i], answer, what_answer);
                    if(is_correct == 1)
                    {
                        correct_signal = 1;
                        while(correct_signal != 0)
                        {
                            continue;
                        }
                    }
                    else
                    {
                        correct_signal = -1;
                        while(correct_signal != 0)
                        {
                            continue;
                        }
                    }
                }
                else
                {
                    int is_correct =  is_corrects_rand(what_answer); //;
                    if(is_correct == 1)
                    {
                        correct_signal = 1;
                        while(correct_signal != 0)
                        {
                            continue;
                        }
                    }
                    else
                    {
                        correct_signal = -1;
                        while(correct_signal != 0)
                        {
                            continue;
                        }
                    }
                }
                break;
            }
        }

        i++;
    }
    pthread_exit(0);
}

/*

int answer_got = 0;
string what_answer;
int signal_client = 0;
string to_send;
int correct_signal = 0; //1 if correct, -1 if wrong and 0 by default (no answer received)

*/

void * process(void * data_client)
{
    int has_answered;
    float score = 0;
    connection_holder * data;
    data = (connection_holder *) data_client;
    start:
    has_answered = 0;
    if(t_end_signal == 1)   //close this thread and end program
    {
        end_send_loss = "### YOU LOST THE QUIZ ###\n" + end_send_loss;
        char * end_send = (char *) malloc(sizeof(char) * (end_send_loss.size() + 1));
        for(int phi = 0 ; phi < end_send_loss.size() ; phi++)
        {
            end_send[phi] = end_send_loss[phi];
        }
        end_send[end_send_loss.size()] = '\0';
        send(data->sock, end_send, end_send_loss.size(), 0);
        end_signal += 1;
        pthread_exit(0);
    }
    while(signal_client == 0)
    {
        continue;
    }
    char * final_send;
    final_send = (char*)malloc(sizeof(char) * (to_send.size() + 1));
    for(int i = 0 ; i < to_send.size(); i++)
    {
        final_send[i] = to_send[i];
    }
    final_send[to_send.size()] = '\0';
    send(data->sock, final_send, to_send.size(), 0);
    char buzzer_pressed = '1';
    recv(data->sock, &buzzer_pressed, sizeof(buzzer_pressed), 0); //received 'p' if pressed
    if(buzzer_pressed == '1')
    {
        has_answered = -2; //no answer by this client
        if(buzz_press != 1)
        {
            answer_got = -2;
        }
        string to_print = "CLIENT ID " + num_to_string(data->client_id) + " DIDN'T ATTEMPT THE QUESTION !";
        print(to_print);
        string score_print = "SCORE OF CLIENT ID " + num_to_string(data->client_id) + " IS " + double_to_string(score);
        print(score_print);
    }
    else        //the buzzer has been pressed
    {
        buzz_press = 1;
        if(answer_got == 0) //nobody has answered this question
        {
            char client_response_nstl[2000] = "\0"; //stores response in non stl way
            string client_response = "\0";
            int bytesReceived = 0;
            bytesReceived = recv(data->sock, client_response_nstl, sizeof(client_response_nstl), 0);
            if(bytesReceived == -1)     //timer of 10 seconds is reached, buzzer pressed but not answered the question
            {
                has_answered = -1;
                answer_got = -1;
            }
            else
            {
                for(int j = 0; j < strlen(client_response_nstl) ; j++)
                {
                    client_response.push_back(client_response_nstl[j]);
                }
                what_answer = client_response;
                has_answered = 1;
                answer_got = 1;
            }
        }
        else            //suppose someone has already pressed the buzzer, then this participant presses buzzer, we need to clear this buffer of data sent by him
        {
            char client_response_waste[2000] = "\0";
            int bytesReceived = 0;
            bytesReceived = recv(data->sock, client_response_waste, sizeof(client_response_waste), 0);
        }

        if(has_answered == -1)
        {
            if(answer_tracker[answer_tracker.size() - 1] == 1)
            {
                string to_print = "CLIENT ID " + num_to_string(data->client_id) + " RESPONDED LATE ";
                print(to_print);
                string score_print = "SCORE OF CLIENT ID " + num_to_string(data->client_id) + " IS " + double_to_string(score);
                print(score_print);
                goto start;
            }
            string to_print = "CLIENT ID " + num_to_string(data->client_id) + " DIDN'T ANSWER THE QUESTION IN TIME !";
            print(to_print);
            score = score - 0.5;
            correct_signal = 0;
            string score_print = "SCORE OF CLIENT ID " + num_to_string(data->client_id) + " IS " + double_to_string(score);
            print(score_print);
            answer_tracker[answer_tracker.size() - 1] = 1;
        }
        else //if(has_answered == 1)
        {
            if(answer_tracker[answer_tracker.size() - 1] == 1)
            {
                string to_print = "CLIENT ID " + num_to_string(data->client_id) + " RESPONDED LATE";
                print(to_print);
                string score_print = "SCORE OF CLIENT ID " + num_to_string(data->client_id) + " IS " + double_to_string(score);
                print(score_print);
                goto start;
            }
            while(correct_signal == 0)
            {
                continue;
            }
            if(correct_signal == 1)
            {
                answer_tracker[answer_tracker.size() - 1] = 1;
                string to_print = "CLIENT ID " + num_to_string(data->client_id) + " GAVE CORRECT ANSWER !";
                print(to_print);
                score = score + 1.0;
                signal_client = 0;
                correct_signal = 0;
                string score_print = "SCORE OF CLIENT ID " + num_to_string(data->client_id) + " IS " + double_to_string(score);
                print(score_print);
            }
            else if(correct_signal == -1)
            {
                answer_tracker[answer_tracker.size() - 1] = 1;
                //print(what_answer);
                string to_print = "CLIENT ID " + num_to_string(data->client_id) + " GAVE WRONG ANSWER !";
                print(to_print);
                score = score - 0.5;
                correct_signal = 0;
                string score_print = "SCORE OF CLIENT ID " + num_to_string(data->client_id) + " IS " + double_to_string(score);
                print(score_print);
            }
        }
        /*
        else
        {
            string to_print = "CLIENT ID " + num_to_string(data->client_id) + " DIDN'T ATTEMPT THE QUESTION !";
            print(to_print);
        }
        */
    }

    if(score <= 4.9 && end_signal != 1)
    {
        goto start;
    }
    else
    {
        t_end_signal = 1;
        if(score >= 5.0)
        {
            string end_send_stl = "### CLIENT WITH ID " + num_to_string(data->client_id) + " HAS WON THE QUIZ ! ###";
            end_send_loss = end_send_stl;
            char * end_send = (char *)malloc(sizeof(char) * (end_send_stl.size() + 1));
            for(int phi = 0 ; phi < end_send_stl.size(); phi++)
            {
                end_send[phi] = end_send_stl[phi];
            }
            end_send[end_send_stl.size()] = '\0';
            send(data->sock, end_send, end_send_stl.size(), 0);
            end_signal += 1;
        }
        else
        {
            end_send_loss = "### YOU LOST THE QUIZ ###\n" + end_send_loss;
            char * end_send = (char *) malloc(sizeof(char) * (end_send_loss.size() + 1));
            for(int phi = 0 ; phi < end_send_loss.size() ; phi++)
            {
                end_send[phi] = end_send_loss[phi];
            }
            end_send[end_send_loss.size()] = '\0';
            send(data->sock, end_send, end_send_loss.size(), 0);
            end_signal += 1;
        }
        print("\n-----------THE QUIZ HAS ENDED-------------");
        fflush(stdout);
        pthread_exit(0);
    }

}

int main()
{
    pthread_mutex_init(&mtx, 0);
    pthread_t thread;
    pthread_t server_thread;

	int clients_to_connect;
	cout<<"Please enter the number of participants in this contest : ";
	fflush(stdout);
	cin>>clients_to_connect;

    server_holder * server_data;
	server_data = (server_holder *)malloc(1 * sizeof(server_holder));
	server_data->total_clients = clients_to_connect;

	//now we start socket programming
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);  //using TCP - reliable
	if(server_socket == -1)
    {
        perror("Could not create socket \n");
    }
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(8080); //connecting to port number 8080
	server_address.sin_addr.s_addr = INADDR_ANY;
    int addrlen = sizeof(server_address);
	if(bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
	{
        perror("Binding failed");
        return -1;
	}

	listen(server_socket, 50); //maximum 50 simultaneous connections allowed

    int total_clients = 0;
    while(total_clients < clients_to_connect)
	{
        connection_holder * connection;
        connection = (connection_holder *) malloc (1 * sizeof(connection_holder));
        connection->client_id = total_clients + 1;
        struct timeval tv;
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        connection->sock = accept(server_socket, (struct sockaddr *) &server_address, (socklen_t * ) &addrlen);
        if(connection->sock <= 0)
        {
            free(connection);
        }
        else
        {
            setsockopt(connection->sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

            //send the client id to the client
            string cid_send_stl = num_to_string(connection->client_id);
            char * cid_send = (char *) malloc(sizeof(char) * (cid_send_stl.size() + 1));
            for(int phi = 0 ; phi < cid_send_stl.size(); phi++)
            {
                cid_send[phi] = cid_send_stl[phi];
            }
            cid_send[cid_send_stl.size()] = '\0';
            send(connection->sock, cid_send, cid_send_stl.size(), 0);

            //start the thread for this client
            pthread_create(&thread, 0, process, (void *) connection);
            total_clients = total_clients + 1;
        }
    }
    pthread_create(&server_thread, 0, server_process, (void *) server_data);
    while(!t_end_signal || (end_signal != clients_to_connect))
    {
        continue;
    }
    close(server_socket);
    pthread_mutex_destroy(&mtx);


	return 0;
}
