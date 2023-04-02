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
    pair<int, int> workingTime;
    int rank;
    int visited;
};
struct VisitDetails
{
    string placeName;
    pair<int, int> visitDuration;
};

int find_number_of_places(int numOfElements)
{
    int numOfPlaces = numOfElements / PLACE_PROPERTIES_COUNT - TITLE_ROW;
    return numOfPlaces;
}

int find_element_index(string element, vector<string> list)
{
    for (int i = 0; i < list.size(); i++)
        if (element == list[i])
            return i;
    return NOT_FOUND;
}

vector<int> find_order_of_informations(vector<string> splittedInput)
{
    vector<int> orderOfInfo;
    vector<string> properties = {"name", "openingTime", "closingTime", "rank"};
    for (int i = 0; i < PLACE_PROPERTIES_COUNT; i++)
    {
        int indexInSplittedInput = find_element_index(properties[i], splittedInput);
        orderOfInfo.push_back(indexInSplittedInput);
    }
    return orderOfInfo;
}

int convert_string_to_minutes(string stringTime)
{
    int minutes = stoi(stringTime.substr(0, 2)) * MINUTES_OF_AN_HOUR + stoi(stringTime.substr(3, 2));
    return minutes;
}

Place convert_strings_to_place(vector<string> stringVec, int placeNum, vector<int> orderOfInformations)
{
    Place finalPlace;
    finalPlace.name = stringVec[orderOfInformations[NAME_INDEX] + placeNum * PLACE_PROPERTIES_COUNT];
    finalPlace.workingTime.first = convert_string_to_minutes(stringVec[orderOfInformations[OPENING_TIME_INDEX] + placeNum * PLACE_PROPERTIES_COUNT]);
    finalPlace.workingTime.second = convert_string_to_minutes(stringVec[orderOfInformations[CLOSING_TIME_INDEX] + placeNum * PLACE_PROPERTIES_COUNT]);
    finalPlace.rank = stoi(stringVec[orderOfInformations[RANK_INDEX] + placeNum * PLACE_PROPERTIES_COUNT]);
    finalPlace.visited = false;
    return finalPlace;
}

vector<Place> convert_to_place_vec(vector<string> inputString)
{
    vector<Place> placesInfo;
    int numberOfPlaces = find_number_of_places(inputString.size());
    vector<int> orderOfInformations = find_order_of_informations(inputString);
    for (int i = 1; i <= numberOfPlaces; i++)
    {
        Place curPlace = convert_strings_to_place(inputString, i, orderOfInformations);
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
    vector<Place> placesInfo = convert_to_place_vec(readFile);
    return placesInfo;
}

string convert_num_to_string(int num)
{
    if (num >= 10)
        return to_string(num);
    else
        return "0" + to_string(num);
}

string convert_minutes_to_string(int input)
{
    int hours = input / MINUTES_OF_AN_HOUR;
    int minutes = input % MINUTES_OF_AN_HOUR;
    string result = convert_num_to_string(hours) + ":" + convert_num_to_string(minutes);
    return result;
}

void print_output(vector<VisitDetails> visits)
{
    for (int i = 0; i < visits.size(); i++)
    {
        cout << "Location " << visits[i].placeName << endl;
        cout << "Visit from " << convert_minutes_to_string(visits[i].visitDuration.first) << " until "
             << convert_minutes_to_string(visits[i].visitDuration.second) << endl;
        cout << "---" << endl;
    }
}

bool is_open(int &curTime, int placeIndex, vector<Place> placesInfo)
{
    if (curTime >= placesInfo[placeIndex].workingTime.first && curTime <= placesInfo[placeIndex].workingTime.second)
        return true;
    else
        return false;
}

bool is_visitable(int &curTime, int placeIndex, vector<Place> placesInfo)
{
    if (curTime + MOVE_DURATION + MIN_VISIT_TIME <= placesInfo[placeIndex].workingTime.second)
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
        if (curTime >= placesInfo[placeIndex].workingTime.first)
            if (placesInfo[placeIndex].rank < placesInfo[visitIndex].rank)
                return placeIndex;
            else
                return visitIndex;
        else
        {
            if (placesInfo[placeIndex].workingTime.first < placesInfo[visitIndex].workingTime.first)
                return placeIndex;
            if (placesInfo[placeIndex].workingTime.first == placesInfo[visitIndex].workingTime.first)
                if (placesInfo[placeIndex].rank < placesInfo[visitIndex].rank)
                    return placeIndex;
            return visitIndex;
        }
    }
}

void change_start_finish_time(int &start, int &finishTime, vector<Place> placesInfo, int visitIndex)
{
    if (start + MOVE_DURATION < placesInfo[visitIndex].workingTime.first)
        start = placesInfo[visitIndex].workingTime.first;
    else
        start += MOVE_DURATION;

    if (placesInfo[visitIndex].workingTime.second - start >= MAX_VISIT_TIME)
        finishTime = start + MAX_VISIT_TIME;
    else
        finishTime = placesInfo[visitIndex].workingTime.second;
}

int find_visit_place_index(int &curTime, int &finishTime, vector<Place> &placesInfo)
{
    int visitIndex = NOT_FOUND;
    for (int placeIndex = 0; placeIndex < placesInfo.size(); placeIndex++)
        if (is_open(curTime, placeIndex, placesInfo) && is_visitable(curTime, placeIndex, placesInfo) && placesInfo[placeIndex].visited == false)
            visitIndex = change_where_to_visit_index(curTime, visitIndex, placeIndex, placesInfo);
    if (visitIndex == NOT_FOUND)
        for (int placeIndex = 0; placeIndex < placesInfo.size(); placeIndex++)
            if (curTime < placesInfo[placeIndex].workingTime.first && is_visitable(curTime, placeIndex, placesInfo) && placesInfo[placeIndex].visited == false)
                visitIndex = change_where_to_visit_index(curTime, visitIndex, placeIndex, placesInfo);
    if (visitIndex >= 0)
    {
        placesInfo[visitIndex].visited = true;
        change_start_finish_time(curTime, finishTime, placesInfo, visitIndex);
    }
    return visitIndex;
}

VisitDetails build_visit_struct(pair<int, int> visitDuration, string placeName)
{
    VisitDetails newVisit;
    newVisit.placeName = placeName;
    newVisit.visitDuration = visitDuration;
    return newVisit;
}

vector<VisitDetails> build_visit_schedule(vector<Place> &placesInfo)
{
    vector<VisitDetails> visitsInfo;
    int startOfCurVisit = START_TIME * MINUTES_OF_AN_HOUR - MOVE_DURATION;
    int endOfVisit = START_TIME * MINUTES_OF_AN_HOUR;
    int indexOfPlaceToVisit = find_visit_place_index(startOfCurVisit, endOfVisit, placesInfo);
    while (indexOfPlaceToVisit != NOT_FOUND)
    {
        string placeName = placesInfo[indexOfPlaceToVisit].name;
        visitsInfo.push_back(build_visit_struct({startOfCurVisit, endOfVisit}, placeName));
        startOfCurVisit = endOfVisit;
        indexOfPlaceToVisit = find_visit_place_index(startOfCurVisit, endOfVisit, placesInfo);
    }
    return visitsInfo;
}

int main(int argc, char *argv[])
{
    vector<Place> placesInfo = read_input(argv[FILE_PATH_INDEX]);
    vector<VisitDetails> visits = build_visit_schedule(placesInfo);
    print_output(visits);
    return 0;
}