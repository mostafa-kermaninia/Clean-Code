#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

#define NOT_FOUND -1
#define PLACE_PROPERTIES_COUNT 4
#define TITLE_ROW 1
#define FILE_PATH_INDEX 1

#define NAME_INDEX 0
#define OPENING_TIME_INDEX 1
#define CLOSING_TIME_INDEX 2
#define RANK_INDEX 3

#define START_TIME 8
#define MINUTES_OF_AN_HOUR 60
#define MOVE_DURATION 30
#define MIN_VISIT_TIME 15
#define MAX_VISIT_TIME 60

struct Place
{
    string name;
    int openingTime;
    int closingTime;
    int rank;
    int visited;
};

int find_number_of_places(vector<string> splittedInput)
{
    int numOfPlaces = splittedInput.size() / PLACE_PROPERTIES_COUNT - TITLE_ROW;
    return numOfPlaces;
}

vector<int> find_order_of_informations(vector<string> splittedInput)
{
    vector<int> orderOfInfo;
    vector<string> properties = {"name", "openingTime", "closingTime", "rank"};
    for (int i = 0; i < PLACE_PROPERTIES_COUNT; i++)
        for (int j = 0; j < PLACE_PROPERTIES_COUNT; j++)
            if (properties[i] == splittedInput[j])
                orderOfInfo.push_back(j);
    return orderOfInfo;
}

int convert_string_to_minutes(string stringTime)
{
    int minutes = 0;
    minutes = stoi(stringTime.substr(0, 2)) * MINUTES_OF_AN_HOUR + stoi(stringTime.substr(3, 2));
    return minutes;
}

vector<Place> convert_strvec_to_structvec(vector<string> inputString)
{
    vector<Place> placesInfo;
    int numberOfPlaces = find_number_of_places(inputString);
    vector<int> orderOfInformations = find_order_of_informations(inputString);
    for (int placeIndex = 1; placeIndex <= numberOfPlaces; placeIndex++)
    {
        Place curPlace;
        curPlace.name = inputString[orderOfInformations[NAME_INDEX] + placeIndex * PLACE_PROPERTIES_COUNT];
        curPlace.openingTime = convert_string_to_minutes(inputString[orderOfInformations[OPENING_TIME_INDEX] + placeIndex * PLACE_PROPERTIES_COUNT]);
        curPlace.closingTime = convert_string_to_minutes(inputString[orderOfInformations[CLOSING_TIME_INDEX] + placeIndex * PLACE_PROPERTIES_COUNT]);
        curPlace.rank = stoi(inputString[orderOfInformations[RANK_INDEX] + placeIndex * PLACE_PROPERTIES_COUNT]);
        curPlace.visited = false;

        placesInfo.push_back(curPlace);
    }
    return placesInfo;
}

vector<Place> read_input(string filePath)
{
    vector<string> readFile;
    ifstream inputFile(filePath);
    string lineOfFile;
    while (getline(inputFile, lineOfFile))
    {
        istringstream line(lineOfFile);
        string fileElement;
        while (getline(line, fileElement, ','))
            readFile.push_back(fileElement);
    }
    inputFile.close();
    vector<Place> placesInfo = convert_strvec_to_structvec(readFile);
    return placesInfo;
}

string convert_minutes_to_string(int input)
{
    string result = "";
    int hours = input / MINUTES_OF_AN_HOUR, minutes = input % MINUTES_OF_AN_HOUR;

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

void print_output(int startOfVisitingTime, int endOfVisitingTime, vector<Place> placesInfo, int placeIndex)
{
    cout << "Location " << placesInfo[placeIndex].name << endl;
    cout << "Visit from " << convert_minutes_to_string(startOfVisitingTime) << " until ";
    cout << convert_minutes_to_string(endOfVisitingTime) << endl
         << "---" << endl;
}

bool is_open(int &curTime, int placeIndex, vector<Place> placesInfo)
{
    if (curTime >= placesInfo[placeIndex].openingTime && curTime <= placesInfo[placeIndex].closingTime)
        return true;
    else
        return false;
}

bool is_visitable(int &curTime, int placeIndex, vector<Place> placesInfo)
{
    if (curTime + MOVE_DURATION + MIN_VISIT_TIME <= placesInfo[placeIndex].closingTime)
        return true;
    else
        return false;
}

int change_where_to_visit_index(int curTime, int visitIndex, int placeIndex, vector<Place> placesInfo)
{
    if (visitIndex == NOT_FOUND)
        return placeIndex;
    else
    {
        if (curTime >= placesInfo[placeIndex].openingTime)
            if (placesInfo[placeIndex].rank < placesInfo[visitIndex].rank)
                return placeIndex;
            else
                return visitIndex;
        else
        {
            if (placesInfo[placeIndex].openingTime < placesInfo[visitIndex].openingTime)
                return placeIndex;
            if (placesInfo[placeIndex].openingTime == placesInfo[visitIndex].openingTime)
                if (placesInfo[placeIndex].rank < placesInfo[visitIndex].rank)
                    return placeIndex;
            return visitIndex;
        }
    }
}

void change_start_finish_time(int &start, int &finishTime, vector<Place> placesInfo, int visitIndex)
{
    if (start + MOVE_DURATION < placesInfo[visitIndex].openingTime)
        start = placesInfo[visitIndex].openingTime;
    else
        start += MOVE_DURATION;

    if (placesInfo[visitIndex].closingTime - start >= MAX_VISIT_TIME)
        finishTime = start + MAX_VISIT_TIME;
    else
        finishTime = placesInfo[visitIndex].closingTime;
}

int find_visit_place_index(int &curTime, int &finishTime, vector<Place> &placesInfo)
{
    int visitIndex = NOT_FOUND;
    for (int placeIndex = 0; placeIndex < placesInfo.size(); placeIndex++)
        if (is_open(curTime, placeIndex, placesInfo) && is_visitable(curTime, placeIndex, placesInfo) && placesInfo[placeIndex].visited == false)
            visitIndex = change_where_to_visit_index(curTime, visitIndex, placeIndex, placesInfo);
    if (visitIndex == NOT_FOUND)
        for (int placeIndex = 0; placeIndex < placesInfo.size(); placeIndex++)
            if (curTime < placesInfo[placeIndex].openingTime && is_visitable(curTime, placeIndex, placesInfo) && placesInfo[placeIndex].visited == false)
                visitIndex = change_where_to_visit_index(curTime, visitIndex, placeIndex, placesInfo);
    if (visitIndex >= 0)
    {
        placesInfo[visitIndex].visited = true;
        change_start_finish_time(curTime, finishTime, placesInfo, visitIndex);
    }
    return visitIndex;
}

int main(int argc, char *argv[])
{
    vector<Place> placesInfo = read_input(argv[FILE_PATH_INDEX]);
    int endOfVisit = START_TIME * MINUTES_OF_AN_HOUR, startOfCurVisit = START_TIME * MINUTES_OF_AN_HOUR - MOVE_DURATION;
    int indexOfPlaceToVisit = find_visit_place_index(startOfCurVisit, endOfVisit, placesInfo);
    while (indexOfPlaceToVisit != NOT_FOUND)
    {
        print_output(startOfCurVisit, endOfVisit, placesInfo, indexOfPlaceToVisit);
        startOfCurVisit = endOfVisit;
        indexOfPlaceToVisit = find_visit_place_index(startOfCurVisit, endOfVisit, placesInfo);
    }
    return 0;
}