#include <iostream>
#include <string>
#include <list>
#include <mpi.h>
#include <stdlib.h>  
#include <ctime>
#include <chrono>

using namespace std;
using namespace std::chrono;

class Player
{
public:
    string name;
    int points;
    string option;
};

int GetWinner(int* players, int size, int processRank);
Player getMaxPlayer(list<Player> players);

list<Player>::iterator it;
list<Player>::iterator it1;
string options[3] = { "stone", "paper", "scissors" };
int n, counter = 1, result, size_subset = 0, * subset = NULL, final_size_subset = 0, * final_subset = NULL, * players = NULL;



int main(int argc, char** argv) {
    // get time now
    milliseconds msStart = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Find out rank, size
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    const int n = 16;

    players = new int[n];
    for (int i = 1; i <= n; i++)
        players[i - 1] = i;

    size_subset = (n / 4);

    final_size_subset = (n / 4) + n % 4;

    subset = new int[size_subset];

    final_subset = new int[final_size_subset];

    MPI_Scatter(players, size_subset, MPI_INT, subset, size_subset, MPI_INT, 0, MPI_COMM_WORLD);


    int winner = GetWinner(subset, size_subset, world_rank);

    MPI_Gather(&winner, 1, MPI_INT, final_subset, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        for (int i = (n % 4); i > 0; i--)
            final_subset[final_size_subset - i] = players[n - i];

        int finalWinner = GetWinner(final_subset, final_size_subset, world_rank);
        std::cout << "the final winner is player " << finalWinner;
    }
    // Finalize the MPI environment.
    MPI_Finalize();
    // get time now
    milliseconds msEnd = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    cout << "\n\n\nExecution Time : " << msEnd.count() - msStart.count() << "\nprocess rank : " << world_rank << "\n\n\n";
}

int GetWinner(int* players, int size, int processRank) {
    list<Player> winners(0), _players(0); Player player, oldPlayer; int round = 1, i = 0;

    cout << "\n- name of each player : (Process rank is " << processRank << ")\n";
    for (int i = 0; i < size; i++)
    {
        player.name = to_string(players[i]);
        _players.push_back(player);
        std::cout << player.name + "\t";
    }
    while (winners.size() != 1)
    {
        if (_players.size() % 2 == 1)
            oldPlayer = getMaxPlayer(_players);
        else
            oldPlayer.points = -1;
        winners.clear();
        std::cout << "\n\nround " << round << " :\n";
        for (it = _players.begin(); it != _players.end(); advance(it, 2))
        {
            it1 = next(it, 1);
            it1->points = 0; it->points = 0; counter = 1;
            while (counter <= 10 || (counter > 10 && it->points == it1->points))
            {
                result = std::rand() % 3;
                it->option = options[result];
                result = std::rand() % 3;
                it1->option = options[result];
                std::cout << "player " << it->name << " choose : " << it->option;
                std::cout << "   player " << it1->name << " choose : " << it1->option << "\n";

                if (it->option == "stone" && it1->option == "paper")
                {
                    it1->points++;
                    std::cout << "\nplayer " << it1->name << " its points " << it1->points << "\n";
                }
                else if (it->option == "stone" && it1->option == "scissors")
                {
                    it->points++;
                    std::cout << "\nplayer " << it->name << " its points " << it->points << "\n";
                }
                else if (it->option == "scissors" && it1->option == "stone")
                {
                    it1->points++;
                    cout << "\nplayer " << it1->name << " its points " << it1->points << "\n";
                }
                else if (it->option == "scissors" && it1->option == "paper")
                {
                    it->points++;
                    cout << "\nplayer " << it->name << " its points " << it->points << "\n";
                }
                else if (it->option == "paper" && it1->option == "stone")
                {
                    it->points++;
                    cout << "\nplayer " << it->name << " its points " << it->points << "\n";
                }
                else if (it->option == "paper" && it1->option == "scissors")
                {
                    it1->points++;
                    cout << "\nplayer " << it1->name << " its points " << it1->points << "\n";
                }
                counter++;
            }

            std::cout << "\nplayer " << it->name << " its points " << it->points;
            std::cout << "\tplayer " << it1->name << " its points " << it1->points << "\n";
            if (it->points > it1->points)
                winners.push_back(*it);
            else
                winners.push_back(*it1);
            i += 2;
        }
        _players.clear();
        if (oldPlayer.points != -1)
            winners.push_back(oldPlayer);
        for (it = winners.begin(); it != winners.end(); it++)
            _players.push_back(*it);
        round++;
    }
    return atoi(winners.begin()->name.c_str());
}

Player getMaxPlayer(list<Player> players)
{
    int max = -1;
    Player player;
    for (it = players.begin(); it != players.end(); it++)
    {
        if (it->points > max)
        {
            player = *it;
            max = it->points;
        }
    }
    list<Player> _players;
    for (it = players.begin(); it != players.end(); it++) {
        if ((*it).name != player.name)
            _players.push_back(*it);
    }
    return player;
}
