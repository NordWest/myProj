#pragma once  
#include <iostream>  
#include <cstdlib>
#include <math.h>
class Fragment {
private:
    char * string;
    int len;
public:
    Fragment() {
        this->len=0;
    }


    Fragment(int len) {
        this->len = len;
        this->string = new char[this->len];
    }
 
    ~Fragment() {
        if (this->len!=0) {
            //this->len = 0;
            delete [] this->string;
        }
    }

    char *  getString() {
        return this->string;
    }
    
    char getCharAtIndex(int x) {
        return this->string[x];
    }
    
    int getSize() {
        return this->len;
    }

    /**
     * fill this fragment with the content of a std::string
     */
    void fill(std::string input) {
        int s = input.size();
        this->string = new char[s];
        for (int j(0);j<s;j++) {
            this->string[j] = input.at(j);
        }
        this->len = s;
    }


    /**
     * fill this fragment with the content of another fragment.
     * cannot be called on itself.
     * will return without updating to contain the required substring if:
     * 1: The starting position is greater than or equal to the given string
     * 2: The specified end token is not found
     */
    void fill (Fragment &input) {
        if(this!=&input) {
            this->string = new char[input.len];
            for (int j(0);j<input.len;j++) {
                this->string[j] = input.string[j];
            }
            this->len = input.len;
        }
    } 
 
    /**
     * return the string found between the specified start and end indexes.
     * cannot be called on itself.
     * will return without updating to contain the required substring if:
     * 1: The starting position is greater than or equal to the given string
     * 2: The specified end token is not found
     */
    void fill (Fragment &input, int start, int stop) {
        if(this!=&input) {
            this->string = new char[stop-start];
            if (start>0&&stop<input.len) { 
                for (int j(start);j<stop;j++) {
                    this->string[j] = input.string[j];
                }
                this->len = stop-start;
            }
        }
    }

    /**
     * return the string found from the specified starting index to the given character.
     * records the index in the string reached.
     * cannot be called on itself.
     * will return without updating to contain the required substring if:
     * 1: The starting position is greater than or equal to the given string
     * 2: The specified end token is not found
     */
    void fill(Fragment &source,  char endChar, int &pos) {
        // first step, advance from the given starting position in the string until the specified end character is found or the end of the string is reached.
        int stringSize(0);
        for (int x(pos);x<source.getSize();x++) {
            if (source.getCharAtIndex(x)==endChar) {
                break;
            }
            stringSize++;
        }
        // now copy the substring into this fragment, deleting the old content first if required 
        if (this->len!=0) {
            delete [] this->string;
            this->len =0;
        }
        this->string = new char[stringSize+1];
        memset(this->string,'\0',stringSize+1);
        int loc(0);
        for (int y(pos);y<pos+stringSize;y++) {
            this->string[loc] = source.getCharAtIndex(y);
            loc++;
        }
        pos+=stringSize+1;
        this->len = stringSize+1;
    }
    


    int toInt() {
        return atoi(this->string);
    }

    float toFloat() {
        return atof(this->string);
    }

	bool operator == ( const Fragment & other) {
		if (this->len != other.len) {
			return false;
		}

        for (int x(0);x<this->len;x++) {
            if (this->string[x] != other.string[x]) {
                return false;
            }
		}
		return true;
	}

	bool operator  != ( const Fragment & other) {
		return !(*this==other);
	}

};
