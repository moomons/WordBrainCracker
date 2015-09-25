//
//  main.cpp
//  WordBrainCracker
//
//  Created by mo on 9/25/15.
//  Copyright © 2015 mons. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <cmath>
#include <ctime>

class square{
public:
    char letter;
    bool available;
    square(){
        letter = '\0';
        available = false;
    }
    
    void Print(){
        std::cout << letter << ", " << available;
    }
    
//    std::ostream & operator << (std::ostream & out, square const & data) {
//        out << letter << ", " ;
//        out << available;
//        return out ;
//    }
};

class word{
public:
    std::string *wordletters;
    int ExpectedLength;
    word(){
        wordletters = new std::string();
        ExpectedLength = 0;
    }
    ~word(){
        delete wordletters;
    }
};

class Cracker{
private:
    std::string RawLettersInput;
    int LettersLeft;
    int MatrixSize;
    int HowManyWords;
    std::vector<std::string> Dict;
    int ExpectedWordLength[9];
    square SQ[9][9];
    square SQ_EMPTY; // 空格子
    bool flag_GotSolution = false;
    clock_t timer;
    
    int SQSetAllAvailable(){
        int i,j;
        for (i = 1; i <= MatrixSize; i++)
            for (j = 1; j <= MatrixSize; j++)
            {
                SQ[i][j].available = true;
            }
        return 0;
    }
    
    
    void DropSquare(square SQ[9][9]){
        int x, y;
        for (x = MatrixSize; x >= 1; x--) // TODO: 2 还是 1?
            for (y = MatrixSize; y >= 1; y--)
            {
                if (SQ[x][y].available == false){ // 如果当前格子已经被 match 掉了
                    
//                    std::cout << "SQ[x][y] = ";
//                    SQ[x][y].Print();
//                    std::cout << std::endl;
//                    std::cout << "SQ[x-1][y] = ";
//                    SQ[x-1][y].Print();
//                    std::cout << std::endl;
//                    std::cout << "SQ[x][y-1] = ";
//                    SQ[x][y-1].Print();
//                    std::cout << std::endl;
                    
                    if (x-1 >= 1 && SQ[x-1][y].available == true){
                        // 上方格子可用
                        SQ[x][y] = SQ[x-1][y];
                        SQ[x-1][y] = SQ_EMPTY;
                    }
                    else if (y-1 >= 1 && SQ[x][y-1].available == true){
                        // 左侧格子可用（或者是右侧？未测试！如果是右侧的话需要修改第二层循环的顺序！）
                        SQ[x][y] = SQ[x][y-1];
                        SQ[x][y-1] = SQ_EMPTY;
                    }
                }
            }
    }
    
