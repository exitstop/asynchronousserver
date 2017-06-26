#include <iostream>     // std::ostream, std::cout, std::cin
#include <string>
#include <regex>

// #include <sstream>
// #include <algorithm>
// #include <iterator>
// #include <vector>

// #include <boost/algorithm/string.hpp>


using std::cout;
using std::cin;
using std::endl;

std::tuple<int, int, int> foo(){
    return std::make_tuple(6,5,3);
}

int main () {

    // if (std::regex_match ("s1bcolor", std::regex("(sub)(.*)") ))
        // std::cout << "string literal matched 123 'hello world'\n";


    std::string command ("hello wolrd -regme 'diliver hello world' -push 'regme hello' -poll 'hello world' ");
    std::smatch smatch;
    // std::regex e ("(\-r)((^')*)");
    std::regex regularExpression (R"(\-regme '[^']*'|\-push '[^']*'|\-poll '[^']*')");  

    // std::cout << "Target sequence: " << s << std::endl;
    // std::cout << "Regular expression: /\\b(sub)([^ ]*)/" << std::endl;
    // std::cout << "The following matches and submatches were found:" << std::endl;

    std::string temp;

    // std::regex_search(command, smatch, regularExpression)
    while (std::regex_search (command, smatch, regularExpression)) {
        temp = smatch.str();
        int firstsize = temp.find(' ',1)+2;
        int sizetemp  = temp.size()-firstsize;
        if(sizetemp<50){
            cout << temp.substr(firstsize, sizetemp-1) << endl;
            cout << temp.find("poll") << " npos: " << std::string::npos << endl;
        }else{
            cout << "строка в кавычках больше 50 символов" << endl;
        }
        command = smatch.suffix().str();
        std::cout << std::endl;
    }




    // std::vector<std::string> strs;
    // boost::split(strs, "boost strip split", boost::is_any_of("\t "));
    // for (auto str : strs) {
    //     cout << " boost: " << str << endl;
    // }
  
    return 0;
}