#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <sstream>
#include <ctime>
#include <algorithm>

using namespace std;

class Book {
public:
    string name, author, publisher;
    int copies;

    void input() {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout<<"Enter book name: "; getline(cin, name);
        cout<<"Enter author name: "; getline(cin, author);
        cout<<"Enter publisher name: "; getline(cin, publisher);
        cout<<"Enter number of copies: ";
        while (!(cin >> copies) || copies < 0) {
            cout<<"Invalid input! Enter a non-negative integer: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    void display() const {
        cout<<"\nBook Name: "<<name
            <<"\nAuthor: "<<author
            <<"\nPublisher: "<<publisher
            <<"\nCopies: "<<copies<<endl;
    }
};

bool verifyCredentials() {
    string username, password, fileUsername, filePassword;
    cout<<"\n--- Librarian Login ---\n";
    cout<<"Enter username: "; cin >> username;
    cout<<"Enter password: "; cin >> password;

    ifstream credFile("credentials.txt");
    if (!credFile) {
        cout<<"Error: Credentials file not found!"<<endl;
        return false;
    }

    while (credFile >> fileUsername >> filePassword) {
        if (username == fileUsername && password == filePassword) {
            cout<<"Login successful.\n";
            return true;
        }
    }
    cout<<"Invalid credentials! Access denied.\n";
    return false;
}

string toLowerCase(const string &str) {
    string lowerStr = str;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}
void addBook() {
    ofstream file("books.txt", ios::app);
    if (!file) {
            cout<<"Error opening file!"<<endl;
            return;
    }
    Book b;
    b.input();
    file<<b.name<<"|"<<b.author<<"|"<<b.publisher<<"|"<<b.copies<<"\n";
    cout<<"Book added successfully.\n";
}

void displayBooks() {
    ifstream file("books.txt");
    if (!file) {
            cout<<"No books found.\n";
            return;
    }

    cout<<"\n--- Book List ---\n";
    string line; bool found = false;
    while (getline(file, line)) {
        if (line.empty())
            continue;
        stringstream ss(line);
        string name, author, publisher, copiesStr;
        getline(ss, name, '|'); getline(ss, author, '|'); getline(ss, publisher, '|'); getline(ss, copiesStr, '|');

        try {
            int copies = stoi(copiesStr);
            Book b{name, author, publisher, copies};
            b.display();
            found = true;
        } catch (...) {
            cout<<"Error: Invalid data format in file.\n";
        }
    }
    if (!found) cout<<"No valid book records to display.\n";
}

void deleteBook() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout<<"Enter book name to delete: ";
    string bookName;
    getline(cin, bookName);

    string bookNameLower = toLowerCase(bookName);

    ifstream inFile("books.txt");
    ofstream outFile("temp.txt");
    if (!inFile || !outFile) {
        cout<<"File error!"<<endl;
        return;
    }

    string line; bool found = false;
    while (getline(inFile, line)) {
        stringstream ss(line);
        string name; getline(ss, name, '|');

        if (toLowerCase(name) != bookNameLower)
            outFile<<line<<"\n";
        else
            found = true;
    }

    inFile.close();
    outFile.close();
    remove("books.txt");
    rename("temp.txt", "books.txt");
    cout<<(found ? "Book deleted successfully.\n" : "Book not found.\n");
}

void modifyBook() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout<<"Enter book name to modify: ";
    string bookName;
    getline(cin, bookName);
    string bookNameLower = toLowerCase(bookName);
    ifstream inFile("books.txt");
    ofstream outFile("temp.txt");
    if (!inFile || !outFile) {
        cout<<"File error!"<<endl;
        return;
    }
    string line; bool found = false;
    while (getline(inFile, line)) {
        stringstream ss(line);
        string name; getline(ss, name, '|');
        if (toLowerCase(name) == bookNameLower) {
            cout<<"Enter new details:\n";
            Book newBook;
            newBook.input();
            outFile<<newBook.name<<"|"<<newBook.author<<"|"<<newBook.publisher<<"|"<<newBook.copies<<"\n";
            found = true;
        } else {
            outFile<<line<<"\n";
        }
    }

    inFile.close();
    outFile.close();
    remove("books.txt");
    rename("temp.txt", "books.txt");
    cout<<(found ? "Book modified successfully.\n" : "Book not found.\n");
}


