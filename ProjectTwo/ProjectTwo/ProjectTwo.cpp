#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// ============================================================================
// Structure Definitions
// ============================================================================

// Structure to hold course data
struct Course {
    string courseNumber;
    string title;
    vector<string> prerequisites;
};

// Node structure for the Binary Search Tree
struct Node {
    Course course;
    Node* left;
    Node* right;

    // Constructor
    Node(Course aCourse) {
        course = aCourse;
        left = nullptr;
        right = nullptr;
    }
};

// ============================================================================
// Class Definition for Binary Search Tree
// ============================================================================
class BinarySearchTree {

private:
    Node* root;

    void destroyTree(Node* node) {
        if (node != nullptr) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }

    void insert(Node* node, Course course) {
        // Compare course numbers to determine left or right placement
        if (course.courseNumber < node->course.courseNumber) {
            if (node->left == nullptr) {
                node->left = new Node(course);
            }
            else {
                insert(node->left, course);
            }
        }
        else {
            if (node->right == nullptr) {
                node->right = new Node(course);
            }
            else {
                insert(node->right, course);
            }
        }
    }

    void printAllCourses(Node* node) {
        // In-Order Traversal: Left, Root, Right (Yields alphanumeric sorting)
        if (node != nullptr) {
            printAllCourses(node->left);
            cout << node->course.courseNumber << ", " << node->course.title << endl;
            printAllCourses(node->right);
        }
    }

public:
    BinarySearchTree() {
        root = nullptr;
    }

    ~BinarySearchTree() {
        destroyTree(root);
    }

    void Insert(Course course) {
        if (root == nullptr) {
            root = new Node(course);
        }
        else {
            this->insert(root, course);
        }
    }

    void PrintAllCourses() {
        this->printAllCourses(root);
    }

    Node* Search(string courseNumber) {
        Node* current = root;

        while (current != nullptr) {
            if (current->course.courseNumber == courseNumber) {
                return current;
            }
            if (courseNumber < current->course.courseNumber) {
                current = current->left;
            }
            else {
                current = current->right;
            }
        }
        return nullptr;
    }
};

// ============================================================================
// Helper & Parsing Functions
// ============================================================================

// Helper function to convert a string to uppercase for case-insensitive matching
string toUpperCase(string str) {
    transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

// Helper function to trim whitespace from string boundaries
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (string::npos == first) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// Tokenizes a line by its comma delimiter
vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    string token;
    stringstream ss(str);
    
    char activeDelimiter = delimiter;
    if (str.find('\t') != string::npos) {
        activeDelimiter = '\t';
    }

    while (getline(ss, token, activeDelimiter)) {
        string cleanedToken = trim(token);
        if (!cleanedToken.empty() || activeDelimiter == ',') {
            tokens.push_back(cleanedToken);
        }
    }
    return tokens;
}

// Loads data file, processes tokens, validates structures, and populates the BST
bool loadCourses(string csvPath, BinarySearchTree* bst) {
    ifstream file(csvPath);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << csvPath << endl;
        return false;
    }

    string line;
    vector<Course> tempCourseList;
    vector<string> allValidCourseNumbers;

    // Parse data and gather all valid course keys for prerequisite cross-referencing
    while (getline(file, line)) {
        if (line.empty()) continue;

        vector<string> tokens = split(line, ',');

        // Basic parameter check 
        if (tokens.size() < 2) {
            cout << "Error: Invalid file format on line: " << line << endl;
            continue;
        }

        Course course;
        course.courseNumber = tokens[0];
        course.title = tokens[1];

        for (size_t i = 2; i < tokens.size(); ++i) {
            if (!tokens[i].empty()) {
                course.prerequisites.push_back(tokens[i]);
            }
        }

        tempCourseList.push_back(course);
        allValidCourseNumbers.push_back(course.courseNumber);
    }
    file.close();

    // Verify prerequisite validity and insert qualified course instances into the BST
    for (const Course& course : tempCourseList) {
        bool isCourseValid = true;

        // Validate whether the listed prerequisites exist in the curriculum data set
        for (const string& prereq : course.prerequisites) {
            if (find(allValidCourseNumbers.begin(), allValidCourseNumbers.end(), prereq) == allValidCourseNumbers.end()) {
                cout << "Error: Prerequisite \"" << prereq << "\" for course \""
                    << course.courseNumber << "\" does not exist." << endl;
                isCourseValid = false;
            }
        }

        // Insert into BST only if format validations and prerequisite verifications clear
        if (isCourseValid) {
            bst->Insert(course);
        }
    }

    return true;
}

// Displays specific course attributes along with formatted prerequisite relations
void printCourseInformation(BinarySearchTree* bst, string targetCourse) {
    targetCourse = toUpperCase(trim(targetCourse));
    Node* foundNode = bst->Search(targetCourse);

    if (foundNode != nullptr) {
        cout << foundNode->course.courseNumber << ", " << foundNode->course.title << endl;
        cout << "Prerequisites: ";
        if (foundNode->course.prerequisites.empty()) {
            cout << "None" << endl;
        }
        else {
            for (size_t i = 0; i < foundNode->course.prerequisites.size(); ++i) {
                cout << foundNode->course.prerequisites[i];
                if (i < foundNode->course.prerequisites.size() - 1) {
                    cout << ", ";
                }
            }
            cout << endl;
        }
    }
    else {
        cout << "Course " << targetCourse << " not found." << endl;
    }
}

// ============================================================================
// Main Method / Driver Menu
// ============================================================================
int main() {
    BinarySearchTree* bst = new BinarySearchTree();
    bool dataLoaded = false;
    string fileName = "CS 300 ABCU_Advising_Program_Input.txt"; // Match text file from your pseudocode Main
    string userInput = "";

    cout << "Welcome to the course planner." << endl;

    while (userInput != "9") {
        cout << endl;
        cout << "  1. Load Data Structure." << endl;
        cout << "  2. Print Course List." << endl;
        cout << "  3. Print Course." << endl;
        cout << "  9. Exit" << endl << endl; 
        cout << "What would you like to do? ";
        cin >> userInput;

        // Route selection block
        if (userInput == "1") {
            if (loadCourses(fileName, bst)) {
                dataLoaded = true;
                cout << "Data successfully loaded into the data structure." << endl;
            }
        }
        else if (userInput == "2") {
            if (!dataLoaded) {
                cout << "Error: Please load the data first (Option 1)." << endl;
            }
            else {
                cout << "Here is a sample schedule:" << endl << endl;
                bst->PrintAllCourses();
            }
        }
        else if (userInput == "3") {
            if (!dataLoaded) {
                cout << "Error: Please load the data first (Option 1)." << endl;
            }
            else {
                string targetCourse;
                cout << "What course do you want to know about? ";
                cin >> targetCourse;
                printCourseInformation(bst, targetCourse);
            }
        }
        else if (userInput == "9") {
            userInput = "9";
            cout << "Thank you for using the course planner!" << endl;
        }
        else {
            cout << userInput << " is not a valid option." << endl;
        }
    }

    delete bst;
    return 0;
}