#include<thread>
#include<future>
#include<iostream>
#include<chrono>


class CallableClass {
    std::string name;
public:
    CallableClass(std::string &name):name(name){}
    void operator()() {
        std::cout<<"Hello my name is "<<name<<"! \n";
    }
    void func(std::string & name) {
        std::cout<<"1234 "<<name<<"\n";
    }
};

void sayHello(std::string & name) {
    std::cout<<"Hello my name is "<<name<<"! \n";
}



void producer(std::promise<std::string> && thrPromise) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    try {
        if (rand() % 2 == 0) {
            std::string msg = "blueberry taste ";
            thrPromise.set_value(msg);
        } else {
            throw std::runtime_error("No liquid left\n");
        }
    } catch (...) {
        thrPromise.set_exception(std::current_exception());
    }
}
void consumer(std::future<std::string> && thrFuture) {
    std::future_status status=std::future_status::deferred;
    while (status!=std::future_status::ready) {
        status=thrFuture.wait_for(std::chrono::milliseconds(420));
        switch (status) {
            case std::future_status::ready:
                std::cout<<"Bartek came to AEI\n";
            break;
            case std::future_status::timeout:
                std::cout<<"Bus was canceled\n";
            break;
            case std::future_status::deferred:
                std::cout<<"Bartek is still sleeping\n";
            break;
        }
    }
    std::cout<<std::flush;
    try {
        std::string product=thrFuture.get();
        std::cout<<"New liquid"<<product<<"\n";
    }
    catch (std::exception & e) {
        std::cout<<e.what()<<"\n";
    }
}

#include<mutex>
std::mutex m;

#include<fstream>
std::vector<int> loadDataInBackground() {
    std::vector<int> data;
    std::ifstream fin("data.txt"); //Big enough file
    int token;
    while (fin>>token) {
        data.push_back(token);
    }
    return data;
}


///Producer thread doing

void fizz(std::vector<std::promise<std::vector<int>>> && promise) {
    std::vector<int> buffer(10);
    std::ifstream fin("data.txt");
    int iteration=0;
    while (!fin.eof()) {
        for (int i=0;i<10;i++) {
            fin>>buffer[i];
            if (fin.eof())
                continue;
        }
        promise.at(iteration).set_value(buffer);
        iteration++;
    }
}
///function of fizzbuzz

void fizzBuzz(std::vector<int> & v) {
    for (auto & element:v)
        if (element%6==0) {
            std::cout<< element<<" FizzBuzz \n";
        }
}
//consumer, need to give data size
void buzz(int size,std::vector<std::future<std::vector<int>>> && ft ) {
    int iteration=0;
    while (size>0) {
        size-=10;
        try {
            ft.at(iteration).wait();
            std::vector<int> data = ft.at(iteration).get();
            fizzBuzz(data);
            data.clear();
            iteration++;
        }
        catch (std::exception & e) {}

    }

}

void fizzBuzzWithThread() {

    std::vector<std::promise<std::vector<int>>> thrPromise(2); //Stale dane, normalnie trzeba by policzyc segmenty
    std::vector<std::future<std::vector<int>>> thrFuture(2); //Stale dane, normalnie trzeba by policzyc segmenty
    for (int i=0;i<2;i++) {
        thrFuture.at(i)=thrPromise.at(i).get_future();
    }
    ///Can do with thread, or with async no problem
   // std::thread producerFizz(fizz,std::move(thrPromise));
    //std::thread consumerFizzBuzz(buzz,15,std::move(thrFuture));
   // producerFizz.join();
  //  consumerFizzBuzz.join();
    std::future<void> func1=std::async(std::launch::async,fizz,std::move(thrPromise));
    std::future<void> func2=std::async(std::launch::async,buzz,15,std::move(thrFuture));
    func1.get();
    func2.get();
}

typedef std::vector<std::vector<int>> matrix;
#include<sstream>
matrix loadMatrix(std::string filename) {
    matrix result;
    std::vector <int> record;
    std::ifstream fin(filename);
    std::string line;
    int token;
    while (std::getline(fin,line)) {
        std::stringstream ss(line);
        while (ss>>token) {
            record.push_back(token);
        }
        result.push_back(record);
        record.clear();
    }
    return result;
}
matrix mult(matrix & A, matrix & B) {
    matrix result;
    result.resize(A.size());
    for (int i=0;i<A.size();i++) {
        result.at(i).resize(B.size());
    }
    for (int i=0;i<A.size();i++)
        for (int j=0;j<B.size();j++) {
            int sum=0;
            for (int k=0;k<A.size();k++) {
                  sum+=A[k][j]*B[i][k];
            }
            result[i][j]=sum;
        }
    return result;
}


