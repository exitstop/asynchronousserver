#include <iostream>
using namespace std;

template<class T = int>
struct myStruct{
    T i;
};

template<class Ttempl = int, template<class> class Tclass = myStruct>
class myClass{
public:
    myClass(Tclass<Ttempl> &_data):data_ptr(&_data){
        cout << "create --test: " << sizeof(data_ptr->i) << endl;
        cout << "create --test: " << data_ptr->i << endl;
        data_ptr->i=66;
    };

private:
    //myStruct data;
    Tclass<Ttempl>* data_ptr;
};

int main(){

    myStruct<int> myS;
    myS.i=554;
    myClass<int> myC(myS);
    cout << "myS.i=" << myS.i << endl;
}
