/*
* Author: Ryan Lynch
* Date: 11/4/2020
* Section: CS 317-03
* 
* CS 317-03 C++ Honors Project portion
* 
* This program implements mergesort and quicksort on an array of strings. This array of strings is obtained from an input text file.
* The program first performs mergesort on the array of strings, then it performs quicksort on a copy of that original array of strings.
* The sorted data from each sort is written to separate output text files.
* Timing information is obtained from the mergesort and quicksort algorithms and is used to compare the performance of the two.
* The preprocessor macro MAX_LINES can be modified to test out the performance of sorting a certain number of lines of strings.
*/
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctype.h>
#include <string>
#include <chrono>
#include <algorithm>

#define MAX_LINES 1000000

using namespace std;

void mergeSort(string *lines, int first, int last, string *temp); // Mergesort primary function
void merge(string *lines, int first, int last, string *temp); // Utility for mergeSort to merge subarrays in sorted order
void quickSort(string *lines, int left, int right); // Quick sort primary function
int hoarePartition(string *lines, int left, int right); // Partitions a subarray using a pivot. Returns the split position.
void readInLines(string *lines, ifstream& inputFile, int& numLines); // Creates the unsorted array by reading in lines from an input file
void writeSortedArrayToFile(string *lines, int linesLength, ofstream& outFile); // Writes the sorted array to an output file
string strToLower(string line); // Convert a line to all lowercase characters to allow for case insensitive sorting
void swap(string *lines, string *a, string *b); // Used in quicksort to swap elements in the array

int main(void)
{
	ifstream linesFile; // Input file of unsorted lines (used for both merge sort and quick sort)
	ofstream mergeSortOutputFile; // Output file of sorted lines using mergesort
	ofstream quickSortOutputFile; // Output file of sorted lines using quicksort
	string inFileName; // Input file name
	string mergeSortOutputFileName; // Output file name for the sorted data obtained using mergesort
	string quickSortOutputFileName; // Output file name for the sorted data obtained using quicksort
	string* mergeSortLines; // List of lines to be sorted using mergesort
	string* quickSortLines;  // List of lines to be sorted using quicksort
	string* temp; // Temporary array used in mergesort
	int numLines; // Number of lines in the input file

	cout << "What is the name of your input file?\n";
	cin >> inFileName;

	// Open the input file for reading
	linesFile.open(inFileName);
	if (!linesFile)
		cout << "Error opening input file.\n\n";
	else
		cout << "Input File opened successfully.\n\n";

	cout << "What is the name of your output file for the data obtained using mergesort?\n";
	cin >> mergeSortOutputFileName;

	// Open the merge sort output file for writing
	mergeSortOutputFile.open(mergeSortOutputFileName);
	if (!mergeSortOutputFile)
		cout << "Error opening mergesort output file.\n\n";
	else
		cout << "Mergesort output File opened successfully.\n\n";

	cout << "What is the name of your output file for the data obtained using quicksort?\n";
	cin >> quickSortOutputFileName;

	// Open the quick sort output file
	quickSortOutputFile.open(quickSortOutputFileName);
	if (!quickSortOutputFile)
		cout << "Error opening quicksort output file.\n\n";
	else
		cout << "Quicksort output File opened successfully.\n\n";

	mergeSortLines = new string[MAX_LINES]; // Create the array to hold the unsorted lines used in merge sort using dynamic allocation
	quickSortLines = new string[MAX_LINES]; // Create the array to hold the unsorted lines used in quick sort using dynamic allocation
	temp = new string[MAX_LINES]; // Create the temporary storage array used in merge sort using dynamic allocation

	// Obtain the unsorted string list for merge sort and quick sort
	readInLines(mergeSortLines, linesFile, numLines);
	copy(mergeSortLines, mergeSortLines + numLines, quickSortLines); // Copy the unsorted string list to be used for merge sort into the array to be used for quick sort

	// Perform the mergesort, obtain the start and end time of the algorithm
	auto mergeSortBegin = chrono::high_resolution_clock::now();
	mergeSort(mergeSortLines, 0, numLines - 1, temp);
	auto mergeSortEnd = chrono::high_resolution_clock::now();

	delete[] temp; // Free memory from the temp array used in mergesort

	auto mergeSortDuration = chrono::duration_cast<chrono::milliseconds>(mergeSortEnd - mergeSortBegin).count(); // Obtain how long the merge sort took in milliseconds

	cout << "Mergesort duration: " << mergeSortDuration << " milliseconds" << endl;

	// Perform the quicksort, obtain the start and end time of the algorithm
	auto quickSortBegin = chrono::high_resolution_clock::now();
	quickSort(quickSortLines, 0, numLines - 1);
	auto quickSortEnd = chrono::high_resolution_clock::now();

	auto quickSortDuration = chrono::duration_cast<chrono::milliseconds>(quickSortEnd - quickSortBegin).count();

	cout << "Quicksort duration: " << quickSortDuration << " milliseconds" << endl;

	// Write the sorted data to separate text files
	writeSortedArrayToFile(mergeSortLines, numLines, mergeSortOutputFile);
	writeSortedArrayToFile(quickSortLines, numLines, quickSortOutputFile);

	// Free memory from the dynamic arrays
	delete[] mergeSortLines;
	delete[] quickSortLines;

	return 0;
}

/*
* The mergesort algorithm.
* Recursively divides the string lines array into 2 arrays, then combines these arrays in the merge step.
* 
* Parameters:
* lines - the lines array
* first - index of the first element in the subarray
* last - index of the last element in the subarray
* temp - the temporary array used to hold the sorted data throughout the mergesort operation
*/
void mergeSort(string *lines, int first, int last, string *temp)
{
	int middle;

	if (first < last) // Does not execute when first = last
	{
		middle = (first + last) / 2;

		mergeSort(lines, first, middle, temp);
		mergeSort(lines, middle + 1, last, temp);
		merge(lines, first, last, temp);
	}
}