void logTransaction(const string &regno, const string &name, const string &bookName, const string &action) {
    ofstream logFile("log.txt", ios::app);
    if (!logFile) {
            cout<<"Error opening log file!"<<endl;
            return;
    }
    time_t now = time(nullptr);
    string timeStr = ctime(&now);
    timeStr.pop_back();
    logFile<<regno<<"|"<<name<<"|"<<bookName<<"|"<<action<<"|"<<timeStr<<"\n";
}

bool updateBookCopies(const string &bookName, int change) {
    string bookNameLower = toLowerCase(bookName);
    ifstream inFile("books.txt");
    ofstream outFile("temp.txt");
    if (!inFile || !outFile)
        return false;

    string line; bool found = false;
    while (getline(inFile, line)) {
        stringstream ss(line);
        string name, author, publisher, copiesStr;
        getline(ss, name, '|');
        getline(ss, author, '|');
        getline(ss, publisher, '|');
        getline(ss, copiesStr, '|');
        int copies = stoi(copiesStr);

        if (toLowerCase(name) == bookNameLower) {
            if (copies + change < 0) {
                cout<<"Not enough copies available.\n";
                outFile<<line<<"\n";
            } else {
                outFile<<name<<"|"<<author<<"|"<<publisher<<"|"<<copies + change<<"\n";
                found = true;
            }
        } else {
            outFile<<line<<"\n";
        }
    }

    inFile.close();
    outFile.close();
    remove("books.txt");
    rename("temp.txt", "books.txt");
    return found;
}


void issueBook() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout<<"Enter registration number: ";
    string regno;
    getline(cin, regno);
    cout<<"Enter student name: ";
    string name;
    getline(cin, name);
    cout<<"Enter book name to issue: ";
    string bookName;
    getline(cin, bookName);

    if (updateBookCopies(bookName, -1)) {
        logTransaction(regno, name, bookName, "Issued");
        cout<<"Book issued successfully.\n";
    } else {
        cout<<"Book not found or insufficient copies.\n";
    }
}

void returnBook() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout<<"Enter registration number: ";
    string regno;
    getline(cin, regno);
    cout<<"Enter student name: ";
    string name;
    getline(cin, name);
    cout<<"Enter book name to return: ";
    string bookName;
    getline(cin, bookName);

    if (updateBookCopies(bookName, 1)) {
        logTransaction(regno, name, bookName, "Returned");
        cout<<"Book returned successfully.\n";
    } else {
        cout<<"Book not found in records.\n";
    }
}

void studentMenu() {
    int choice;
    do {
        cout<<"\n--- Student Menu ---\n";
        cout<<"1. Issue Book\n2. Return Book\n3. Show Books\n4. Back\nEnter choice: ";

        while (!(cin >> choice)) {
            cout<<"Invalid input! Enter a number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        switch (choice) {
            case 1: issueBook(); break;
            case 2: returnBook(); break;
            case 3: displayBooks(); break;
            case 4: cout<<"Returning to main menu...\n"; break;
            default: cout<<"Invalid choice! Try again.\n";
        }
    } while (choice != 4);
}

void librarianMenu() {
    if (!verifyCredentials())
        return;

    int choice;
    do {
        cout<<"\n--- Librarian Menu ---\n";
        cout<<"1. Add Book\n2. Display Books\n3. Delete Book\n4. Modify Book\n5. Back\nEnter choice: ";

        while (!(cin >> choice)) {
            cout<<"Invalid input! Enter a number: ";
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        switch (choice) {
            case 1: addBook(); break;
            case 2: displayBooks(); break;
            case 3: deleteBook(); break;
            case 4: modifyBook(); break;
            case 5: cout<<"Returning to main menu...\n"; break;
            default: cout<<"Invalid choice! Try again.\n";
        }
    } while (choice != 5);
}

void menu() {
    int choice;
    do {
        cout<<"\n--- Library Management System ---\n";
        cout<<"1. Librarian Mode\n2. Student Mode\n3. Exit\nEnter choice: ";

        while (!(cin >> choice)) {
            cout<<"Invalid input! Enter a number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        switch (choice) {
            case 1: librarianMenu(); break;
            case 2: studentMenu(); break;
            case 3: cout<<"Exiting the system...\n"; break;
            default: cout<<"Invalid choice! Try again.\n";
        }
    } while (choice != 3);
}

int main() {
    menu();
    return 0;
}
