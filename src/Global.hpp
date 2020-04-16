#ifndef MASCARPONELOGS_GLOBAL_HPP
#define MASCARPONELOGS_GLOBAL_HPP

#ifdef DEBUG
    #include <iostream>
    #include <thread>
    #define D(x) (x)
    #define D_threadID "[threadID:" << std::this_thread::get_id() << "]"
#else 
    #define D(x) do{}while(0)
#endif // DEBUG

#endif // MASCARPONELOGS_GLOBAL_HPP