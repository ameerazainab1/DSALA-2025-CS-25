#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

/* ================= COLUMN ================= */

class Column
{
public:
    string name;
    string type;
    unsigned int constraints; // 1 PK, 2 NotNull, 4 Unique
};

/* ================= ROW ================= */

class Row
{
public:
    vector<string> values;
};

/* ================= TABLE ================= */

class Table
{
public:
    string tableName;
    vector<Column> columns;
    vector<Row*> rows;

    ~Table()
    {
        for (int i = 0; i < rows.size(); i++)
            delete rows[i];
    }

    void create()
    {
        cout << "Enter table name: ";
        cin >> tableName;

        int n;
        cout << "Number of columns: ";
        cin >> n;

        for (int i = 0; i < n; i++)
        {
            Column c;
            cout << "Column name: ";
            cin >> c.name;

            cout << "Type (int/string): ";
            cin >> c.type;

            cout << "Constraints (1=PK,2=NotNull,4=Unique, combine like 3): ";
            cin >> c.constraints;

            columns.push_back(c);
        }

        cout << "Table created successfully.\n";
    }

    void insert()
    {
        Row* r = new Row();

        for (int i = 0; i < columns.size(); i++)
        {
            string val;
            cout << "Enter " << columns[i].name << ": ";
            cin >> val;

            if ((columns[i].constraints & 2) && val == "")
            {
                cout << "Not Null violation!\n";
                delete r;
                return;
            }

            if (columns[i].constraints & 1 || columns[i].constraints & 4)
            {
                for (int j = 0; j < rows.size(); j++)
                {
                    if (rows[j]->values[i] == val)
                    {
                        cout << "Duplicate not allowed!\n";
                        delete r;
                        return;
                    }
                }
            }

            r->values.push_back(val);
        }

        rows.push_back(r);
        cout << "Record inserted.\n";
    }

    void selectAll()
    {
        if (rows.size() == 0)
        {
            cout << "No records.\n";
            return;
        }

        for (int i = 0; i < columns.size(); i++)
            cout << columns[i].name << "\t";

        cout << endl;

        for (int i = 0; i < rows.size(); i++)
        {
            for (int j = 0; j < rows[i]->values.size(); j++)
                cout << rows[i]->values[j] << "\t";
            cout << endl;
        }
    }

    void save(ofstream& file)
    {
        file << "TABLE " << tableName << endl;

        for (int i = 0; i < columns.size(); i++)
        {
            file << columns[i].name << " "
                << columns[i].type << " "
                << columns[i].constraints << endl;
        }

        file << "DATA\n";

        for (int i = 0; i < rows.size(); i++)
        {
            for (int j = 0; j < rows[i]->values.size(); j++)
                file << rows[i]->values[j] << " ";
            file << endl;
        }

        file << "ENDTABLE\n";
    }
};

/* ================= MAIN ================= */

int main()
{
    vector<Table*> tables;
    int choice;

    do
    {
        cout << "\n1. Create Table\n";
        cout << "2. Insert\n";
        cout << "3. Select\n";
        cout << "4. Save All\n";
        cout << "5. Load All\n";
        cout << "6. Exit\n";
        cout << "Choice: ";
        cin >> choice;

        if (choice == 1)
        {
            Table* t = new Table();
            t->create();
            tables.push_back(t);
        }

        else if (choice == 2)
        {
            string name;
            cout << "Enter table name: ";
            cin >> name;

            for (int i = 0; i < tables.size(); i++)
            {
                if (tables[i]->tableName == name)
                {
                    tables[i]->insert();
                }
            }
        }

        else if (choice == 3)
        {
            string name;
            cout << "Enter table name: ";
            cin >> name;

            for (int i = 0; i < tables.size(); i++)
            {
                if (tables[i]->tableName == name)
                {
                    tables[i]->selectAll();
                }
            }
        }

        else if (choice == 4)
        {
            ofstream file("database.txt");

            for (int i = 0; i < tables.size(); i++)
                tables[i]->save(file);

            file.close();
            cout << "Saved successfully.\n";
        }

        else if (choice == 5)
        {
            ifstream file("database.txt");

            if (!file)
            {
                cout << "No file found.\n";
                continue;
            }

            while (!file.eof())
            {
                string word;
                file >> word;

                if (word == "TABLE")
                {
                    Table* t = new Table();
                    file >> t->tableName;

                    while (file >> word)
                    {
                        if (word == "DATA")
                            break;

                        Column c;
                        c.name = word;
                        file >> c.type;
                        file >> c.constraints;
                        t->columns.push_back(c);
                    }

                    while (file >> word)
                    {
                        if (word == "ENDTABLE")
                            break;

                        Row* r = new Row();
                        r->values.push_back(word);

                        for (int i = 1; i < t->columns.size(); i++)
                        {
                            string val;
                            file >> val;
                            r->values.push_back(val);
                        }

                        t->rows.push_back(r);
                    }

                    tables.push_back(t);
                }
            }

            file.close();
            cout << "Loaded successfully.\n";
        }

    } while (choice != 6);

    return 0;
}