/*
* The merge operation used in mergeSort.
* Merges two sorted arrays into one sorted array by taking a pointer to the left half of the array and a pointer to the right half of the array
* and comparing elements until one pointer reaches the end of its half of the array. The elements are put into the temp array as the comparisons happen.
* Then, it copies the remaining elements in the array with unread elements to the temp array. The temp array is then copied into the lines array.
* This step of mergesort takes O(n) time.
* The string comparison works by viewing the strings in their lowercase form (using strToLower()) and then comparing the strings.
* 
* Parameters:
* lines - the lines array
* first - index of the first element in the subarray
* last - index of the last element in the subarray
* temp - the temporary array used to hold the sorted data throughout the mergesort operation
*/
void merge(string *lines, int first, int last, string *temp)
{
	int middle = (first + last) / 2;
	int leftArrIndex = first; // Pointer to the left array that gets updated throughout the comparisons
	int rightArrIndex = middle + 1; // Pointer to the right array that gets updated throughout the comparisons
	int rightLast = last; // The last index in the right array
	int leftLast = middle; // The last index in the left array
	int index = first; // Used to store strings into the temp array and copy the temp array to the lines array. Gets updated after every comparison

	while (leftArrIndex <= middle && rightArrIndex <= last) // Stops when either subarray is fully scanned
	{
		if (strToLower(lines[leftArrIndex]) < strToLower(lines[rightArrIndex])) // Case-insensitive comparison of strings
			temp[index] = lines[leftArrIndex++];
		else
			temp[index] = lines[rightArrIndex++];

		index++;
	}

	// Copy the rest of the left subarray if necessary
	while (leftArrIndex <= leftLast)
		temp[index++] = lines[leftArrIndex++];

	// Copy the rest of the right subarray if necessary
	while (rightArrIndex <= rightLast)
		temp[index++] = lines[rightArrIndex++];

	// Copy the temp array to the lines array for this particular range of indices
	for (index = first; index <= rightLast; index++)
		lines[index] = temp[index];

}

/*
* The quicksort algorithm.
* Creates a split point where all elements to the left of the point are less than the element at the split, while all elements to the right are greater than the element at the split.
* The element at the split point is then considered to be in its correct location. Quicksort is then recursively performed on all other elements around this split point.
* 
* Parameters:
* lines - the lines array
* left - the first index in this partition of the array
* right - the last index in this partition of the array
*/
void quickSort(string* lines, int left, int right)
{
	int split; // The position the array is split at

	if (left < right)
	{
		split = hoarePartition(lines, left, right);
		quickSort(lines, left, split - 1);
		quickSort(lines, split + 1, right);
	}
}

/*
* The partitioning part of quicksort where a split point is identified.
* Works by putting all elements less than the pivot in the left side of the array, and all elements greater in the right side.
* The pivot element is swapped with the j index when i and j cross over, and then the j index is the split point.
* 
* Paramters:
* lines - the lines array
* left - the first index in this partition of the array
* right - the last index in this partition of the array
*/
int hoarePartition(string* lines, int left, int right)
{
	int pivot = left; // Using the left index (median) as the pivot

	int i = left; // i starts at the left index (pivot)
	int j = right + 1; // j starts one after the right index

	do
	{
		do
			if (i == right) // Prevent i from going out of bounds
				break;
			else
				i++;
		while (strToLower(lines[i]) < strToLower(lines[pivot])); // Stops when a value >= the pivot is encountered
		do
			j--;
		while (strToLower(lines[j]) > strToLower(lines[pivot])); // Stops when a value <= the pivot is encountered

		swap(lines, &lines[i], &lines[j]);
	} while (i < j); // Stops when i coincides with or crosses over j

	swap(lines, &lines[i], &lines[j]); // Undo the last swap that occurred when i >= j

	swap(lines, &lines[pivot], &lines[j]); // Swap the pivot with the element at index j

	return j; // Return the split position j
}

/*
* Populates the lines array by going line by line in an input file to obtain lines.
* Also, makes sure that no more than MAX_LINES are read in.
* 
* Parameters:
* lines - the array of lines to be populated
* inputFile - The input file containing the lines of strings
* numWords - The number of lines in the input file. Gets passed by reference in order for main to know how many lines get read in.
*/
void readInLines(string *lines, ifstream& inputFile, int& numLines)
{
	string line; // Line currently being read
	int index = 0; // Current string index

	while (getline(inputFile, line) && index < MAX_LINES) // Reads in each string without going over the maximum limit
		lines[index++] = line;

	numLines = index; // The number of lines (declared in main) gets assigned a value
}

/*
* Writes a sorted array to an output file.
* 
* Parameters:
* lines - the sorted array of strings
* linesLength - the length of the array of lines
* outFile - the output file to write the sorted strings
*/
void writeSortedArrayToFile(string *lines, int linesLength, ofstream& outFile)
{
	for (int i = 0; i < linesLength; i++)
		outFile << lines[i] << endl;
}

/*
* Converts a string to lowercase by converting all of its letters to lowercase.
* 
* Parameters:
* line - the string line to be made lowercase
* 
* Returns:
* the string line in lowercase form
*/
string strToLower(string line)
{
	int lineLength = line.length();

	for (int i = 0; i < lineLength; i++)
		line[i] = tolower(line[i]); // Make this letter lowercase

	return line;
}

/*
* Swaps two elements in the lines array using pointers to their address in the lines array
*/
void swap(string *lines, string *a, string *b)
{
	string temp = *a;
	*a = *b;
	*b = temp;
}