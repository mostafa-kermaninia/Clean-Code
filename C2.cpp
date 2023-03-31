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

int number_of_places_finder(vector<string> splittedInput)
{
    int numOfPlaces = splittedInput.size() / 4 - 1;
    return numOfPlaces;
}

vector<int> order_of_informations_finder(vector<string> splittedInput)
{
    vector<int> orderOfInfo;
    vector<string> info = {"name", "openingTime", "closingTime", "rank"};
    for (int placeIndex = 0; placeIndex < 4; placeIndex++)
        for (int j = 0; j < 4; j++)
            if (info[placeIndex] == splittedInput[j])
                orderOfInfo.push_back(j);
    return orderOfInfo;
}

int string_to_minutes(string stringTime)
{
    int minutes = 0;
    minutes = stoi(stringTime.substr(0, 2)) * 60 + stoi(stringTime.substr(3, 2));
    return minutes;
}

vector<Place> string_to_struct_vector(vector<string> inputString)
{
    vector<Place> placesInfo;
    int numberOfPlaces = number_of_places_finder(inputString);
    vector<int> orderOfInformations = order_of_informations_finder(inputString);
    for (int placeIndex = 1; placeIndex <= numberOfPlaces; placeIndex++)
    {
        Place curPlace;

        curPlace.name = inputString[orderOfInformations[0] + placeIndex * 4];
        curPlace.openingTime = string_to_minutes(inputString[orderOfInformations[1] + placeIndex * 4]);
        curPlace.closingTime = string_to_minutes(inputString[orderOfInformations[2] + placeIndex * 4]);
        curPlace.rank = stoi(inputString[orderOfInformations[3] + placeIndex * 4]);
        curPlace.visited = 0;

        placesInfo.push_back(curPlace);
    }
    return placesInfo;
}

vector<Place> input_read(string fileAddress)
{
    vector<string> readFile;
    ifstream inputFile(fileAddress);
    string lineOfFile;
    while (getline(inputFile, lineOfFile))
    {

        istringstream line(lineOfFile);
        string fileElement;
        while (getline(line, fileElement, ','))
            readFile.push_back(fileElement);
    }
    inputFile.close();
    vector<Place> placesInfo = string_to_struct_vector(readFile);
    return placesInfo;
}

string minutes_to_string(int input)
{
    string result = "";
    int hours = input / 60, minutes = input % 60;

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

void output_printer(int startOfVisitingTime, int endOfVisitingTime, vector<Place> placesInfo, int placeIndex)
{
    cout << "Location " << placesInfo[placeIndex].name << endl;
    cout << "Visit from " << minutes_to_string(startOfVisitingTime) << " until ";
    cout << minutes_to_string(endOfVisitingTime) << endl
         << "---" << endl;
}

bool this_is_open(int &curTime, int placeIndex, vector<Place> placesInfo)
{
    if (curTime >= placesInfo[placeIndex].openingTime && curTime <= placesInfo[placeIndex].closingTime)
        return true;
    else
        return false;
}

bool this_is_visitable(int &curTime, int placeIndex, vector<Place> placesInfo)
{
    if (curTime + 30 + 15 <= placesInfo[placeIndex].closingTime)
        return true;
    else
        return false;
}

int change_visit_index(int curTime, int visitIndex, int placeIndex, vector<Place> placesInfo)
{
    if (visitIndex == -1)
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

void change_start_finish(int &start, int &finishTime, vector<Place> placesInfo, int visitIndex)
{
    if (start + 30 < placesInfo[visitIndex].openingTime)
        start = placesInfo[visitIndex].openingTime;
    else
        start += 30;

    if (placesInfo[visitIndex].closingTime - start >= 60)
        finishTime = start + 60;
    else
        finishTime = placesInfo[visitIndex].closingTime;
}

int visit_place_index(int &curTime, int &finishTime, vector<Place> &placesInfo)
{
    int visitIndex = -1;
    for (int placeIndex = 0; placeIndex < placesInfo.size(); placeIndex++)
        if (this_is_open(curTime, placeIndex, placesInfo) && this_is_visitable(curTime, placeIndex, placesInfo) && placesInfo[placeIndex].visited == 0)
            visitIndex = change_visit_index(curTime, visitIndex, placeIndex, placesInfo);
    if (visitIndex == -1)
        for (int placeIndex = 0; placeIndex < placesInfo.size(); placeIndex++)
            if (curTime < placesInfo[placeIndex].openingTime && this_is_visitable(curTime, placeIndex, placesInfo) && placesInfo[placeIndex].visited == 0)
                visitIndex = change_visit_index(curTime, visitIndex, placeIndex, placesInfo);
    if (visitIndex >= 0)
    {
        placesInfo[visitIndex].visited = 1;
        change_start_finish(curTime, finishTime, placesInfo, visitIndex);
    }
    return visitIndex;
}

int main(int argc, char *argv[])
{
    vector<Place> placesInfo = input_read(argv[1]);
    int endOfVisit = 8 * 60, startOfCurVisit = 8 * 60 - 30;
    int indexOfPlaceToVisit = visit_place_index(startOfCurVisit, endOfVisit, placesInfo);
    while (indexOfPlaceToVisit != -1)
    {
        output_printer(startOfCurVisit, endOfVisit, placesInfo, indexOfPlaceToVisit);
        startOfCurVisit = endOfVisit;
        indexOfPlaceToVisit = visit_place_index(startOfCurVisit, endOfVisit, placesInfo);
    }
    return 0;
}