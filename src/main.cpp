# include <functional>
# include <list>
# include <mutex>
# include <thread>
# include <vector>
# include <iostream>

using namespace std;

class Worker {
	public:
		int nthreads;

		Worker(int n) {
			nthreads = n;
		}

		void myMethod() {
      		cout << "Threads specified: " << nthreads;
    	};
};

int main() {
	Worker worker_threads(4);
	Worker event_loop(1);

	event_loop.myMethod();
	worker_threads.myMethod();

  	return 0;
}
