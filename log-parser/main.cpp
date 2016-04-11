/* 
 * A simple command line parser to extract dau, revenue, and orderId parameters 
 * from log files in folder on Linux. 
 * 
 * File:   main.cpp
 * Author: eric
 *
 * Created on March 29, 2016, 4:25 PM
 */

#include <cstdlib>
#include <iostream>
#include <glob.h>
#include <vector>
#include <map> 
#include <string>
#include <stdio.h>
#include <algorithm>
#include <fstream>
#include <cstring>

// HTTP POST libcurl
#include <curl/curl.h>

using namespace __gnu_cxx; 
using namespace std;

using std::ifstream;
using std::vector;
using std::cout;


using std::endl;

const int MAX_CHARS_PER_LINE = 65535;
const int MAX_TOKENS_PER_LINE = 50;


class userManager 
{
public: 
   // DAU Daily Active User 
   vector<string> _userList;
   
public:   
    userManager(){};
    ~userManager(){};
 
};

class payManager 
{
private: 
    // REV : revenue 
    int _revenue;
    
   // TCOUNT : Transaction Count 
   vector<string> _orderList;
   
   // PAY UU: Paid Unique User 
   vector<string> _userList;
   
public:   
    payManager()
    {
        _revenue = 0;
        _orderList.clear();
        _userList.clear();
    };
    ~payManager(){};
 
    void addRevenue(int money) { if(money > 0 )_revenue += money; }
    int  getRevenue() {return _revenue; }
  
    void addOrder(string id) { if(id.length() > 0 ) _orderList.push_back(id); }
    int  getOrderCount() {return _orderList.size(); }
    
    void addUser(string id)   
    {
        if(id.length() == 0 ) return;
        
        if ( std::find(_userList.begin(), _userList.end(), id) == _userList.end() )
            _userList.push_back(id); 
    }
    int  getUserCount() {return _userList.size(); }
};

// Initial Date Map and User List
map<std::string, userManager> dauMap;
typedef map<std::string, userManager>::iterator Dau_type;
typedef pair <std::string, userManager> Dau_Pair;

// Initial Pay Map and Order List
map<std::string, payManager> payMap;
typedef map<std::string, payManager>::iterator Pay_type;
typedef pair <std::string, payManager> Pay_Pair;

// Get a list of file names in folder 
vector<string> globVector(const string& pattern){
    glob_t glob_result;
    glob(pattern.c_str(),GLOB_TILDE,NULL,&glob_result);
    vector<string> files;
    for(unsigned int i=0;i<glob_result.gl_pathc;++i){
        files.push_back(string(glob_result.gl_pathv[i]));
    }
    globfree(&glob_result);
    return files;
}

// Send an http request to back end for analysis
void sendPOST(string timeStr, int revenue, int du, int pau, int tcount)
{
  CURL *curl;
  CURLcode res;
 
  if(time == NULL) 
      return;
  
  // In windows, this will init the winsock stuff 
  curl_global_init(CURL_GLOBAL_ALL);

  curl = curl_easy_init();
  if(curl) 
  {
    // Url
    curl_easy_setopt(curl, CURLOPT_URL, "http://Your Server IP/KPI/addRevenue.php");
    // Now specify the POST data 
    // k=bad975ca9f322b135a1feaa4903974b2&d=2016-04-05&r=500&dau=1000&pu=50&tc=80
    string sendStr = "";
    sprintf( const_cast<char*>(sendStr.c_str()), "k=bad975ca9f322b135a1feaa4903974b2&d=%s&r=%d&dau=%d&pu=%d&tc=%d", timeStr.c_str(), revenue, du, pau, tcount);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, sendStr.c_str());
 
    // Perform the request, res will get the return code 
    res = curl_easy_perform(curl);
    // Check for errors
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    // always cleanup
    curl_easy_cleanup(curl);
  }
  
  curl_global_cleanup();
    
}
// Parse middle string 
string parseValue(string input, int s, int e)
{
     char *ptr = const_cast<char*>(input.c_str());
    
    int start = 0;
    int end = 0;
    int n = 0, count=0;
    while(*ptr)
    {    
        if(iswspace(*ptr) )
        {
          count++;
          if( count == s ) start = n;
          else if(count == e ) end = n;
        }
      
        n++;
        ptr++;
    }
        
    char *ptr2 = const_cast<char*>(input.c_str());
    char buf[MAX_CHARS_PER_LINE];
    memset(buf, 0, MAX_CHARS_PER_LINE); 
    memcpy( buf, ptr2+start, end-start);
    
    string ans(buf);
    // remove white space 
    ans.erase(remove_if(ans.begin(), ans.end(), ::isspace), ans.end());
    return ans;   
}

