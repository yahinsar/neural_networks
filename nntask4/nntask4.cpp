#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cctype>
#include <iomanip>
#include <algorithm>

using namespace std;

class Matrix {
private:
    vector< vector <vector<int>>> matrix;
    vector<int> vector;

public:
    Matrix(const   std::vector<  std::vector<std::vector<int>>>& matrixData, const   std::vector<int>& vectorData) : matrix(matrixData), vector(vectorData) {}

    std::vector< std::vector <std::vector<int>>> getMatrix() {
        return matrix;
    }
    std::vector<int> getVector() {
        return vector;
    }

};

Matrix readFromFile(const string& matrixFileName, const string& vectorFileName) {
    vector <vector< vector<int>>> matrixData;
    vector<int> vectorData;

    ifstream vectorFile(vectorFileName);
    if (!vectorFile.is_open()) {
        cerr << "Ошибка открытия файла\n";
        exit(EXIT_FAILURE);
    }

    int rowSize;
    string line;
    getline(vectorFile, line);
    stringstream ss(line);
    int value;
    while (ss >> value) {
        vectorData.push_back(value);
    }
    vectorFile.close();
    rowSize = vectorData.size();

    ifstream matrixFile(matrixFileName);
    if (!matrixFile.is_open()) {
        cerr << "Ошибка открытия файла\n";
        exit(EXIT_FAILURE);
    }

    int strNumber = 1;
    
    while (getline(matrixFile, line)) {
        vector <vector <int>> strM;
        vector<int> row;
        stringstream ss(line);
        char ch;
        while (ss >> ch) {
            if (isdigit(ch)) {
                ss.putback(ch);
                int value;
                ss >> value;
                row.push_back(value);
            }
            if (ch == ']') {
                strM.push_back(row);
                if (rowSize != row.size()) {
                    cout << "Ошибка в строке " << strNumber << ":  некорректное число компонент нейронов\n";
                    exit(EXIT_FAILURE);
                }
                row.clear();
            }
            if (ch != ']' && ch != '[' && ch != ',' && ch != ' ' && !isdigit(ch)) {
                cout << "Ошибка в строке " << strNumber << ":  неправильно задан вес матрицы межнейронной связи\n";
                exit(EXIT_FAILURE);
            }
        }
        matrixData.push_back(strM);
        strNumber++;
    }
    matrixFile.close();

    Matrix myMatrix = Matrix(matrixData, vectorData);
    return myMatrix;
}

void matrixToJSON(Matrix matrix, const string& fileName) {
    vector< vector <vector<int>>> matrixVec = matrix.getMatrix();
    ofstream jsonFile(fileName);
    if (!jsonFile.is_open()) {
        cerr << "Ошибка открытия файла\n";
        exit(EXIT_FAILURE);
    }
    jsonFile << "[\n";

    for (int i = 0; i < matrixVec.size(); ++i) {
        jsonFile << "\t[\n";
        jsonFile << "\t\t[\n";
        for (int j = 0; j < matrixVec[i].size(); ++j) {
            jsonFile << "\t\t\t[\n";
            for (int k = 0; k < matrixVec[i][j].size(); ++k) {
                jsonFile << "\t\t\t\t" << matrixVec[i][j][k];
                if (k < matrixVec[i][j].size() - 1) {
                    jsonFile << ",";
                }
                jsonFile << "\n";
            }
            jsonFile << "\t\t\t]";
            if (j + 1 < matrixVec[i].size()) {
                jsonFile << ",";
            }
            jsonFile << "\n";
        }
        jsonFile << "\t\t]";
        jsonFile << "\n";
        jsonFile << "\t]";
        if (i + 1 < matrixVec.size()) {
            jsonFile << ",";
        }
        jsonFile << "\n";
    }

    jsonFile << "]\n";

    jsonFile.close();

    return;
}

vector<double> resFunc(Matrix matrix) {
    cout << setprecision(16);
    vector< vector <vector<int>>> matrixVec = matrix.getMatrix();
    vector<int> vectorVec = matrix.getVector();
    vector<double> vecDouble;
    for (int i = 0; i < vectorVec.size(); i++) {
        vecDouble.push_back(static_cast<double>(vectorVec[i]));
    }
    vector<vector<double>> newM;

    for (const auto& layer : matrixVec) {
        vector<double> vecFor;
        for (const auto& n : layer) {
            double res = 0;
            for (int i = 0; i < vecDouble.size(); ++i) {
                res += static_cast<double>(n[i]) * vecDouble[i];
            }
            res /= (1 + abs(res));
            vecFor.push_back(res);
        }
        newM.push_back(vecFor);
        vecDouble = vecFor;
    }

    return newM[newM.size() - 1];
}

string findInStr(string const& str, int n) {
    if (str.length() < n) {
        return str;
    }
    return str.substr(0, n);
}

void resToFile(vector<double> resultMatrix, const string& fileName) {
    ofstream outFile(fileName, ios::out | ios::trunc);
    outFile << setprecision(16);
    if (!outFile.is_open()) {
        cerr << "Ошибка открытия файла\n";
        return;
    }

    for (int i = 0; i < resultMatrix.size(); i++)
        outFile << resultMatrix[i] << " ";

    outFile.close();
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "rus");
    string input1, input2, output1, output2;
    for (int i = 0; argv[i]; i++)
    {
        string checkStr = string(argv[i]);
        if (checkStr.length() > 4) {
            string ifStr1 = findInStr(checkStr, 7);
            string ifStr2 = findInStr(checkStr, 8);
            string subStr = checkStr.substr(7, checkStr.length());
            string subStr2 = checkStr.substr(8, checkStr.length());
            if (ifStr1 == "input1=") {
                input1 = subStr;
            }
            if (ifStr1 == "input2=") {
                input2 = subStr;
            }
            if (ifStr2 == "output1=") {
                output1 = subStr2;
            }
            if (ifStr2 == "output2=") {
                output2 = subStr2;
            }
        }
    }


    Matrix myMatrix = readFromFile(input1, input2);

    matrixToJSON(myMatrix, output1);

    vector<double> resultMatrix = resFunc(myMatrix);

    resToFile(resultMatrix, output2);

    return 0;
}