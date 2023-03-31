#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

struct Place
{
    string name;
    int openingTime;
    int closingTime;
    int rank;
    int visited;
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

vector<Place> string_to_struct_vector(vector<string> input_string)
{
    vector<Place> places_info;
    int number_of_places = number_of_places_finder(input_string);
    vector<int> order_of_informations = order_of_informations_finder(input_string);
    for (int i = 1; i <= number_of_places; i++)
    {
        Place place_info;

        place_info.name = input_string[order_of_informations[0] + i * 4];
        place_info.openingTime = string_to_minutes(input_string[order_of_informations[1] + i * 4]);
        place_info.closingTime = string_to_minutes(input_string[order_of_informations[2] + i * 4]);
        place_info.rank = stoi(input_string[order_of_informations[3] + i * 4]);
        place_info.visited = 0;

        places_info.push_back(place_info);
    }
    return places_info;
}

vector<Place> input_read(string input_address)
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
    vector<Place> places_info = string_to_struct_vector(readed_file);
    return places_info;
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

void output_printer(int start, int finnish, vector<Place> places_info, int place_index)
{
    cout << "Location " << places_info[place_index].name << endl;
    cout << "Visit from " << minutes_to_string(start) << " until ";
    cout << minutes_to_string(finnish) << endl
         << "---" << endl;
}

bool this_is_open(int &start, int i, vector<Place> places_info)
{
    if (start >= places_info[i].openingTime && start <= places_info[i].closingTime)
        return true;
    else
        return false;
}

bool this_is_visitable(int &start, int i, vector<Place> places_info)
{
    if (start + 30 + 15 <= places_info[i].closingTime)
        return true;
    else
        return false;
}

int change_visit_index(int start, int visit_index, int i, vector<Place> places_info)
{
    if (visit_index == -1)
        return i;
    else
    {
        if (start >= places_info[i].openingTime)
            if (places_info[i].rank < places_info[visit_index].rank)
                return i;
            else
                return visit_index;
        else
        {
            if (places_info[i].openingTime < places_info[visit_index].openingTime)
                return i;
            if (places_info[i].openingTime == places_info[visit_index].openingTime)
                if (places_info[i].rank < places_info[visit_index].rank)
                    return i;
            return visit_index;
        }
    }
}

void change_start_finnish(int &start, int &finnish, vector<Place> places_info, int visit_index)
{
    if (start + 30 < places_info[visit_index].openingTime)
        start = places_info[visit_index].openingTime;
    else
        start += 30;

    if (places_info[visit_index].closingTime - start >= 60)
        finnish = start + 60;
    else
        finnish = places_info[visit_index].closingTime;
}

int visit_place_index(int &start, int &finnish, vector<Place> &places_info)
{
    int visit_index = -1;
    for (int i = 0; i < places_info.size(); i++)
        if (this_is_open(start, i, places_info) && this_is_visitable(start, i, places_info) && places_info[i].visited == 0)
            visit_index = change_visit_index(start, visit_index, i, places_info);
    if (visit_index == -1)
        for (int i = 0; i < places_info.size(); i++)
            if (start < places_info[i].openingTime && this_is_visitable(start, i, places_info) && places_info[i].visited == 0)
                visit_index = change_visit_index(start, visit_index, i, places_info);
    if (visit_index >= 0)
    {
        places_info[visit_index].visited = 1;
        change_start_finnish(start, finnish, places_info, visit_index);
    }
    return visit_index;
}

int main(int argc, char *argv[])
{
    vector<Place> places_info = input_read(argv[1]);
    int finnish = 8 * 60, start = 8 * 60 - 30;
    int index_of_place_to_visit = visit_place_index(start, finnish, places_info);
    while (index_of_place_to_visit != -1)
    {
        output_printer(start, finnish, places_info, index_of_place_to_visit);
        start = finnish;
        index_of_place_to_visit = visit_place_index(start, finnish, places_info);
    }
    return 0;
}