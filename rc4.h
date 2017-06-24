#ifndef ___RC4_H___
#define ___RC4_H___
namespace marusa{
	#define SWAP(x,y) {x^=y;y^=x;x^=y;}
/*
*  AUTHOR : Karcrack
*  DATE   : 100113
*  PURPOSE: RC4+ C++ cipher/decipher.
*/
template<unsigned int SIZE, class TYPE> struct myArr{
private:
    TYPE *data;
public:
    operator int(){
        return (long)data;
    }
 
    myArr& operator=(TYPE* rhs){
        this->data = rhs;
        return *this;
    };
 
    TYPE& operator[](unsigned int idx){
        return data[idx%SIZE];
    };
};
 
template <unsigned int KEY_SIZE>class RC4P{
private:
    unsigned int    i,
                    j;
 
    myArr<256, char>     S;
    myArr<KEY_SIZE, char>K;
public:
    RC4P(char S[], char K[]){
        this->S     = S;
        this->K     = K;
    };
    void calculate(char O[],unsigned int size){
        //KSA
        if(S==0 || K==0)
            return;
 
        j = 0;
        for(i = 0; i<256; i++)
            S[i] = i;
 
        for(i = 0; i<256; i++){
            j = j + S[i] + K[i];
            SWAP(S[i], S[j])
        };
        //PRGA
        unsigned int x = 0,
                     a,
                     b,
                     c;
        i = 0;
        j = 0;
        while(x<size){
            i++;
            a = S[i];
            j+= a;
            b = S[j];
            S[i] = b;
            S[j] = a;
            c = S[i<<5 ^ j>>3] + S[j<<5 ^ i>>3];
            O[x]^= (S[a+b] + S[c^0xAA]) ^ S[j+b];
            x++;
            i++;
        };
    };
};
} /* namespace marusa */

#endif /* ___RC4_H___ */