    // Return Values:
    // 0 means word found
    // 1 means letter not available
    // -1 means word not found
    int FindWord(std::vector<std::string> DictInput, square SQ[9][9], int WordCount, int RecurDepth, int Cord_X, int Cord_Y){
        // Check availability of the current letter
        if (SQ[Cord_X][Cord_Y].available == false || 0){
            return 1;
        }
        
        SQ[Cord_X][Cord_Y].available = false;
        
        std::vector<std::string> ParsedDict;
        std::vector<std::string>::iterator dict_it;
        for (dict_it = DictInput.begin(); dict_it < DictInput.end(); dict_it++){
            if ((*dict_it).c_str()[RecurDepth] == SQ[Cord_X][Cord_Y].letter){
                ParsedDict.push_back(*dict_it);
            }
        }
        
        if (RecurDepth+1 == ExpectedWordLength[WordCount+1] && ParsedDict.size() > 0){ // 达到要求的长度了
            // 开始检查
            for (dict_it = ParsedDict.begin(); dict_it < ParsedDict.end(); dict_it++){
                if ((*dict_it).size() == ExpectedWordLength[WordCount+1]){
                    flag_GotSolution = true;
                    std::cout << "Word " << WordCount+1 << ": " << *dict_it << std::endl;
                }
            }
            
            if (flag_GotSolution){
                flag_GotSolution = false;
                
                square SQ_ForNextWord[9][9];
                memcpy(SQ_ForNextWord, SQ, 9*9*sizeof(square));
                // 模拟消除一个单词之后的下落效果
                DropSquare(SQ_ForNextWord);
                
                int i, j;
                for (i = 1; i <= MatrixSize; i++)
                    for (j = 1; j <= MatrixSize; j++)
                    {
                        FindWord(Dict, SQ_ForNextWord, WordCount+1, 0, i, j);
                    }
            }
            
        }
        else{
            
            // 6  7  8    .Y
            // 5     1    X
            // 4  3  2
            //
            // (-1,-1)  (-1, 0)  (-1, 1)
            // ( 0,-1)  ( 0, 0)  ( 0, 1)
            // ( 1,-1)  ( 1, 0)  ( 1, 1)
            FindWord(ParsedDict, SQ, WordCount, RecurDepth+1, Cord_X  , Cord_Y+1); // 1
            FindWord(ParsedDict, SQ, WordCount, RecurDepth+1, Cord_X+1, Cord_Y+1); // 2
            FindWord(ParsedDict, SQ, WordCount, RecurDepth+1, Cord_X+1, Cord_Y  ); // 3
            FindWord(ParsedDict, SQ, WordCount, RecurDepth+1, Cord_X+1, Cord_Y-1); // 4
            FindWord(ParsedDict, SQ, WordCount, RecurDepth+1, Cord_X  , Cord_Y-1); // 5
            FindWord(ParsedDict, SQ, WordCount, RecurDepth+1, Cord_X-1, Cord_Y-1); // 6
            FindWord(ParsedDict, SQ, WordCount, RecurDepth+1, Cord_X-1, Cord_Y  ); // 7
            FindWord(ParsedDict, SQ, WordCount, RecurDepth+1, Cord_X-1, Cord_Y+1); // 8
            
        }
        
        SQ[Cord_X][Cord_Y].available = true;
        
        return 0;
    }
    
public:
    Cracker(bool UseDebugInput){
        int i, j;
        
        // MARK: Input
        if (UseDebugInput == false){
            std::cout << "Letters: ";
            std::cin >> RawLettersInput;
            // TODO: Check if all the input characters are letters (using ASCII)
            // TODO: Convert all the letters to lower-case
            // TODO: Judge Matrix Size from the number of letters, and output to the user
            MatrixSize = (int)floor(sqrt(RawLettersInput.size()));
            
            std::cout << "How many words? ";
            std::cin >> HowManyWords;
            
            for (i = 1; i <= HowManyWords; i++)
            {
                std::cout << "Length for Word " << i+1 << ": ";
                std::cin >> ExpectedWordLength[i];
            }
        }
        else{
            // Debug Input
            RawLettersInput = "lselidlod";
            MatrixSize = (int)floor(sqrt(RawLettersInput.size()));
            HowManyWords = 2;
            ExpectedWordLength[1] = 4;
            ExpectedWordLength[2] = 5;
        }
        
        // MARK: Parse the raw input to class square
        std::string::iterator it = RawLettersInput.begin();
        for (i = 1; i <= MatrixSize; i++)
            for (j = 1; j <= MatrixSize; j++)
            {
                SQ[i][j].letter = *(it++);
                SQ[i][j].available = true;
            }
        
        // MARK: Load the dictionary
        // TODO: Detect the existence of file "web2"
        std::ifstream DictFileStream("web2");
        std::copy(std::istream_iterator<std::string>(DictFileStream),
                  std::istream_iterator<std::string>(),
                  std::back_inserter(Dict));
        
        
        // MARK: Start Searching words from (1, 1)
        int WordCount = 0;
        
        timer = clock();
        for (i = 1; i <= MatrixSize; i++)
            for (j = 1; j <= MatrixSize; j++)
            {
                FindWord(Dict, SQ, WordCount, 0, i, j);
            }
        timer = clock() - timer;
        
        std::cout << "Time elapsed: " << (float)timer/CLOCKS_PER_SEC << " sec." << std::endl;
    }
};

int main(int argc, const char * argv[]) {
    
    Cracker CR(true);
    
    std::cout << "Done." << std::endl;
    
    return 0;
}
