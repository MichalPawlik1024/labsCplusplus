#include<iostream>
#include<fstream>
#include<thread>
#include<future>
#include<vector>
///Producer thread doing

void fizz(std::vector<std::promise<std::vector<int>>> && promise) {
    std::vector<int> buffer(10);
    std::ifstream fin("data.txt");
    if (!fin.is_open()) {
        throw std::runtime_error("File does not exist");
    }
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

void fizzBuzz(std::vector<int> & v, int vSize) {
    for (int i=0;i<vSize;i++)
        if (v[i]%6==0) {
            std::cout<< v[i]<<" FizzBuzz \n";
        }
}
///consumer, need to give data size
///No other simple way of synchronizing
void buzz(int size,std::vector<std::future<std::vector<int>>> && ft ) {
    int iteration=0;
    while (size>0) {
        size-=10;
            ft.at(iteration).wait();
            std::vector<int> data = ft.at(iteration).get();
            fizzBuzz(data,size>=0?10:-size); ///Preventing buffer from loading garbage
            data.clear();
            iteration++;
    }

}

int determineDataSize() {
    std::ifstream fin("data.txt");
    int size=0;
    int tok;
    while (fin>>tok) {
        size++;
    }
    return size;
}

void fizzBuzzWithThread() {

    int totalData = determineDataSize();
    int bufferSize=totalData/10+(totalData%10!=0);

    std::vector<std::promise<std::vector<int>>> thrPromise(bufferSize); //Stale dane, normalnie trzeba by policzyc segmenty
    std::vector<std::future<std::vector<int>>> thrFuture(bufferSize); //Stale dane, normalnie trzeba by policzyc segmenty
    ///Data of std::future can be only readed once. So we need a vector of promise and future!
    for (int i=0;i<bufferSize;i++) {
        thrFuture.at(i)=thrPromise.at(i).get_future();
    }
    ///Can do with thread, or with async no problem
    // std::thread producerFizz(fizz,std::move(thrPromise));
    //std::thread consumerFizzBuzz(buzz,15,std::move(thrFuture));
    // producerFizz.join();
    //  consumerFizzBuzz.join();
    std::future<void> func1; ///This way you can safely manage exceptions
    std::future<void> func2;
    try {
        func1 = std::async(std::launch::async,fizz,std::move(thrPromise));
        func2 = std::async(std::launch::async,buzz,determineDataSize(),std::move(thrFuture));
        func1.get();
        func2.get();
    }
    catch (std::exception & e) {
        std::cerr << e.what() << '\n';
    }

}


int main() {

    fizzBuzzWithThread();
    return 0;
}