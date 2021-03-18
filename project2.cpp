#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>
using namespace std;

const int numberOfFeatures = 100;   //Number of feature
const int numberOfData = 500;       //Number of row

struct Data{        //Data struct  holds category and features
    int category;
    vector<double> features;
    Data( int category, vector<double>features ){
        this->category = category;
        this->features = features;
    }
};

struct DataSet{     //DataSet struct holds accuracy of a set of feature(s)
    double accuracy;
    vector<int> index;
    DataSet(double a, vector<int> i){
        this->accuracy = a;
        this->index = i;
    }
    
    void displayIndex(){
        for( auto& x: index ){
            if( x!=-1 )
                printf("%d ", x+1);
        }
    }
};

struct Comparator{
    bool operator()( const DataSet* right, const DataSet* left ) const{
        return right->accuracy < left->accuracy;
    }
};

double nearestDis = INFINITY;           //nearestDis stores the smallest value between two data
double category = 0, features = 0;      //Each Data object has Category, 1 or 2, and a number of features
double highestAccuracy = 0, sum = 0;    //highestAccuracy stores the highest accuracy percentage and sum stores the total distance given multiple features
int correct = 0, nearestIndex = 0, highestAccuracyIndex = -1, lowestAccuracyIndex = -1;   //correct stores the number of correct category. nearestIndex stores the index of the nearest neighbor,

vector<Data*>            dataCollection;       //dataCollection stores the data from given text file, it stores Data object
vector<int>              indexSetOfFeatures;   //indexSetOfFeatures stores the index of the data that will be added into a set
priority_queue<DataSet*, vector<DataSet*>, Comparator> dataSet;   //dataSet stores DataSet object, which stores accuracy of a set of features and indexes of said features for forward search

string inputLine;   //inputLine is used to parse test file

void forwardSearch(){
    for( int x = 0; x < numberOfFeatures; x++ ){            //x is the current level of the tree
        for( int x2 = 0; x2 < numberOfFeatures; x2++ ){     //x2 is the index of the current feature we are checking
            if( find(indexSetOfFeatures.begin(), indexSetOfFeatures.end(), x2) == indexSetOfFeatures.end() ){
                for( int y = 0; y < numberOfData; y++ ){        //y is the current data for which we need to find the nearst neighbor
                    for( int y2 = 0; y2 < numberOfData; y2++ ){ //y2 is the data that we are currently comparing with y
                        if( y != y2 ){
                            //When we first start analysing for froward. Only runs the first time.
                            if( x==0 ){
                                if( fabs(dataCollection[y]->features[x2] - dataCollection[y2]->features[x2]) < nearestDis ){
                                    nearestDis = fabs(dataCollection[y]->features[x2] - dataCollection[y2]->features[x2] );
                                    nearestIndex = y2;
                                }
                            }else{
                                //Calculating the nearest neighbor using multiple features. Skip duplicate features.
                                double tempSum = 0;
                                for( int n = 0; n < indexSetOfFeatures.size(); n++ ){
                                    tempSum += pow( fabs( dataCollection[y]->features[indexSetOfFeatures[n]] - dataCollection[y2]->features[indexSetOfFeatures[n]] ), 2);
                                }
                                tempSum += pow( fabs( dataCollection[y]->features[x2] - dataCollection[y2]->features[x2] ), 2);
                                sum = sqrt(tempSum);
                                if( sum < nearestDis ){
                                    nearestDis = sum;
                                    nearestIndex = y2;
                                }
                            }
                        }
                    }
                    //After comparison, check if nearest neighbor is the same category
                    if( dataCollection[y]->category == dataCollection[nearestIndex]->category ){
                        correct++;  //increment the number of correct comparison
                    }
                    nearestDis = INFINITY;  //reset distance to INFINITY for the next search
                }
                //looking for the feature that has the highest accuracy and store the index of that feature
                if( highestAccuracy < double(correct)/numberOfData ){
                    highestAccuracy = double(correct)/numberOfData;
                    highestAccuracyIndex = x2;
                }
                correct = 0;    //reset correct for the next search
            }
        }
        indexSetOfFeatures.push_back( highestAccuracyIndex ); //The index of highest accuracy feature in this level gets stored.
        DataSet *data = new DataSet(highestAccuracy, indexSetOfFeatures);
        dataSet.push( data );
        printf("On level %d, adding feature #%d to the set. The new set has accuracy %.2f%%\n", x+1, highestAccuracyIndex+1, highestAccuracy*100);
    
        highestAccuracy = 0;
        highestAccuracyIndex = -1;
    }
}