void matrixesWithThreads() {
    std::packaged_task<matrix(std::string)> packagedTask(loadMatrix);
    std::future<matrix> future = packagedTask.get_future();
    std::string filename1="MatrixA.txt";
    std::string filename2="MatrixB.txt";
    std::thread load1(std::move(packagedTask),filename1);
/* Z asynciem
 * std::future<matrix>=
    std::async(std::launch::async,loadMatrix,filename1);
*/
    matrix B=loadMatrix(filename2);
    load1.join();
    future.wait();
    matrix A=future.get();
    matrix C=mult(A,B);
        for (int i=0;i<C.size();i++) {
            for (int j=0;j<C.at(i).size();j++) {
            std::cout<<C.at(i).at(j)<<"\t";
            }
            std::cout<<"\n";
        }

}
#include<regex>
#include<ranges>
std::vector<std::string> loadData(std::string fileName) {
    std::ifstream fin(fileName);
    std::vector<std::string> data(std::istream_iterator<std::string>{fin},{});
    return data;
}

typedef std::common_type_t<std::chrono::duration<long long, std::ratio<1, 1000000000>>, std::chrono::duration<long long, std::ratio<1, 1000000000>>>  TIME;

std::vector<std::string> rangesSegment(std::vector<std::string> & segment) {
    std::regex pattern(R"(^[\w-\.]+@([\w-]+\.)+[\w-]{2,4}$)");
    auto view=segment|std::ranges::views::filter([=](std::string & mail){
    return std::regex_match(mail, pattern);
    });
    std::vector<std::string> result(view.begin(), view.end());
    std::ranges::copy(view.begin(), view.end(), std::ranges::begin(result));
    return result;

}
void regexSegment(std::vector<std::string> & segment) {
    std::regex pattern(R"(^[\w-\.]+@([\w-]+\.)+[\w-]{2,4}$)");
    int index=0;
    for (auto & mail:segment) {
        if (!std::regex_match(mail, pattern)) {
            segment.erase(segment.begin()+index);
            index--;
        }
        index++;
    }

}
void regexAndRanges() {
    std::vector<std::string> fullData=loadData("mails.txt");
    std::vector<std::string> regexSegmentV(fullData.begin(), fullData.begin()+fullData.size()/2+1);
    std::vector<std::string> rangesSegmentV(fullData.begin()+fullData.size()/2+1, fullData.end());
    std::future<std::vector<std::string>> rngFtr=std::async(std::launch::async,rangesSegment,std::ref(rangesSegmentV));
    regexSegment(regexSegmentV);
    rngFtr.wait();
    std::vector<std::string> rangesRes=rngFtr.get();
    std::vector<std::string> fullRes(regexSegmentV.begin(),regexSegmentV.end());
    fullRes.push_back("empty");
    std::ranges::copy(rangesRes.begin(), rangesRes.end(),fullRes.rbegin() );
        for (auto & correctMail: fullRes) {
            std::cout << correctMail << std::endl;
        }

}



int main() {

    std::future<std::vector<int>> ftr=std::async(std::launch::async, loadDataInBackground);
    for (int i=0;i<100;i++) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout<<"Long process in main...";
    }
    ftr.wait();
    std::vector<int> data=ftr.get();
    std::promise<std::string> thrPromise;
    std::future<std::string> ftrr=thrPromise.get_future();
    std::thread producerThread(producer, std::move(thrPromise));
    std::thread consumerThread(consumer, std::move(ftrr));
    if (producerThread.joinable())
        producerThread.join();
    if (consumerThread.joinable())
        consumerThread.join();

    std::string name="Bill";
    std::thread threadWithFunction(sayHello,std::ref(name));
    std::thread threadWithLambda([](std::string & name) {
        std::cout<<"Hello my name is "<<name<<"! \n";
    },std::ref(name));
    CallableClass clbcls(name);
    std::thread threadWithCallable(clbcls);
    std::thread threadWithClassMethod(&CallableClass::func, clbcls,std::ref(name));
    threadWithClassMethod.join();
    threadWithCallable.join();
    threadWithLambda.join();
    threadWithFunction.join();


    std::packaged_task<int(int,int)> sum([](int A, int B){return A+B;});
    std::future<int> result=sum.get_future();
    std::future<void> wrapper=std::async(std::move(sum),5,6);
    for (int i=0;i<10;i++)
        std::cout<<"Wating...";
    std::cout<<"The sum is: "<<result.get()<<std::endl;


    std::future<void> ftrV=std::async(std::launch::async,[]() {
        std::cout<<"I throw exceptions";
        throw std::runtime_error("I throw exceptions");
    });
    try {
        ftrV.get();
    }
    catch (std::exception & e) {
        std::cout<<e.what()<<std::endl;
    }



































}



