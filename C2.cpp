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


int compare_open_places(int selectedPlaceIndex, int curPlaceIndex, vector<Place> placesInfo)
{
    if (selectedPlaceIndex == NOT_FOUND)
        return curPlaceIndex;

    Place curPlace = placesInfo[curPlaceIndex];
    Place selectedPlace = placesInfo[selectedPlaceIndex];
    if (curPlace.rank < selectedPlace.rank)
        return curPlaceIndex;
    return selectedPlaceIndex;
}

int compare_still_closed_places(int selectedPlaceIndex, int curPlaceIndex, vector<Place> placesInfo)
{
    if (selectedPlaceIndex == NOT_FOUND)
        return curPlaceIndex;
    else
    {
        Place curPlace = placesInfo[curPlaceIndex];
        Place selectedPlace = placesInfo[selectedPlaceIndex];
        if (curPlace.workingTime.first < selectedPlace.workingTime.first)
            return curPlaceIndex;
        if (curPlace.workingTime.first == selectedPlace.workingTime.first && curPlace.rank < selectedPlace.rank)
            return curPlaceIndex;
    }
    return selectedPlaceIndex;
}

bool is_visitable(int curTime, Place selectedPlace)
{
    int timeToArrive = curTime + MOVE_DURATION;
    if (timeToArrive + MIN_VISIT_TIME <= selectedPlace.workingTime.second && selectedPlace.visited == false)
        return true;
    return false;
}

int find_visit_place_index(int curTime, vector<Place> placesInfo)
{
    int selectedPlaceIndex = NOT_FOUND;
    for (int i = 0; i < placesInfo.size(); i++)
        if (curTime >= placesInfo[i].workingTime.first && is_visitable(curTime, placesInfo[i]))
            selectedPlaceIndex = compare_open_places(selectedPlaceIndex, i, placesInfo);

    if (selectedPlaceIndex == NOT_FOUND)
        for (int i = 0; i < placesInfo.size(); i++)
            if (curTime < placesInfo[i].workingTime.first && is_visitable(curTime, placesInfo[i]))
                selectedPlaceIndex = compare_still_closed_places(selectedPlaceIndex, i, placesInfo);
    return selectedPlaceIndex;
}

int find_end_of_visiting_time(int startTime, Place selectedPlace)
{
    if (selectedPlace.workingTime.second - startTime >= MAX_VISIT_TIME)
        return startTime + MAX_VISIT_TIME;
    else
        return selectedPlace.workingTime.second;
}

int find_start_of_visiting_time(int curTime, Place selectedPlace)
{
    if (curTime + MOVE_DURATION < selectedPlace.workingTime.first)
        return selectedPlace.workingTime.first;
    else
        return curTime + MOVE_DURATION;
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
    int curTime = START_TIME * MINUTES_OF_AN_HOUR - MOVE_DURATION;
    int indexOfPlaceToVisit = find_visit_place_index(curTime, placesInfo);
    while (indexOfPlaceToVisit != NOT_FOUND)
    {
        placesInfo[indexOfPlaceToVisit].visited = true;
        int startOfVisit = find_start_of_visiting_time(curTime, placesInfo[indexOfPlaceToVisit]);
        int endOfVisit = find_end_of_visiting_time(startOfVisit, placesInfo[indexOfPlaceToVisit]);
        string placeName = placesInfo[indexOfPlaceToVisit].name;
        visitsInfo.push_back(build_visit_struct({startOfVisit, endOfVisit}, placeName));

        curTime = endOfVisit;
        indexOfPlaceToVisit = find_visit_place_index(curTime, placesInfo);
    }
    return visitsInfo;
}


string convert_num_to_string(int num)
{
    if (num >= 10)
        return to_string(num);
    else
        return "0" + to_string(num);
}

string convert_minutes_to_string(int inputMinutes)
{
    int hours = inputMinutes / MINUTES_OF_AN_HOUR;
    int minutes = inputMinutes % MINUTES_OF_AN_HOUR;
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


int main(int argc, char *argv[])
{
    vector<Place> placesInfo = read_input(argv[FILE_PATH_INDEX]);
    vector<VisitDetails> visits = build_visit_schedule(placesInfo);
    print_output(visits);
    return 0;
}