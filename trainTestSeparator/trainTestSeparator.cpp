#include <iostream>
#include <fstream>
#include <time.h>

using namespace std;

void help(const char* progName);

int main(int argc,char** argv)
{
    help(argv[0]);

    const char* sourcePath = argc > 1 ?  argv[1]: "./source.txt";
    const char* trainDestPath = argc>2 ?  argv[2]: "./train.txt";
    const char* testDestPath = argc>3 ? argv [3]: "./test.txt";
    unsigned int value = argc>4 ? stoi(argv[4]) : 20;
    
    ifstream source  (sourcePath);
    ofstream train (trainDestPath);
    ofstream test(testDestPath);
    
    if (!source)
    {
        cerr<< "ERROR, couldn't open source file "<<endl;
        return -1;
    }
    if (!train)
    { 
        cerr<< "ERROR, couldn't open training file "<<endl;
        return -1;
    }
    if (!test)
    { 
         cerr<< "ERROR, couldn't open testing file "<<endl;
         return -1;
    }
    while (!source.eof())
    {
        int random;
        srand(time(NULL));
        random = rand() % value;
        string content;

        for (int i=0;i<random;i++)
        {
            source>>content;
            train<<content<<endl;
        }
        source>>content;
        test<<content<<endl;
    }
    source.close();
    test.close();
    train.close();
     
    return 0;
}

void help(const char* progName)
{
    cout << "This program sperates randomly training and testing images"<<endl;
    cout <<"USAGE : "<< progName << " <path to source file> " << "<path to training file>  <path to testing file> <value> "<<endl<<endl;
    cout << "Default is : "<<progName<<"./source.txt ./train.txt ./test.txt 20"  <<endl;
}
