#include <stdlib.h>
#include <time.h>

#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <iterator>

#define ID_TEST
#include "id.h"

id_t i2p = ID();

int main(void)
{
      using namespace std;
      srand(static_cast<unsigned>(time(0)) );

      string line;
      istringstream iss;
      vector<int>   v;
      for (;getline(cin, line), cin; )
      {
            iss.str(line);
            iss.clear();
            bool get = false;
            iss>>get;

            if ( get ) {
                  int id = id_acquire(&i2p);
                  if (id!=id_invalid) {
                        v.push_back(id);
                        cout<<"acquire id "<<setw(3)<<id<<endl;
                  } else {
                        cout<<"acquire id failed, out of memory\n";
                  }
            }
            else {
                  if ( !v.empty() ) {
                        int i = static_cast<int>( rand()%v.size() );
                        cout<<"release id "<<setw(3)<<v[i]<<endl;
                        id_release(&i2p, v[i]);
                        v.erase(v.begin()+i);
                  } else {
                        cout<<"no id held\n";
                  }
            }
            sort(v.begin(), v.end() );
            cout<<"[";
            copy(v.begin(), v.end(), ostream_iterator<int>(cout,",") );
            cout<<"]\n";
      }
      return 0;
}

#undef NDEBUG
#include "id.c"
