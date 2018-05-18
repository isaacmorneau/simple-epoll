## A Simple Clear Epoll Tutorial (With Splice)

---

Nov 9, 2017

Many times when someone is trying to get faster socket I/O, they read an article on how [epoll](http://man7.org/linux/man-pages/man7/epoll.7.html)
is amazing and how it should be used as its better than both [select](http://man7.org/linux/man-pages/man2/select.2.html) and [poll](http://man7.org/linux/man-pages/man2/poll.2.html).
While this is the case, the tutorials for it are from what I've found quite confusing. This is my effort to demystify it.

The code I'm using in this is posted to the [simple-epoll](https://github.com/isaacmorneau/simple-epoll) repo and it's a high speed echo server.

---

Starting off in `main.c` epoll works by having a structure that holds all the information on the file descriptors (FDs) you're listening to.
The system, when getting events, will put them all at the start of the array you allocate and then return the number of events.

To start setting up epoll you need to create the structure with `epoll_create1`, as the original `epoll_create` has unused params and doesnt allow you to set flags.
Once the structure has been made we can add the listening socket to the structure to be notified when it recieves a connection request.

Now that the basic server setup has been completed we loop, asking epoll to wake us when something happens. This is the `epoll_wait` function.
The return from it is how many events need to be handled or in other terms how many FDs that epoll was watching have had things happen to them.

> Note: The `#pragma omp parallel` is a directive to openMP to run the next code on as many cores as I have. 
`epoll_wait` is threadsafe so if another event happens while one thread or process is waiting epoll will wake another thread to handle it.

Choosing how to handle the events makes up the majority of the rest of the code. The `if`s in the for loop check 3 main things.

1. Was there an error in this event? If so, clean up the data structures for that client and continue.
2. Was there data to read on this fd? If so we break into two nested `if`s
    1. Was the data recieved on the listening socket? Notice the check is on `sfd` the original int for the listening socket.
    2. The data recieved must be from a client so echo it back.
3. The event must be a notification that we can write. This rarely happens but its for if the client recv window was full so when we were echoing the data back we had to wait so now we can continue echoing.

The bulk of code would happen typically on the 2.2 path where data was recieved and it was one of the clients connected sending data to the server. That functionality here is very simple as it just calls `echo`
which loops over [splice](http://man7.org/linux/man-pages/man2/splice.2.html) (another very cool function for speedy FDs transfers)
to just move the pages into a pipe, and then reassigning the pages back to the same FD.

In `wrapper.c` there are two functions `make_connected` and `make_bound` that have a loop on trying to bind or connect, respectively. This is due to how the replacement for the old version of resolving and address using 
`gethostbyname` worked vs the new way using [getaddrinfo](http://man7.org/linux/man-pages/man3/getaddrinfo.3.html) works. Because there are multiple possible network interfaces and IP types that are possible to connect
to, a function that only returns one (such as `gethostbyname`) doesn't suffice. To handle this, `getaddrinfo` returns a linked list that will be iterated over until one of the results works, thus the reason for the loop. Once
one of the results is successful you can stop looping.