// Parse last string 
string parseLastValue(string input, int index)
{
    char *ptr = const_cast<char*>(input.c_str());
    
    int start = 0;
    int end = 0;
    int n = 0, count=0;
    while(*ptr)
    {  
        if(iswspace(*ptr) )
        {
          count++;
          if( count == index ) 
          {    
              start = n;
              end = input.size();
          }
        }
      
        n++;
        ptr++;
    }
    
 
    char *ptr2 = const_cast<char*>(input.c_str());    
    char buf2[MAX_CHARS_PER_LINE];
    memset(buf2, 0, MAX_CHARS_PER_LINE);
    memcpy( buf2, ptr2+start, end-start);
    
    string ans(buf2);
    // remove white space 
    ans.erase(remove_if(ans.begin(), ans.end(), ::isspace), ans.end());
    
    return ans;
    
}

// Parse Date
string getDate(string input)
{
    return parseLastValue(input, 3);
    
}

// Parse User Id
string getUserId(string input)
{
    return parseValue(input, 3, 4);
    
}

// Parse Revenue
string getRev(string input)
{
   return parseValue(input, 8, 9);
    
}

// Parse Order Id
string getOrderId(string input)
{
    return parseLastValue(input, 9);
}


// Parse DAU
int parseDAU(string filepath)
{
  if(filepath.length() <= 0 ) return 0;
        
  // create a file-reading object
  ifstream fin;
  fin.open(filepath.c_str()); // open a file
  if (!fin.good()) 
    return -1; // exit if file not found 
  
  while (!fin.eof())
  {
    // read an entire line into memory
    char buf[MAX_CHARS_PER_LINE];
    fin.getline(buf, MAX_CHARS_PER_LINE);
    
    string oldStr(buf); 
    int start = 0;
    
    if(oldStr.length() > 0 )
    {
        int pos  = oldStr.find(" ");
        string temp = oldStr.substr(0, pos);
        
        oldStr = oldStr.substr(pos, oldStr.length());
        cout <<  "  temp: " << temp << endl;
        cout <<  "  oldStr: " << oldStr << endl;
        
        // Parse Date 
        string userDate = getDate(temp);
        cout <<  "  userDate:" << userDate << endl;
        
        // Parse UserId 
        string userId = getUserId(oldStr);
        cout <<  "  userId:" << userId << endl;
    

        // Update Date and User List
        if(dauMap.find(userDate) == dauMap.end())
        {
            userManager user;
            dauMap.insert(Dau_Pair( userDate, user));
            
        }else {
           
            Dau_type itr = dauMap.find(userDate);
            if ( std::find(itr->second._userList.begin(), itr->second._userList.end(), userId) == itr->second._userList.end() )
            {
                itr->second._userList.push_back(userId); 
                cout <<  "Date: " <<   dauMap.find(userDate)->first << "  user size:" << itr->second._userList.size() << endl;
            
               
            }
     
        }

    }

  }
 
  cout << "################################ "<< endl;
   for(Dau_type itr = dauMap.begin(); itr != dauMap.end(); itr++) 
  {
      cout << "Date:" << itr->first << " Users Count:" << itr->second._userList.size() << endl;
  }
}

