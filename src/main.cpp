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
		condition_variable cv;
    	vector<thread> threads;
		mutex mtx;
		list<function<void()>> tasks;

		bool running = true;

		// Constructor
		Worker(int n) {
			nthreads = n;
		}

		// Initialize threads
		void start() {
			running = true;

			for (int i = 0; i < nthreads; i++) {

				threads.emplace_back([i, this] {

					while(running) {
						function<void()> task; {
							unique_lock<mutex> lock(mtx);

							while(tasks.empty() && running) {
								cv.wait(lock);
							}

							if(!tasks.empty()) {
								task = *tasks.begin();
								tasks.pop_front();
							} 

							if (task) task();

						}
					}

				});
			}
		}

		void stop() {
			running = false;
			cv.notify_all();
		}

		void post(function<void()> function) {
			unique_lock<mutex> lock(mtx);
        	tasks.emplace_back(function);
        	cv.notify_one();
		}

		void post_timeout(function<void()> function, int timeout){
			this_thread::sleep_for(chrono::milliseconds(timeout));
			unique_lock<mutex> lock(mtx);
        	tasks.emplace_back(function);
        	cv.notify_one();
    	}


		void join() {
			for (auto &thread : threads){
            	thread.join();
        	}
		}
};

int main() {
	Worker worker_threads(4);
	Worker event_loop(1);

	worker_threads.start(); // Create 4 internal threads
	event_loop.start(); // Create 1 internal thread

	worker_threads.post([] {
	// Task A
		cout << "\n Task A running in thread:" << this_thread::get_id() << endl;

	});
	worker_threads.post([] {
	// Task B
		cout << "\n Task B running in thread:" << this_thread::get_id() << endl;

	// Might run in parallel with task A
	});
	event_loop.post([] {
	// Task C
		cout << "\n Task C running in thread:" << this_thread::get_id() << endl;
	// Might run in parallel with task A and B
	});
	event_loop.post([] {
	// Task D
		cout << "\n Task D running in thread:" << this_thread::get_id() << endl;
	// Will run after task C
	// Might run in parallel with task A and B
	});

	
	worker_threads.post_timeout([]{
		cout << "\n Waiting task running in thread:" << this_thread::get_id() << endl;
    }, 3000);
	

	this_thread::sleep_for(chrono::milliseconds(100));

	worker_threads.stop();
	event_loop.stop();

	worker_threads.join(); // Calls join() on the worker threads
	event_loop.join(); // Calls join() on the event thread
	
	return 0;
}
