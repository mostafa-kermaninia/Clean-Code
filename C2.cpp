#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

struct P_INFO
{
    vector<string> name;
    vector<int> openingTime;
    vector<int> closingTime;
    vector<int> rank;
    vector<int> visited;
};

int number_of_places_finder(vector<string> readed_string)
{
    int num_of_places = readed_string.size() / 4 - 1;
    return num_of_places;
}

vector<int> order_of_informations_finder(vector<string> readed_string)
{
    vector<int> order_of_informations;
    vector<string> info = {"name", "openingTime", "closingTime", "rank"};
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (info[i] == readed_string[j])
                order_of_informations.push_back(j);
    return order_of_informations;
}

int string_to_minutes(string string_time)
{
    int minutes = 0;
    minutes = stoi(string_time.substr(0, 2)) * 60 + stoi(string_time.substr(3, 2));
    return minutes;
}

P_INFO string_to_struct(vector<string> input_string)
{
    P_INFO place_info;
    int number_of_places = number_of_places_finder(input_string);
    vector<int> order_of_informations = order_of_informations_finder(input_string);
    for (int i = 1; i <= number_of_places; i++)
    {
        place_info.name.push_back(input_string[order_of_informations[0] + i * 4]);
        place_info.openingTime.push_back(string_to_minutes(input_string[order_of_informations[1] + i * 4]));
        place_info.closingTime.push_back(string_to_minutes(input_string[order_of_informations[2] + i * 4]));
        int rank = stoi(input_string[order_of_informations[3] + i * 4]);
        place_info.rank.push_back(rank);
        place_info.visited.push_back(0);
    }
    return place_info;
}

P_INFO input_read(string input_address)
{
    vector<string> readed_file;
    string filename = input_address;
    ifstream input_file(filename);
    string line_of_file;
    while (getline(input_file, line_of_file))
    {
        line_of_file = line_of_file;
        istringstream line(line_of_file);
        string file_element;
        while (getline(line, file_element, ','))
            readed_file.push_back(file_element);
    }
    input_file.close();
    P_INFO place_info = string_to_struct(readed_file);
    return place_info;
}

string minutes_to_string(int input)
{
    string result = "";
    int hours = input / 60;
    int minutes = input % 60;
    if (hours >= 10)
        result += to_string(hours);
    else
    {
        result += "0";
        result += to_string(hours);
    }
    result += ":";
    if (minutes >= 10)
        result += to_string(minutes);
    else
    {
        result += "0";
        result += to_string(minutes);
    }
    return result;
}

void output_printer(int start, int finnish, P_INFO place_info, int place_index)
{
    cout << "Location " << place_info.name[place_index] << endl;
    cout << "Visit from " << minutes_to_string(start) << " until ";
    cout << minutes_to_string(finnish) << endl
         << "---" << endl;
}

bool this_is_open(int &start, int i, P_INFO place_info)
{
    if (start >= place_info.openingTime[i] && start <= place_info.closingTime[i])
        return true;
    else
        return false;
}

bool this_is_visitable(int &start, int i, P_INFO place_info)
{
    if (start + 30 + 15 <= place_info.closingTime[i])
        return true;
    else
        return false;
}

int change_visit_index(int start, int visit_index, int i, P_INFO place_info)
{
    if (visit_index == -1)
        return i;
    else
    {
        if (start >= place_info.openingTime[i])
            if (place_info.rank[i] < place_info.rank[visit_index])
                return i;
            else
                return visit_index;
        else
        {
            if (place_info.openingTime[i] < place_info.openingTime[visit_index])
                return i;
            if (place_info.openingTime[i] == place_info.openingTime[visit_index])
                if (place_info.rank[i] < place_info.rank[visit_index])
                    return i;
            return visit_index;
        }
    }
}

void change_start_finnish(int &start, int &finnish, P_INFO place_info, int visit_index)
{
    if (start + 30 < place_info.openingTime[visit_index])
        start = place_info.openingTime[visit_index];
    else
        start += 30;

    if (place_info.closingTime[visit_index] - start >= 60)
        finnish = start + 60;
    else
        finnish = place_info.closingTime[visit_index];
}

int visit_place_index(int &start, int &finnish, P_INFO &place_info)
{
    int visit_index = -1;
    for (int i = 0; i < place_info.name.size(); i++)
        if (this_is_open(start, i, place_info) && this_is_visitable(start, i, place_info) && place_info.visited[i] == 0)
            visit_index = change_visit_index(start, visit_index, i, place_info);
    if (visit_index == -1)
        for (int i = 0; i < place_info.name.size(); i++)
            if (start < place_info.openingTime[i] && this_is_visitable(start, i, place_info) && place_info.visited[i] == 0)
                visit_index = change_visit_index(start, visit_index, i, place_info);
    if (visit_index >= 0)
    {
        place_info.visited[visit_index] = 1;
        change_start_finnish(start, finnish, place_info, visit_index);
    }
    return visit_index;
}

int main(int argc, char *argv[])
{
    P_INFO place_info = input_read(argv[1]);
    int finnish = 8 * 60, start = 8 * 60 - 30;
    int index_of_place_to_visit = visit_place_index(start, finnish, place_info);
    while (index_of_place_to_visit != -1)
    {
        output_printer(start, finnish, place_info, index_of_place_to_visit);
        start = finnish;
        index_of_place_to_visit = visit_place_index(start, finnish, place_info);
    }
    return 0;
}