void backwardSearch(){
    for( int x=0; x<numberOfFeatures; x++ ){
        indexSetOfFeatures.push_back( x );
    }
    for( int y = 0; y < numberOfData; y++ ){        //y is the current data for which we need to find the nearst neighbor
        for( int y2 = 0; y2 < numberOfData; y2++ ){ //y2 is the data that we are currently comparing with y
            if( y != y2 ){  //don't compare distance with itself
                double tempSum = 0;
                for( int n = 0; n < indexSetOfFeatures.size(); n++ ){
                    tempSum += pow( fabs( dataCollection[y]->features[indexSetOfFeatures[n]] - dataCollection[y2]->features[indexSetOfFeatures[n]] ), 2);
                }
                sum = sqrt(tempSum);
                if( sum < nearestDis ){
                    nearestDis = sum;
                    nearestIndex = y2;
                }
            }
        }
        //After comparison, check if nearest neighbor is the same category
        if( dataCollection[y]->category == dataCollection[nearestIndex]->category ){
            correct++;  //increment the number of correct comparison
        }
        nearestDis = INFINITY;  //reset distance to INFINITY for the next search
    }
    printf("On level %d, with all features. \tThis set has accuracy %.2f%%\n", 0, double(correct)/double(numberOfData)*100);
    sum=0;
    nearestIndex = -1;
    correct = 0;
    
    for( int x = 0; x < numberOfFeatures; x++ ){            //x is the current level of the tree
        for( int x2 = 0; x2 < numberOfFeatures; x2++ ){     //x2 is the index of the current feature we are checking
            if( find(indexSetOfFeatures.begin(), indexSetOfFeatures.end(), x2) != indexSetOfFeatures.end() ){
                for( int y = 0; y < numberOfData; y++ ){        //y is the current data for which we need to find the nearst neighbor
                    for( int y2 = 0; y2 < numberOfData; y2++ ){ //y2 is the data that we are currently comparing with y
                        if( y != y2 && x != numberOfFeatures-1 ){  //don't compare distance with itself
                            double tempSum = 0;
                            for( int n = 0; n < indexSetOfFeatures.size(); n++ ){
//                                if( indexSetOfFeatures[n] != -1 ){
                                if( indexSetOfFeatures[n] != x2 ){
                                    tempSum += pow( fabs( dataCollection[y]->features[indexSetOfFeatures[n]] - dataCollection[y2]->features[indexSetOfFeatures[n]] ), 2);
                                }
                            }
                            sum = sqrt(tempSum);
                            if( sum < nearestDis ){
                                nearestDis = sum;
                                nearestIndex = y2;
                            }
                        }
                    }
                    //After comparison, check if nearest neighbor is the same category
                    if( dataCollection[y]->category == dataCollection[nearestIndex]->category ){
                        correct++;  //increment the number of correct comparison
                    }
                    nearestDis = INFINITY;  //reset distance to INFINITY for the next search
                }
                if( highestAccuracy < double(correct)/double(numberOfData) ){
                    highestAccuracy = double(correct)/double(numberOfData);
                    highestAccuracyIndex = x2;
                }
            }
            correct = 0;    //reset correct for the next search
        }
        auto r =  find(indexSetOfFeatures.begin(), indexSetOfFeatures.end(), highestAccuracyIndex);
        if( r != indexSetOfFeatures.end() )
            indexSetOfFeatures.erase( r );
        DataSet *data = new DataSet(highestAccuracy, indexSetOfFeatures);
        dataSet.push( data );
        printf("On level %d, reomving feature #%d out of the set. \tThe new set has accuracy %.2f%%\n", x+1, highestAccuracyIndex+1, highestAccuracy*100);
        highestAccuracy = 0;
        highestAccuracyIndex = -1;
    }
}

int main(){
    ifstream inputFile("CS170_largetestdata__19.txt");
    if( !(inputFile.is_open()) ){
        //if file couldn't be open or couldn't be found, the program will shut down
        printf("File open failed. Program ending..\n");
        exit(0);
    }
    
    //extracting data from the given text file and put them into dataCollection
    while( getline(inputFile, inputLine) ){
        vector<double> tempListFeatures;
        stringstream input(inputLine);
        input >> category;
        while( input >> features ){
            tempListFeatures.push_back(features);
        }
        Data *data = new Data(category, tempListFeatures);
        dataCollection.push_back( data );
    }
    inputFile.close();
    
    printf("Please choose a search algorithm: \n\t1)Forward Search\n\t2)Backward Search\n");
    int method;
    cin >> method;
    if( method == 1 ){
        printf("Begin forward search...\n");
        forwardSearch();
    }else if( method == 2 ){
        printf("Begin backward search...\n");
        backwardSearch();
    }else{
        printf("Did you typed something weird? Program ending..\n");
    }
    
    printf("The best 2 sets are \n\t{");
    dataSet.top()->displayIndex();
    printf("} with accuracy %.2f%%.\n", 100*dataSet.top()->accuracy);
    dataSet.pop();
    printf("\t{");
    dataSet.top()->displayIndex();
    printf("} with accuracy %.2f%%.\n", 100*dataSet.top()->accuracy);
    dataSet.pop();
}