// Parse Revenue and OrderId
int parseRevenueOrderId(string filepath)
{
  
  if(filepath.length() <= 0 ) return 0;
        
    // create a file-reading object
  ifstream fin;
  fin.open(filepath.c_str()); // open a file
  if (!fin.good()) 
    return -1; // exit if file not found
  
  // read each line of the file
  const char* token[MAX_TOKENS_PER_LINE] = {};

    
  while (!fin.eof())
 {
    // read an entire line into memory
    char buf[MAX_CHARS_PER_LINE];
    fin.getline(buf, MAX_CHARS_PER_LINE);
    
  
    string oldStr(buf); 
    int start = 0;
    
    if(oldStr.length() > 0 )
    {
       
        int pos  = oldStr.find(" ");
        string temp = oldStr.substr(0, pos);
        
        oldStr = oldStr.substr(pos, oldStr.length());
        
        cout <<  "  str1: " << temp << endl;
        cout <<  "  str2: " << oldStr << endl;
        
        // Parse Date 
        string userDate = getDate(temp);
        cout <<  "  userDate:" << userDate << endl;
        
        // Parse User Id 
        string userId = getUserId(oldStr);
        cout <<  "  userId:" << userId << endl;
         
       // Parse Revenue 
        string money = getRev(oldStr);
        cout <<  "  revenue :" << money << endl;
        
        // Parse orderId
        string orderId = getOrderId(oldStr);
        cout <<  "  orderId:" << orderId << endl;
        
        // Update Date and User List
        if(payMap.find(userDate) == payMap.end())
        {
            payManager payment;
            payment.addRevenue(atoi(money.c_str()));
            payment.addOrder(orderId);
            payment.addUser(userId);
             
            payMap.insert(Pay_Pair( userDate, payment));
            
        }else {
           
            Pay_type itr = payMap.find(userDate);
            if ( itr != payMap.end() )
            {
                itr->second.addOrder(orderId);  
                itr->second.addRevenue(atoi( money.c_str() )); 
                itr->second.addUser(userId);                 
   
            }
     
        }
        
    }

 }
  
  cout << "################################ "<< endl;
  for(Pay_type itr = payMap.begin(); itr != payMap.end(); itr++) 
  {
      cout << "Date:" << itr->first << " Revenue:" << itr->second.getRevenue() << " OrderCount:" << itr->second.getOrderCount() << " UserCount:" << itr->second.getUserCount() << endl;
  }
 
}

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);

    strftime(buf, sizeof(buf), "%Y%m%d", &tstruct);

    return buf;
}


int main(int argc, char** argv) {

   if ( argc != 2 ) {// argc should be 2 for correct execution
        cout<<"missing Parameter[1] 1: Parse All Log; 2: Parse Today`s log "<< argv[0] << endl;
		cout<<"missing Parameter[2] folder path "<< endl;
   }
  
  // Parameter[1]: 
  // 1: Parse All Log; 2: Parse Today`s log
  int mode = atoi(argv[1]);
  
  // Parameter[2]: folder path
  string folderPath(argv[2]);
  string fullLogPath = "";
  

  // Parse folder 
  std::cout << "--------------- Parse file name -------------------"<<'\n';  
  sprintf( const_cast<char*>(fullLogPath.c_str()), "%s/log_login/*", folderPath.c_str());
  vector<string> files = globVector(fullLogPath.c_str());
  
  for (std::vector<string>::iterator it = files.begin(); it != files.end(); ++it)
  {
        std::cout << " All Path " << *it << endl;   
          std::cout << "---------------Parse DAU --------------------"<<'\n';
        // Get today time
        if(mode == 1)
        {
            string currentDate = currentDateTime();
            std::cout << " Get now date  " << currentDate << endl;

            if((*it).find(currentDateTime()) != std::string::npos)
            {
                std::cout << " OK Get now date  " << currentDateTime() << " Path" << (*it) << endl;
                parseDAU (*it); 
            }
        }else {
            // All Log
            parseDAU (*it); 
        }
        
  }
  

  std::cout << "--------------- Parse revenue and transaction id   --------------------"<<'\n';
  string fullLogPayPath = "";
  sprintf( const_cast<char*>(fullLogPayPath.c_str()), "%s/log_recharge/*", folderPath.c_str());
  
  vector<string> filesRev = globVector(fullLogPayPath.c_str());
  for (std::vector<string>::iterator it = filesRev.begin(); it != filesRev.end(); ++it)
  {
        // Get Today`s log
        if(mode == 1)
        {
            string currentDate = currentDateTime();
            std::cout << " Get now date  " << currentDate << endl;

            if((*it).find(currentDateTime()) != std::string::npos)
            {
                std::cout << " OK Get now date  " << currentDateTime() << " Path" << (*it) << endl;
                parseRevenueOrderId (*it); 
            }
        }else {
            // Get All Log
            parseRevenueOrderId (*it); 
        }
      
  }
  
  
  std::cout << "--------------- Send parsed result to back end with HTTP POST  --------------------"<<'\n';
  for(Dau_type ditr = dauMap.begin(); ditr != dauMap.end(); ditr++) 
  {   
      Pay_type pitr = payMap.find(ditr->first);
      if ( pitr != payMap.end() )
      {
          cout << " Revenue:" << pitr->second.getRevenue() << " OrderCount:" << pitr->second.getOrderCount() << " PAY UU:" << pitr->second.getUserCount() << endl;                   
          sendPOST(ditr->first, pitr->second.getRevenue(), ditr->second._userList.size(), pitr->second.getUserCount(), pitr->second.getOrderCount());
      }else {
          sendPOST(ditr->first, 0, ditr->second._userList.size(), 0, 0);

      } 
  }

 
  return 0; 
   
}

